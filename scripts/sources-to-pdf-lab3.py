#!/usr/bin/env python3

import os
import sys
from pathlib import Path

os.environ.setdefault("WEASYPRINT_DLL_DIRECTORIES", "D:/SDKs/msys64/ucrt64/bin")

from pygments import highlight
from pygments.lexers import CppLexer
from pygments.formatters import HtmlFormatter
from weasyprint import HTML

files = [Path("apps/lab3.cpp")]
modules = sorted(set(p.parent for p in Path("src").rglob("*.hpp")))
for module in modules:
    files.extend(sorted(module.glob("*.hpp")))
    files.extend(sorted(module.glob("*.tpp")))
    files.extend(sorted(module.glob("*.cpp")))

formatter = HtmlFormatter(style="friendly", linenos=True)
css = formatter.get_style_defs(".highlight")
lexer = CppLexer()

sections = []
for f in files:
    code = Path(f).read_text(encoding="utf-8")
    highlighted = highlight(code, lexer, formatter)
    sections.append(
        f'<h2>{f}</h2>\n{highlighted}\n'
        f'<div style="page-break-after: always;"></div>'
    )

html = (
    "<!DOCTYPE html>\n<html><head><style>\n"
    f"{css}\n"
    "@page { margin: 1.5cm 1.5cm 1.5cm 1.5cm; }\n"
    "body { font-family: monospace; font-size: 12px; margin: 0; }\n"
    ".highlight { background: none !important; }\n"
    ".highlight pre { background: none !important; }\n"
    "h1.title { font-family: 'Times New Roman', Times, serif; font-size: 18px; font-weight: bold; text-align: center; }\n"
    "h2 { font-family: 'Times New Roman', Times, serif; font-size: 18px; font-weight: bold; color: #333; border-bottom: 1px solid #ccc; padding-bottom: 0.3em; }\n"
    "</style></head><body>\n"
    '<h1 class="title">Додаток 1. Лістинг коду програми</h1>\n'
    f"{''.join(sections)}\n"
    "</body></html>"
)

output_dir = Path("docs/lab3")
output_dir.mkdir(parents=True, exist_ok=True)
output = sys.argv[1] if len(sys.argv) > 1 else str(output_dir / "Козлов КВ-33 ЛАБ3 Додаток 1.pdf")
HTML(string=html).write_pdf(output)
print("Generated sources PDF in docs/lab3/")
