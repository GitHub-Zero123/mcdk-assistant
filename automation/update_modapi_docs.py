# -*- coding: utf-8 -*-
"""刷新 knowledge/ModAPI 下的网易 ModAPI 文档。

数据源：https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/1-ModAPI-beta/
流水线：抓取页面 HTML -> markdownify 转 Markdown -> mdformat 规范化，
输出格式与仓库既有文档保持一致（表格紧凑、列表项间空行、行内代码标记等）。

依赖：
    pip install requests beautifulsoup4 markdownify mdformat mdformat-gfm

用法：
    python automation/update_modapi_docs.py                 # 抓取并写入
    python automation/update_modapi_docs.py --dry-run       # 只报告将要写入的内容
    python automation/update_modapi_docs.py --refresh       # 忽略本地 HTML 缓存重新抓取
    python automation/update_modapi_docs.py --verify-stable # 重新检查需要以稳定版为准的页面
    python automation/update_modapi_docs.py --rebuild-index # 写入后重建索引缓存
"""
from __future__ import print_function

import argparse
import difflib
import json
import os
import re
import sys
import time
from urllib.parse import quote

import requests
from bs4 import BeautifulSoup, NavigableString, Tag
from markdownify import MarkdownConverter

try:
    import mdformat
except ImportError:  # pragma: no cover - 依赖缺失时给出可执行的提示
    mdformat = None

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KNOWLEDGE_ROOT = os.path.join(REPO_ROOT, 'knowledge')
MODAPI_DIR = os.path.join(KNOWLEDGE_ROOT, 'ModAPI')
CACHE_DIR = os.path.join(REPO_ROOT, '.cache_modapi_docs', 'beta')
CACHE_DIR_STABLE = os.path.join(REPO_ROOT, '.cache_modapi_docs', 'stable')

SITE_ROOT = 'https://mc.163.com/dev/mcmanual/mc-dev/mcdocs'
BETA_SECTION = '1-ModAPI-beta'
STABLE_SECTION = '1-ModAPI'
SIDEBAR_URL = SITE_ROOT + '/'
USER_AGENT = ('Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 '
              '(KHTML, like Gecko) Chrome/122.0 Safari/537.36')

# 站点侧边栏里属于 ModAPI 章节、但不参与知识库分类的目录，跳过
SKIP_PREFIXES = ('更新信息/',)

# beta 版删掉了部分章节而稳定版仍保留时，这几页以稳定版为准。
# 判定方式：稳定版正文是 beta 的严格超集且内容更多，用 --verify-stable 可重新核对。
STABLE_OVERRIDE = {
    '事件/物品',
    '事件/控制',
    '接口/世界/指令',
    '接口/自定义UI/通用设置',
}

# ── HTML -> Markdown ────────────────────────────────────────────────

# 正文里需要补行内代码标记的写法
TYPE_CALL = r'\b(?:list|tuple|dict|set)\s*\([^)]*\)|\b(?:list|tuple|dict|set)\s*\[[^\]]*\]'
DEF_SIG = r'\bdef\s+[A-Za-z_]\w*\s*\([^)]*\)'
BRACKET_LITERAL = r'\[[^\[\]]*\]'
# 方括号里带数字/引号/波浪号的才当成字面量代码，排除 [namespace:name] 这类占位写法
BRACKET_IS_LITERAL = re.compile(r'[\d\'"~]')
TOKEN_RE = re.compile('(?:%s)|(?:%s)|(?:%s)' % (DEF_SIG, TYPE_CALL, BRACKET_LITERAL))

SKIP_PARENTS = {'pre', 'code', 'script', 'style'}
EXTERNAL_HREF = re.compile(r'^(?:[a-zA-Z][a-zA-Z0-9+.\-]*:|//|#)')
# 围栏行：站点上有「列表项里嵌代码块」的写法（`  - ```），行首会带列表标记，
# 状态跟踪必须认得它，否则围栏配对会错位、漏掉后面的代码块。
FENCE = re.compile(r'^(\s*(?:[-*+]|\d+[.)])?\s*)```')
# 只有纯缩进的围栏才适合换成占位行（带列表标记的那种交给 mdformat 自己解析）
FENCE_PLAIN = re.compile(r'^(\s*)```')
PLACEHOLDER = re.compile(r'@@MCDKCODE(\d+)@@')


def _code_language(el):
    for cls in el.get('class') or []:
        if cls.startswith('language-'):
            return cls[len('language-'):]
    return ''


class _Converter(MarkdownConverter):
    def convert_pre(self, el, text, parent_tags):
        if not text:
            return ''
        lang = _code_language(el) or self.options['code_language']
        # 代码正文原样保留；顶层代码块在闭合围栏前留一个空行，与既有文档一致
        tail = '' if el.find_parent('li') is not None else '\n'
        return '\n```%s\n%s%s```\n' % (lang, text, tail)


def _strip_internal_links(node):
    """站点内部相对链接退化成纯文本，只保留锚点与绝对链接。"""
    for a in node.find_all('a'):
        href = a.get('href')
        if href and EXTERNAL_HREF.match(href):
            continue
        a.unwrap()


def _absolutize_assets(node):
    for img in node.find_all('img'):
        if (img.get('src') or '').startswith('/'):
            img['src'] = 'https://mc.163.com' + img['src']


def _inject_code_spans(node):
    """给 list(...) / tuple(...) / [0,1] / def f(...) 之类写法套上 <code>。"""
    for text_node in list(node.find_all(string=True)):
        parent = text_node.parent
        if parent is None or parent.name in SKIP_PARENTS:
            continue
        text = str(text_node)
        spans = []
        for m in TOKEN_RE.finditer(text):
            token = m.group(0)
            if token.startswith('[') and not BRACKET_IS_LITERAL.search(token):
                continue
            if spans and m.start() < spans[-1][1]:
                continue
            spans.append((m.start(), m.end()))
        if not spans:
            continue
        pieces, pos = [], 0
        for start, end in spans:
            if start > pos:
                pieces.append(NavigableString(text[pos:start]))
            code = Tag(name='code')
            code.string = text[start:end]
            pieces.append(code)
            pos = end
        if pos < len(text):
            pieces.append(NavigableString(text[pos:]))
        for piece in reversed(pieces):
            text_node.insert_after(piece)
        text_node.extract()


def _prepare(html):
    soup = BeautifulSoup(html, 'html.parser')
    node = soup.select_one('div.theme-default-content')
    if node is None:
        raise RuntimeError('页面缺少 theme-default-content 容器')
    for a in node.select('a.header-anchor'):
        a.decompose()
    _strip_internal_links(node)
    _absolutize_assets(node)
    _inject_code_spans(node)
    return node


def _dedent_tables(text):
    """列表项内部的表格不额外缩进（缩进会让 mdformat 把表格踢出列表项）。"""
    return '\n'.join(l.lstrip(' ') if re.match(r'^ {1,4}\|', l) else l
                     for l in text.split('\n'))


def _strip_line_trailing(text):
    """去掉行尾空格，避免 mdformat 把 <br> 渲染成硬换行反斜杠。"""
    out, in_fence = [], False
    for line in text.split('\n'):
        if FENCE.match(line):
            in_fence = not in_fence
        out.append(line if in_fence else line.rstrip())
    return '\n'.join(out)


def _shield_code(text):
    """普通围栏代码块换成占位行，避免 mdformat 改写代码正文。

    列表项里的围栏（`  - ```）不适合占位——替换会丢掉列表标记——整体交给
    mdformat 处理；但配对扫描必须认得它，否则后面的围栏会全部错位。
    """
    lines = text.split('\n')
    out, blocks = [], []
    i = 0
    while i < len(lines):
        if not FENCE.match(lines[i]):
            out.append(lines[i])
            i += 1
            continue
        j = i + 1
        while j < len(lines) and not FENCE.match(lines[j]):
            j += 1
        if j >= len(lines):
            out.extend(lines[i:])
            break
        plain = FENCE_PLAIN.match(lines[i])
        if plain:
            indent = plain.group(1)
            blocks.append((indent, lines[i:j + 1]))
            out.append('%s@@MCDKCODE%d@@' % (indent, len(blocks) - 1))
        else:
            out.extend(lines[i:j + 1])
        i = j + 1
    return '\n'.join(out), blocks


def _restore_code(text, blocks):
    out = []
    for line in text.split('\n'):
        m = PLACEHOLDER.search(line)
        if not m:
            out.append(line)
            continue
        old_indent, block = blocks[int(m.group(1))]
        new_indent = line[:len(line) - len(line.lstrip(' '))]
        for bl in block:
            if bl.strip() == '':
                out.append('')
            elif bl.startswith(old_indent):
                out.append(new_indent + bl[len(old_indent):])
            else:
                out.append(new_indent + bl.lstrip(' '))
    return '\n'.join(out)


def _compact_tables(text):
    """去掉单元格对齐补齐，并把分隔行统一成 | --- |。"""
    out, in_fence, prev_is_table = [], False, False
    for line in text.split('\n'):
        if FENCE.match(line):
            in_fence = not in_fence
            out.append(line)
            prev_is_table = False
            continue
        if not in_fence and line.strip().startswith('|'):
            body = [c.strip() for c in line.strip().split('|')][1:-1]
            if prev_is_table and body and all(
                    re.fullmatch(r':?-{2,}:?', c) for c in body):
                line = '| ' + ' | '.join('---' for _ in body) + ' |'
            else:
                line = '| ' + ' | '.join(body) + ' |'
            prev_is_table = True
        else:
            prev_is_table = False
        out.append(line)
    return '\n'.join(out)


def _collapse_spaces(text):
    """正文里连续空格压成一个（代码块内保持原样）。"""
    out, in_fence = [], False
    for line in text.split('\n'):
        if FENCE.match(line):
            in_fence = not in_fence
            out.append(line)
            continue
        if in_fence:
            out.append(line)
            continue
        out.append(re.sub(r'(?<=\S) {2,}(?=\S)', ' ', line))
    return '\n'.join(out)


def html_to_markdown(html):
    raw = _Converter(heading_style='ATX', bullets='-').convert(str(_prepare(html)))
    raw = _dedent_tables(raw)
    raw = _strip_line_trailing(raw)
    shielded, blocks = _shield_code(raw)
    formatted = mdformat.text(shielded, extensions={'gfm'},
                              options={'wrap': 'keep', 'number': False})
    out = _restore_code(formatted, blocks)
    out = _compact_tables(out)
    out = _collapse_spaces(out)
    out = re.sub(r'\n{3,}', '\n\n', out)
    return out.strip() + '\n'


def content_text(html):
    """页面正文的纯文本，用于比较两版内容。"""
    node = _prepare(html)
    return re.sub(r'\s+', ' ', node.get_text())


# ── 抓取 ────────────────────────────────────────────────────────────

_session = requests.Session()
_session.headers['User-Agent'] = USER_AGENT


def collect_pages(section):
    """从站点侧边栏收集章节内的全部页面（相对路径，不含 .html）。"""
    resp = _session.get(SIDEBAR_URL, timeout=60)
    resp.raise_for_status()
    body = resp.content.decode('utf-8', 'replace')
    pattern = re.escape('mcdocs/%s/' % section) + r'([^"#]+?\.html)'
    pages = {m.group(1).split('?')[0][:-len('.html')]
             for m in re.finditer(pattern, body)}
    return sorted(pages)


def fetch_page(section, rel, cache_dir, refresh=False):
    path = os.path.join(cache_dir, rel.replace('/', '__') + '.html')
    if os.path.exists(path) and not refresh and os.path.getsize(path) > 2048:
        with open(path, encoding='utf-8') as f:
            return f.read()
    url = '%s/%s/%s.html?catalog=1' % (SITE_ROOT, section, quote(rel))
    last_err = None
    for attempt in range(3):
        try:
            resp = _session.get(url, timeout=90)
            body = resp.content.decode('utf-8', 'replace')
            if resp.status_code != 200:
                last_err = 'HTTP %d' % resp.status_code
            elif 'theme-default-content' not in body:
                last_err = '缺少正文容器（%d 字节）' % len(body)
            else:
                os.makedirs(cache_dir, exist_ok=True)
                with open(path, 'w', encoding='utf-8') as f:
                    f.write(body)
                time.sleep(0.3)
                return body
        except Exception as exc:  # noqa: BLE001 - 网络异常需要重试
            last_err = repr(exc)
        time.sleep(1.5 * (attempt + 1))
    raise RuntimeError('抓取失败 %s: %s' % (rel, last_err))


# ── 写入 ────────────────────────────────────────────────────────────


def md_path(rel):
    return os.path.join(MODAPI_DIR, rel.replace('/', os.sep) + '.md')


def existing_pages():
    found = set()
    for dirpath, _dirnames, filenames in os.walk(MODAPI_DIR):
        for name in filenames:
            if name.endswith('.md'):
                rel = os.path.relpath(os.path.join(dirpath, name), MODAPI_DIR)
                found.add(rel[:-len('.md')].replace(os.sep, '/'))
    return found


def verify_stable(pages):
    """报告哪些页面应当改用稳定版（稳定版正文是 beta 的严格超集且更长）。"""
    use_stable = []
    for rel in pages:
        beta = fetch_page(BETA_SECTION, rel, CACHE_DIR)
        stable = fetch_page(STABLE_SECTION, rel, CACHE_DIR_STABLE)
        bt, st = content_text(beta), content_text(stable)
        if bt == st:
            continue
        sm = difflib.SequenceMatcher(None, bt, st)
        beta_only = sum(len(bt[i1:i2]) for tag, i1, i2, _j1, _j2
                        in sm.get_opcodes() if tag in ('delete', 'replace'))
        stable_only = sum(len(st[j1:j2]) for tag, _i1, _i2, j1, j2
                          in sm.get_opcodes() if tag in ('insert', 'replace'))
        if beta_only == 0 and stable_only > 0:
            use_stable.append((rel, stable_only))
    use_stable.sort(key=lambda item: -item[1])
    print('需要以稳定版为准的页面：')
    for rel, size in use_stable:
        print('    %-40s 稳定版独有 %d 字符' % (rel, size))
    print('与 STABLE_OVERRIDE 的差异：')
    names = {rel for rel, _ in use_stable}
    for rel in sorted(names - STABLE_OVERRIDE):
        print('    + %s' % rel)
    for rel in sorted(STABLE_OVERRIDE - names):
        print('    - %s' % rel)


def touch_watched_dirs():
    """知识库索引缓存只看顶层目录 mtime；原地改内容不会让缓存失效，这里补一次。"""
    now = time.time()
    for name in ('ModAPI',):
        path = os.path.join(KNOWLEDGE_ROOT, name)
        if os.path.exists(path):
            os.utime(path, (now, now))


def rebuild_index():
    exe = os.path.join(REPO_ROOT, 'build', 'x64-msvc-release', 'tools',
                       'mcdk-index-compiler', 'mcdk-index-compiler.exe')
    if not os.path.exists(exe):
        print('未找到 mcdk-index-compiler，跳过索引重建：', exe)
        return
    import subprocess
    for out_dir in (os.path.join(REPO_ROOT, 'build', 'x64-msvc-release'),
                    os.path.join(REPO_ROOT, 'install', 'x64-msvc-release')):
        if not os.path.isdir(out_dir):
            continue
        subprocess.check_call([exe, '--output-dir', out_dir,
                               '--dicts-dir', os.path.join(REPO_ROOT, 'dicts'),
                               '--knowledge-dir', KNOWLEDGE_ROOT])
        print('索引缓存已更新:', out_dir)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--dry-run', action='store_true', help='只报告，不写文件')
    parser.add_argument('--refresh', action='store_true', help='忽略本地 HTML 缓存')
    parser.add_argument('--verify-stable', action='store_true',
                        help='重新核对需要以稳定版为准的页面')
    parser.add_argument('--rebuild-index', action='store_true',
                        help='写入后重建索引缓存')
    args = parser.parse_args()

    if mdformat is None:
        print('缺少依赖 mdformat，请先执行：'
              'pip install requests beautifulsoup4 markdownify mdformat mdformat-gfm')
        return 2

    pages = [p for p in collect_pages(BETA_SECTION)
             if not p.startswith(SKIP_PREFIXES)]
    print('站点侧边栏页面数：%d' % len(pages))

    if args.verify_stable:
        verify_stable(pages)
        return 0

    written, failed = 0, []
    for i, rel in enumerate(pages, 1):
        try:
            html = fetch_page(BETA_SECTION, rel, CACHE_DIR, refresh=args.refresh)
            section = BETA_SECTION
            if rel in STABLE_OVERRIDE:
                html = fetch_page(STABLE_SECTION, rel, CACHE_DIR_STABLE,
                                  refresh=args.refresh)
                section = STABLE_SECTION
        except RuntimeError as exc:
            failed.append(str(exc))
            continue
        markdown = html_to_markdown(html)
        if len(markdown) < 40:
            failed.append('%s: 转换结果过短' % rel)
            continue
        if args.dry_run:
            print('  [%3d/%d] %-40s %s (%d 字节)' % (i, len(pages), rel, section, len(markdown)))
            written += 1
            continue
        path = md_path(rel)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        # 仓库既有文档为 CRLF（core.autocrlf=true），写入时保持一致
        with open(path, 'w', encoding='utf-8', newline='\r\n') as f:
            f.write(markdown)
        written += 1

    if args.dry_run:
        print('转换成功：%d / %d' % (written, len(pages)))
        return 1 if failed else 0

    # 清理上游已下线或已并入子目录的旧文件
    stale = sorted(existing_pages() - set(pages))
    for rel in stale:
        os.remove(md_path(rel))
        print('  已移除 %s' % rel)

    print('写入 %d 个文件，移除 %d 个文件' % (written, len(stale)))
    for item in failed:
        print('  !!', item)

    touch_watched_dirs()
    if args.rebuild_index:
        rebuild_index()
    else:
        print('提示：知识库索引缓存对子目录内的内容改动不敏感，'
              '读端首个进程会按 mtime 指纹重建；如需立即生效可加 --rebuild-index。')
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
