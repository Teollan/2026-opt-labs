#!/usr/bin/env python3

import sys
import subprocess
from pathlib import Path
from html import escape

from pygments import highlight
from pygments.lexers import get_lexer_by_name
from pygments.formatters import HtmlFormatter
from weasyprint import HTML

examples = sorted(Path("examples").glob("*.signal"))

formatter = HtmlFormatter(style="friendly", linenos=True)
css = formatter.get_style_defs(".highlight")
signal_lexer = get_lexer_by_name("text")

sections = []
for example in examples:
    name = example.stem.replace("-", " ").title()
    source = Path(example).read_text()
    highlighted_source = highlight(source, signal_lexer, formatter)

    result = subprocess.run(
        ["./build/apps/lab1", str(example)],
        capture_output=True, text=True
    )
    output = result.stdout + result.stderr

    sections.append(
        f'<h2>{name}</h2>\n'
        f'<h3>{example}</h3>\n'
        f'{highlighted_source}\n'
        f'<h3>Output</h3>\n'
        f'<pre class="output">{escape(output)}</pre>\n'
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
    "h3 { font-family: 'Times New Roman', Times, serif; font-size: 14px; font-weight: bold; color: #555; margin-top: 1em; }\n"
    "pre.output { font-family: monospace; font-size: 11px; padding: 0.5em; border: 1px solid #ccc; white-space: pre-wrap; }\n"
    "</style></head><body>\n"
    '<h1 class="title">\u0414\u043e\u0434\u0430\u0442\u043e\u043a 2. \u0420\u0435\u0437\u0443\u043b\u044c\u0442\u0430\u0442\u0438 \u0442\u0435\u0441\u0442\u0443\u0432\u0430\u043d\u043d\u044f</h1>\n'
    f"{''.join(sections)}\n"
    "</body></html>"
)

output = sys.argv[1] if len(sys.argv) > 1 else "docs/Козлов КВ-33 ЛАБ1 Додаток 2.pdf"
HTML(string=html).write_pdf(output)
print(f"Generated {output}")
