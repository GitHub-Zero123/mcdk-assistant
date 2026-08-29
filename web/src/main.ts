import DOMPurify from "dompurify";
import { marked } from "marked";
import {
  BookOpen,
  ChevronLeft,
  ChevronRight,
  FileText,
  LoaderCircle,
  Menu,
  Search,
  X,
  createIcons,
} from "lucide";
import "./styles.css";
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

const state = {
  query: "",
  scope: "all" as SearchScope,
  page: 1,
  hasMore: false,
  searching: false,
  selectedPath: "",
  searchController: null as AbortController | null,
  documentController: null as AbortController | null,
};

const app = document.querySelector<HTMLDivElement>("#app");
if (!app) throw new Error("Missing application root");

app.innerHTML = `
  <header class="app-header">
    <div class="header-inner">
      <a class="brand" href="/" aria-label="MCDK 资料库首页">
        <span class="brand-mark" aria-hidden="true"><span></span><span></span><span></span><span></span></span>
        <span class="brand-copy"><strong>MCDK</strong><small>资料库</small></span>
      </a>
      <div class="index-status" id="index-status"><span class="status-dot"></span><span>正在连接</span></div>
    </div>
  </header>

  <main>
    <section class="search-band" aria-labelledby="search-title">
      <div class="search-inner">
        <div class="search-heading">
          <p class="eyebrow">MINECRAFT DEVELOPMENT KNOWLEDGE</p>
          <h1 id="search-title">开发资料，一处检索</h1>
        </div>
        <form class="search-form" id="search-form" role="search">
          <i data-lucide="search" aria-hidden="true"></i>
          <input id="search-input" type="search" autocomplete="off" maxlength="256" placeholder="搜索事件、API、组件或开发指南" aria-label="搜索资料" />
          <button class="icon-button clear-button" id="clear-button" type="button" title="清空搜索" aria-label="清空搜索" hidden>
            <i data-lucide="x" aria-hidden="true"></i>
          </button>
          <button class="search-button" type="submit"><i data-lucide="search" aria-hidden="true"></i><span>搜索</span></button>
        </form>
        <nav class="scope-tabs" id="scope-tabs" aria-label="资料分类"></nav>
      </div>
    </section>

    <section class="workspace" id="workspace">
      <div class="results-pane">
        <div class="results-toolbar">
          <div>
            <p class="toolbar-label">检索结果</p>
            <p class="result-summary" id="result-summary">等待查询</p>
          </div>
          <button class="icon-button mobile-document-button" id="mobile-document-button" type="button" title="打开文档" aria-label="打开文档" hidden>
            <i data-lucide="menu" aria-hidden="true"></i>
          </button>
        </div>
        <div class="results-list" id="results-list" aria-live="polite"></div>
        <nav class="pagination" id="pagination" aria-label="搜索结果分页" hidden>
          <button class="icon-button" id="previous-page" type="button" title="上一页" aria-label="上一页"><i data-lucide="chevron-left"></i></button>
          <span id="page-number">第 1 页</span>
          <button class="icon-button" id="next-page" type="button" title="下一页" aria-label="下一页"><i data-lucide="chevron-right"></i></button>
        </nav>
      </div>

      <aside class="document-pane" id="document-pane" aria-label="文档阅读器">
        <div class="document-toolbar">
          <div class="document-location">
            <span class="source-badge" id="document-source">资料</span>
            <span id="document-path">未选择文档</span>
          </div>
          <button class="icon-button document-close" id="document-close" type="button" title="关闭文档" aria-label="关闭文档">
            <i data-lucide="x"></i>
          </button>
        </div>
        <article class="document-content" id="document-content">
          <div class="document-empty">
            <i data-lucide="book-open" aria-hidden="true"></i>
            <p>未选择文档</p>
          </div>
        </article>
      </aside>
    </section>
  </main>
`;

createIcons({ icons: { BookOpen, ChevronLeft, ChevronRight, FileText, LoaderCircle, Menu, Search, X } });

const elements = {
  form: required<HTMLFormElement>("#search-form"),
  input: required<HTMLInputElement>("#search-input"),
  clear: required<HTMLButtonElement>("#clear-button"),
  tabs: required<HTMLElement>("#scope-tabs"),
  status: required<HTMLElement>("#index-status"),
  summary: required<HTMLElement>("#result-summary"),
  list: required<HTMLElement>("#results-list"),
  pagination: required<HTMLElement>("#pagination"),
  previous: required<HTMLButtonElement>("#previous-page"),
  next: required<HTMLButtonElement>("#next-page"),
  page: required<HTMLElement>("#page-number"),
  documentPane: required<HTMLElement>("#document-pane"),
  documentPath: required<HTMLElement>("#document-path"),
  documentSource: required<HTMLElement>("#document-source"),
  documentContent: required<HTMLElement>("#document-content"),
  documentClose: required<HTMLButtonElement>("#document-close"),
  mobileDocument: required<HTMLButtonElement>("#mobile-document-button"),
};

function required<T extends Element>(selector: string): T {
  const element = document.querySelector<T>(selector);
  if (!element) throw new Error(`Missing element: ${selector}`);
  return element;
}

function renderTabs(): void {
  elements.tabs.replaceChildren(
    ...scopes.map((scope) => {
      const button = document.createElement("button");
      button.type = "button";
      button.className = "scope-tab";
      button.dataset.scope = scope;
      button.textContent = scopeLabels[scope];
      button.setAttribute("aria-pressed", String(scope === state.scope));
      if (scope === state.scope) button.classList.add("active");
      return button;
    }),
  );
}

function readUrlState(): void {
  const params = new URLSearchParams(location.search);
  const query = params.get("q")?.trim() ?? "";
  const requestedScope = params.get("scope");
  const requestedPage = Number(params.get("page") ?? "1");
  state.query = query.slice(0, 256);
  state.scope = scopes.includes(requestedScope as SearchScope) ? (requestedScope as SearchScope) : "all";
  state.page = Number.isInteger(requestedPage) && requestedPage > 0 ? requestedPage : 1;
  state.selectedPath = params.get("doc") ?? "";
  elements.input.value = state.query;
  elements.clear.hidden = state.query.length === 0;
}

function writeUrlState(): void {
  const params = new URLSearchParams();
  if (state.query) params.set("q", state.query);
  if (state.scope !== "all") params.set("scope", state.scope);
  if (state.page > 1) params.set("page", String(state.page));
  if (state.selectedPath) params.set("doc", state.selectedPath);
  const query = params.toString();
  history.replaceState(null, "", query ? `?${query}` : location.pathname);
}

async function runSearch(): Promise<void> {
  state.searchController?.abort();
  if (!state.query) {
    state.hasMore = false;
    elements.summary.textContent = "等待查询";
    elements.list.replaceChildren(emptyState("search", "尚无检索结果"));
    elements.pagination.hidden = true;
    return;
  }

  state.searching = true;
  const controller = new AbortController();
  state.searchController = controller;
  elements.summary.textContent = `正在检索“${state.query}”`;
  elements.list.replaceChildren(loadingState("正在检索资料"));
  elements.pagination.hidden = true;

  try {
    const result = await searchDocuments(state.query, state.scope, state.page, PAGE_SIZE, controller.signal);
    state.hasMore = result.has_more;
    elements.summary.textContent = result.items.length
      ? `“${result.query}” · 本页 ${result.items.length} 条`
      : `“${result.query}” · 未找到结果`;
    elements.list.replaceChildren(
      ...(result.items.length ? result.items.map(renderResult) : [emptyState("file-text", "没有匹配的资料")]),
    );
    elements.pagination.hidden = result.items.length === 0 || (state.page === 1 && !state.hasMore);
    elements.page.textContent = `第 ${state.page} 页`;
    elements.previous.disabled = state.page <= 1;
    elements.next.disabled = !state.hasMore;
  } catch (error) {
    if (controller.signal.aborted) return;
    elements.summary.textContent = "检索失败";
    elements.list.replaceChildren(errorState(error));
  } finally {
    if (state.searchController === controller) state.searchController = null;
    state.searching = false;
  }
}

function renderResult(item: SearchItem): HTMLElement {
  const button = document.createElement("button");
  button.type = "button";
  button.className = "result-item";
  button.dataset.path = item.path;
  if (item.path === state.selectedPath) button.classList.add("selected");

  const head = document.createElement("span");
  head.className = "result-head";
  const badge = document.createElement("span");
  badge.className = `source-badge source-${item.source}`;
  badge.textContent = scopeLabels[item.source];
  const path = document.createElement("span");
  path.className = "result-path";
  path.textContent = `${item.path} · L${item.line_start}`;
  head.append(badge, path);

  const title = document.createElement("strong");
  title.className = "result-title";
  title.textContent = item.title;
  const snippet = document.createElement("span");
  snippet.className = "result-snippet";
  appendHighlightedText(snippet, item.snippet, state.query);
  button.append(head, title, snippet);
  button.addEventListener("click", () => void openDocument(item));
  return button;
}

function appendHighlightedText(container: HTMLElement, text: string, query: string): void {
  const terms = query
    .split(/[\s,.;:，。；：]+/u)
    .map((term) => term.trim())
    .filter((term) => term.length >= 2)
    .sort((a, b) => b.length - a.length)
    .slice(0, 8);
  if (!terms.length) {
    container.textContent = text;
    return;
  }
  const pattern = new RegExp(`(${terms.map(escapeRegExp).join("|")})`, "giu");
  for (const part of text.split(pattern)) {
    if (!part) continue;
    if (terms.some((term) => term.toLocaleLowerCase() === part.toLocaleLowerCase())) {
      const mark = document.createElement("mark");
      mark.textContent = part;
      container.append(mark);
    } else {
      container.append(document.createTextNode(part));
    }
  }
}

function escapeRegExp(value: string): string {
  return value.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}

async function openDocument(item: SearchItem | { path: string; source?: SearchItem["source"] }): Promise<void> {
  state.documentController?.abort();
  state.selectedPath = item.path;
  writeUrlState();
  markSelectedResult();
  elements.documentPane.classList.add("open");
  elements.mobileDocument.hidden = false;
  elements.documentPath.textContent = item.path;
  elements.documentSource.textContent = scopeLabels[item.source ?? "other"];
  elements.documentContent.replaceChildren(loadingState("正在读取文档"));

  const controller = new AbortController();
  state.documentController = controller;
  try {
    const document = await fetchDocument(item.path, controller.signal);
    renderDocument(document);
  } catch (error) {
    if (controller.signal.aborted) return;
    elements.documentContent.replaceChildren(errorState(error));
  } finally {
    if (state.documentController === controller) state.documentController = null;
  }
}

function renderDocument(documentData: DocumentResponse): void {
  elements.documentPath.textContent = documentData.path;
  elements.documentSource.textContent = scopeLabels[documentData.source];
  marked.setOptions({ gfm: true, breaks: false });
  const rendered = marked.parse(documentData.content, { async: false }) as string;
  const clean = DOMPurify.sanitize(rendered, {
    USE_PROFILES: { html: true },
    FORBID_TAGS: ["style", "iframe", "object", "embed", "form", "input", "button"],
    FORBID_ATTR: ["style", "srcset"],
  });
  const body = document.createElement("div");
  body.className = "markdown-body";
  body.innerHTML = clean;
  for (const link of body.querySelectorAll<HTMLAnchorElement>("a")) {
    link.target = "_blank";
    link.rel = "noopener noreferrer";
  }
  elements.documentContent.replaceChildren(body);
}

function closeDocument(): void {
  state.documentController?.abort();
  state.selectedPath = "";
  writeUrlState();
  markSelectedResult();
  elements.documentPane.classList.remove("open");
  elements.mobileDocument.hidden = true;
  elements.documentPath.textContent = "未选择文档";
  elements.documentSource.textContent = "资料";
  elements.documentContent.replaceChildren(emptyState("book-open", "未选择文档"));
}

function markSelectedResult(): void {
  for (const result of elements.list.querySelectorAll<HTMLElement>(".result-item")) {
    result.classList.toggle("selected", state.selectedPath !== "" && result.dataset.path === state.selectedPath);
  }
}

function iconState(icon: string, label: string, className: string): HTMLElement {
  const wrapper = document.createElement("div");
  wrapper.className = className;
  const iconElement = document.createElement("i");
  iconElement.setAttribute("data-lucide", icon);
  iconElement.setAttribute("aria-hidden", "true");
  const text = document.createElement("p");
  text.textContent = label;
  wrapper.append(iconElement, text);
  queueMicrotask(() => createIcons({ icons: { BookOpen, FileText, LoaderCircle, Search }, attrs: { "stroke-width": 1.8 } }));
  return wrapper;
}

function emptyState(icon: string, label: string): HTMLElement {
  return iconState(icon, label, "empty-state");
}

function loadingState(label: string): HTMLElement {
  return iconState("loader-circle", label, "loading-state");
}

function errorState(error: unknown): HTMLElement {
  const message = error instanceof ApiError || error instanceof Error ? error.message : "请求失败";
  return iconState("file-text", message, "error-state");
}

function submitSearch(resetPage = true): void {
  state.query = elements.input.value.trim();
  if (resetPage) state.page = 1;
  elements.clear.hidden = state.query.length === 0;
  writeUrlState();
  void runSearch();
}

elements.form.addEventListener("submit", (event) => {
  event.preventDefault();
  submitSearch();
});

elements.input.addEventListener("input", () => {
  elements.clear.hidden = elements.input.value.length === 0;
});

elements.clear.addEventListener("click", () => {
  elements.input.value = "";
  closeDocument();
  submitSearch();
  elements.input.focus();
});

elements.tabs.addEventListener("click", (event) => {
  const button = (event.target as HTMLElement).closest<HTMLButtonElement>("button[data-scope]");
  if (!button) return;
  state.scope = button.dataset.scope as SearchScope;
  state.page = 1;
  renderTabs();
  writeUrlState();
  if (state.query) void runSearch();
});

elements.previous.addEventListener("click", () => {
  if (state.page <= 1 || state.searching) return;
  state.page -= 1;
  writeUrlState();
  void runSearch();
  scrollTo({ top: 0, behavior: "smooth" });
});

elements.next.addEventListener("click", () => {
  if (!state.hasMore || state.searching) return;
  state.page += 1;
  writeUrlState();
  void runSearch();
  scrollTo({ top: 0, behavior: "smooth" });
});

elements.documentClose.addEventListener("click", closeDocument);
elements.mobileDocument.addEventListener("click", () => elements.documentPane.classList.add("open"));

window.addEventListener("popstate", () => {
  readUrlState();
  renderTabs();
  void runSearch();
  if (state.selectedPath) void openDocument({ path: state.selectedPath });
  else closeDocument();
});

async function initialize(): Promise<void> {
  readUrlState();
  renderTabs();
  elements.list.replaceChildren(emptyState("search", "尚无检索结果"));
  try {
    const meta = await fetchMeta();
    elements.status.classList.add("online");
    elements.status.replaceChildren(statusDot(), document.createTextNode(`${meta.documents.toLocaleString("zh-CN")} 篇资料`));
  } catch {
    elements.status.classList.add("offline");
    elements.status.replaceChildren(statusDot(), document.createTextNode("服务不可用"));
  }
  if (state.query) await runSearch();
  if (state.selectedPath) await openDocument({ path: state.selectedPath });
}

function statusDot(): HTMLSpanElement {
  const dot = document.createElement("span");
  dot.className = "status-dot";
  return dot;
}

void initialize();
