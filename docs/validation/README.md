# Validation, 2026-10-25

One physical ASUS ROG Strix G533QS, GA104M `10de:249c`, ASUS subsystem `1043:1602`, 16GB. NVIDIA Windows driver 621.01. This is not a general laptop support claim.

RUN05 completed physical U confirmation, FWSEC/V67, SS0 write, one FLR, DMA cleanup and same-boot Windows handoff. SS1 remained 6. Selected evidence:

```text
MANUAL_U confirmed; proceeding with one planned operation
selector write: SS0=0x88888888
SS1 preserved: 0x6 (no write)
PCIe FLR returned: result=0
PATCH_TRANSACTION result=0 containment=0 started=1 quiesced=1 fault=0 retained=0 maps=0 FLR=1 safe=1
PATCH_STATE phase=independent-post-flr SS0=88888888 SS1=00000006 PLM=ffffffff matched=1
FULL_FINAL status=0 touched=1 restored=1
```

The GPU was subsequently visible to Windows and CUDA. [Before](before.txt), [after](after.txt) and [six numerical spot checks](numerical-checks.txt) are included. Current PyTorch was 2.8.0+cu128; the earlier handoff described 2.6.0+cu124, without baseline environment metadata. Software versions are a measurement confounder.

Simulation executed 114 scenario runs: real transaction core with GA104 inputs (20), actual preflight/independent-ownership/transaction/finish chain (29), Windows discovery/load/handoff functions (22), BME protocol/cache containment (29), and physical U-gate logic (14). Categories overlap. Windows handoff was modeled separately, not a real Windows VM boot. GPU firmware responses, PCI, DMA and MMIO were simulated; GPU microcode was not executed by these tests.

The simulation inputs used the local captured PROM, profile, PCI snapshot and five generated firmware assets. PROM/profile/firmware binaries are intentionally excluded from publication. Re-running hardware-specific tests requires compatible local inputs; see TESTING.md. Long-term stability, power-cycle repetitions, sleep/resume and application benchmarks remain untested.
