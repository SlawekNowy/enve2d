#include "videobox.h"
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}
#include "mainwindow.h"
#include <QDebug>
#include "Sound/singlesound.h"
#include "canvas.h"
#include "Sound/soundcomposition.h"

VideoBox::VideoBox(const QString &filePath) :
    AnimationBox() {
    setName("Video");

    setFilePath(filePath);
}

void VideoBox::setParent(BoxesGroup *parent) {
    if(mParent != NULL && mSound != NULL) {
        getParentCanvas()->getSoundComposition()->removeSound(mSound);
    }
    AnimationBox::setParent(parent);
    if(mParent != NULL && mSound != NULL) {
        getParentCanvas()->getSoundComposition()->addSound(mSound);
    }
}

void VideoBox::makeDuplicate(Property *targetBox) {
    AnimationBox::makeDuplicate(targetBox);
    VideoBox *animationBoxTarget = (VideoBox*)targetBox;
    animationBoxTarget->setFilePath(mSrcFilePath);
    animationBoxTarget->duplicateAnimationBoxAnimatorsFrom(
                mTimeScaleAnimator.data());
}

BoundingBox *VideoBox::createNewDuplicate() {
    return new VideoBox(mSrcFilePath);
}

#include <QFileDialog>
void VideoBox::changeSourceFile() {
    MainWindow::getInstance()->disableEventFilter();
    QString importPath = QFileDialog::getOpenFileName(
                                            MainWindow::getInstance(),
                                            "Change Source", "",
                                            "Video Files (*.mp4 *.mov *.avi)");
    MainWindow::getInstance()->enableEventFilter();
    if(!importPath.isEmpty()) {
        setFilePath(importPath);
    }
}

void VideoBox::setFilePath(QString path) {
    mSrcFilePath = path;
    reloadFile();
}
#include "filesourcescache.h"
void VideoBox::reloadFile() {
    mAnimationCacheHandler = (AnimationCacheHandler*)
                                FileSourcesCache::getHandlerForFilePath(
                                                        mSrcFilePath);
    if(mAnimationCacheHandler == NULL) {
        mAnimationCacheHandler = new VideoCacheHandler(mSrcFilePath);
    } else {
        ((VideoCacheHandler*)mAnimationCacheHandler)->clearCache();
    }
    AnimationBox::reloadFile();
}

bool hasSound(const char* path) {
    // get format from audio file
    AVFormatContext* format = avformat_alloc_context();
    if (avformat_open_input(&format, path, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open file '%s'\n", path);
        return false;
    }
    if(avformat_find_stream_info(format, NULL) < 0) {
        fprintf(stderr, "Could not retrieve stream info from file '%s'\n", path);
        return false;
    }

    // Find the index of the first audio stream
    for (uint i = 0; i < format->nb_streams; i++) {
        const AVMediaType &mediaType = format->streams[i]->codec->codec_type;
        if(mediaType == AVMEDIA_TYPE_AUDIO) {
            return true;
        }
    }

    avformat_free_context(format);

    // success
    return false;
}

void VideoBox::reloadSound() {
    if(hasSound(mSrcFilePath.toLatin1().data())) {
        if(mSound == NULL) {
            mSound = new SingleSound(mSrcFilePath,
                                     (FixedLenAnimationRect*)mDurationRectangle);
            ca_addChildAnimator(mSound);
            if(mParent != NULL) {
                getParentCanvas()->getSoundComposition()->addSound(mSound);
            }
        } else {
            mSound->reloadDataFromFile();
        }
    } else {
    }
}
