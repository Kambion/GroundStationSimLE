#include "sdrworker.h"
#include <QDebug>
#include <QLibrary>
#include <QString>
#include "rtl-sdr.h"

#define DEFAULT_BUF_LENGTH		(16 * 16384)


constexpr int SAMPLE_RATE = 2400000;   //2.4MHz
constexpr int CENTER_FREQ = 89000000; //89.0MHz

static rtlsdr_dev_t *dev = NULL;

void sdrCallback(unsigned char *buf, uint32_t len, void *ctx){
    qDebug() << buf[0];
}

SDRWorker::SDRWorker(CAudioBuffer *audioBuffer){
    this->buffer = audioBuffer;
    active = true;
}

void SDRWorker::run()
{
    uint32_t out_block_size = DEFAULT_BUF_LENGTH;

    //uint8_t *sdrBuffer = (uint8_t*)malloc(out_block_size * sizeof(uint8_t));

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

    uint8_t buf[DEFAULT_BUF_LENGTH];

    int n_read = 0;

    rtlsdr_read_async(dev, sdrCallback, NULL, 0, out_block_size);

    //while(active){
        //int data = rand();
        //buffer->writeData((const char*)&data, sizeof(int));
        //usleep(1);
    //}

    rtlsdr_close(dev);
    qDebug() << "RTL-SDR Thread end";
}

void SDRWorker::exit(){
    rtlsdr_cancel_async(dev);
    active = false;
}
