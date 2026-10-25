# Testing

## EFI build

`bun run build` compiles the actual EFI with warnings as errors and checks its PE structure. This does not execute firmware or confirm hardware support.

## Windows host simulation

Python 3.9+, LLVM clang/lld-link/llvm-dlltool, Windows 64-bit:

```powershell
python tools/simulate.py
```

Runs 22 Windows discovery/load/handoff cases and 29 BME protocol/cache/containment cases. Minimal host-only C declarations use the Windows system CRT; they are not linked into the EFI image. No Windows SDK or full C runtime development package is required by this runner.

For the tested G533QS/GA104 fixture, provide your compatible local captures and generated assets:

```powershell
python tools/simulate.py --prom captures/PROM.BIN --profile captures/PROFILE.BIN --assets build/firmware-assets
```

Adds 20 actual transaction-core scenarios and 29 actual preflight/independent-display/ownership/transaction/finish scenarios. The fixture pins the tested GPU's observed PCI state, memory geometry and FWSEC layout. It is not a general simulator for arbitrary boards. The profile and PROM are validated; firmware responses remain modeled. No GPU microcode executes and no physical MMIO/DMA/PCI access occurs.

Output: `build/simulation/*.log`. Logs, captures and binaries remain ignored by Git.

## Other inherited tests

`bun run test` retains the upstream suites and requires a complete host C development environment plus Python on PATH. Some desktop-only expectations describe the original dual-selector/GOP design and require adaptation before being treated as coverage of this SS0-only port. The commands above are the supported local simulation route for this publication.

## Reported coverage

The local validation session also exercised 14 extracted production confirmation-gate cases (U/u, Esc, timeout and input/reset/console failures in manual and quiet modes). Those ad hoc extracted harnesses are not presented as automatically reproduced by the runner above. Together with the four suites above, the session had 114 scenario executions, with overlapping coverage. See [validation limits](docs/validation/README.md).
