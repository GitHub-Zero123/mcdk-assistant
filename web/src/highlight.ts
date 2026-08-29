// Only the grammars that actually show up in Bedrock/ModAPI documentation are
// registered — the full highlight.js bundle would dwarf the rest of the app.
import hljs from "highlight.js/lib/core";
import bash from "highlight.js/lib/languages/bash";
import javascript from "highlight.js/lib/languages/javascript";
import json from "highlight.js/lib/languages/json";
import python from "highlight.js/lib/languages/python";
import typescript from "highlight.js/lib/languages/typescript";
import xml from "highlight.js/lib/languages/xml";
import yaml from "highlight.js/lib/languages/yaml";

hljs.registerLanguage("bash", bash);
hljs.registerLanguage("javascript", javascript);
hljs.registerLanguage("json", json);
hljs.registerLanguage("python", python);
hljs.registerLanguage("typescript", typescript);
hljs.registerLanguage("xml", xml);
hljs.registerLanguage("yaml", yaml);

const autoSubset = ["json", "python", "javascript", "typescript", "bash", "xml", "yaml"];

/** Highlighting is O(n) in source length, so oversized blocks stay plain. */
const MAX_BLOCK = 24_000;
const MAX_AUTO_BLOCK = 4_000;

/** Languages where a bare `name(` is a call and `.name` is a member access. */
const callStyleLanguages = new Set(["python", "javascript", "typescript"]);

function languageOf(code: HTMLElement): string | null {
  for (const name of code.classList) {
    if (!name.startsWith("language-")) continue;
    const requested = name.slice("language-".length).toLowerCase();
    const resolved = hljs.getLanguage(requested);
    return resolved ? (resolved.name ?? requested).toLowerCase() : null;
  }
  return null;
}

/**
 * highlight.js only tags declarations, so documentation snippets — which are
 * almost entirely calls on existing objects — come back nearly unstyled. This
 * pass colours call sites and member names in the regions hljs left as plain
 * text, which is exactly where an editor would colour them.
 */
function enrichIdentifiers(code: HTMLElement): void {
  const pattern = /(\.?)([A-Za-z_]\w*)([ \t]*\()?/gu;
  // Only direct text children are untouched by hljs; anything nested is
  // already a string, comment or keyword and must be left alone.
  const plain = [...code.childNodes].filter(
    (node): node is Text => node.nodeType === Node.TEXT_NODE && (node.nodeValue?.length ?? 0) > 0,
  );

  for (const node of plain) {
    const text = node.nodeValue ?? "";
    const fragment = document.createDocumentFragment();
    let cursor = 0;
    let touched = false;

    for (const match of text.matchAll(pattern)) {
      const [whole, dot, name, call] = match;
      if (!name || (!call && !dot)) continue;
      const start = match.index + (dot ? dot.length : 0);
      if (start > cursor) fragment.append(text.slice(cursor, start));
      const span = document.createElement("span");
      span.className = call ? "hljs-title function_" : "hljs-property";
      span.textContent = name;
      fragment.append(span);
      cursor = match.index + whole.length - (call ? call.length : 0);
      touched = true;
    }

    if (!touched) continue;
    if (cursor < text.length) fragment.append(text.slice(cursor));
    node.replaceWith(fragment);
  }
}

function paint(code: HTMLElement): void {
  const source = code.textContent ?? "";
  if (!source.trim() || source.length > MAX_BLOCK) return;

  let language = languageOf(code);
  if (language) {
    code.innerHTML = hljs.highlight(source, { language, ignoreIllegals: true }).value;
  } else if (source.length <= MAX_AUTO_BLOCK) {
    const guess = hljs.highlightAuto(source, autoSubset);
    if (!guess.language || guess.relevance < 5) return;
    language = guess.language;
    code.innerHTML = guess.value;
  } else {
    return;
  }

  code.classList.add("hljs");
  if (callStyleLanguages.has(language)) enrichIdentifiers(code);
}

/**
 * Documents here run to tens of thousands of lines, so blocks are highlighted
 * as they scroll into view rather than all at once on open.
 */
export function highlightLazily(
  root: HTMLElement,
  viewport: HTMLElement,
  afterPaint?: (code: HTMLElement) => void,
): IntersectionObserver | null {
  const blocks = [...root.querySelectorAll<HTMLElement>("pre > code")];
  if (!blocks.length) return null;

  const observer = new IntersectionObserver(
    (entries) => {
      for (const entry of entries) {
        if (!entry.isIntersecting) continue;
        const code = entry.target as HTMLElement;
        observer.unobserve(code);
        paint(code);
        afterPaint?.(code);
      }
    },
    { root: viewport, rootMargin: "600px 0px" },
  );

  for (const block of blocks) observer.observe(block);
  return observer;
}
