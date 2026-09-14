# EXOTIC

EXOTIC is an experimental governed software-intelligence platform. The repository is being consolidated around a small verified core before higher-autonomy features are added.

## Current status

**v0.2.0-dev — prototype. Not production-ready and not an AGI claim.**

The current engineering target is one reproducible governed action:

`objective -> capability lookup -> identity/policy gate -> sandbox/simulation execution -> independent verification -> commit -> append-only evidence`

The Core v0.2 evidence release is intentionally narrower than the full EXOTIC roadmap. New platform families do not outrank this vertical slice.

## Build

Requirements: CMake 3.20+, a C++20 compiler, and Node.js for the architecture/manifest release gate.

```bash
node tooling/validate-architecture.mjs
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the core demonstration after building:

```bash
./build/CoreRuntime/exotic_core_demo
```

On multi-config generators such as Visual Studio, the executable may be under a configuration directory such as `Debug` or `Release`.

## Architecture rule

EXOTIC separates five operational planes:

1. **Control** — identity, policy, authority, budgets and desired state.
2. **Intelligence** — planning, bounded cognition and model-backed reasoning; no implicit authority.
3. **Execution** — constrained capabilities, workers and tools.
4. **Evidence** — append-only events, verification results and future logs/traces/artifacts.
5. **Experience** — APIs, Studio and Operations surfaces that project durable platform truth.

Verification is a mandatory gate between execution and accepted state. Intelligence does not automatically imply authority.

## Core v0.2 implemented behavior

`CoreRuntime/` currently implements:

- explicit capability IDs with authority, risk and enabled state;
- identity registration and capability grants;
- deny-by-default behavior for unknown or ungranted capabilities;
- simulation-first policy behavior;
- deterministic sandbox capabilities;
- independent verification before result commitment;
- append-only event sequencing with persistent journal reload;
- SHA-256 event hash chaining and tamper rejection;
- restart recovery of the event journal;
- bounded Free-Agent cognition whose external action path requires explicit authorization;
- CTest coverage plus Windows and Ubuntu CI;
- a machine-readable `platform.manifest.json` and architecture validator.

## Deliberate limits

The following are **not yet production-verified**:

- fsync/transaction-backed power-loss durability;
- durable materialized state and migrations;
- approval records, expiry/revocation and resource budgets;
- isolated worker processes and idempotent durable Operations;
- production authentication, secrets, TLS and remote deployment;
- `/health`, `/version`, `/capabilities`, `/events` and sanitized `/public/status` HTTP endpoints;
- signed evidence and complete logs/metrics/traces/artifact correlation.

`platform.manifest.json` marks these endpoint contracts as `Proposed` until executable evidence exists.

## Journal compatibility

The v0.2 evidence chain uses SHA-256. Journals created by earlier prototype builds that used implementation-defined `std::hash` checksums are intentionally not accepted as v0.2 evidence. Archive or discard prototype journals before upgrading; a future migration tool may preserve them as explicitly legacy evidence, but v0.2 will not silently promote weak legacy hashes.

See `platform.manifest.json` and `CoreRuntime/ARCHITECTURE.md` for the canonical machine-readable and technical baselines.
