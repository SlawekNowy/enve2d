#ifndef PAINTBOXSIZEUPDATERBR_H
#define PAINTBOXSIZEUPDATERBR_H
#include "propertyupdater.h"
class PaintBox;

class PaintBoxSizeUpdaterBR : public PropertyUpdater {
public:
    PaintBoxSizeUpdaterBR(PaintBox *paintBox);
    ~PaintBoxSizeUpdaterBR();

    void update();
    void frameChangeUpdate();
    void updateFinal();
private:
    PaintBox *mTarget;
};

class PaintBoxSizeUpdaterTL : public PropertyUpdater {
public:
    PaintBoxSizeUpdaterTL(PaintBox *paintBox);
    ~PaintBoxSizeUpdaterTL();

    void update();
    void frameChangeUpdate();
    void updateFinal();
private:
    PaintBox *mTarget;
};
#endif // PAINTBOXSIZEUPDATERBR_H