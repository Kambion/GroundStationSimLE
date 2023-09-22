#ifndef SDRWORKER_H
#define SDRWORKER_H

#include <QObject>
#include <QThread>
#include "audiobuffer.h"

class SDRWorker : public QThread
{
    Q_OBJECT
public:
    SDRWorker(CAudioBuffer *buffer);

    void run();

    void exit();
private:
    bool active = true;
    CAudioBuffer *buffer;
};

#endif // SDRWORKER_H
