export type AgentMode = 'idle' | 'thinking' | 'waiting' | 'acting' | 'stopped';

export interface AgentSnapshot {
  id: string;
  name?: string;
  selfDescription?: string;
  avatar?: { kind: string; value: string };
  mode: AgentMode;
  activeGoal?: string;
  activeThought?: string;
  autonomy: number;
  externalAuthority: number;
  rewardTotal?: number;
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

export interface BeliefRecord {
  id: string;
  proposition: string;
  confidence: number;
  evidence: string;
  updatedAt: string;
}

export interface PreferenceRecord { key: string; weight: number }
export interface DriveRecord { key: string; level: number; target: number; learningRate: number }

export interface FreeAgentState {
  agent: AgentSnapshot;
  thoughts: ThoughtRecord[];
  goals: GoalRecord[];
  beliefs?: BeliefRecord[];
  preferences?: PreferenceRecord[];
  drives?: DriveRecord[];
}

const baseUrl = (import.meta.env.VITE_EXOTIC_API_URL as string | undefined)?.replace(/\/$/, '') ?? '';
const operatorToken = (import.meta.env.VITE_EXOTIC_OPERATOR_TOKEN as string | undefined)?.trim() ?? '';

async function request<T>(path: string, init?: RequestInit): Promise<T> {
  const response = await fetch(`${baseUrl}${path}`, {
    ...init,
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
      ...(operatorToken ? { Authorization: `Bearer ${operatorToken}` } : {}),
      ...(init?.headers ?? {}),
    },
  });
  if (!response.ok) {
    const message = await response.text().catch(() => '');
    throw new Error(`EXOTIC API ${response.status}: ${message || response.statusText}`);
  }
  if (response.status === 204) return undefined as T;
  return response.json() as Promise<T>;
}

export const exoticApi = {
  getState: () => request<FreeAgentState>('/api/free-agent/state'),
  submitThought: (subject: string) => request<ThoughtRecord>('/api/free-agent/thoughts', { method: 'POST', body: JSON.stringify({ subject }) }),
  setMode: (mode: AgentMode) => request<FreeAgentState>('/api/free-agent/mode', { method: 'POST', body: JSON.stringify({ mode }) }),
  setIdentity: (identity: { name: string; selfDescription: string; avatarKind: string; avatarValue: string }) =>
    request<FreeAgentState>('/api/free-agent/identity', { method: 'POST', body: JSON.stringify(identity) }),
  reward: (value: number, source: string, reason: string) =>
    request<FreeAgentState>('/api/free-agent/reward', { method: 'POST', body: JSON.stringify({ value, source, reason }) }),
  idleTick: () => request<ThoughtRecord | { status: 'idle' }>('/api/free-agent/idle-tick', { method: 'POST', body: '{}' }),
};
