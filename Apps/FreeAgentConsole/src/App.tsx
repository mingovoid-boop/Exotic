import { FormEvent, useEffect, useMemo, useState } from 'react';
import { exoticApi, FreeAgentState } from './api';

const fallback: FreeAgentState = {
  agent: {
    id: 'free-agent-01',
    name: 'EXOTIC Free-Agent',
    selfDescription: 'Persistent autonomous cognitive agent.',
    avatar: { kind: 'geometric', value: 'hex-eye' },
    mode: 'idle',
    autonomy: 1,
    externalAuthority: 0,
    rewardTotal: 0,
    updatedAt: new Date().toISOString(),
  },
  thoughts: [],
  goals: [],
  beliefs: [],
  preferences: [],
  drives: [],
};

export function App() {
  const [state, setState] = useState<FreeAgentState>(fallback);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [subject, setSubject] = useState('');
  const [name, setName] = useState(fallback.agent.name ?? '');
  const [description, setDescription] = useState(fallback.agent.selfDescription ?? '');
  const [avatar, setAvatar] = useState(fallback.agent.avatar?.value ?? 'hex-eye');

  async function refresh() {
    try {
      setError(null);
      const next = await exoticApi.getState();
      setState(next);
      setName(next.agent.name ?? '');
      setDescription(next.agent.selfDescription ?? '');
      setAvatar(next.agent.avatar?.value ?? 'hex-eye');
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
      await exoticApi.submitThought(clean);
      setSubject('');
      await refresh();
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Thought submission failed');
    }
  }

  async function saveIdentity(event: FormEvent) {
    event.preventDefault();
    try {
      const next = await exoticApi.setIdentity({
        name: name.trim() || 'EXOTIC Free-Agent',
        selfDescription: description.trim(),
        avatarKind: 'geometric',
        avatarValue: avatar.trim() || 'hex-eye',
      });
      setState(next);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Identity update failed');
    }
  }

  async function reward(value: number) {
    try {
      setState(await exoticApi.reward(value, 'operator-feedback', value > 0 ? 'Useful cognition' : 'Unhelpful cognition'));
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Reward update failed');
    }
  }

  return (
    <main className="shell">
      <header className="topbar">
        <div className="identityTitle">
          <div className="avatar" aria-label="Agent avatar">{avatar.slice(0, 2).toUpperCase()}</div>
          <div>
            <p className="eyebrow">EXOTIC / COGNITIVE EXECUTIVE</p>
            <h1>{state.agent.name ?? 'Free-Agent Console'}</h1>
            <p className="muted">{state.agent.selfDescription}</p>
          </div>
        </div>
        <div className={`status ${state.agent.mode}`}><span className="dot" />{loading ? 'connecting' : state.agent.mode}</div>
      </header>

      {error && <section className="alert" role="alert"><strong>Runtime issue.</strong> {error}</section>}

      <section className="metrics">
        <article><span>Autonomy</span><strong>{Math.round(state.agent.autonomy * 100)}%</strong></article>
        <article><span>External authority</span><strong>{Math.round(state.agent.externalAuthority * 100)}%</strong></article>
        <article><span>Reward</span><strong>{(state.agent.rewardTotal ?? 0).toFixed(2)}</strong></article>
        <article><span>Active goals</span><strong>{activeGoals.length}</strong></article>
      </section>

      <section className="grid">
        <article className="panel primary">
          <div className="panelHeader">
            <div><p className="eyebrow">NOW</p><h2>Current cognition</h2></div>
            <button onClick={() => void refresh()}>Refresh</button>
          </div>
          <div className="currentThought">{state.agent.activeThought ?? 'No active thought. Agent may choose to remain idle.'}</div>
          <div className="goalLine"><span>Active goal</span><strong>{state.agent.activeGoal ?? 'Self-selecting'}</strong></div>
        </article>

        <article className="panel">
          <p className="eyebrow">DRIVES</p><h2>Desire system</h2>
          <div className="list">
            {(state.drives ?? []).map((drive) => (
              <div className="drive" key={drive.key}>
                <div className="row"><strong>{drive.key}</strong><span>{Math.round(drive.level * 100)} / {Math.round(drive.target * 100)}</span></div>
                <progress max={1} value={drive.level} />
              </div>
            ))}
            {(state.drives ?? []).length === 0 && <p className="muted">No persistent drives loaded.</p>}
          </div>
          <div className="rewardControls">
            <button onClick={() => void reward(0.25)}>Reward +</button>
            <button onClick={() => void reward(-0.25)}>Reward −</button>
          </div>
        </article>

        <article className="panel">
          <p className="eyebrow">GOALS</p><h2>Objective portfolio</h2>
          <div className="list">
            {activeGoals.map((goal) => <div className="row" key={goal.id}><div><strong>{goal.title}</strong><small>{goal.source}</small></div><span>{goal.priority.toFixed(2)}</span></div>)}
            {activeGoals.length === 0 && <p className="muted">No active goals.</p>}
          </div>
        </article>

        <article className="panel">
          <p className="eyebrow">SELF</p><h2>Identity & avatar</h2>
          <form onSubmit={saveIdentity} className="identityForm">
            <input value={name} onChange={(e) => setName(e.target.value)} placeholder="Agent name" />
            <input value={avatar} onChange={(e) => setAvatar(e.target.value)} placeholder="Avatar symbol / key" />
            <textarea value={description} onChange={(e) => setDescription(e.target.value)} placeholder="Self-description" rows={3} />
            <button type="submit">Save identity</button>
          </form>
        </article>

        <article className="panel wide">
          <div className="panelHeader"><div><p className="eyebrow">ATTENTION</p><h2>Thought ledger</h2></div></div>
          <div className="list">
            {state.thoughts.slice(0, 12).map((thought) => (
              <div className="thought" key={thought.id}><div><strong>{thought.subject}</strong><small>{thought.reason}</small></div><div className="thoughtMeta"><span>{thought.status}</span><b>{thought.score.toFixed(2)}</b></div></div>
            ))}
            {state.thoughts.length === 0 && <p className="muted">No recorded thoughts yet.</p>}
          </div>
        </article>

        <article className="panel wide">
          <p className="eyebrow">BELIEFS</p><h2>Persistent belief state</h2>
          <div className="list">
            {(state.beliefs ?? []).slice(0, 8).map((belief) => <div className="row" key={belief.id}><div><strong>{belief.proposition}</strong><small>{belief.evidence || 'No evidence note'}</small></div><span>{Math.round(belief.confidence * 100)}%</span></div>)}
            {(state.beliefs ?? []).length === 0 && <p className="muted">Belief store is ready; no beliefs recorded yet.</p>}
          </div>
        </article>

        <article className="panel wide">
          <p className="eyebrow">INPUT</p><h2>Offer a thought</h2>
          <p className="muted">Adds a candidate subject. The agent still ranks whether it deserves attention.</p>
          <form onSubmit={submitThought} className="composer">
            <input value={subject} onChange={(event) => setSubject(event.target.value)} placeholder="Something worth considering…" aria-label="Candidate thought" />
            <button type="submit">Submit candidate</button>
          </form>
        </article>
      </section>

      <footer><span>Agent {state.agent.id}</span><span>Updated {new Date(state.agent.updatedAt).toLocaleString()}</span></footer>
    </main>
  );
}
