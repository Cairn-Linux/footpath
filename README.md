# Footpath

A shell that grows with a child. Six real words at five years old, about
twelve at seven, and at nine the same words work in a real terminal.

Footpath is a program, not a terminal emulator: every command is ours, so
every answer is calm, every mistake suggests the next thing to type, and
nothing a child types can change anything. `DESIGN.md` is the
specification. The name is a small path a child walks that leads
somewhere; the child sees a tile called "Terminal".

It began as the restricted shell inside [Cairn Linux](https://github.com/Cairn-Linux/cairn)
and was split out on 2026-09-05 (ADR-0014 there). Cairn's launcher is its
first host.

## What is here

- `core/`: the interpreter and the world it sees, Qt Core only. `World`
  (doors and the read-only `home`), `HomeLayout` (built in, or a version-1
  JSON file), `Interpreter` (the six words), `Hints` (every sentence said
  after a mistake), `DidYouMean`, `Completion`, `Reply`.
- `ui/`: the QML module `Footpath`. `Terminal.qml` is the surface a host
  places in its window; `TerminalSession` feeds it and `OutputModel` holds
  the lines.
- Tests for all of it: `ctest --preset debug` runs six suites offscreen.

Landed so far:

- **Slice 1 (2026-09-04, in Cairn):** the interpreter over the doors.
- **Slice 2 (2026-09-05, in Cairn):** `home`, `exit`, the surface, hosted
  inside Cairn's launcher.
- **Split (2026-09-05):** this repository; the session takes doors from any
  host and the surface is themed by properties.

Next: a standalone window with a demo world (DESIGN §7), a `--home` layout
file, L2's words.

## Build

```sh
cmake --preset debug && cmake --build --preset debug && ctest --preset debug
```

Qt 6.11 with `qt6-qtdeclarative-devel` and `qt6-linguist`; clang if your
gcc lacks the sanitizer runtimes (`CC=clang CXX=clang++` on the first
configure). Refresh the translation catalogue with
`cmake --build --preset debug --target footpath_lupdate`.

## Hosting it

Add this repository to your CMake tree (Cairn uses a git submodule pinned
to a tag), link `footpathplugin`, and in QML:

```qml
import Footpath

TerminalSession {
    id: session
    childName: "Sam"
    onLaunchRequested: (title, exec) => startIt(title, exec)
    onLeft: goBackToWhereverYouCameFrom()
}

Terminal {
    session: session
    onExited: goBackToWhereverYouCameFrom()
    // Theme it from your own tokens:
    groundColor: myTokens.ink
    fontFamily: myTokens.mono
}
```

Give the session its doors with `setDoors()` from C++ or `addDoor()` from
QML, then `reset()` for a fresh sitting. The session starts nothing: it
emits `launchRequested` and the host decides.

## The six words

| Typed | Does |
|---|---|
| `ls`, `ls notes` | shows what is here, or what is in a folder here |
| `cd notes`, `cd ..`, `cd /`, `cd` | goes into a folder, up one, or back to the root |
| `open draw` | names the program for the host to start; on a note, reads it |
| `cat hello` | reads a note |
| `help`, `help open` | one line per command |
| `exit` | leaves; the host returns to wherever it came from |

Input is case-insensitive; a command takes one name; a name is one step.
The prompt is the location: `/`, `/make`, `/home/sam/notes`. Completion
shows the rest of the first match as ghost text; Tab or Right accepts it;
Up recalls.

## What it says when something goes wrong

Every sentence names the next thing to type and never blames the child.
The tests pin every string.

| Situation | Says |
|---|---|
| unknown command, close to one | I don't know "opn". Did you mean open? |
| unknown command | I don't know "sudo". Type help to see what I know. |
| two names | open takes one name at a time. |
| `open` or `cat` alone | open needs a name. Type ls to see them. |
| a folder where a thing was wanted | make is a place. Type cd make to go there. |
| a thing where a folder was wanted | draw is a thing to open, not a place. Type open draw. |
| a note where a folder or thing was wanted | hello is a note. Type cat hello to read it. |
| `cat` on a thing | draw is a thing to open, not to read. Type open draw. |
| the thing is elsewhere, from the root | draw is in make. Type cd make first. |
| the thing is elsewhere, from a folder | draw is in make. Type cd / and then cd make. |
| a top-level folder named from inside another | practice is at the top. Type cd / first. |
| a name close to one here | I can't find "drw". Did you mean draw? |
| no such name | I can't find "freddi" here. Type ls to see what is here. |
| an empty folder | Nothing here yet. |

Did-you-mean allows one edit for words of three letters or fewer and two
for longer ones, and a tie goes to the earlier candidate.

## Licence

Apache-2.0. See `LICENSE` and `NOTICE`. "Cairn Linux" and its mark are
Cairn's trademarks and are not part of Footpath.
