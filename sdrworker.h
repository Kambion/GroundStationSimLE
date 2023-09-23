#ifndef SDRWORKER_H
#define SDRWORKER_H

#include <QObject>
#include <QThread>
#include "audiobuffer.h"
#include "rtl-sdr.h"

constexpr int BYTES_TO_READ = 2*256*1024;

constexpr int DEFAULT_FC = 80e6;
constexpr int DEFAULT_RS = 1.024e6;
constexpr int DEFAULT_READ_SIZE = 1024;

constexpr int SAMPLE_RATE = 1800000;   //2.4MHz
constexpr int CENTER_FREQ = 89000000; //89.0MHz


static rtlsdr_dev_t *dev = NULL;


class SDRWorker : public QThread
{
    Q_OBJECT
public:
    SDRWorker(CAudioBuffer *buffer, bool* active);

    void run();
private:
    bool initSDR();
    bool setUserValues();

    bool* active = nullptr;
    CAudioBuffer *audioBuffer;
    int err_ppm = 1;
};

#endif // SDRWORKER_H
