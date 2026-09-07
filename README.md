# EXOTIC

EXOTIC is an experimental governed software-intelligence platform. The repository is being consolidated around a small verified core before higher-autonomy features are added.

## Current status

**v0.1-dev — prototype. Not production-ready and not an AGI claim.**

The current engineering target is one reproducible end-to-end action:

`objective -> capability lookup -> policy gate -> sandbox/simulation execution -> verification -> commit -> append-only evidence`

## Build

Requirements: CMake 3.20+ and a C++20 compiler.

```bash
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

EXOTIC separates four concerns:

1. **Control** — objectives, policy, budgets, approvals.
2. **Intelligence** — planning and model-backed reasoning.
3. **Execution** — constrained workers and tools.
4. **Verification** — independent checks and evidence before state is accepted.

Intelligence does not automatically imply authority.

## Core v0.1

`CoreRuntime/` currently implements:

- append-only in-process event sequencing;
- explicit capability IDs with authority and risk metadata;
- deny-by-default behavior for unknown capabilities;
- simulation-first policy behavior;
- a deterministic sandbox demonstration capability;
- verification before result commitment;
- CTest coverage of the initial contracts.

This is intentionally small. Persistence, signatures, durable audit storage, approvals, resource budgets, API endpoints, process isolation, model adapters, and production security still need to be implemented and tested.

See `platform.manifest.json` for the machine-readable platform baseline.
