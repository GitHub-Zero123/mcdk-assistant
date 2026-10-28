# -*- coding: utf-8 -*-
"""刷新 knowledge/ShaderSDK 下的渲染龙系列文档。

来源：https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/
依赖：pip install requests beautifulsoup4 markdownify mdformat mdformat-gfm
用法：python automation/update_shadersdk_docs.py [--dry-run] [--refresh] [--rebuild-index]

复用 update_modapi_docs 的请求重试、HTML 缓存和 Markdown 格式化。
章节目录可能返回 403，因此从文档总目录的侧边栏发现页面，并继续发现正文链接。
README.md 为手工维护的工程接入说明，不参与爬取或清理。
"""
import argparse
from pathlib import Path
from urllib.parse import quote, unquote, urljoin, urlsplit

from bs4 import BeautifulSoup

import update_modapi_docs as shared

SECTION = '4-ShaderSDK'
BASE_URL = shared.SITE_ROOT + '/' + SECTION + '/'
OUTPUT_DIR = Path(shared.KNOWLEDGE_ROOT) / 'ShaderSDK'
CACHE_DIR = str(Path(shared.REPO_ROOT) / '.cache_shadersdk_docs')


def page_url(rel):
    return BASE_URL + quote(rel) + '.html?catalog=1'


def collect_links(html, base_url):
    """仅接纳同站点、同章节的 HTML 页面，去重锚点与查询参数。"""
    root = urlsplit(BASE_URL)
    pages = set()
    for link in BeautifulSoup(html, 'html.parser').select('a[href]'):
        url = urlsplit(urljoin(base_url, link['href']))
        path = unquote(url.path)
        if url.scheme != root.scheme or url.netloc != root.netloc:
            continue
        if not path.startswith(root.path) or not path.endswith('.html'):
            continue
        rel = path[len(root.path):-5]
        if not rel or any(part in ('', '.', '..') for part in rel.split('/')):
            continue
        if any(char in rel for char in '\\:\x00') or rel.casefold() == 'readme':
            continue
        pages.add(rel)
    return pages


def to_markdown(html, url):
    # shared 的转换器会移除相对链接；先补全地址，保留跨页参考和图片。
    soup = BeautifulSoup(html, 'html.parser')
    node = soup.select_one('div.theme-default-content')
    if node is None:
        raise RuntimeError('页面缺少 theme-default-content 容器')
    for tag, attr in [('a', 'href'), ('img', 'src')]:
        for element in node.select('%s[%s]' % (tag, attr)):
            value = element[attr]
            if not value.startswith('#'):
                element[attr] = urljoin(url, value)
    markdown = shared.html_to_markdown(str(soup))
    if len(markdown.strip()) < 40:
        raise RuntimeError('转换结果过短')
    return markdown.rstrip() + '\n\n来源：[%s](%s)\n' % (SECTION, url)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--dry-run', action='store_true', help='只抓取和转换，不写知识库')
    parser.add_argument('--refresh', action='store_true', help='忽略本地 HTML 缓存')
    parser.add_argument('--rebuild-index', action='store_true', help='成功写入后重建索引')
    args = parser.parse_args(argv)
    if shared.mdformat is None:
        print('请安装依赖：pip install requests beautifulsoup4 markdownify mdformat mdformat-gfm')
        return 2

    try:
        response = shared._session.get(shared.SIDEBAR_URL, timeout=60)
        response.raise_for_status()
        pending = collect_links(response.content.decode('utf-8'), shared.SIDEBAR_URL)
        if not pending:
            raise RuntimeError('未发现 ShaderSDK 页面，保留现有知识库')
        converted = {}
        while pending:
            rel = min(pending)
            pending.remove(rel)
            html = shared.fetch_page(SECTION, rel, CACHE_DIR, refresh=args.refresh)
            converted[rel] = to_markdown(html, page_url(rel))
            pending.update(collect_links(html, page_url(rel)) - converted.keys())
            print('  %s (%d 字符)' % (rel, len(converted[rel])))
    except (RuntimeError, ValueError, shared.requests.RequestException) as exc:
        print('抓取失败，未修改知识库：%s' % exc)
        return 1

    if args.dry_run:
        print('转换成功：%d；未写入知识库' % len(converted))
        return 0

    # 全部抓取成功后才写入；不自动删除本地文件，以保留手工补充资料。
    changed = 0
    for rel, markdown in sorted(converted.items()):
        path = OUTPUT_DIR / (rel + '.md')
        if path.exists() and path.read_text(encoding='utf-8') == markdown:
            continue
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(markdown, encoding='utf-8', newline='\n')
        changed += 1
    if changed:
        # 指纹仅监视顶层目录 mtime；覆盖已有文件时也要让索引失效。
        shared.os.utime(OUTPUT_DIR, None)
    print('转换 %d 篇，更新 %d 篇：%s' % (len(converted), changed, OUTPUT_DIR))
    if args.rebuild_index:
        shared.rebuild_index()
    else:
        print('可加 --rebuild-index 重建索引（请先编译新版 mcdk-index-compiler）。')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
