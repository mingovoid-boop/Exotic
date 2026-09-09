# EXOTIC Free-Agent Console

Production-oriented web console for the EXOTIC Cognitive Executive / persistent Free-Agent runtime.

## Architecture

```text
Free-Agent Console (React/TypeScript)
          |
          | HTTP JSON
          v
Free-Agent API (C++20, localhost:8421)
          |
          +--> Operator session boundary
          |
          v
FreeAgentService
   |             |
   |             +--> IdleCognitionScheduler
   v
SQLite AgentStore
   |
   +--> identity + avatar
   +--> beliefs
   +--> goals
   +--> preferences
   +--> drives / rewards
   +--> operator sessions
   +--> thought history
   |
   v
FreeAgentExecutive -> AttentionScorer -> ActionGate
```

The runtime remains authoritative. Cognitive autonomy is separate from external authority; the UI cannot bypass `ActionGate`.

## Runtime persistence

The server uses an embedded, pinned SQLite 3.53.4 amalgamation. The default database is:

```text
.exotic/free-agent.db
```

Override it with `EXOTIC_AGENT_DB`. WAL mode, foreign keys, and restart-safe structured tables are enabled. The legacy append-only thought journal remains as an additional audit/recovery trail.

Persistent records include agent identity and avatar metadata, confidence-scored beliefs, goals, preferences, desire/drive state, reward events, operator sessions, and thoughts.

## Build the runtime

```powershell
cmake -S . -B build -DEXOTIC_BUILD_CORE_RUNTIME=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Start on Windows:

```powershell
$env:EXOTIC_OPERATOR_TOKEN="change-me-for-local-use"
$env:EXOTIC_AGENT_DB=".exotic/free-agent.db"
$env:EXOTIC_FREE_AGENT_JOURNAL=".exotic/free-agent-ledger.tsv"
.\build\CoreRuntime\Release\exotic_free_agent_server.exe
```

The API binds only to `127.0.0.1:8421`. A 30-second idle cognition scheduler may autonomously select a useful internal subject when the persisted goals, beliefs, preferences, or drives indicate something deserves attention. That cognition does not grant external execution authority.

## Operator control

`EXOTIC_OPERATOR_TOKEN` is the local bootstrap credential. It can directly authorize local mutations or create a one-hour persisted operator session through `POST /api/operator/session`. Session tokens are stored only as SHA-256 digests and can be revoked/expired independently.

## Console

```powershell
cd Apps/FreeAgentConsole
npm install
Copy-Item .env.example .env
npm run dev
```

The console exposes identity/avatar customization, desire/drive levels, cumulative reward feedback, goals, beliefs, and thought history.

## Runtime API

- `GET /health`
- `GET /version`
- `GET /api/free-agent/state`
- `POST /api/operator/session`
- `POST /api/free-agent/thoughts`
- `POST /api/free-agent/mode`
- `POST /api/free-agent/identity`
- `POST /api/free-agent/reward`
- `POST /api/free-agent/idle-tick`

## Quality gates

```bash
npm run lint
npm run test
npm run build
ctest --test-dir build -C Release --output-on-failure
```

`exotic_agent_store_tests` verifies restart persistence for identity/avatar, beliefs, goals, preferences, drives and reward totals; it also verifies operator-session validation/revocation and idle-cognition candidate generation.

## Production boundary

This is now a persistent local agent runtime, not merely an in-memory cognition demo. Remote deployment should still add TLS termination, stronger account authentication, database migrations/backups, rate limiting, structured audit signatures, and a proper multi-user authorization model before exposing the service outside localhost.
