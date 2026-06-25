# tree-sitter-typescript

Vendored subset of `tree-sitter/tree-sitter-typescript`.

Upstream: https://github.com/tree-sitter/tree-sitter-typescript
License: MIT, see `LICENSE`.

This copy keeps only the TypeScript grammar files required by the C++ build:

- `typescript/src/parser.c`
- `typescript/src/scanner.c`
- `typescript/src/grammar.json`
- `typescript/src/node-types.json`
- `typescript/src/tree_sitter/*.h`
- `bindings/c/tree-sitter-typescript.h`

TSX grammar, language bindings, examples, tests, package manager files, and
grammar-generation helpers are intentionally omitted.
