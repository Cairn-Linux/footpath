// SPDX-License-Identifier: Apache-2.0
pragma ComponentBehavior: Bound
import QtQuick
import Footpath

// The terminal's screen: large mono type on a dark ground, a prompt that is
// the location, ghost completion after the cursor, and one way out. It draws
// what the session says and decides nothing (DESIGN §6).
Rectangle {
    id: terminal

    required property TerminalSession session

    // The look is the host's to set from its own design tokens; Cairn binds
    // its brand. These defaults keep it legible on its own.
    property color groundColor: "#1E1E1E"
    property color textColor: "#EDEDED"
    property color hintColor: "#9DC3D2"
    property color folderColor: "#9DC3D2"
    property color noteColor: "#EDEDED"
    property color makeColor: "#D9A03C"
    property color practiceColor: "#7B9A6D"
    property color gamesColor: "#EDEDED"
    property color machineColor: "#4A7F95"
    property string fontFamily: "monospace"
    property real fontSize: 18
    property real lineHeight: 1.7
    property real margin: 28
    property real chipRadius: 5
    // A games chip is an outline this wide, so a game never looks like a note.
    property real chipLineWidth: 2

    // Escape, or the child typed exit.
    signal exited

    color: terminal.groundColor

    Accessible.role: Accessible.Pane
    Accessible.name: qsTr("Terminal")

    function takeFocus() {
        input.forceActiveFocus();
    }

    function iconColor(icon) {
        switch (icon) {
        case OutputModel.Folder:
            return terminal.folderColor;
        case OutputModel.Make:
            return terminal.makeColor;
        case OutputModel.Practice:
            return terminal.practiceColor;
        case OutputModel.Games:
            return terminal.gamesColor;
        case OutputModel.Machine:
            return terminal.machineColor;
        case OutputModel.Note:
            return terminal.noteColor;
        }
        return Qt.alpha(terminal.groundColor, 0);
    }

    ListView {
        id: output

        objectName: "terminalOutput"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: promptRow.top
        anchors.margins: terminal.margin
        anchors.bottomMargin: 0
        interactive: false
        clip: true
        model: terminal.session.output
        // The newest line is always in view; the child never scrolls.
        onCountChanged: positionViewAtEnd()
        onHeightChanged: positionViewAtEnd()

        delegate: Row {
            id: line

            required property string text
            required property int icon
            required property bool isInput

            width: output.width
            spacing: terminal.fontSize / 2

            Rectangle {
                readonly property bool outlined: line.icon === OutputModel.Games

                objectName: "chip"
                width: terminal.fontSize * 0.8
                height: width
                radius: terminal.chipRadius
                anchors.verticalCenter: parent.verticalCenter
                color: outlined ? Qt.alpha(terminal.groundColor, 0) : terminal.iconColor(line.icon)
                border.width: outlined ? terminal.chipLineWidth : 0
                border.color: terminal.iconColor(line.icon)
            }

            Text {
                width: line.width - terminal.fontSize * 0.8 - line.spacing
                wrapMode: Text.WordWrap
                text: line.text
                font.family: terminal.fontFamily
                font.pixelSize: terminal.fontSize
                lineHeight: terminal.lineHeight
                color: line.isInput ? terminal.hintColor : terminal.textColor
            }
        }
    }

    Row {
        id: promptRow

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: terminal.margin
        spacing: terminal.fontSize / 2

        Text {
            id: prompt

            text: terminal.session.location + " >"
            font.family: terminal.fontFamily
            font.pixelSize: terminal.fontSize
            color: terminal.hintColor
        }

        Item {
            width: promptRow.width - prompt.width - promptRow.spacing
            height: input.height

            // The rest of the most likely word, after what is typed.
            Text {
                x: input.contentWidth
                text: terminal.session.ghost(input.text)
                font: input.font
                color: terminal.hintColor
                opacity: 0.6
            }

            TextInput {
                id: input

                objectName: "terminalInput"
                property int historySteps: 0

                width: parent.width
                font.family: terminal.fontFamily
                font.pixelSize: terminal.fontSize
                color: terminal.textColor
                selectionColor: terminal.hintColor
                selectedTextColor: terminal.groundColor
                cursorVisible: activeFocus
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

                Accessible.role: Accessible.EditableText
                Accessible.name: qsTr("Type a command")

                Keys.onReturnPressed: accept()
                Keys.onEnterPressed: accept()
                Keys.onTabPressed: takeGhost()
                Keys.onEscapePressed: terminal.exited()
                Keys.onUpPressed: recall(historySteps + 1)
                Keys.onDownPressed: recall(historySteps - 1)
                Keys.onRightPressed: event => {
                    if (cursorPosition === text.length)
                        takeGhost();
                    else
                        event.accepted = false;
                }

                function accept() {
                    terminal.session.run(text);
                    text = "";
                    historySteps = 0;
                }

                function takeGhost() {
                    text = text + terminal.session.ghost(text);
                    cursorPosition = text.length;
                }

                function recall(steps) {
                    const earlier = terminal.session.recall(steps);
                    if (steps < 0)
                        return;
                    historySteps = steps;
                    text = earlier;
                    cursorPosition = text.length;
                }
            }
        }
    }
}
