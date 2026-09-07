# EXOTIC Core v0.1 Architecture

## Invariant

No result becomes accepted system state merely because an intelligence component proposed or produced it.

## Flow

1. Receive objective.
2. Resolve an explicit capability ID.
3. Evaluate policy against capability authority and risk.
4. Execute only within the capability's allowed boundary.
5. Emit evidence as ordered events.
6. Verify the result independently from execution.
7. Commit only verified results.

## Planes

### Control
Owns objective admission, policy, approval, budgets and emergency stops.

### Intelligence
Produces plans, predictions and candidate actions. It has no implicit authority.

### Execution
Runs constrained capabilities. External side effects are disabled by default in v0.1.

### Verification
Evaluates evidence and acceptance criteria. Failed verification prevents commit.

## Event rules

The in-process v0.1 store is append-only through its public interface and assigns monotonic sequence numbers. It is not yet durable or tamper-evident. Durable storage, hashes/signatures, correlation IDs and recovery are required before production use.

## Capability rules

Capabilities are identified by stable IDs and carry authority, risk and enabled state. Unknown capabilities are denied. High-risk actions require an approval mechanism that is not yet implemented.

## Next required increments

- durable SQLite event and state store;
- capability manifest loading and schema validation;
- actor identity and authorization grants;
- approval records and expiry/revocation;
- resource budgets and reservations;
- isolated worker process boundary;
- verifier interfaces and multiple verification strategies;
- HTTP health/version/capabilities/events read API;
- crash recovery and idempotency;
- signed/tamper-evident audit chain;
- adversarial and failure-injection tests.
