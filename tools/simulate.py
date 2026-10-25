#!/usr/bin/env python3
"""Run actual C code against modeled EFI/PCI/DMA services on Windows.
Firmware, captured PROM and local profiles are supplied by the caller.
SPDX-License-Identifier: GPL-2.0-only
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "build" / "simulation"
CRT = ROOT / "tools" / "simulation" / "crt"


def tool(name):
    llvm = os.environ.get("LLVM_BIN")
    candidate = Path(llvm) / (name + ".exe") if llvm else None
    if candidate and candidate.is_file():
        return str(candidate)
    found = shutil.which(name)
    if not found:
        raise SystemExit(f"Missing {name}; install LLVM and set LLVM_BIN or PATH")
    return found


def run(command):
    subprocess.run([str(x) for x in command], cwd=ROOT, check=True)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--prom", type=Path)
    p.add_argument("--assets", type=Path)
    p.add_argument("--profile", type=Path)
    args = p.parse_args()
    if os.name != "nt":
        p.error("This minimal host runner currently requires Windows + LLVM")
    if any([args.prom, args.assets, args.profile]) and not all([args.prom, args.assets, args.profile]):
        p.error("Supply --prom, --assets and --profile together for GA104 fixture tests")
    for value in [args.prom, args.assets, args.profile]:
        if value and not value.exists():
            p.error(f"Input does not exist: {value}")
    OUT.mkdir(parents=True, exist_ok=True)
    clang, link, dlltool = tool("clang"), tool("lld-link"), tool("llvm-dlltool")
    flags = ["-target", "x86_64-pc-windows-msvc", "-std=c11", "-O1", "-ffreestanding",
             "-fno-builtin", "-fno-stack-protector", "-mno-stack-arg-probe", "-ffunction-sections",
             "-I", str(CRT), "-I", "deps/edk2/MdePkg/Include", "-I", "deps/edk2/MdePkg/Include/X64"]
    # Only host tests use these declarations/imports; the EFI links no host CRT.
    run([dlltool, "-m", "i386:x86-64", "-d", CRT / "msvcrt.def", "-l", OUT / "msvcrt.lib"])
    run([clang, *flags, "-c", CRT / "entry.c", "-o", OUT / "entry.obj"])

    def suite(name, sources, defines=(), inputs=()):
        objects = []
        for i, source in enumerate(sources):
            obj = OUT / f"{name}-{i}.obj"
            run([clang, *flags, *defines, "-c", source, "-o", obj])
            objects.append(obj)
        exe = OUT / (name + ".exe")
        run([link, "/nodefaultlib", "/entry:HostEntry", "/subsystem:console", "/opt:ref",
             "/stack:8388608,8388608", "/out:" + str(exe), *objects, OUT / "entry.obj", OUT / "msvcrt.lib"])
        result = subprocess.run([str(exe), *map(str, inputs)], cwd=ROOT, capture_output=True, text=True)
        (OUT / (name + ".log")).write_text(result.stdout + result.stderr, encoding="utf-8")
        print(result.stdout + result.stderr)
        result.check_returncode()

    core = ["src/unlock/port.c", "src/unlock/compat.c", "src/unlock/vendor/unlock_610.c",
            "src/unlock/vendor/prep_610.c", "src/unlock/vendor/layout_610.c",
            "src/unlock/vendor/vbios_610.c", "src/unlock/vendor/gfw_eval.c"]
    shared = ["-D__KERNEL__", "-DGU_UEFI_PORT", "-I", "src/unlock/compat"]
    suite("windows", ["src/gop/test_windows.c"])
    suite("master", ["src/unlock/tests/test_master_attributes.c", *core],
          [*shared, "-DGU_TARGET_PCI_ID=0x220610deU"])
    if args.prom:
        specific = [*shared, "-DGU_INDEPENDENT_DISPLAY", "-DGU_TARGET_PCI_ID=0x249c10deU",
                    "-DGU_BOARD_ID=0x249c10deU", "-DGU_BOARD_SUBSYSTEM=0x16021043U",
                    "-DGU_BOARD_CHIP=0xb74000a1U", "-DGU_FB_MIB=0x4000U", "-DGU_BAR_SIZE=0x1000000ULL"]
        suite("ga104-core", ["src/unlock/tests/test_ga104_simulation.c", *core], specific,
              [args.prom.resolve(), args.assets.resolve()])
        production = [*shared, "-DGU_INDEPENDENT_DISPLAY", "-DGU_RUNTIME_PROFILE_BUILD",
                      "-DGU_SSD_EXPERIMENT", "-DGU_GOP_EXPERIMENT", "-DGC_FULL_UNLOCK"]
        suite("ga104-chain", ["src/unlock/tests/test_ga104_boot.c", "src/unlock/app.c",
              "src/unlock/ownership.c", "src/unlock/checks.c", "src/unlock/sha256.c",
              "src/unlock/runtime_profile.c", *core, "src/gop/full.c", "src/gop/reset.c"],
              production, [args.prom.resolve(), args.assets.resolve(), args.profile.resolve()])
    print("Simulation complete. No real GPU services were called. See build/simulation/*.log")


if __name__ == "__main__":
    main()
