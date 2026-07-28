import React, { useMemo, useState } from "react";
import { createRoot } from "react-dom/client";
import { Search, Command, Network, Bot, Gauge, ShieldCheck, FolderTree, Cpu, Boxes, Activity, Sparkles, PanelRight, Zap, GitBranch, Database, Settings, Layers, Play } from "lucide-react";
import "./style.css";

const nodes = [
  { name: "Identity", type: "Core", x: 110, y: 80, color: "yellow" },
  { name: "Entity", type: "Universal Object", x: 310, y: 80, color: "blue" },
  { name: "Relationship", type: "Graph Link", x: 530, y: 80, color: "pink" },
  { name: "State", type: "Temporal", x: 760, y: 80, color: "yellow" },
  { name: "Event", type: "Signal", x: 190, y: 235, color: "pink" },
  { name: "Command", type: "Action", x: 420, y: 235, color: "blue" },
  { name: "Automation", type: "Workflow", x: 650, y: 235, color: "yellow" },
  { name: "Knowledge", type: "Memory", x: 880, y: 235, color: "pink" },
  { name: "AI Architect", type: "Assistant", x: 330, y: 390, color: "blue" },
  { name: "Build Doctor", type: "Quality", x: 610, y: 390, color: "yellow" }
];

const edges = [[0,1],[1,2],[2,3],[1,4],[4,5],[5,6],[6,7],[2,8],[8,9],[7,9]];
const commands = ["Create Entity", "Connect Relationship", "Generate Workflow", "Run Build Doctor", "Package Version", "Open Graph", "Inspect State", "Export Docs"];

function App() {
  const [selected, setSelected] = useState(nodes[1]);
  const [density, setDensity] = useState("compact");
  const [palette, setPalette] = useState(false);

  const health = useMemo(() => [["Graph", 96], ["Runtime", 91], ["Docs", 82], ["Security", 88], ["AI Ready", 90]], []);

  return (
    <div className={`studio ${density}`}>
      <header className="topbar">
        <div className="brand"><div className="logo">E</div><div><strong>Exotic Studio</strong><span>v1.1 Universal Laws Runtime</span></div></div>
        <button className="search" onClick={() => setPalette(true)}><Search size={14}/> Search or command <kbd>Ctrl K</kbd></button>
        <div className="density"><button onClick={() => setDensity("compact")}>Compact</button><button onClick={() => setDensity("standard")}>Standard</button></div>
        <div className="status"><ShieldCheck size={14}/> Stable</div>
      </header>

      <main className="grid">
        <aside className="sidebar">
          <Section title="Workspace" icon={<FolderTree/>} items={["Dashboard","Universal Graph","Projects","Domains","Assets","Docs"]}/>
          <Section title="Core" icon={<Cpu/>} items={["Identity","Entity","Component","Relationship","State","Event","Memory"]}/>
          <Section title="Runtime" icon={<Play/>} items={["Commands","Automation","AI","Security","SDK","Packages"]}/>
        </aside>

        <section className="workspace">
          <div className="workspaceHead">
            <div><h1>Universal Object Graph</h1><p>Everything is Identity → Entity → Relationship → State → Event → Command.</p></div>
            <div className="viewButtons"><button>Force</button><button>Tree</button><button>Matrix</button></div>
          </div>

          <div className="canvas">
            <svg className="edges">{edges.map(([a,b],i)=><line key={i} x1={nodes[a].x+58} y1={nodes[a].y+22} x2={nodes[b].x+58} y2={nodes[b].y+22}/>)}</svg>
            {nodes.map((node)=><button key={node.name} className={`node ${node.color} ${selected.name===node.name?"selected":""}`} style={{left:node.x,top:node.y}} onClick={()=>setSelected(node)}><strong>{node.name}</strong><span>{node.type}</span></button>)}
          </div>
        </section>

        <aside className="inspector">
          <div className="panelTitle"><PanelRight size={15}/> Inspector</div>
          <div className="card selectedCard"><small>Selected</small><h2>{selected.name}</h2><p>{selected.type}</p></div>
          <div className="kv"><span>Relationships</span><b>{edges.length}</b><span>Events</span><b>42</b><span>Commands</span><b>{commands.length}</b><span>Health</span><b>94%</b></div>
          <Panel title="AI Architect" icon={<Bot/>} text="Generate systems through the graph, not disconnected files." button="Build from Graph" />
          <div className="card"><h3><Gauge size={14}/> Build Doctor</h3>{health.map(([n,v])=><div className="meter" key={n}><div><span>{n}</span><b>{v}%</b></div><progress value={v} max="100"/></div>)}</div>
          <div className="card"><h3><Command size={14}/> Commands</h3>{commands.map(c=><button className="command" key={c}><Zap size={12}/>{c}</button>)}</div>
        </aside>
      </main>

      <footer className="footer"><span><Activity size={12}/> Runtime Online</span><span><Network size={12}/> 10 Nodes / 10 Links</span><span><Database size={12}/> Memory Ready</span><span><GitBranch size={12}/> v1.1</span><span><Boxes size={12}/> Modular Core</span><span><Settings size={12}/> Compact Mode</span></footer>

      {palette && <div className="overlay" onClick={()=>setPalette(false)}><div className="palette" onClick={e=>e.stopPropagation()}><div className="paletteInput"><Command size={16}/><input autoFocus placeholder="Run command..."/></div>{commands.map(c=><button key={c} onClick={()=>setPalette(false)}><Layers size={13}/>{c}</button>)}</div></div>}
    </div>
  );
}

function Section({title, icon, items}) { return <div className="section"><h3>{React.cloneElement(icon,{size:14})}{title}</h3>{items.map(i=><button key={i}>{i}</button>)}</div>; }
function Panel({title, icon, text, button}) { return <div className="card"><h3>{React.cloneElement(icon,{size:14})}{title}</h3><p>{text}</p><button className="primary"><Sparkles size={13}/>{button}</button></div>; }

createRoot(document.getElementById("root")).render(<App/>);
