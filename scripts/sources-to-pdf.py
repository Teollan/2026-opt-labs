#!/usr/bin/env python3

import sys
import glob
from pathlib import Path

from pygments import highlight
from pygments.lexers import CppLexer
from pygments.formatters import HtmlFormatter
from weasyprint import HTML

files = sorted(Path("apps").glob("*.cpp"))
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
    code = Path(f).read_text()
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
    '<h1 class="title">\u0414\u043e\u0434\u0430\u0442\u043e\u043a 1. \u041b\u0456\u0441\u0442\u0438\u043d\u0433 \u043a\u043e\u0434\u0443 \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u0438</h1>\n'
    f"{''.join(sections)}\n"
    "</body></html>"
)

output = sys.argv[1] if len(sys.argv) > 1 else "docs/Козлов КВ-33 ЛАБ1 Додаток 1.pdf"
HTML(string=html).write_pdf(output)
print(f"Generated {output}")
