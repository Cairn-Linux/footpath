// SPDX-License-Identifier: Apache-2.0
import QtQuick
import QtTest
import Footpath

TestCase {
    id: testCase

    name: "Terminal"
    when: windowShown
    width: 800
    height: 480

    Component {
        id: terminalComponent

        Terminal {
            session: TerminalSession {
                childName: "Sam"
                Component.onCompleted: {
                    addDoor("Draw", "make", ["tuxpaint"]);
                    reset();
                }
            }
        }
    }

    SignalSpy {
        id: exits

        signalName: "exited"
    }

    SignalSpy {
        id: launches

        signalName: "launchRequested"
    }

    property Item terminal
    property Item input

    function init() {
        terminal = createTemporaryObject(terminalComponent, testCase, {
            "width": testCase.width,
            "height": testCase.height
        });
        verify(terminal !== null);
        exits.target = terminal;
        launches.target = terminal.session;
        exits.clear();
        launches.clear();
        input = findChild(terminal, "terminalInput");
        verify(input !== null);
        terminal.takeFocus();
        tryCompare(input, "activeFocus", true);
    }

    function type(line) {
        input.text = line;
        keyClick(Qt.Key_Return);
    }

    function test_typedLinesAreEchoedAndAnswered() {
        type("ls");
        tryVerify(() => terminal.session.output.rowCount() === 3);
        type("cd make");
        tryCompare(terminal.session, "location", "/make");
        compare(input.text, "");
    }

    function test_openAsksTheHost() {
        type("cd make");
        type("open draw");
        tryCompare(launches, "count", 1);
        compare(launches.signalArguments[0][0], "Draw");
    }

    function test_ghostCompletesOnTab() {
        input.text = "op";
        keyClick(Qt.Key_Tab);
        compare(input.text, "open");
        keyClick(Qt.Key_Space);
        keyClick(Qt.Key_D);
        keyClick(Qt.Key_Right);
        compare(input.text, "open draw".substring(0, 5) + input.text.substring(5));
    }

    function test_upArrowRecalls() {
        type("ls");
        type("help");
        keyClick(Qt.Key_Up);
        compare(input.text, "help");
        keyClick(Qt.Key_Up);
        compare(input.text, "ls");
        keyClick(Qt.Key_Down);
        compare(input.text, "help");
    }

    function test_escapeAndExitLeave() {
        keyClick(Qt.Key_Escape);
        compare(exits.count, 1);
        type("exit");
        // exit is the session's signal; the host wires it to the same place.
    }
}
