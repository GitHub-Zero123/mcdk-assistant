import "./styles.css";
import { highlightLazily } from "./highlight";
import { icon, type IconName } from "./icons";
import { initSplitters } from "./layout";
import {
  appendHighlighted,
  cleanSnippet,
  collectHeadings,
  highlightInPlace,
  highlightTerms,
  renderMarkdown,
  type Heading,
} from "./markdown";
import {
  ApiError,
  fetchDocument,
  fetchMeta,
  scopes,
  searchDocuments,
  type DocumentResponse,
  type SearchItem,
  type SearchScope,
} from "./api";

const PAGE_SIZE = 20;
const DEBOUNCE_MS = 260;
const THEME_KEY = "mcdk-theme";

const scopeLabels: Record<SearchScope | "other", string> = {
  all: "全部",
  api: "ModAPI",
  event: "事件",
  enum: "枚举",
  wiki: "Bedrock Wiki",
  dev: "Bedrock Dev",
  qumod: "QuMod",
  netease: "网易教程",
  other: "其他",
};

const samples = ["CreateExplosion", "molang", "loot_table", "自定义方块", "玩家交互", "粒子特效"];

const state = {
  query: "",
  scope: "all" as SearchScope,
  page: 1,
  hasMore: false,
  items: [] as SearchItem[],
  active: -1,
  selectedPath: "",
  searchController: null as AbortController | null,
  documentController: null as AbortController | null,
  debounce: 0,
  headings: [] as Heading[],
  spy: null as IntersectionObserver | null,
  code: null as IntersectionObserver | null,
};

const app = document.querySelector<HTMLDivElement>("#app");
if (!app) throw new Error("Missing application root");

app.innerHTML = `
  <header class="topbar">
    <a class="logo" href="/" title="MCDK 资料库">
      <span class="logo__mark" aria-hidden="true"></span>
      <span class="logo__text">mcdk<span>资料库</span></span>
    </a>

    <form class="omnibox" id="form" role="search">
      <span class="omnibox__icon" id="form-icon"></span>
      <input id="input" type="search" autocomplete="off" spellcheck="false" maxlength="256"
             placeholder="搜索接口、事件、组件、教程…" aria-label="搜索资料" />
      <button class="omnibox__clear" id="clear" type="button" title="清空" aria-label="清空" hidden></button>
      <kbd class="omnibox__hint" id="hint">Ctrl K</kbd>
    </form>

    <div class="topbar__end">
      <span class="health" id="health" title="索引状态"><i class="health__dot"></i><span>连接中</span></span>
      <button class="ghost" id="theme" type="button" aria-label="切换主题"></button>
    </div>
  </header>

  <div class="shell" id="shell">
    <nav class="rail" aria-label="资料分类">
      <p class="rail__label">分类</p>
      <div class="rail__scopes" id="scopes"></div>
      <div class="rail__keys">
        <p class="rail__label">快捷键</p>
        <dl>
          <dt><kbd>Ctrl</kbd><kbd>K</kbd></dt><dd>聚焦搜索</dd>
          <dt><kbd>↑</kbd><kbd>↓</kbd></dt><dd>切换结果</dd>
          <dt><kbd>Enter</kbd></dt><dd>打开文档</dd>
          <dt><kbd>Esc</kbd></dt><dd>关闭 / 清空</dd>
        </dl>
      </div>
    </nav>

    <div class="handle" data-resize="rail" role="separator" aria-orientation="vertical"
         tabindex="0" title="拖动调整宽度（双击复位）" aria-label="调整分类栏宽度"></div>

    <section class="results" aria-label="检索结果">
      <div class="results__bar">
        <span class="results__count" id="count">输入关键词开始检索</span>
        <span class="results__timing" id="timing"></span>
      </div>
      <div class="results__scroll" id="hits" aria-live="polite"></div>
      <div class="pager" id="pager" hidden>
        <button class="ghost" id="prev" type="button" title="上一页" aria-label="上一页"></button>
        <span id="page">1</span>
        <button class="ghost" id="next" type="button" title="下一页" aria-label="下一页"></button>
      </div>
    </section>

    <div class="handle" data-resize="results" role="separator" aria-orientation="vertical"
         tabindex="0" title="拖动调整宽度（双击复位）" aria-label="调整结果栏宽度"></div>

    <section class="doc" id="doc" aria-label="文档">
      <div class="doc__bar">
        <span class="tag" id="doc-tag">资料</span>
        <span class="doc__path" id="doc-path"></span>
        <span class="doc__lines" id="doc-lines"></span>
        <button class="ghost" id="doc-copy" type="button" title="复制路径" aria-label="复制路径"></button>
        <button class="ghost" id="doc-close" type="button" title="关闭 (Esc)" aria-label="关闭文档"></button>
      </div>
      <div class="doc__body">
        <div class="doc__scroll" id="doc-scroll"></div>
        <div class="handle handle--toc" data-resize="toc" role="separator" aria-orientation="vertical"
             tabindex="0" title="拖动调整宽度（双击复位）" aria-label="调整目录宽度"></div>
        <aside class="toc" id="toc" hidden><p class="rail__label">目录</p><ol id="toc-list"></ol></aside>
      </div>
    </section>
  </div>
`;

function need<T extends Element>(selector: string): T {
  const element = document.querySelector<T>(selector);
  if (!element) throw new Error(`Missing element: ${selector}`);
  return element;
}

const ui = {
  shell: need<HTMLElement>("#shell"),
  form: need<HTMLFormElement>("#form"),
  formIcon: need<HTMLElement>("#form-icon"),
  input: need<HTMLInputElement>("#input"),
  clear: need<HTMLButtonElement>("#clear"),
  hint: need<HTMLElement>("#hint"),
  health: need<HTMLElement>("#health"),
  theme: need<HTMLButtonElement>("#theme"),
  scopes: need<HTMLElement>("#scopes"),
  count: need<HTMLElement>("#count"),
  timing: need<HTMLElement>("#timing"),
  hits: need<HTMLElement>("#hits"),
  pager: need<HTMLElement>("#pager"),
  prev: need<HTMLButtonElement>("#prev"),
  next: need<HTMLButtonElement>("#next"),
  page: need<HTMLElement>("#page"),
  doc: need<HTMLElement>("#doc"),
  docTag: need<HTMLElement>("#doc-tag"),
  docPath: need<HTMLElement>("#doc-path"),
  docLines: need<HTMLElement>("#doc-lines"),
  docCopy: need<HTMLButtonElement>("#doc-copy"),
  docClose: need<HTMLButtonElement>("#doc-close"),
  docScroll: need<HTMLElement>("#doc-scroll"),
  toc: need<HTMLElement>("#toc"),
  tocList: need<HTMLOListElement>("#toc-list"),
};

ui.formIcon.append(icon("search", 15));
ui.clear.append(icon("x", 14));
ui.prev.append(icon("left", 15));
ui.next.append(icon("right", 15));
ui.docCopy.append(icon("copy", 14));
ui.docClose.append(icon("x", 15));

const isApple = /Mac|iPhone|iPad/u.test(navigator.platform);
ui.hint.textContent = isApple ? "⌘K" : "Ctrl K";

/* ── theme: system → light → dark → system ─────────────────────────────── */

type ThemeMode = "system" | "light" | "dark";

const themeIcons: Record<ThemeMode, IconName> = { system: "monitor", light: "sun", dark: "moon" };
const themeTitles: Record<ThemeMode, string> = { system: "主题：跟随系统", light: "主题：浅色", dark: "主题：深色" };

function readThemeMode(): ThemeMode {
  const stored = document.documentElement.dataset["theme"];
  return stored === "light" || stored === "dark" ? stored : "system";
}

function applyThemeMode(mode: ThemeMode): void {
  if (mode === "system") delete document.documentElement.dataset["theme"];
  else document.documentElement.dataset["theme"] = mode;
  try {
    if (mode === "system") localStorage.removeItem(THEME_KEY);
    else localStorage.setItem(THEME_KEY, mode);
  } catch {
    // Storage can be unavailable in private mode; the toggle still works per session.
  }
  ui.theme.replaceChildren(icon(themeIcons[mode], 15));
  ui.theme.title = themeTitles[mode];
}

ui.theme.addEventListener("click", () => {
  const order: ThemeMode[] = ["system", "light", "dark"];
  const next = order[(order.indexOf(readThemeMode()) + 1) % order.length] ?? "system";
  applyThemeMode(next);
});

applyThemeMode(readThemeMode());

/* ── url state ─────────────────────────────────────────────────────────── */

function readUrl(): void {
  const params = new URLSearchParams(location.search);
  const requestedScope = params.get("scope");
  const requestedPage = Number(params.get("page") ?? "1");
  state.query = (params.get("q")?.trim() ?? "").slice(0, 256);
  state.scope = scopes.includes(requestedScope as SearchScope) ? (requestedScope as SearchScope) : "all";
  state.page = Number.isInteger(requestedPage) && requestedPage > 0 ? requestedPage : 1;
  state.selectedPath = params.get("doc") ?? "";
  ui.input.value = state.query;
  ui.clear.hidden = state.query.length === 0;
}

function writeUrl(): void {
  const params = new URLSearchParams();
  if (state.query) params.set("q", state.query);
  if (state.scope !== "all") params.set("scope", state.scope);
  if (state.page > 1) params.set("page", String(state.page));
  if (state.selectedPath) params.set("doc", state.selectedPath);
  const query = params.toString();
  history.replaceState(null, "", query ? `?${query}` : location.pathname);
}

/* ── scopes ────────────────────────────────────────────────────────────── */

function renderScopes(): void {
  ui.scopes.replaceChildren(
    ...scopes.map((scope) => {
      const button = document.createElement("button");
      button.type = "button";
      button.className = "scope";
      button.dataset["scope"] = scope;
      button.textContent = scopeLabels[scope];
      button.setAttribute("aria-pressed", String(scope === state.scope));
      return button;
    }),
  );
}

ui.scopes.addEventListener("click", (event) => {
  const button = (event.target as HTMLElement).closest<HTMLButtonElement>("button[data-scope]");
  const scope = button?.dataset["scope"];
  if (!scope || scope === state.scope) return;
  state.scope = scope as SearchScope;
  state.page = 1;
  renderScopes();
  writeUrl();
  void runSearch();
});

/* ── placeholders ──────────────────────────────────────────────────────── */

function notice(name: IconName, title: string, detail?: string, className = "notice"): HTMLElement {
  const box = document.createElement("div");
  box.className = className;
  box.append(icon(name, 20));
  const heading = document.createElement("p");
  heading.className = "notice__title";
  heading.textContent = title;
  box.append(heading);
  if (detail) {
    const text = document.createElement("p");
    text.className = "notice__detail";
    text.textContent = detail;
    box.append(text);
  }
  return box;
}

function startPanel(): HTMLElement {
  const box = document.createElement("div");
  box.className = "start";
  const label = document.createElement("p");
  label.className = "rail__label";
  label.textContent = "试试这些";
  const list = document.createElement("div");
  list.className = "start__chips";
  for (const sample of samples) {
    const chip = document.createElement("button");
    chip.type = "button";
    chip.className = "chip";
    chip.textContent = sample;
    chip.addEventListener("click", () => {
      ui.input.value = sample;
      submit();
      ui.input.focus();
    });
    list.append(chip);
  }
  box.append(label, list);
  return box;
}

function skeleton(): HTMLElement {
  const box = document.createElement("div");
  box.className = "skeleton";
  for (const widths of [["38%", "72%", "94%"], ["30%", "64%", "88%"], ["42%", "78%", "90%"], ["34%", "60%", "96%"], ["40%", "70%", "84%"], ["28%", "74%", "92%"]]) {
    const row = document.createElement("div");
    row.className = "skeleton__row";
    for (const width of widths) {
      const bar = document.createElement("span");
      bar.className = "skeleton__bar";
      bar.style.width = width;
      row.append(bar);
    }
    box.append(row);
  }
  return box;
}

function failure(error: unknown): HTMLElement {
  const message = error instanceof ApiError || error instanceof Error ? error.message : "请求失败";
  return notice("file", "请求失败", message, "notice notice--error");
}

/* ── search ────────────────────────────────────────────────────────────── */

async function runSearch(): Promise<void> {
  state.searchController?.abort();
  state.active = -1;
  state.items = [];

  if (!state.query) {
    state.hasMore = false;
    ui.count.textContent = "输入关键词开始检索";
    ui.timing.textContent = "";
    ui.hits.replaceChildren(startPanel());
    ui.pager.hidden = true;
    return;
  }

  const controller = new AbortController();
  state.searchController = controller;
  ui.count.textContent = "检索中…";
  ui.timing.textContent = "";
  ui.hits.replaceChildren(skeleton());
  ui.pager.hidden = true;

  const started = performance.now();
  try {
    const result = await searchDocuments(state.query, state.scope, state.page, PAGE_SIZE, controller.signal);
    const elapsed = Math.round(performance.now() - started);
    state.items = result.items;
    state.hasMore = result.has_more;

    ui.count.textContent = result.items.length
      ? `${result.items.length} 条结果 · ${scopeLabels[state.scope]}`
      : `无匹配结果 · ${scopeLabels[state.scope]}`;
    ui.timing.textContent = `${elapsed}ms`;

    if (result.items.length) {
      const terms = highlightTerms(state.query);
      ui.hits.replaceChildren(...result.items.map((item, index) => renderHit(item, index, terms)));
      ui.hits.scrollTop = 0;
    } else {
      ui.hits.replaceChildren(notice("search", "没有找到相关资料", "换个关键词，或切换到其他分类再试一次。"));
    }

    ui.pager.hidden = result.items.length === 0 || (state.page === 1 && !state.hasMore);
    ui.page.textContent = String(state.page);
    ui.prev.disabled = state.page <= 1;
    ui.next.disabled = !state.hasMore;
    markSelection();
  } catch (error) {
    if (controller.signal.aborted) return;
    ui.count.textContent = "检索失败";
    ui.timing.textContent = "";
    ui.hits.replaceChildren(failure(error));
  } finally {
    if (state.searchController === controller) state.searchController = null;
  }
}

function renderHit(item: SearchItem, index: number, terms: string[]): HTMLElement {
  const hit = document.createElement("button");
  hit.type = "button";
  hit.className = "hit";
  hit.dataset["path"] = item.path;
  hit.dataset["index"] = String(index);

  const top = document.createElement("span");
  top.className = "hit__top";
  const tag = document.createElement("span");
  tag.className = "tag";
  tag.textContent = scopeLabels[item.source];
  const path = document.createElement("span");
  path.className = "hit__path";
  path.textContent = `${item.path}:${item.line_start}`;
  path.title = `${item.path}:${item.line_start}`;
  top.append(tag, path);

  const title = document.createElement("span");
  title.className = "hit__title";
  appendHighlighted(title, item.title, terms);

  const snippet = document.createElement("span");
  snippet.className = "hit__snippet";
  appendHighlighted(snippet, cleanSnippet(item.snippet), terms);

  hit.append(top, title, snippet);
  hit.addEventListener("click", () => {
    state.active = index;
    void openDocument(item);
  });
  return hit;
}

function markSelection(): void {
  for (const hit of ui.hits.querySelectorAll<HTMLElement>(".hit")) {
    hit.classList.toggle("is-open", state.selectedPath !== "" && hit.dataset["path"] === state.selectedPath);
    hit.classList.toggle("is-active", Number(hit.dataset["index"]) === state.active);
  }
}

function moveActive(delta: number): void {
  if (!state.items.length) return;
  const next = state.active < 0 ? (delta > 0 ? 0 : state.items.length - 1) : state.active + delta;
  state.active = Math.min(Math.max(next, 0), state.items.length - 1);
  markSelection();
  ui.hits.querySelector<HTMLElement>(`.hit[data-index="${state.active}"]`)?.scrollIntoView({ block: "nearest" });
}

/* ── document ──────────────────────────────────────────────────────────── */

async function openDocument(item: { path: string; source?: SearchItem["source"]; title?: string }): Promise<void> {
  state.documentController?.abort();
  state.selectedPath = item.path;
  writeUrl();
  markSelection();

  ui.shell.classList.add("is-reading");
  ui.docTag.textContent = scopeLabels[item.source ?? "other"];
  ui.docPath.textContent = item.path;
  ui.docPath.title = item.path;
  ui.docLines.textContent = "";
  ui.toc.hidden = true;
  state.code?.disconnect();
  state.code = null;
  ui.docScroll.replaceChildren(notice("file", "正在读取…"));

  const controller = new AbortController();
  state.documentController = controller;
  try {
    const document = await fetchDocument(item.path, controller.signal);
    renderDocument(document, item.title);
  } catch (error) {
    if (controller.signal.aborted) return;
    ui.docScroll.replaceChildren(failure(error));
  } finally {
    if (state.documentController === controller) state.documentController = null;
  }
}

function renderDocument(data: DocumentResponse, anchorTitle?: string): void {
  ui.docTag.textContent = scopeLabels[data.source];
  ui.docPath.textContent = data.path;
  ui.docPath.title = data.path;
  ui.docLines.textContent = `${data.total_lines.toLocaleString("zh-CN")} 行`;

  const terms = highlightTerms(state.query);
  const body = renderMarkdown(data.content);
  highlightInPlace(body, terms);
  state.headings = collectHeadings(body);
  ui.docScroll.replaceChildren(body);
  ui.docScroll.scrollTop = 0;
  renderToc();

  state.code?.disconnect();
  // Syntax highlighting rewrites the block, so query marks are re-applied after.
  state.code = highlightLazily(body, ui.docScroll, (code) => highlightInPlace(code, terms));

  const target = anchorTitle ? state.headings.find((heading) => heading.text === anchorTitle.trim()) : undefined;
  if (!target) return;
  target.element.scrollIntoView({ block: "start" });
  target.element.classList.add("is-target");
  setTimeout(() => target.element.classList.remove("is-target"), 1500);
}

function renderToc(): void {
  state.spy?.disconnect();
  state.spy = null;
  if (state.headings.length < 3) {
    ui.toc.hidden = true;
    ui.tocList.replaceChildren();
    return;
  }

  ui.tocList.replaceChildren(
    ...state.headings.map((heading) => {
      const row = document.createElement("li");
      row.className = `toc__item toc__item--h${heading.level}`;
      row.dataset["for"] = heading.id;
      const link = document.createElement("button");
      link.type = "button";
      link.textContent = heading.text;
      link.title = heading.text;
      link.addEventListener("click", () => heading.element.scrollIntoView({ block: "start", behavior: "smooth" }));
      row.append(link);
      return row;
    }),
  );
  ui.toc.hidden = false;

  state.spy = new IntersectionObserver(
    (entries) => {
      const visible = entries.find((entry) => entry.isIntersecting);
      if (!visible) return;
      for (const row of ui.tocList.children) {
        row.classList.toggle("is-current", (row as HTMLElement).dataset["for"] === visible.target.id);
      }
    },
    { root: ui.docScroll, rootMargin: "0px 0px -72% 0px", threshold: 0 },
  );
  for (const heading of state.headings) state.spy.observe(heading.element);
}

function closeDocument(): void {
  state.documentController?.abort();
  state.spy?.disconnect();
  state.spy = null;
  state.code?.disconnect();
  state.code = null;
  state.selectedPath = "";
  state.headings = [];
  writeUrl();
  markSelection();
  ui.shell.classList.remove("is-reading");
  ui.docScroll.replaceChildren();
  ui.tocList.replaceChildren();
  ui.toc.hidden = true;
}

ui.docClose.addEventListener("click", closeDocument);

ui.docCopy.addEventListener("click", async () => {
  if (!state.selectedPath) return;
  try {
    await navigator.clipboard.writeText(state.selectedPath);
    ui.docCopy.replaceChildren(icon("check", 14));
    ui.docCopy.classList.add("is-done");
    setTimeout(() => {
      ui.docCopy.replaceChildren(icon("copy", 14));
      ui.docCopy.classList.remove("is-done");
    }, 1200);
  } catch {
    // Clipboard access can be denied; the path stays visible in the toolbar.
  }
});

/* ── input wiring ──────────────────────────────────────────────────────── */

function submit(resetPage = true): void {
  clearTimeout(state.debounce);
  const query = ui.input.value.trim();
  if (resetPage) state.page = 1;
  state.query = query;
  ui.clear.hidden = query.length === 0;
  writeUrl();
  void runSearch();
}

ui.form.addEventListener("submit", (event) => {
  event.preventDefault();
  submit();
});

ui.input.addEventListener("input", () => {
  ui.clear.hidden = ui.input.value.length === 0;
  clearTimeout(state.debounce);
  state.debounce = window.setTimeout(() => submit(), DEBOUNCE_MS);
});

ui.clear.addEventListener("click", () => {
  ui.input.value = "";
  submit();
  ui.input.focus();
});

ui.prev.addEventListener("click", () => {
  if (state.page <= 1) return;
  state.page -= 1;
  writeUrl();
  void runSearch();
});

ui.next.addEventListener("click", () => {
  if (!state.hasMore) return;
  state.page += 1;
  writeUrl();
  void runSearch();
});

ui.input.addEventListener("keydown", (event) => {
  if (event.key === "ArrowDown" || event.key === "ArrowUp") {
    event.preventDefault();
    moveActive(event.key === "ArrowDown" ? 1 : -1);
    return;
  }
  if (event.key === "Enter" && state.active >= 0) {
    const item = state.items[state.active];
    if (item) {
      event.preventDefault();
      void openDocument(item);
    }
  }
});

window.addEventListener("keydown", (event) => {
  const typing = event.target instanceof HTMLInputElement || event.target instanceof HTMLTextAreaElement;

  if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === "k") {
    event.preventDefault();
    ui.input.focus();
    ui.input.select();
    return;
  }
  if (event.key === "/" && !typing) {
    event.preventDefault();
    ui.input.focus();
    return;
  }
  if (event.key === "Escape") {
    if (state.selectedPath) closeDocument();
    else if (ui.input.value) {
      ui.input.value = "";
      submit();
    }
    ui.input.focus();
    return;
  }
  if (!typing && (event.key === "ArrowDown" || event.key === "ArrowUp") && state.items.length) {
    event.preventDefault();
    moveActive(event.key === "ArrowDown" ? 1 : -1);
  }
});

window.addEventListener("popstate", () => {
  readUrl();
  renderScopes();
  void runSearch();
  if (state.selectedPath) void openDocument({ path: state.selectedPath });
  else closeDocument();
});

/* ── boot ──────────────────────────────────────────────────────────────── */

function dot(): HTMLElement {
  const element = document.createElement("i");
  element.className = "health__dot";
  return element;
}

function label(value: string): HTMLElement {
  const element = document.createElement("span");
  element.textContent = value;
  return element;
}

async function initialize(): Promise<void> {
  readUrl();
  renderScopes();
  initSplitters(ui.shell);
  ui.hits.replaceChildren(startPanel());

  try {
    const meta = await fetchMeta();
    ui.health.classList.add("is-online");
    ui.health.replaceChildren(dot(), label(`${meta.documents.toLocaleString("zh-CN")} 篇`));
  } catch {
    ui.health.classList.add("is-offline");
    ui.health.replaceChildren(dot(), label("服务不可用"));
  }

  if (state.query) await runSearch();
  if (state.selectedPath) await openDocument({ path: state.selectedPath });
  if (!state.query) ui.input.focus();
}

void initialize();
