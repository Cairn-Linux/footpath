# CLAUDE.md — working in the Footpath repository

Footpath is a shell that grows with a child: six real words at five years
old, about twelve at seven, and at nine the same words work in bash. It is
a program, not a terminal emulator. `DESIGN.md` is the specification; do
not silently diverge from it. It began inside Cairn Linux
(Cairn-Linux/cairn, ADR-0014 there), whose launcher is its first host.

## Non-negotiables

1. **Nothing phones home.** No network, no accounts, no analytics.
2. **The world is read-only.** No word the child knows makes, changes or
   deletes anything. The interpreter never writes a file or starts a
   process; `open` is a request to the host.
3. **Real words only.** Every command is one bash accepts. No invented
   kid-verbs.
4. **Suggest, never scold.** A mistake is answered with the next thing to
   type. No errors, codes, stack traces or blame. Calm adult voice, short
   sentences, sentence case, no exclamation marks.
5. **Same answer every time.** No randomness, no timing, no hidden state.
6. **The host decides what is real.** Doors, the child's name and the level
   come from the host; the look comes from the host's tokens through the
   `Terminal` properties. Defaults live only in `ui/Terminal.qml`.

## Language and toolkit

C++20 with Qt 6 and QML, written to be read by someone learning C++.

- **QML draws, C++ decides.** No business logic in QML JavaScript.
- Tiny classes, one per `.h`/`.cpp` pair, under ~200 lines.
- Ownership is visible: `QObject`s get a parent; everything else is a value
  or `std::unique_ptr`. No raw `new` without a parent, no `delete`.
- Boring over clever. `std::optional` for maybe, `enum class` for kinds,
  errors as values, never an exception across a Qt boundary.
- `core/` depends on Qt Core only and is tested on its own; `ui/` is the
  QML module `Footpath`.
- Build: CMake + Ninja, `-Wall -Wextra -Werror`, sanitizers in Debug.
  clang-format and clang-tidy clean before every commit; qmlformat and
  qmllint for QML.
- Every user-facing string goes through `tr()` / `qsTr()`; every control
  has an `Accessible.name`.
- Every command, every sentence and every refusal case is a test.

## Conventions

- SPDX line at the top of every source file: `Apache-2.0`.
- `git commit -s`; imperative one-line subject; the first time a C++ or Qt
  concept appears, the commit message says in a sentence what it is.
- Docs: Markdown, ~80 columns, one sentence per idea.
- `README.md` lists the slices that have landed; keep it current in the
  same change as the code.
