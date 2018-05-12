#ifndef UPDATABLE_H
#define UPDATABLE_H
#include <QList>
#include "selfref.h"
#include <QEventLoop>
class PaintControler;

class MinimalExecutor : public StdSelfRef {
public:
    MinimalExecutor() {}
    bool finished() { return mFinished; }

    virtual void addDependent(MinimalExecutor *updatable) = 0;

    virtual void clear() {
        mFinished = false;
        tellDependentThatFinished();
    }

    void decDependencies() {
        nDependancies--;
        if(mGUIThreadExecution) {
            if(nDependancies == 0) {
                GUI_process();
                mFinished = true;
                tellDependentThatFinished();
            }
        }
    }
    void incDependencies() {
        nDependancies++;
    }
protected:
    virtual void GUI_process() {}
    bool mGUIThreadExecution = false;
    void tellDependentThatFinished() {
        foreach(MinimalExecutor *dependent, mCurrentExecutionDependent) {
            dependent->decDependencies();
        }
        mCurrentExecutionDependent.clear();
    }

    bool mFinished = false;
    QList<MinimalExecutor*> mCurrentExecutionDependent;
    int nDependancies = 0;
};

class GUI_ThreadExecutor : public MinimalExecutor {
    GUI_ThreadExecutor() {
        mGUIThreadExecution = true;
    }

    void addDependent(MinimalExecutor *updatable) {
        if(updatable == NULL) return;
        if(!finished()) {
            if(mCurrentExecutionDependent.contains(updatable)) return;
            mCurrentExecutionDependent << updatable;
            updatable->incDependencies();
        }
    }
protected:
    void GUI_process() = 0;
};

class _Executor : public MinimalExecutor {
public:
    _Executor() {}
    virtual ~_Executor() {}
    void setCurrentPaintControler(PaintControler *paintControler) {
        mCurrentPaintControler = paintControler;
    }

    virtual void beforeUpdate() {
        mSelfRef = ref<_Executor>();
        mBeingProcessed = true;
        mCurrentExecutionDependent = mNextExecutionDependent;
        mNextExecutionDependent.clear();
    }

    virtual void _processUpdate() = 0;

    void updateFinished() {
        mFinished = true;
        mBeingProcessed = false;
        afterUpdate();
        mSelfRef.reset();
    }

    virtual void afterUpdate() {
        mCurrentPaintControler = NULL;
        tellDependentThatFinished();
    }

    bool isBeingProcessed() { return mBeingProcessed; }

    void waitTillProcessed();

    bool readyToBeProcessed() {
        return nDependancies == 0 && !mBeingProcessed;
    }

    void clear() {
        MinimalExecutor::clear();
        mBeingProcessed = false;
        mSelfRef.reset();
        foreach(MinimalExecutor *dependent, mNextExecutionDependent) {
            dependent->decDependencies();
        }
        mNextExecutionDependent.clear();
    }

    void addDependent(MinimalExecutor *updatable) {
        if(updatable == NULL) return;
        if(!finished()) {
            if(mNextExecutionDependent.contains(updatable)) return;
            mNextExecutionDependent << updatable;
            updatable->incDependencies();
        }
    }
protected:
    PaintControler *mCurrentPaintControler = NULL;
    bool mBeingProcessed = false;
    std::shared_ptr<_Executor> mSelfRef;

    QList<MinimalExecutor*> mNextExecutionDependent;
};

class _ScheduledExecutor : public _Executor {
public:
    _ScheduledExecutor() {
        mFinished = true;
    }
    ~_ScheduledExecutor();

    void beforeUpdate();

    virtual void schedulerProccessed();

    void addScheduler();

    virtual bool shouldUpdate() {
        return true;
    }

    bool isAwaitingUpdate() { return mAwaitingUpdate; }
    bool schedulerAdded() { return mSchedulerAdded; }

    void clear();
    virtual bool isFileUpdatable() { return false; }
protected:
    virtual void addSchedulerNow();
    bool mSchedulerAdded = false;
    bool mAwaitingUpdate = false;
};

#endif // UPDATABLE_H
