// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Interpreter.h"
#include "OutputModel.h"
#include "World.h"

#include <QList>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QStringList>

// One sitting at the terminal. The host gives it the doors (what the child
// may open) and the child's name; it builds the world with the built-in
// home, feeds typed lines to the interpreter, keeps the output and the
// history, and asks the host to start what open names. It starts nothing
// itself (DESIGN §7).
class TerminalSession : public QObject {
    Q_OBJECT
    QML_ELEMENT
    // Names the child's folder in home. Defaults to the login's home folder.
    Q_PROPERTY(QString childName READ childName WRITE setChildName NOTIFY childNameChanged)
    Q_PROPERTY(QString location READ location NOTIFY locationChanged)
    Q_PROPERTY(OutputModel* output READ output CONSTANT)

public:
    explicit TerminalSession(QObject* parent = nullptr);

    QString childName() const;
    void setChildName(const QString& name);
    QString location() const;
    OutputModel* output() const;

    // The doors, from C++ hosts. Takes effect at the next reset().
    void setDoors(const QList<World::Door>& doors);
    QList<World::Door> doors() const;
    // The doors, one at a time, from QML hosts. `kind` is make, practice,
    // games or machine; anything else is machine.
    Q_INVOKABLE void addDoor(const QString& title, const QString& kind, const QStringList& exec);
    Q_INVOKABLE void clearDoors();

    // A fresh sitting: empty screen, at the root, world rebuilt from the doors.
    Q_INVOKABLE void reset();
    Q_INVOKABLE void run(const QString& line);
    // The rest of the most likely word for what is typed so far, or nothing.
    Q_INVOKABLE QString ghost(const QString& line) const;
    // Lines typed earlier this sitting; 1 is the latest. Empty past the end.
    Q_INVOKABLE QString recall(int stepsBack) const;

signals:
    void childNameChanged();
    void locationChanged();
    void launchRequested(const QString& title, const QStringList& exec);
    void left();

private:
    QString m_childName;
    QList<World::Door> m_doors;
    Interpreter m_interpreter;
    OutputModel* m_output;
    QStringList m_history;
};
