#ifndef SDRWORKER_H
#define SDRWORKER_H

#include <QObject>
#include <QThread>
#include "audiobuffer.h"
#include "rtl-sdr.h"
#include <QBuffer>


constexpr int BYTES_TO_READ = 2*256*1024;

constexpr int DEFAULT_FC = 80e6;
constexpr int DEFAULT_RS = 1.024e6;
constexpr int DEFAULT_READ_SIZE = 1024;

constexpr int SAMPLE_RATE = 1800000;   //1.8MHz
constexpr int CENTER_FREQ = 144800000; //144.800MHz


static rtlsdr_dev_t *dev = NULL;


class SDRWorker : public QThread
{
    Q_OBJECT
public:
    SDRWorker(CAudioBuffer *buffer, QBuffer* output, bool* active);

    void run();
    static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);
private:

    bool initSDR();
    void initAudio();
    bool setUserValues();

    bool* active = nullptr;
    CAudioBuffer *audioBuffer;
    QBuffer* outputBuffer;

    void* context[3];
    int err_ppm = 1;

};

#endif // SDRWORKER_H
