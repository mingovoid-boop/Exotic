# EXOTIC Free-Agent Console

Production-oriented web console for the EXOTIC Cognitive Executive / Free-Agent runtime.

## Goals

- Keep the UI separate from the cognitive runtime.
- Treat runtime state as authoritative; never fake successful connectivity.
- Make cognition, goals, thought ranking, autonomy, and external authority inspectable.
- Preserve a narrow typed API boundary so the C++ runtime can evolve independently.
- Stay lightweight enough to maintain and update quickly.

## Local development

```bash
cd Apps/FreeAgentConsole
npm install
cp .env.example .env
npm run dev
```

Windows PowerShell:

```powershell
cd Apps/FreeAgentConsole
npm install
Copy-Item .env.example .env
npm run dev
```

Default UI port: `4173`.

## Required runtime API

The console expects the runtime base URL from `VITE_EXOTIC_API_URL` and currently targets:

- `GET /api/free-agent/state`
- `POST /api/free-agent/thoughts` with `{ "subject": string }`
- `POST /api/free-agent/mode` with `{ "mode": "idle" | "thinking" | "waiting" | "acting" | "stopped" }`

The browser app does not grant authority itself. Runtime authorization remains server-side.

## Quality gates

```bash
npm run lint
npm run test
npm run build
```

## Production direction

Next backend slice should expose the typed endpoints above from CoreRuntime, persist the thought/goal ledger, add authentication/authorization for operator actions, and publish health/version/capability metadata alongside the existing EXOTIC runtime contracts.
