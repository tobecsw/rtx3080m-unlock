# Changelog

## v0.1.0-experimental — 2026-10-25

- Published the verified v22 independent-display port for one ASUS G533QS / RTX 3080 Laptop 16GB.
- Enable only PCI memory decoding during probe/profile checks; leave Bus Master off until the transaction.
- Validate an independent display and an unbound compute target instead of desktop full-GOP teardown.
- Obtain exclusive PCI access and recheck ownership/display before reset.
- Write SS0 only, preserve current SS1, require one FLR and verified cleanup before Windows handoff.
- Require the physical U gate even in quiet mode.
- Capture boot selectors only after BAR validation; this fixes the v21 initialization-order failure.
- Add GA104 transaction/chain fixtures, portable Windows host simulation runner, measurements and provenance.

The shipped EFI is the exact successfully tested v22 binary. Packaging and documentation changes do not alter GPU transaction code.
