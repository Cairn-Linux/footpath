// SPDX-License-Identifier: Apache-2.0
#include "TerminalSession.h"
#include "World.h"

#include <QSignalSpy>
#include <QTest>

namespace {

QStringList texts(const OutputModel* output) {
    QStringList out;
    for (const OutputModel::Line& line : output->lines()) {
        out.append(line.text);
    }
    return out;
}

class TerminalSessionTest : public QObject {
    Q_OBJECT

    // Five doors, as a host would hand them over.
    static QList<World::Door> fixtureDoors() {
        const QStringList sh = {QStringLiteral("/bin/sh"), QStringLiteral("-c")};
        return {
            {.title = QStringLiteral("Quits badly"),
             .kind = World::Kind::Make,
             .exec = sh + QStringList{QStringLiteral("exit 3")}},
            {.title = QStringLiteral("Quits cleanly"),
             .kind = World::Kind::Make,
             .exec = sh + QStringList{QStringLiteral("exit 0")}},
            {.title = QStringLiteral("Missing program"),
             .kind = World::Kind::Make,
             .exec = {QStringLiteral("/nonexistent")}},
            {.title = QStringLiteral("Nothing set up"), .kind = World::Kind::Make, .exec = {}},
            {.title = QStringLiteral("Practice"),
             .kind = World::Kind::Practice,
             .exec = sh + QStringList{QStringLiteral("exit 0")}},
        };
    }

    static void giveDoors(TerminalSession& session) {
        session.setDoors(fixtureDoors());
        session.reset();
    }

private slots:
    void startsAtTheRootWithAnEmptyScreen() {
        TerminalSession session(this);
        session.setChildName(QStringLiteral("Sam"));
        giveDoors(session);
        QCOMPARE(session.location(), QStringLiteral("/"));
        QCOMPARE(session.output()->rowCount(), 0);
    }

    void echoesTheLineThenTheAnswer() {
        TerminalSession session(this);
        session.setChildName(QStringLiteral("Sam"));
        giveDoors(session);
        session.run(QStringLiteral("ls"));
        const QList<OutputModel::Line> lines = session.output()->lines();
        QCOMPARE(lines.size(), 4);
        QCOMPARE(lines[0].text, QStringLiteral("/ > ls"));
        QVERIFY(lines[0].isInput);
        QCOMPARE(lines[1].text, QStringLiteral("make"));
        QCOMPARE(lines[1].icon, OutputModel::Icon::Folder);
        QVERIFY(!lines[1].isInput);
        QCOMPARE(lines[3].text, QStringLiteral("home"));
        session.run(QStringLiteral("cd make"));
        QCOMPARE(session.location(), QStringLiteral("/make"));
        session.run(QStringLiteral("ls"));
        QVERIFY(texts(session.output()).contains(QStringLiteral("quits-badly")));
    }

    void openAsksTheLauncher() {
        TerminalSession session(this);
        giveDoors(session);
        const QSignalSpy launches(&session, &TerminalSession::launchRequested);
        session.run(QStringLiteral("cd make"));
        session.run(QStringLiteral("open quits-cleanly"));
        QCOMPARE(launches.count(), 1);
        QCOMPARE(launches.first().at(0).toString(), QStringLiteral("Quits cleanly"));
        QCOMPARE(launches.first().at(1).toStringList().first(), QStringLiteral("/bin/sh"));
    }

    void exitSaysLeft() {
        TerminalSession session(this);
        giveDoors(session);
        const QSignalSpy lefts(&session, &TerminalSession::left);
        session.run(QStringLiteral("exit"));
        QCOMPARE(lefts.count(), 1);
    }

    void homeIsNamedAfterTheChild() {
        TerminalSession session(this);
        session.setChildName(QStringLiteral("Sam"));
        giveDoors(session);
        session.run(QStringLiteral("cd home"));
        session.run(QStringLiteral("ls"));
        QCOMPARE(session.output()->lines().last().text, QStringLiteral("sam"));
    }

    void resetForgetsEverything() {
        TerminalSession session(this);
        giveDoors(session);
        session.run(QStringLiteral("cd make"));
        session.reset();
        QCOMPARE(session.location(), QStringLiteral("/"));
        QCOMPARE(session.output()->rowCount(), 0);
        QVERIFY(session.recall(1).isEmpty());
    }

    void ghostIsTheRestOfTheWord() {
        TerminalSession session(this);
        giveDoors(session);
        QCOMPARE(session.ghost(QStringLiteral("op")), QStringLiteral("en"));
        QCOMPARE(session.ghost(QStringLiteral("cd ma")), QStringLiteral("ke"));
        QCOMPARE(session.ghost(QStringLiteral("cd zz")), QString());
        QCOMPARE(session.ghost(QString()), QStringLiteral("ls"));
    }

    void recallWalksBackThroughThisSitting() {
        TerminalSession session(this);
        giveDoors(session);
        session.run(QStringLiteral("ls"));
        session.run(QStringLiteral("cd make"));
        QCOMPARE(session.recall(1), QStringLiteral("cd make"));
        QCOMPARE(session.recall(2), QStringLiteral("ls"));
        QCOMPARE(session.recall(3), QString());
        QCOMPARE(session.recall(0), QString());
    }

    void doorsFromQmlArriveOneAtATime() {
        TerminalSession session(this);
        session.addDoor(QStringLiteral("Draw"), QStringLiteral("make"),
                        {QStringLiteral("tuxpaint")});
        session.addDoor(QStringLiteral("Putt-Putt"), QStringLiteral("games"), {});
        session.addDoor(QStringLiteral("Odd"), QStringLiteral("toys"), {});
        session.reset();
        session.run(QStringLiteral("ls"));
        QCOMPARE(texts(session.output()).mid(1),
                 (QStringList{QStringLiteral("make"), QStringLiteral("games"),
                              QStringLiteral("machine"), QStringLiteral("home")}));
        session.clearDoors();
        session.reset();
        session.run(QStringLiteral("ls"));
        QCOMPARE(texts(session.output()).mid(1), QStringList{QStringLiteral("home")});
    }

    void blankLinesAreNotEchoed() {
        TerminalSession session(this);
        giveDoors(session);
        session.run(QStringLiteral("   "));
        QCOMPARE(session.output()->rowCount(), 0);
    }
};

} // namespace

QTEST_GUILESS_MAIN(TerminalSessionTest)
#include "tst_terminalsession.moc"
