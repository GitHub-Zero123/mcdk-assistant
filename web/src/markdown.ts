import DOMPurify from "dompurify";
import { marked } from "marked";

export interface Heading {
  id: string;
  level: number;
  text: string;
  element: HTMLElement;
}

marked.setOptions({ gfm: true, breaks: false });

/**
 * A leading `---` block is document metadata, not content. Left in place it
 * parses as a setext heading and shows up as a junk entry in the outline.
 */
export function stripFrontMatter(source: string): string {
  const opening = /^---[ \t]*\r?\n/u.exec(source);
  if (!opening) return source;
  const body = source.slice(opening[0].length);
  const closing = /\r?\n---[ \t]*(?:\r?\n|$)/u.exec(body);
  if (!closing) return source;
  return body.slice(closing.index + closing[0].length).replace(/^\s*\r?\n/u, "");
}

export function renderMarkdown(source: string): HTMLElement {
  const rendered = marked.parse(stripFrontMatter(source), { async: false }) as string;
  const clean = DOMPurify.sanitize(rendered, {
    USE_PROFILES: { html: true },
    FORBID_TAGS: ["style", "iframe", "object", "embed", "form", "input", "button"],
    FORBID_ATTR: ["style", "srcset"],
  });
  const body = document.createElement("div");
  body.className = "prose";
  body.innerHTML = clean;
  for (const link of body.querySelectorAll<HTMLAnchorElement>("a")) {
    link.target = "_blank";
    link.rel = "noopener noreferrer";
  }
  for (const block of body.querySelectorAll<HTMLPreElement>("pre")) {
    block.tabIndex = 0;
  }
  // Most documents reference images that were never copied into the index, so
  // a failed load is the common case, not an exception.
  for (const image of body.querySelectorAll<HTMLImageElement>("img")) {
    image.loading = "lazy";
    image.decoding = "async";
    image.addEventListener("error", () => image.replaceWith(missingImage(image)), { once: true });
  }
  // Wide reference tables get their own scroll container instead of forcing the
  // whole column to squeeze columns down to a single character.
  for (const table of body.querySelectorAll<HTMLTableElement>("table")) {
    const wrap = document.createElement("div");
    wrap.className = "tablewrap";
    wrap.tabIndex = 0;
    table.replaceWith(wrap);
    wrap.append(table);
  }
  return body;
}

/** Extensions that appear in the game asset index, mapped to a grammar. */
const fileLanguages: Record<string, string> = {
  json: "json",
  material: "json",
  mcmeta: "json",
  geo: "json",
  js: "javascript",
  ts: "typescript",
  py: "python",
  yml: "yaml",
  yaml: "yaml",
  xml: "xml",
  html: "xml",
  sh: "bash",
};

export function isMarkdownPath(path: string): boolean {
  return /\.mdx?$/iu.test(path);
}

/**
 * Game assets are data files, not prose. Running them through the markdown
 * parser would mangle them, so they go straight into a highlightable block.
 */
export function renderSourceFile(source: string, path: string): HTMLElement {
  const extension = path.includes(".") ? (path.split(".").pop() ?? "").toLowerCase() : "";
  const language = fileLanguages[extension];

  const body = document.createElement("div");
  body.className = "prose prose--file";
  const block = document.createElement("pre");
  block.tabIndex = 0;
  const code = document.createElement("code");
  if (language) code.className = `language-${language}`;
  code.textContent = source;
  block.append(code);
  body.append(block);
  return body;
}

/** A quiet one-line stand-in, instead of the browser's broken-image glyph. */
function missingImage(image: HTMLImageElement): HTMLElement {
  const box = document.createElement("span");
  box.className = "img-missing";
  box.textContent = image.getAttribute("alt")?.trim() || fileNameOf(image.getAttribute("src")) || "图片";
  box.title = image.getAttribute("src") ?? "";
  return box;
}

function fileNameOf(source: string | null): string {
  if (!source) return "";
  const withoutQuery = source.split(/[?#]/u)[0] ?? "";
  return decodeURIComponent(withoutQuery.split("/").pop() ?? "");
}

export function collectHeadings(body: HTMLElement): Heading[] {
  const used = new Set<string>();
  const headings: Heading[] = [];
  for (const element of body.querySelectorAll<HTMLElement>("h1, h2, h3")) {
    const text = (element.textContent ?? "").trim();
    if (!text) continue;
    const base = slugify(text);
    let id = base;
    for (let suffix = 2; used.has(id); suffix += 1) id = `${base}-${suffix}`;
    used.add(id);
    element.id = id;
    headings.push({ id, level: Number(element.tagName.slice(1)), text, element });
  }
  return headings;
}

function slugify(text: string): string {
  const slug = text
    .toLocaleLowerCase()
    .replace(/\s+/gu, "-")
    .replace(/[^\p{L}\p{N}_-]/gu, "");
  return slug || "section";
}

/** Splits a user query into the terms worth highlighting, longest first. */
export function highlightTerms(query: string): string[] {
  const separators = /[\s,.;:/\\()[\]{}"'`，。；：、！？]+/u;
  return [
    ...new Set(
      query
        .split(separators)
        .map((term) => term.trim().toLocaleLowerCase())
        .filter((term) => term.length >= 2),
    ),
  ]
    .sort((left, right) => right.length - left.length)
    .slice(0, 8);
}

function termPattern(terms: string[]): RegExp {
  return new RegExp(`(${terms.map(escapeRegExp).join("|")})`, "giu");
}

function escapeRegExp(value: string): string {
  return value.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}

/** Appends `text` to `container`, wrapping query matches in <mark>. */
export function appendHighlighted(container: HTMLElement, text: string, terms: string[]): void {
  if (!terms.length) {
    container.textContent = text;
    return;
  }
  for (const part of text.split(termPattern(terms))) {
    if (!part) continue;
    if (terms.includes(part.toLocaleLowerCase())) {
      const mark = document.createElement("mark");
      mark.textContent = part;
      container.append(mark);
    } else {
      container.append(document.createTextNode(part));
    }
  }
}

const skippedTags = new Set(["MARK", "SCRIPT", "STYLE", "A"]);

/** Marks query hits in rendered markdown without re-parsing the sanitized HTML. */
export function highlightInPlace(root: HTMLElement, terms: string[]): void {
  if (!terms.length) return;
  const pattern = termPattern(terms);
  const walker = document.createTreeWalker(root, NodeFilter.SHOW_TEXT, {
    acceptNode(node) {
      const parent = node.parentElement;
      if (!parent || skippedTags.has(parent.tagName)) return NodeFilter.FILTER_REJECT;
      pattern.lastIndex = 0;
      return pattern.test(node.nodeValue ?? "") ? NodeFilter.FILTER_ACCEPT : NodeFilter.FILTER_REJECT;
    },
  });

  const targets: Text[] = [];
  for (let node = walker.nextNode(); node; node = walker.nextNode()) targets.push(node as Text);

  for (const node of targets) {
    const holder = document.createElement("span");
    appendHighlighted(holder, node.nodeValue ?? "", terms);
    const fragment = document.createDocumentFragment();
    fragment.append(...holder.childNodes);
    node.replaceWith(fragment);
  }
}

/** Search snippets arrive as raw markdown; front matter and table pipes are noise. */
export function cleanSnippet(text: string): string {
  return text
    .replace(/^-{3,}\s+.*?\s+-{3,}\s+/u, "")
    .replace(/\|\s*(?::?-{2,}:?\s*\|)+/gu, " ")
    .replace(/^#{1,6}\s+/u, "")
    .replace(/\s*\|\s*/gu, "  ")
    .replace(/\s{2,}/gu, " ")
    .trim();
}
