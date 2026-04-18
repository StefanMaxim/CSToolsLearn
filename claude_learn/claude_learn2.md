# Agentic Development:

cannot prompt and say "build my app"
instead, follow structured process, one step at a time

like with junior devs: give one small task at a time, review them, and iterate

## Buzzwords:

PRD: Product Requirements Document
what features you want,
how they shoudl behave
examples of usage (should be in idea.md)


MCPs: these are the tools the AI can use
git: manages changes in your workspace
context tools: read your codebase
task tools: create task lists


TDD: test-driven development
write a test first, what shoudl happen

Linter: tests code style and correctness (idea is to allways test what the AI outputs)

Taskmaster: a tool that breaks down tasks into smaller pieces





### WHat learned from playing around:

apparently on Codex have have things called threads, or mutliple agents that can work on a codebase at the same time
using 
/agent to see all agents and switch actively displayed one

/



Ask Codex to read SPEC.md and produce the implementation plan only.
Review that plan.
Ask it to implement Step 1 only.
Test.
Ask it to implement Step 2 only.
Repeat.

exe:
Read SPEC.md and treat it as the source of truth.

Implement the project in milestones, not all at once.

Start with Milestone 1 only:

* create index.html, style.css, script.js, and README.md
* scaffold the canvas, HUD, overlays, and config structure
* wire basic DOM initialization
* do not implement gameplay yet

Constraints:

* vanilla HTML/CSS/JS only
* HTML5 Canvas for rendering
* no external assets
* all tunable values must be grouped in a top-level CONFIG section
* all paths must work on GitHub Pages from a subdirectory

When Milestone 1 is done, stop and summarize:

* files created
* main structures added
* any assumptions made





Example prompt:
Read SPEC.md and treat it as the source of truth.

Implement the project in milestones, not all at once.

Start with Milestone 1 only:

* create index.html, style.css, script.js, and README.md
* scaffold the canvas, HUD, overlays, and config structure
* wire basic DOM initialization
* do not implement gameplay yet

Constraints:

* vanilla HTML/CSS/JS only
* HTML5 Canvas for rendering
* no external assets
* all tunable values must be grouped in a top-level CONFIG section
* all paths must work on GitHub Pages from a subdirectory

When Milestone 1 is done, stop and summarize:

* files created
* main structures added
* any assumptions made


if it looks good:
I tested milestone 1 locally using a local server. The page loads and the scaffold appears. Proceed to milestone 2 only.


CLAUDE.md helps Claude behave correctly and consistently when working on your codebase. It usually includes:

1. Project context
What the project does
Key architecture decisions
Important files or modules
2. Coding rules & conventions
Style guidelines (naming, formatting, etc.)
Framework-specific patterns
Do’s and don’ts
3. How Claude should act
“Prefer minimal changes”
“Write tests for new features”
“Don’t modify X files”
“Ask before making breaking changes”
4. Environment & workflow info
How to run the project
Build/test commands
Dependencies
5. Task-specific instructions
How to implement features
How to debug issues
How to review PRs


~1,000–3,000 words (most common sweet spot)


Also, should have a Docs:
architecture, code standards, API (to prevent it from scanning codebase every time you chat)
CLAUDE.md shoudl direct to the correct file (or you do it manually)
contains what its purpose and what behavior 

CLAUDE.md is 
Rules
Coding style
How to interpret specs
How to execute tasks
(look at examples online)

the actual implementation guide is in prompts/architecture