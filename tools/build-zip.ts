// SPDX-License-Identifier: GPL-2.0-only
// Public packages deliberately exclude locally generated firmware and profiles.
import { resolve } from "node:path"
import { $ } from "bun"
const root = resolve(import.meta.dir, "..")
const python = process.env.PYTHON ?? "python"
await $`${python} tools/package_release.py`.cwd(root)
