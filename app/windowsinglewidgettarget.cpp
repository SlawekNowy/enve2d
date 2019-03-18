#include "windowsinglewidgettarget.h"
#include "GUI/canvaswindow.h"
#include "singlewidgetabstraction.h"

WindowSingleWidgetTarget::WindowSingleWidgetTarget(
        CanvasWindow * const window) : mWindow(window) {}

void WindowSingleWidgetTarget::SWT_addChildrenAbstractions(
        SingleWidgetAbstraction *abstraction,
        const UpdateFuncs &updateFuncs,
        const int& visiblePartWidgetId) {
    mWindow->SWT_addChildrenAbstractions(abstraction, updateFuncs,
                                         visiblePartWidgetId);
}

