export type AgentMode = 'idle' | 'thinking' | 'waiting' | 'acting' | 'stopped';

export interface AgentSnapshot {
  id: string;
  mode: AgentMode;
  activeGoal?: string;
  activeThought?: string;
  autonomy: number;
  externalAuthority: number;
  updatedAt: string;
}

export interface ThoughtRecord {
  id: string;
  subject: string;
  score: number;
  reason: string;
  status: 'queued' | 'selected' | 'completed' | 'abandoned';
  createdAt: string;
}

export interface GoalRecord {
  id: string;
  title: string;
  priority: number;
  source: 'user' | 'self' | 'system';
  status: 'active' | 'paused' | 'completed' | 'abandoned';
}

export interface FreeAgentState {
  agent: AgentSnapshot;
  thoughts: ThoughtRecord[];
  goals: GoalRecord[];
}

const baseUrl = (import.meta.env.VITE_EXOTIC_API_URL as string | undefined)?.replace(/\/$/, '') ?? '';

async function request<T>(path: string, init?: RequestInit): Promise<T> {
  const response = await fetch(`${baseUrl}${path}`, {
    ...init,
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
      ...(init?.headers ?? {}),
    },
  });

  if (!response.ok) {
    const message = await response.text().catch(() => '');
    throw new Error(`EXOTIC API ${response.status}: ${message || response.statusText}`);
  }

  return response.json() as Promise<T>;
}

export const exoticApi = {
  getState: () => request<FreeAgentState>('/api/free-agent/state'),
  submitThought: (subject: string) =>
    request<ThoughtRecord>('/api/free-agent/thoughts', {
      method: 'POST',
      body: JSON.stringify({ subject }),
    }),
  setMode: (mode: AgentMode) =>
    request<AgentSnapshot>('/api/free-agent/mode', {
      method: 'POST',
      body: JSON.stringify({ mode }),
    }),
};
