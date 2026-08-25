#!/usr/bin/env python3
"""Clean x64-msvc-release outputs while preserving its CMake build tree."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import stat
import subprocess
import sys
import time


SCRIPT_PATH = Path(__file__).resolve()
REPO_ROOT = SCRIPT_PATH.parent.parent
BUILD_ROOT = REPO_ROOT / "build"
TARGET_NAME = "x64-msvc-release"
TARGET = BUILD_ROOT / TARGET_NAME
CACHE_FILENAMES = {
    "mcdk_index_cache.bin",
    "mcdk_sapi_index_cache.bin",
    "mcdk_solutions_cache.bin",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Run the configured build system's clean target and remove generated "
            "MCDK index bins. The build directory and CMake configuration remain."
        )
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="show the clean command and cache files without changing anything",
    )
    return parser.parse_args()


def validate_target() -> None:
    if not (REPO_ROOT / "CMakeLists.txt").is_file():
        raise RuntimeError(f"repository root is invalid: {REPO_ROOT}")
    if TARGET.name != TARGET_NAME or TARGET.parent != BUILD_ROOT:
        raise RuntimeError(f"refusing unexpected target: {TARGET}")
    if TARGET.is_symlink():
        raise RuntimeError(f"refusing a symbolic-link build directory: {TARGET}")

    expected = BUILD_ROOT.resolve(strict=False) / TARGET_NAME
    resolved = TARGET.resolve(strict=False)
    if resolved != expected:
        raise RuntimeError(
            f"refusing target outside the expected build root: {resolved}"
        )


def tree_size(root: Path) -> int:
    total_bytes = 0
    for dirpath, dirnames, filenames in os.walk(root, followlinks=False):
        dirnames[:] = [
            name for name in dirnames if not (Path(dirpath) / name).is_symlink()
        ]
        for filename in filenames:
            try:
                total_bytes += (Path(dirpath) / filename).stat().st_size
            except OSError:
                pass
    return total_bytes


def format_size(size: int) -> str:
    value = float(size)
    for unit in ("B", "KiB", "MiB", "GiB", "TiB"):
        if value < 1024.0 or unit == "TiB":
            return f"{value:.2f} {unit}"
        value /= 1024.0
    raise AssertionError("unreachable")


def find_index_caches() -> list[Path]:
    caches: list[Path] = []
    for dirpath, dirnames, filenames in os.walk(TARGET, followlinks=False):
        dirnames[:] = [
            name for name in dirnames if not (Path(dirpath) / name).is_symlink()
        ]
        for filename in filenames:
            if filename in CACHE_FILENAMES:
                caches.append(Path(dirpath) / filename)
    return sorted(caches)


def is_configured() -> bool:
    has_generator = (TARGET / "build.ninja").is_file() or (TARGET / "Makefile").is_file()
    return (TARGET / "CMakeCache.txt").is_file() and has_generator


def remove_file(path: Path) -> None:
    if path.is_symlink():
        raise RuntimeError(f"refusing to remove symbolic link: {path}")
    try:
        path.unlink()
    except PermissionError:
        path.chmod(stat.S_IWRITE)
        path.unlink()


def clean_target(args: argparse.Namespace) -> int:
    validate_target()
    if not TARGET.is_dir():
        raise RuntimeError(f"configured build directory not found: {TARGET}")

    before_size = tree_size(TARGET)
    cache_paths = find_index_caches()
    clean_command = ["cmake", "--build", str(TARGET), "--target", "clean"]

    print(f"Target: {TARGET}")
    print(f"Current size: {format_size(before_size)}")
    print("Build clean: " + subprocess.list2cmdline(clean_command))
    if cache_paths:
        print("Index caches:")
        for path in cache_paths:
            print(f"  {path.relative_to(REPO_ROOT)} ({format_size(path.stat().st_size)})")
    else:
        print("Index caches: none")

    if args.dry_run:
        print("Dry run: nothing was changed.")
        return 0

    started = time.perf_counter()
    failed = False

    if is_configured():
        try:
            completed = subprocess.run(clean_command, cwd=REPO_ROOT, check=False)
            if completed.returncode != 0:
                print(
                    f"Build clean failed with exit code {completed.returncode}.",
                    file=sys.stderr,
                )
                failed = True
        except OSError as exc:
            print(f"Could not run CMake clean: {exc}", file=sys.stderr)
            failed = True
    else:
        print("CMake build files are missing; skipped the build-system clean.")
        failed = True

    for path in cache_paths:
        if not path.exists():
            continue
        try:
            remove_file(path)
        except (OSError, RuntimeError) as exc:
            print(f"Could not remove cache {path}: {exc}", file=sys.stderr)
            failed = True

    after_size = tree_size(TARGET)
    elapsed = time.perf_counter() - started
    reclaimed = max(0, before_size - after_size)
    print(f"Reclaimed: {format_size(reclaimed)} in {elapsed:.2f}s")
    print(f"Remaining build tree: {format_size(after_size)}")
    print(f"Preserved: {TARGET / 'CMakeCache.txt'}")
    print("Next: cmake --build --preset x64-msvc-release")
    return 1 if failed else 0


def main() -> int:
    try:
        return clean_target(parse_args())
    except (OSError, RuntimeError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
