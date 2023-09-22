#ifndef SDRWORKER_H
#define SDRWORKER_H

#include <QObject>
#include <QThread>
#include "audiobuffer.h"

class SDRWorker : public QThread
{
    Q_OBJECT
public:
    SDRWorker(CAudioBuffer *buffer, bool* active);

    void run();
private:
    bool* active = nullptr;
    CAudioBuffer *audioBuffer;
};

#endif // SDRWORKER_H
