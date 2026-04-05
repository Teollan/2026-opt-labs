#!/usr/bin/env python3
"""Split a PDF into 10-page chunks for Claude Code reading."""

import sys
from pathlib import Path
import fitz

def split_pdf(pdf_path, output_dir=None):
    pdf_path = Path(pdf_path)
    if output_dir is None:
        output_dir = pdf_path.parent / (pdf_path.stem + "_parts")
    else:
        output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    doc = fitz.open(pdf_path)
    total = doc.page_count
    print(f"{pdf_path.name}: {total} pages")

    for start in range(0, total, 10):
        end = min(start + 10, total)
        new_doc = fitz.open()
        new_doc.insert_pdf(doc, from_page=start, to_page=end - 1)
        out = output_dir / f"{pdf_path.stem}_p{start+1}-{end}.pdf"
        new_doc.save(out)
        new_doc.close()
        print(f"  {out.name}")

    doc.close()
    print(f"Done. {((total - 1) // 10) + 1} chunks in {output_dir}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python pdf-split.py <file.pdf> [output_dir]")
        sys.exit(1)
    split_pdf(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None)
