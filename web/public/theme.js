// Applied before first paint so the shell never flashes the wrong theme.
// Kept as a separate file because the server's CSP forbids inline scripts.
try {
  var saved = localStorage.getItem("mcdk-theme");
  if (saved === "light" || saved === "dark") document.documentElement.dataset.theme = saved;
} catch (error) {
  /* Storage can be unavailable; the system preference then applies. */
}
