#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""(JIANT 0.6.46) The packages the live site holds now, for make_site.py's PREV_DIR (the earlier versions the
installer keeps offering). The site lists them in firmware/versions.json; a site from before that has one package,
named in the installer's metadata. Nothing found is no error: the new site then has the current version only.

  web/fetch_prev.py SITE_URL OUT_DIR"""
import json
import re
import sys
import urllib.request
from pathlib import Path

SAFE = re.compile(r"felucca-[A-Za-z0-9.-]+\.fwsc")


def get(url):
    with urllib.request.urlopen(url, timeout=60) as r:
        return r.read()


def main(site, out):
    site, out = site.rstrip("/"), Path(out)
    out.mkdir(parents=True, exist_ok=True)
    try:
        listed = json.loads(get(site + "/firmware/versions.json"))
    except Exception:
        listed = []
        try:                                    # an older site: the installer's one package
            html = get(site + "/webapp/installer/").decode("utf-8", "replace")
            m = re.search(r'"version": "([^"]+)", "product": "[^"]+", "pkg": "\.\./\.\./firmware/(felucca-[^"]+)"', html)
            if m:
                listed = [{"version": m.group(1), "file": m.group(2)}]
        except Exception as e:
            print(f"fetch_prev: nothing from {site}: {e}")
    kept = []
    for e in listed[:12] if isinstance(listed, list) else []:
        f = str(e.get("file", ""))
        if not SAFE.fullmatch(f):
            continue
        try:
            (out / f).write_bytes(get(f"{site}/firmware/{f}"))
            kept.append({"version": str(e.get("version", "")), "file": f})
        except Exception as x:
            print(f"fetch_prev: {f}: {x}")
    (out / "versions.json").write_text(json.dumps(kept) + "\n", encoding="utf-8")
    print(f"fetch_prev: {len(kept)} earlier package(s): {' '.join(e['version'] for e in kept)}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(*sys.argv[1:3])
