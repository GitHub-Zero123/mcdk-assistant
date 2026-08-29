import { icon } from "./icons";
import type { Hit } from "./api";

const MAX_QUERY = 64;
const MIN_QUERY = 2;
const PANEL_WIDTH = 380;
const ANCHOR_GAP = 8;
const MARGIN = 8;

export interface QuickFindOptions {
  /** Elements whose selections offer a quick search. */
  roots: HTMLElement[];
  search: (query: string, signal: AbortSignal) => Promise<Hit[]>;
  renderHit: (hit: Hit, index: number, terms: string[], onOpen: () => void) => HTMLElement;
  terms: (query: string) => string[];
  onOpen: (hit: Hit) => void;
}

export interface QuickFind {
  /** True while the panel owns the keyboard, so the app defers to it. */
  readonly open: boolean;
  close(): void;
}

/**
 * Selecting text anywhere in the reader offers a fuzzy lookup in a floating
 * panel. It is deliberately independent of the main search box: it never
 * touches the query, the scope, the URL or the result list.
 */
export function initQuickFind(options: QuickFindOptions): QuickFind {
  const chip = document.createElement("button");
  chip.type = "button";
  chip.className = "qf-chip";
  chip.hidden = true;

  const panel = document.createElement("div");
  panel.className = "qf-panel";
  panel.hidden = true;

  const bar = document.createElement("div");
  bar.className = "qf-panel__bar";
  const queryLabel = document.createElement("span");
  queryLabel.className = "qf-panel__query";
  const close = document.createElement("button");
  close.type = "button";
  close.className = "ghost qf-panel__close";
  close.title = "关闭 (Esc)";
  close.setAttribute("aria-label", "关闭");
  close.append(icon("x", 14));
  bar.append(queryLabel, close);

  const list = document.createElement("div");
  list.className = "qf-panel__list";
  panel.append(bar, list);

  document.body.append(chip, panel);

  let query = "";
  let anchor = { x: 0, y: 0 };
  /** Kept live so the floating UI can follow the text as the reader scrolls. */
  let anchorRange: Range | null = null;
  let hits: Hit[] = [];
  let active = -1;
  let controller: AbortController | null = null;
  let pending = 0;
  let dragging = false;

  function hideChip(): void {
    chip.hidden = true;
  }

  function closePanel(): void {
    controller?.abort();
    controller = null;
    panel.hidden = true;
    hits = [];
    active = -1;
    list.replaceChildren();
  }

  function dismiss(): void {
    closePanel();
    hideChip();
    anchorRange = null;
  }

  /**
   * Anchors `element` to the selection, keeping it fully on screen. Measured
   * after the element is visible, since a hidden element has no box.
   */
  function place(element: HTMLElement): void {
    const box = element.getBoundingClientRect();
    const width = box.width || element.offsetWidth;
    const height = box.height || element.offsetHeight;

    const maxLeft = Math.max(innerWidth - width - MARGIN, MARGIN);
    const left = Math.min(Math.max(anchor.x, MARGIN), maxLeft);

    // Flip above the selection when there is no room beneath it.
    const above = anchor.y - height - ANCHOR_GAP * 2;
    const fitsBelow = anchor.y + height + MARGIN <= innerHeight;
    const maxTop = Math.max(innerHeight - height - MARGIN, MARGIN);
    const top = Math.min(Math.max(fitsBelow ? anchor.y : above, MARGIN), maxTop);

    element.style.left = `${Math.round(left)}px`;
    element.style.top = `${Math.round(top)}px`;
  }

  /** Recomputes the anchor point; false when the text is gone or off screen. */
  function reanchor(): boolean {
    if (!anchorRange || !anchorRange.commonAncestorContainer.isConnected) return false;
    const rect = anchorRange.getBoundingClientRect();
    if (!rect.width && !rect.height) return false;
    if (rect.bottom < 0 || rect.top > innerHeight || rect.right < 0 || rect.left > innerWidth) return false;
    anchor = { x: rect.left, y: rect.bottom + ANCHOR_GAP };
    return true;
  }

  /**
   * Run straight from the scroll handler rather than on a frame callback:
   * measuring two elements is cheap, and layout is already current here.
   */
  function follow(): void {
    if (chip.hidden && panel.hidden) return;
    // Scrolling the selection out of view is the only thing that dismisses;
    // otherwise the floating UI simply travels with the text.
    if (!reanchor()) {
      dismiss();
      return;
    }
    if (!chip.hidden) place(chip);
    if (!panel.hidden) place(panel);
  }

  function markActive(): void {
    for (const row of list.children) {
      row.classList.toggle("is-active", Number((row as HTMLElement).dataset["index"]) === active);
    }
    list.querySelector<HTMLElement>(`[data-index="${active}"]`)?.scrollIntoView({ block: "nearest" });
  }

  function move(delta: number): void {
    if (!hits.length) return;
    active = active < 0 ? (delta > 0 ? 0 : hits.length - 1) : active + delta;
    active = Math.min(Math.max(active, 0), hits.length - 1);
    markActive();
  }

  function status(text: string): HTMLElement {
    const box = document.createElement("p");
    box.className = "qf-panel__status";
    box.textContent = text;
    return box;
  }

  async function run(): Promise<void> {
    controller?.abort();
    const own = ++pending;
    const signal = new AbortController();
    controller = signal;

    hits = [];
    active = -1;
    list.replaceChildren(status("检索中…"));

    try {
      const found = await options.search(query, signal.signal);
      if (own !== pending) return;
      hits = found;
      if (!found.length) {
        list.replaceChildren(status("没有匹配的资料"));
        return;
      }
      const terms = options.terms(query);
      list.replaceChildren(
        ...found.map((hit, index) =>
          options.renderHit(hit, index, terms, () => {
            options.onOpen(hit);
            dismiss();
          }),
        ),
      );
      list.scrollTop = 0;
      place(panel);
    } catch (error) {
      if (signal.signal.aborted || own !== pending) return;
      list.replaceChildren(status(error instanceof Error ? error.message : "检索失败"));
    }
  }

  function openPanel(): void {
    if (query.length < MIN_QUERY) return;
    hideChip();
    queryLabel.textContent = query;
    queryLabel.title = query;
    panel.hidden = false;
    panel.style.width = `${Math.min(PANEL_WIDTH, innerWidth - MARGIN * 2)}px`;
    place(panel);
    void run();
  }

  chip.addEventListener("mousedown", (event) => event.preventDefault());
  chip.addEventListener("click", openPanel);
  close.addEventListener("click", dismiss);

  function readSelection(): string {
    const selection = getSelection();
    if (!selection || selection.isCollapsed || selection.rangeCount === 0) return "";
    const range = selection.getRangeAt(0);
    const container =
      range.commonAncestorContainer instanceof Element
        ? range.commonAncestorContainer
        : range.commonAncestorContainer.parentElement;
    if (!container || !options.roots.some((root) => root.contains(container))) return "";

    // range.toString() is the document text; Selection.toString() is the
    // *rendered* text, which can come back empty in a backgrounded tab.
    const text = range.toString().replace(/\s+/gu, " ").trim();
    if (text.length < MIN_QUERY) return "";

    const rect = range.getBoundingClientRect();
    if (!rect.width && !rect.height) return "";
    anchorRange = range.cloneRange();
    anchor = { x: rect.left, y: rect.bottom + ANCHOR_GAP };
    return text.slice(0, MAX_QUERY);
  }

  function offerSelection(): void {
    if (!panel.hidden) return;
    const found = readSelection();
    if (!found) {
      hideChip();
      return;
    }
    query = found;
    chip.replaceChildren(icon("search", 13), chipLabel(found));
    chip.hidden = false;
    place(chip);
  }

  let debounce = 0;
  document.addEventListener("selectionchange", () => {
    clearTimeout(debounce);
    // Waiting for the drag to finish keeps the chip from chasing the cursor.
    if (dragging) return;
    debounce = window.setTimeout(offerSelection, 140);
  });

  document.addEventListener("pointerdown", (event) => {
    const target = event.target as Node;
    if (chip.contains(target) || panel.contains(target)) return;
    dragging = true;
    if (!panel.hidden) closePanel();
    hideChip();
  });

  function endDrag(): void {
    if (!dragging) return;
    dragging = false;
    clearTimeout(debounce);
    debounce = window.setTimeout(offerSelection, 40);
  }

  document.addEventListener("pointerup", endDrag);
  // A drag released outside the window would otherwise leave the flag stuck on.
  document.addEventListener("pointercancel", endDrag);
  addEventListener("blur", endDrag);

  document.addEventListener(
    "scroll",
    (event) => {
      if (panel.contains(event.target as Node)) return;
      follow();
    },
    { capture: true, passive: true },
  );

  addEventListener("resize", follow);

  document.addEventListener(
    "keydown",
    (event) => {
      // Alt+Enter is the keyboard route from a selection straight to results.
      if (event.altKey && event.key === "Enter" && panel.hidden) {
        const found = readSelection();
        if (!found) return;
        event.preventDefault();
        query = found;
        openPanel();
        return;
      }
      if (panel.hidden) return;

      if (event.key === "Escape") {
        event.preventDefault();
        event.stopPropagation();
        dismiss();
        return;
      }
      if (event.key === "ArrowDown" || event.key === "ArrowUp") {
        event.preventDefault();
        event.stopPropagation();
        move(event.key === "ArrowDown" ? 1 : -1);
        return;
      }
      if (event.key === "Enter" && active >= 0) {
        const hit = hits[active];
        if (!hit) return;
        event.preventDefault();
        event.stopPropagation();
        options.onOpen(hit);
        dismiss();
      }
    },
    true,
  );

  return {
    get open() {
      return !panel.hidden;
    },
    close: dismiss,
  };
}

function chipLabel(text: string): HTMLElement {
  const label = document.createElement("span");
  label.textContent = text.length > 22 ? `${text.slice(0, 22)}…` : text;
  return label;
}
