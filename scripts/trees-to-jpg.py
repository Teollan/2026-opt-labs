#!/usr/bin/env python3
"""Generate syntax tree JPG images for example SIGNAL programs."""

import subprocess
import sys
from pathlib import Path

EXAMPLES = sorted(
    (p for p in Path("examples").glob("*.signal")
     if p.stem.startswith("valid") or p.stem.startswith("syntax-error")),
    key=lambda p: (not p.stem.startswith("valid"), p.stem)
)

LAB2 = "./build/apps/lab2"
DOT = "D:/Programs/graphviz/bin/dot.exe"
OUTPUT_DIR = Path("docs/lab2/trees")


def main():
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    for example in EXAMPLES:
        dot_file = OUTPUT_DIR / f"{example.stem}.dot"
        jpg_file = OUTPUT_DIR / f"{example.stem}.jpg"

        result = subprocess.run(
            [LAB2, "-s", str(example), "-d", str(dot_file)],
            capture_output=True, text=True,
            encoding="utf-8", errors="replace",
            timeout=10
        )

        if not dot_file.exists():
            print(f"SKIP {example.stem} (no dot output)")
            continue

        subprocess.run(
            [DOT, "-Tjpg", "-Gdpi=200", str(dot_file), "-o", str(jpg_file)],
            check=True, timeout=10
        )

        dot_file.unlink()
        print(f"OK   {jpg_file}")

    print(f"\nDone. {len(list(OUTPUT_DIR.glob('*.jpg')))} images in {OUTPUT_DIR}")


if __name__ == "__main__":
    main()
