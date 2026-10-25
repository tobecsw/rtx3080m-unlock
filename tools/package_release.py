#!/usr/bin/env python3
"""Build source and USB archives without NVIDIA firmware or local captures.
SPDX-License-Identifier: GPL-2.0-only
"""
import argparse
import hashlib
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parent.parent
VERSION = "v0.1.0-experimental"
IGNORED_DIRS = {".git", "build", "node_modules", "__pycache__", ".venv", "captures", "dist"}
IGNORED_EXT = {".bin", ".efi", ".exe", ".dll", ".obj", ".o", ".pdb", ".map", ".zip", ".log", ".pyc"}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--output", type=Path, default=ROOT / "build" / "release")
    args = p.parse_args()
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    efi = ROOT / "build" / "efi" / "BOOTX64.EFI"
    if not efi.is_file():
        p.error("Build first: bun run build")
    prefix = "rog-rtx3080m-unlock-" + VERSION
    source = out / (prefix + "-source.zip")
    usb = out / (prefix + "-usb.zip")
    with zipfile.ZipFile(source, "w", zipfile.ZIP_DEFLATED) as z:
        for file in sorted(ROOT.rglob("*")):
            rel = file.relative_to(ROOT)
            if not file.is_file() or any(x in IGNORED_DIRS for x in rel.parts):
                continue
            if file.suffix.lower() in IGNORED_EXT or file.name.startswith(".env"):
                continue
            z.write(file, Path("rog-rtx3080m-unlock") / rel)
    with zipfile.ZipFile(usb, "w", zipfile.ZIP_DEFLATED) as z:
        z.write(efi, "EFI/BOOT/BOOTX64.EFI")
        z.writestr("RTX-UNLOCK/DEBUG.ON", "")
        for name in ["README.md", "README.zh-CN.md", "LICENSE", "THIRD_PARTY_NOTICES.md",
                     "package.json", "bun.lock", "CHANGELOG.md", "Fetch-Firmware.cmd"]:
            z.write(ROOT / name, name)
        for directory in ["LICENSES", "docs", "benchmarks", "tools/firmware"]:
            for file in sorted((ROOT / directory).rglob("*")):
                if file.is_file() and file.suffix.lower() not in IGNORED_EXT:
                    z.write(file, file.relative_to(ROOT))
        z.write(ROOT / "static" / "UNLOCK.md", "RTX-UNLOCK/UNLOCK.md")
    lines = []
    for file, label in [(source, source.name), (usb, usb.name), (efi, "EFI/BOOT/BOOTX64.EFI")]:
        lines.append(hashlib.sha256(file.read_bytes()).hexdigest() + "  " + label)
    (out / "SHA256SUMS.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
