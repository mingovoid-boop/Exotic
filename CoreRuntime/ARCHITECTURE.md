# EXOTIC Core v0.2 Architecture

## Invariant

No result becomes accepted system state merely because an intelligence component proposed or produced it.

## Governed action flow

1. Receive an objective.
2. Resolve an explicit capability ID.
3. Resolve the actor identity and capability grant.
4. Evaluate policy against capability authority, risk and execution mode.
5. Execute only within the capability's allowed boundary.
6. Emit ordered evidence events.
7. Verify the result independently from execution.
8. Commit only verified results.

## Operational planes

### Control
Owns identity, grants, policy, approvals, budgets, desired state and emergency stops. Current v0.2 implements identity grants plus a bounded policy gate; approvals, durable budgets and emergency controls remain future work.

### Intelligence
Produces plans, predictions and bounded cognitive choices. It has no implicit authority. The current Free-Agent executive may Think or Stop autonomously, but an external Act decision requires an explicit grant.

### Execution
Runs constrained capabilities. External side effects are disabled by default in the current evidence release; sandbox/simulation behavior is the canonical execution mode.

### Evidence
Owns append-only causal evidence and verification records. Current v0.2 persists an ordered journal, reloads it after restart, chains records with SHA-256, and rejects chain corruption. Future evidence work includes schema versions, correlation/causation IDs, artifacts, logs, metrics, traces and signed provenance.

### Experience
Owns APIs, Studio and Operations projections. Experience surfaces must not become the only source of authoritative state. Core v0.2 implements a localhost-only read API. Raw evidence routes remain internal, while `/public/status` is a deliberately sanitized projection intended for exact reverse-proxy publication later.

## Verification gate

Verification is not a separate authority plane. It is a mandatory gate between Execution and accepted state. A successful execution does not imply commitment; a failed verification prevents `state.committed`.

## Event rules

The v0.2 event journal:

- is append-only through its public interface;
- assigns monotonically increasing sequence numbers;
- records each event's previous hash;
- computes event digests with SHA-256;
- reloads persisted events after process restart;
- validates sequence, previous-hash linkage and digest integrity before accepting a journal.

This proves application-process restart recovery and tamper detection for the stored chain. It does **not** yet prove transaction-atomic or fsync-backed durability across kernel crash, filesystem failure or power loss.

Prototype journals created before the SHA-256 migration used implementation-defined `std::hash` values and are intentionally not accepted as v0.2 evidence.

## Capability and authority rules

Capabilities are identified by stable IDs and carry authority, risk and enabled state. Unknown capabilities are denied. Actors must hold an explicit capability grant. High-risk actions remain denied until an approval mechanism is implemented. Intelligence components cannot grant themselves authority.

## Current canonical capabilities

- `core.observe` — read, low risk.
- `core.plan` — no side-effect authority, low risk.
- `core.execute.echo` — sandbox authority, low risk.

The runtime also contains implemented identity, policy, verification, event-evidence, read-only status, and bounded Free-Agent features. These are recorded separately from action capabilities in `platform.manifest.json` so the registry does not pretend to expose actions that do not exist.

## HTTP trust boundary

The status executable binds only to `127.0.0.1`.

Internal routes:

- `GET /health`
- `GET /version`
- `GET /capabilities`
- `GET /events`

Sanitized projection:

- `GET /public/status`

`/public/status` contains only platform/version, runtime readiness, integrity result, event count, capability count, last sequence, and last event time. It intentionally excludes actors, subjects, payloads, event hashes, previous hashes, grants, and full capability metadata.

The phrase **public status** describes the projection's allowed schema, not direct network exposure. Remote binding remains disabled in v0.2. A future reverse proxy may expose only this route after TLS and deployment verification.

## Release gates

A Core v0.2 candidate must satisfy all applicable gates before promotion:

1. `tooling/validate-architecture.mjs` passes.
2. The manifest version and maturity labels are internally consistent.
3. Windows and Ubuntu CMake builds succeed.
4. CTest discovers at least one test and all tests pass.
5. SHA-256 known-answer vectors pass.
6. The journal survives process restart and preserves its chain.
7. A deliberately corrupted journal is rejected.
8. Unauthorized capability execution is denied.
9. The loopback HTTP server responds successfully on all five routes.
10. `/public/status` passes a raw-evidence field denylist in both unit and network smoke tests.

## Next required increments

In dependency order unless repository evidence proves an equivalent foundation already exists:

1. durable SQLite event/state transactions, migrations, projections and checkpoints;
2. approval records with expiry/revocation and durable resource budgets;
3. idempotent durable Operation records and isolated worker boundary;
4. correlation/causation IDs plus artifact references;
5. OpenTelemetry-compatible logs, metrics and traces;
6. signed evidence and stronger provenance controls;
7. exact reverse-proxy publication of `/public/status` and `mingo.center` integration;
8. failure-injection, concurrency, repeated-restart and hard-crash tests.
