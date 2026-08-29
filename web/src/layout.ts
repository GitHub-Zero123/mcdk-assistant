// Column widths live in CSS custom properties so dragging never re-lays out
// anything but the grid itself.
const STORAGE_KEY = "mcdk-layout";

interface Pane {
  variable: string;
  min: number;
  max: number;
  fallback: number;
  /** True when the pane sits to the right of its handle, so dragging inverts. */
  trailing?: boolean;
}

const panes: Record<string, Pane> = {
  rail: { variable: "--rail-w", min: 150, max: 340, fallback: 198 },
  results: { variable: "--results-w", min: 280, max: 780, fallback: 400 },
  toc: { variable: "--toc-w", min: 150, max: 420, fallback: 226, trailing: true },
};

type Sizes = Record<string, number>;

function load(): Sizes {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) return {};
    const parsed: unknown = JSON.parse(raw);
    if (!parsed || typeof parsed !== "object") return {};
    const sizes: Sizes = {};
    for (const name of Object.keys(panes)) {
      const value = (parsed as Record<string, unknown>)[name];
      if (typeof value === "number" && Number.isFinite(value)) sizes[name] = value;
    }
    return sizes;
  } catch {
    return {};
  }
}

export function initSplitters(shell: HTMLElement): void {
  // The width the user asked for, kept separate from the width that currently
  // fits. Clamping in place would make a narrow window permanently shrink the
  // panes, with no way back once the window grew again.
  const desired: Sizes = { ...load() };

  function clamp(pane: Pane, value: number): number {
    const width = shell.clientWidth;
    const ceiling = width > 0 ? Math.min(pane.max, Math.max(pane.min, width * 0.5)) : pane.max;
    return Math.round(Math.min(Math.max(value, pane.min), ceiling));
  }

  function apply(name: string, value?: number): void {
    const pane = panes[name];
    if (!pane) return;
    if (value !== undefined) desired[name] = Math.round(value);
    shell.style.setProperty(pane.variable, `${clamp(pane, desired[name] ?? pane.fallback)}px`);
  }

  function persist(): void {
    try {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(desired));
    } catch {
      // Widths are a convenience; losing them is harmless.
    }
  }

  for (const name of Object.keys(panes)) apply(name);

  for (const handle of shell.querySelectorAll<HTMLElement>("[data-resize]")) {
    const name = handle.dataset["resize"];
    const pane = name ? panes[name] : undefined;
    if (!name || !pane) continue;

    handle.addEventListener("pointerdown", (event) => {
      if (event.button !== 0) return;
      event.preventDefault();
      const startX = event.clientX;
      const startWidth = clamp(pane, desired[name] ?? pane.fallback);
      try {
        handle.setPointerCapture(event.pointerId);
      } catch {
        // Capture is an optimisation; the drag still tracks without it.
      }
      handle.classList.add("is-dragging");
      document.body.classList.add("is-resizing");

      const direction = pane.trailing ? -1 : 1;
      const move = (moved: PointerEvent) => apply(name, startWidth + direction * (moved.clientX - startX));
      const stop = () => {
        handle.removeEventListener("pointermove", move);
        handle.classList.remove("is-dragging");
        document.body.classList.remove("is-resizing");
        persist();
      };

      handle.addEventListener("pointermove", move);
      handle.addEventListener("pointerup", stop, { once: true });
      handle.addEventListener("pointercancel", stop, { once: true });
    });

    handle.addEventListener("dblclick", () => {
      apply(name, pane.fallback);
      persist();
    });

    // Keyboard parity for the drag, since the handles are focusable.
    handle.addEventListener("keydown", (event) => {
      const step = event.shiftKey ? 40 : 12;
      const current = clamp(pane, desired[name] ?? pane.fallback);
      const direction = pane.trailing ? -1 : 1;
      if (event.key === "ArrowLeft") apply(name, current - direction * step);
      else if (event.key === "ArrowRight") apply(name, current + direction * step);
      else return;
      event.preventDefault();
      persist();
    });
  }

  // Re-derive from the requested widths so panes recover when the window grows.
  addEventListener("resize", () => {
    for (const name of Object.keys(panes)) apply(name);
  });
}
