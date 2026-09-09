# EXOTIC Free-Agent Console

Production-oriented web console for the EXOTIC Cognitive Executive / Free-Agent runtime.

## Architecture

```text
Free-Agent Console (React/TypeScript)
          |
          | HTTP JSON
          v
Free-Agent API (C++20, localhost:8421)
          |
          v
FreeAgentService
          |
          +--> durable cognition journal
          |
          v
FreeAgentExecutive -> AttentionScorer -> ActionGate
```

The runtime remains authoritative. The browser does not grant itself external authority and does not fabricate connected state.

## Build the runtime

From the repository root:

```powershell
cmake -S . -B build -DEXOTIC_BUILD_CORE_RUNTIME=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Start the API on Windows:

```powershell
$env:EXOTIC_OPERATOR_TOKEN="change-me-for-local-use"
$env:EXOTIC_FREE_AGENT_JOURNAL=".exotic/free-agent-ledger.tsv"
.\build\CoreRuntime\Release\exotic_free_agent_server.exe
```

Linux/macOS build layouts usually run:

```bash
EXOTIC_OPERATOR_TOKEN="change-me-for-local-use" EXOTIC_FREE_AGENT_JOURNAL=".exotic/free-agent-ledger.tsv" ./build/CoreRuntime/exotic_free_agent_server
```

The API binds only to `127.0.0.1:8421` by default. If `EXOTIC_OPERATOR_TOKEN` is set, all POST mutations require `Authorization: Bearer <token>`. If it is unset, the server prints a warning and local mutation endpoints are unauthenticated.

Thought history is restored from the append-only journal on restart. `EXOTIC_FREE_AGENT_JOURNAL` overrides the default `.exotic/free-agent-ledger.tsv` path.

## Run the console

```powershell
cd Apps/FreeAgentConsole
npm install
Copy-Item .env.example .env
npm run dev
```

Set the same token in `.env`:

```text
VITE_EXOTIC_API_URL=http://localhost:8421
VITE_EXOTIC_OPERATOR_TOKEN=change-me-for-local-use
```

Default UI port: `4173`.

## Runtime API

- `GET /health`
- `GET /version`
- `GET /api/free-agent/state`
- `POST /api/free-agent/thoughts` with `{ "subject": string }`
- `POST /api/free-agent/mode` with `{ "mode": "idle" | "thinking" | "waiting" | "acting" | "stopped" }`

Submitting a thought creates a candidate and passes it through the real `FreeAgentExecutive`; it is not automatically treated as an externally authorized action.

## Quality gates

Console:

```bash
npm run lint
npm run test
npm run build
```

Runtime:

```bash
ctest --test-dir build -C Release --output-on-failure
```

## Current production boundary

This branch establishes the maintained end-to-end application base: typed console, real C++ runtime bridge, cognition service, action-gated executive, local operator authentication, durable restart-safe thought history, and CI coverage. SQLite-backed structured persistence and multi-user identity/session management remain the next infrastructure upgrade before remote deployment.
