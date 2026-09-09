import { FormEvent, useEffect, useMemo, useState } from 'react';
import { exoticApi, FreeAgentState } from './api';

const fallback: FreeAgentState = {
  agent: {
    id: 'free-agent-01',
    mode: 'idle',
    autonomy: 1,
    externalAuthority: 0,
    updatedAt: new Date().toISOString(),
  },
  thoughts: [],
  goals: [],
};

export function App() {
  const [state, setState] = useState<FreeAgentState>(fallback);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [subject, setSubject] = useState('');

  async function refresh() {
    try {
      setError(null);
      setState(await exoticApi.getState());
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Unable to reach EXOTIC runtime');
    } finally {
      setLoading(false);
    }
  }

  useEffect(() => {
    void refresh();
    const timer = window.setInterval(() => void refresh(), 5000);
    return () => window.clearInterval(timer);
  }, []);

  const activeGoals = useMemo(
    () => state.goals.filter((goal) => goal.status === 'active').sort((a, b) => b.priority - a.priority),
    [state.goals],
  );

  async function submitThought(event: FormEvent) {
    event.preventDefault();
    const clean = subject.trim();
    if (!clean) return;
    try {
      setError(null);
      await exoticApi.submitThought(clean);
      setSubject('');
      await refresh();
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Thought submission failed');
    }
  }

  return (
    <main className="shell">
      <header className="topbar">
        <div>
          <p className="eyebrow">EXOTIC / COGNITIVE EXECUTIVE</p>
          <h1>Free-Agent Console</h1>
        </div>
        <div className={`status ${state.agent.mode}`}>
          <span className="dot" />
          {loading ? 'connecting' : state.agent.mode}
        </div>
      </header>

      {error && (
        <section className="alert" role="alert">
          <strong>Runtime unavailable.</strong> {error}
        </section>
      )}

      <section className="metrics">
        <article><span>Autonomy</span><strong>{Math.round(state.agent.autonomy * 100)}%</strong></article>
        <article><span>External authority</span><strong>{Math.round(state.agent.externalAuthority * 100)}%</strong></article>
        <article><span>Active goals</span><strong>{activeGoals.length}</strong></article>
        <article><span>Thought queue</span><strong>{state.thoughts.length}</strong></article>
      </section>

      <section className="grid">
        <article className="panel primary">
          <div className="panelHeader">
            <div><p className="eyebrow">NOW</p><h2>Current cognition</h2></div>
            <button onClick={() => void refresh()}>Refresh</button>
          </div>
          <div className="currentThought">
            {state.agent.activeThought ?? 'No active thought. Agent may choose to remain idle.'}
          </div>
          <div className="goalLine">
            <span>Active goal</span>
            <strong>{state.agent.activeGoal ?? 'Self-selecting'}</strong>
          </div>
        </article>

        <article className="panel">
          <p className="eyebrow">GOALS</p>
          <h2>Objective portfolio</h2>
          <div className="list">
            {activeGoals.length === 0 && <p className="muted">No active goals.</p>}
            {activeGoals.map((goal) => (
              <div className="row" key={goal.id}>
                <div><strong>{goal.title}</strong><small>{goal.source}</small></div>
                <span>{goal.priority.toFixed(2)}</span>
              </div>
            ))}
          </div>
        </article>

        <article className="panel wide">
          <div className="panelHeader">
            <div><p className="eyebrow">ATTENTION</p><h2>Thought ledger</h2></div>
          </div>
          <div className="list">
            {state.thoughts.length === 0 && <p className="muted">No recorded thoughts yet.</p>}
            {state.thoughts.slice(0, 12).map((thought) => (
              <div className="thought" key={thought.id}>
                <div><strong>{thought.subject}</strong><small>{thought.reason}</small></div>
                <div className="thoughtMeta"><span>{thought.status}</span><b>{thought.score.toFixed(2)}</b></div>
              </div>
            ))}
          </div>
        </article>

        <article className="panel wide">
          <p className="eyebrow">INPUT</p>
          <h2>Offer a thought</h2>
          <p className="muted">This adds a candidate subject. The agent remains free to rank it below other concerns.</p>
          <form onSubmit={submitThought} className="composer">
            <input value={subject} onChange={(event) => setSubject(event.target.value)} placeholder="Something worth considering…" aria-label="Candidate thought" />
            <button type="submit">Submit candidate</button>
          </form>
        </article>
      </section>

      <footer>
        <span>Agent {state.agent.id}</span>
        <span>Updated {new Date(state.agent.updatedAt).toLocaleString()}</span>
      </footer>
    </main>
  );
}
