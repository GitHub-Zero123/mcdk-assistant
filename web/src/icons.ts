// Hand-rolled 24x24 stroke icons. Inlining the handful we actually use keeps
// the bundle small and avoids a second pass to swap placeholders for markup.
const paths = {
  search: ["M11 4a7 7 0 1 0 0 14 7 7 0 0 0 0-14", "M16.2 16.2 21 21"],
  x: ["M6 6l12 12", "M18 6 6 18"],
  copy: [
    "M9 9h9a1 1 0 0 1 1 1v9a1 1 0 0 1-1 1H9a1 1 0 0 1-1-1v-9a1 1 0 0 1 1-1",
    "M6 15H5a1 1 0 0 1-1-1V5a1 1 0 0 1 1-1h9a1 1 0 0 1 1 1v1",
  ],
  check: ["M4.5 12.5 9 17l10.5-10.5"],
  left: ["M14.5 5 8 12l6.5 7"],
  right: ["M9.5 5 16 12l-6.5 7"],
  sun: [
    "M12 7.8a4.2 4.2 0 1 0 0 8.4 4.2 4.2 0 0 0 0-8.4",
    "M12 2.4v2",
    "M12 19.6v2",
    "M2.4 12h2",
    "M19.6 12h2",
    "M5.2 5.2 6.6 6.6",
    "M17.4 17.4l1.4 1.4",
    "M18.8 5.2 17.4 6.6",
    "M6.6 17.4 5.2 18.8",
  ],
  moon: ["M20 14.5A8.4 8.4 0 0 1 9.5 4 8.5 8.5 0 1 0 20 14.5"],
  monitor: ["M4 5.5h16a1 1 0 0 1 1 1v9a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1v-9a1 1 0 0 1 1-1", "M9 20.5h6", "M12 16.5v4"],
  file: ["M13 3H7a1 1 0 0 0-1 1v16a1 1 0 0 0 1 1h10a1 1 0 0 0 1-1V8z", "M13 3v5h5"],
} as const;

export type IconName = keyof typeof paths;

const svgNs = "http://www.w3.org/2000/svg";

export function icon(name: IconName, size = 16): SVGSVGElement {
  const svg = document.createElementNS(svgNs, "svg");
  svg.setAttribute("viewBox", "0 0 24 24");
  svg.setAttribute("width", String(size));
  svg.setAttribute("height", String(size));
  svg.setAttribute("fill", "none");
  svg.setAttribute("stroke", "currentColor");
  svg.setAttribute("stroke-width", "1.6");
  svg.setAttribute("stroke-linecap", "round");
  svg.setAttribute("stroke-linejoin", "round");
  svg.setAttribute("aria-hidden", "true");
  svg.classList.add("icon");
  for (const definition of paths[name]) {
    const path = document.createElementNS(svgNs, "path");
    path.setAttribute("d", definition);
    svg.append(path);
  }
  return svg;
}
