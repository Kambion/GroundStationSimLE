#include "sdrworker.h"
#include <QDebug>
#include <QLibrary>
#include <QString>
#include "rtl-sdr.h"
#include <fstream>
#include <math.h>

#define DEFAULT_BUF_LENGTH		(1 * 16384)
#define MAXIMUM_OVERSAMPLE		16
#define MAXIMUM_BUF_LENGTH		(MAXIMUM_OVERSAMPLE * DEFAULT_BUF_LENGTH)

constexpr int SAMPLE_RATE = 2400000;   //2.4MHz
constexpr int CENTER_FREQ = 89000000; //89.0MHz

static rtlsdr_dev_t *dev = NULL;

SDRWorker::SDRWorker(CAudioBuffer *audioBuffer, bool* active){
    this->audioBuffer = audioBuffer;
    this->active = active;
}

void SDRWorker::run()
{
    if(rtlsdr_open(&dev, 0) < 0){
        qDebug() << "rtlsdr_open failed";
    }

    qDebug() << "Starting SDR";

    if(rtlsdr_set_sample_rate(dev, SAMPLE_RATE) < 0){
        qDebug() << "Error while setting sample rate";
    }else{
        qDebug() << "Sample rate set to " << SAMPLE_RATE/1000000.0 << "MHz";
    }
    if(rtlsdr_set_center_freq(dev, CENTER_FREQ) < 0){
        qDebug() << "Error while setting frequency";
    }else{
        qDebug() << "Frequency set to " << CENTER_FREQ/1000000.0 << "MHz";
    }

    rtlsdr_set_tuner_gain_mode(dev, 0);

    rtlsdr_reset_buffer(dev);

    qDebug() << "Reading...";

    //rtlsdr_read_async(dev, sdrCallback, &active, 0, out_block_size);

    uint8_t buffer[DEFAULT_BUF_LENGTH];
    int n_read = 0;

    while(*active){
        rtlsdr_read_sync(dev, buffer, DEFAULT_BUF_LENGTH, &n_read);
        //TODO:
        //Signal Processing
    }

    rtlsdr_close(dev);
    qDebug() << "RTL-SDR Thread end";
}

