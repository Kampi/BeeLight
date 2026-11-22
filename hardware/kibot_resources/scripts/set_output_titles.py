"""Set readable document titles for the generated KiBot results.

KiBot leaves the title of several PDF and HTML files empty, generic or set to a temporary file name.
Browsers show this title in the tab. All titles start with the project name of the KiCad project.
"""
import argparse
import html
import json
import os
import re
from pathlib import Path

from pypdf import PdfReader, PdfWriter

# Longer suffixes first, '-NPTH_drill_map' also ends with 'PTH_drill_map'
PDF_TITLES = (
    ("-assembly", "Assembly"),
    ("-fabrication", "Fabrication"),
    ("-NPTH_drill_map", "NPTH Drill Map"),
    ("-PTH_drill_map", "PTH Drill Map"),
)
TITLE_TAG = re.compile(r"<title[^>]*>(.*?)</title>", re.IGNORECASE | re.DOTALL)


def set_pdf_title(path, project):
    label = next((text for suffix, text in PDF_TITLES if path.stem.endswith(suffix)), None)
    if label is None:
        return False
    title = f"{project} - {label}"
    reader = PdfReader(path)
    if (reader.metadata or {}).get("/Title") == title:
        return False
    writer = PdfWriter(clone_from=reader)
    writer.add_metadata({"/Title": title})
    temp = path.with_name(path.name + ".tmp")
    with open(temp, "wb") as f:
        writer.write(f)
    os.replace(temp, path)
    return True


def html_label(path, current, root):
    if path == root / "index.html":
        return "Results"
    if path.name == "index.html" and path.parent.name == "KiRI":
        return "KiRI"
    if path.stem.endswith("-ibom"):
        return "Interactive BOM"
    if path.stem.endswith("-bom"):
        return "Bill of Materials"
    if "-navigate" in path.stem:
        if current == "Main page":
            return "Results"
        return current.strip("/").replace("/", " / ")
    return None


def set_html_title(path, project, root):
    text = path.read_bytes().decode("utf-8", "surrogateescape")
    match = TITLE_TAG.search(text)
    current = html.unescape(match.group(1)).strip() if match else ""
    if current.startswith(f"{project} - "):
        return False
    label = html_label(path, current, root)
    if label is None:
        return False
    tag = f"<title>{html.escape(f'{project} - {label}')}</title>"
    if match:
        text = text[:match.start()] + tag + text[match.end():]
    else:
        text, count = re.subn(r"<head[^>]*>", lambda m: m.group(0) + tag, text, count=1, flags=re.IGNORECASE)
        if count == 0:
            return False
    path.write_bytes(text.encode("utf-8", "surrogateescape"))
    return True


def main():
    parser = argparse.ArgumentParser(description="Set the document titles of the KiBot results")
    parser.add_argument("-o", "--output-dir", type=Path, required=True, help="Directory with the KiBot results")
    parser.add_argument("-p", "--project-file", type=Path, required=True, help="Path to the .kicad_pro file")
    args = parser.parse_args()

    with open(args.project_file, encoding="utf-8") as f:
        project = json.load(f)["text_variables"]["PROJECT_NAME"]

    pdf_count = html_count = 0
    for path in sorted(p for p in args.output_dir.rglob("*") if p.is_file()):
        suffix = path.suffix.lower()
        if suffix == ".pdf":
            pdf_count += set_pdf_title(path, project)
        elif suffix in (".html", ".htm"):
            html_count += set_html_title(path, project, args.output_dir)
    print(f"Updated titles: {pdf_count} PDF, {html_count} HTML")


if __name__ == "__main__":
    main()
