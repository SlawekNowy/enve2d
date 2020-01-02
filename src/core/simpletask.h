// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SIMPLETASK_H
#define SIMPLETASK_H
#include <QObject>
#include <functional>

class SimpleTask : public QObject {
    Q_OBJECT
    typedef std::function<void(void)> Func;
public:
    static SimpleTask *sSchedule(const Func& func);
    static void sProcessAll();
private:
    SimpleTask(const Func& func);
    void process() {
        mFunc();
        emit finished();
    }

    const Func mFunc;
    static QList<SimpleTask*> sTasks;
signals:
    void finished();
};

class SimpleTaskScheduler : public QObject {
    typedef std::function<void(void)> Func;
public:
    SimpleTaskScheduler(const Func& func);

    void schedule();
    void operator()() { schedule(); }
private:
    bool mScheduled = false;
    const Func mFunc;
};

#endif // SIMPLETASK_H
