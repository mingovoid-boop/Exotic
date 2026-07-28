\# Exotic Kernel



\## Objective



The Exotic Kernel owns lifecycle, orchestration, registration, diagnostics, configuration, scheduling, events, and access to core engines.



\## Why



Without a kernel, systems couple directly to each other. With a kernel, Exotic has one stable center that applications, plugins, engines, and future self-building tools can depend on.



\## Alternatives Considered



1\. Service Locator: simple, but hides dependencies.

2\. Dependency Injection: flexible, but can become complex too early.

3\. ECS: excellent for simulations, but not enough for full platform orchestration.

4\. Microservices: powerful later, premature now.



\## Recommendation



Build a lightweight kernel now, then add DI, ECS, plugins, and distributed services later only where they provide clear value.



\## Core Responsibilities



\- Initialize and shut down Exotic.

\- Own Universal State Graph access.

\- Register engines and services.

\- Route events.

\- Provide diagnostics.

\- Support future self-build, self-test, and self-repair workflows.

