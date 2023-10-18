#define _USE_MATH_DEFINES
#include "sdrworker.h"
#include <QDebug>
#include <QLibrary>
#include <QString>
#include <fstream>
#include <math.h>
#include <complex>
#include <vector>
#include <limits>

constexpr double sos9[4][6] = {{ 8.13160323e-09,  1.62632065e-08,  8.13160323e-09,  1.00000000e+00, -1.80102222e+00,  8.13719178e-01},
                    {1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.80826915e+00,  8.40588279e-01},
                    {1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.83026610e+00,  8.91360262e-01},
                    {1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.87714628e+00,  9.60659875e-01}};

constexpr double sos4[4][6] = {{ 4.03592929e-06,  8.07185858e-06,  4.03592929e-06,  1.00000000e+00, -1.56114552e+00,  6.20886720e-01},
                    { 1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.52504583e+00,  6.76620197e-01},
                    { 1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.49217571e+00,  7.79218666e-01},
                    { 1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.51874590e+00,  9.18540466e-01}};

constexpr double sos2[4][6] = {{ 6.98737077e-04,  1.39747415e-03,  6.98737077e-04,  1.00000000e+00,  -1.11407399e+00,  3.44905050e-01},
                    {1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -9.04148511e-01,  4.64939328e-01},
                    { 1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -6.44955463e-01,  6.53189753e-01},
                    { 1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -4.95922536e-01,  8.74392030e-01}};

constexpr double sos8[4][6] = {{ 2.02485310e-08,  4.04970621e-08,  2.02485310e-08,  1.00000000e+00, -1.77677361e+00,  7.92697396e-01},
                    { 1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.78198389e+00,  8.22531910e-01},
                    { 1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.80215495e+00,  8.78901597e-01},
                    { 1.00000000e+00,  2.00000000e+00, 1.00000000e+00,  1.00000000e+00, -1.85081259e+00,  9.56022879e-01 }};


constexpr double zi9[4][2] = {{ 2.55361726e-06, -2.07641258e-06},
                    { 3.14494957e-04, -2.63952402e-04},
                    { 2.04415018e-02, -1.81862975e-02},
                    { 9.73501515e-01, -9.34387200e-01}};

constexpr double zi4[4][2] ={{ 2.66191580e-04, -1.63744743e-04},
                    { 6.86099186e-03, -4.55489954e-03},
                    { 9.22437239e-02, -7.03035913e-02},
                    {8.94885131e-01, -8.13893168e-01}};

constexpr double zi2[4][2] = {{ 0.01140946, -0.00347744},
                    { 0.07425697, -0.02804637},
                    { 0.25627412, -0.1374433 },
                    { 0.65162078, -0.52673379}};


constexpr double zi8[4][2] = {{ 5.06611383e-06, -4.01169767e-06},
                    { 4.96675533e-04, -4.07628808e-04},
                    { 2.56498364e-02, -2.24829196e-02},
                    { 9.68108476e-01, -9.24383780e-01}};

std::vector<std::complex<double>> data;


template <typename T>
std::vector<T> validate_pad(std::vector<T>& x, int edge){

    T left_end = x.front();
    T right_end = x.back();

    std::vector<T> left_ext;
    left_ext.resize(edge);
    std::vector<T> right_ext;
    right_ext.resize(edge);
    for(int i = 0; i < edge; i++){
        left_ext[i] = x[edge-i];
        right_ext[i] = x[x.size() - 2 - i];
    }
    std::vector<T> result;
    result.resize(x.size() + 2*edge);
    for(int i = 0; i < edge; i++){
        result[i] = 2.0*left_end - left_ext[i];
        result[x.size()+edge+i] = 2.0*right_end - right_ext[i];
    }
    for(int i = 0; i < x.size(); i++){
        result[edge+i] = x[i];
    }
    return result;
}

template <typename T>
void sosfilt(const double sos[4][6], std::vector<T>& x, T zi[4][2], bool reverse_axis){
    for(int n = 0; n < x.size(); n++){
        for(int s = 0; s < 4; s++){
            int i = n;
            if(reverse_axis) i = x.size() - n - 1;
            T x_n = x[i];
            x[i] = sos[s][0] * x_n + zi[s][0];
            zi[s][0] = (sos[s][1] * x_n - sos[s][4] * x[i] + zi[s][1]);
            zi[s][1] = (sos[s][2] * x_n - sos[s][5] * x[i]);
        }
    }
}


template <typename T>
void sosfiltfilt(const double sos[4][6], std::vector<T>& x){
    int edge = 27;
    std::vector<T> ext = validate_pad(x, edge);



    T zi[4][2];
    T zi_copy[4][2];

    T x_0 = ext.front();
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 2; j++){
            if(sos[0][0] == sos9[0][0]){
                zi[i][j] = zi9[i][j];
            }
            else if(sos[0][0] == sos8[0][0]){
                zi[i][j] = zi8[i][j];
            }
            else if(sos[0][0] == sos4[0][0]){
                zi[i][j] = zi4[i][j];
            }
            else{
                zi[i][j] = zi2[i][j];
            }
            zi_copy[i][j] = zi[i][j];
            zi[i][j] *= x_0;
        }
    }
    sosfilt(sos, ext, zi, false);

    T y_0 = ext.back();
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 2; j++){
            zi_copy[i][j] *= y_0;
        }
    }
    sosfilt(sos, ext, zi_copy, true);

    for(int i = 0; i < x.size(); i++){
        x[i] = ext[i+edge];
    }
}

template <typename T>
std::vector<T> decimate(std::vector<T>& x, int q){
    int n = 8;
    //some magic numbers :) (from python)
    //sos = cheby1(8, 0.05, 0.8 / q, output='sos')

    if(q == 9) //9
       sosfiltfilt(sos9, x);
    else if(q == 8)
       sosfiltfilt(sos8, x);
    else if(q == 4)//4
       sosfiltfilt(sos4, x);
    else //2
       sosfiltfilt(sos2, x);
    std::vector<T> result;
    result.resize(int(ceil((double)x.size()/q)));
    for(int i = 0; i < x.size(); i+=q){
        result[i/q] = x[i];
    }
    return result;
}



void demod(std::vector<double>& output, uint32_t len){
    double bwFM = 200000;
    int decRate = SAMPLE_RATE/bwFM;

    data = decimate(data, decRate);

    int newFs = SAMPLE_RATE/decRate;
    //############################



    //############################
    //działa jak trzeba
    //y4 = x3[1:] * np.conj(x3[:-1])
    //x4 = np.angle(y4)
    output.resize(data.size()-1);

    std::complex<double> tmp;
    for(int i = 1; i < data.size(); i++){
        tmp = data[i-1];
        tmp.imag(-1*tmp.imag());
        output[i-1] = std::arg(data[i]*tmp);
    }
    //############################


    //############################

    // tu jest niby dolnoprzepustowy ale jestem leniwym kutasem więc go nie ma

    //############################
    //znowu decimate(), tak jak wyżej pewnie do zmiany
    //x6 = sig.decimate(x5, dec_audio)
    double audioFreq = 22050;
    int dec_audio = int(newFs/audioFreq);

    output = decimate(output, dec_audio);
    //############################


    /*std::ofstream file("demod_output.txt", std::ios::app);
    for(int i = 0; i < output.size(); i++){
        output[i] *= 0.1; //volume adjust
        file << output[i] << '\n';
    }
    file.close();*/
}


void SDRWorker::rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx){
    CAudioBuffer *audioBuffer = (CAudioBuffer*) ((void**)ctx)[0];
    QBuffer *outputBuffer = (QBuffer*) ((void**)ctx)[1];
    bool *active = (bool*) ((void**)ctx)[2];

    if(!*active){
        rtlsdr_cancel_async(dev);
        return;
    }

    data.resize(BYTES_TO_READ/2);
    for(uint32_t i = 0, q = 1; i < len; i+=2, q+=2){
        data[i/2].real(buf[i]/(255.0/2) - 1);
        data[i/2].imag(buf[q]/(255.0/2) - 1);
    }
    std::vector<double> output;

    demod(output, len/2);

    std::vector<int16_t> integerOutput;
    integerOutput.resize(output.size());

    for(int i = 0; i < output.size(); i++){\
        integerOutput[i] = std::numeric_limits<int16_t>::max()*output[i]/8;
    }
    qint64 toWrite = sizeof(int16_t)*output.size();
    outputBuffer->write((const char*)integerOutput.data(), toWrite);
    outputBuffer->seek(outputBuffer->pos() - toWrite);
    audioBuffer->writeData((const char*)integerOutput.data(), toWrite);
    //rtlsdr_cancel_async(dev);
}


SDRWorker::SDRWorker(CAudioBuffer *audioBuffer, QBuffer* output, bool* active){
    this->audioBuffer = audioBuffer;
    this->outputBuffer = output;
    this->active = active;
}
//####RTL-SDR DONGLE INIT####
// returns true if success
bool SDRWorker::initSDR(){

    int result;
    result = rtlsdr_open(&dev, 0);
        if(result < 0) return false;

    result = rtlsdr_set_testmode(dev, 0);
        if(result < 0) return false;

    result = rtlsdr_reset_buffer(dev);
        if(result < 0) return false;

    result = rtlsdr_set_sample_rate(dev, DEFAULT_RS);
        if(result < 0) return false;

    result = rtlsdr_set_center_freq(dev, DEFAULT_FC);
        if(result < 0) return false;

    result = rtlsdr_set_tuner_bandwidth(dev, 0);
    if(result < 0) return false;

    result = rtlsdr_set_tuner_gain_mode(dev, 0);
        if(result < 0) return false;

    result = rtlsdr_set_agc_mode(dev, 0);
        if(result < 0) return false;


    return true;
}

//####RTL-SDR SETTING SELECTED VALUES####
// returns true if success
bool SDRWorker::setUserValues(){
    int result;
    result = rtlsdr_set_sample_rate(dev, SAMPLE_RATE);
        if(result < 0) return false;
    result = rtlsdr_set_center_freq(dev, CENTER_FREQ);
        if(result < 0) return false;
    result = rtlsdr_set_freq_correction(dev, err_ppm);
        if(result < 0) return false;
    return true;
}

void SDRWorker::run()
{
    context[0] = audioBuffer;
    context[1] = outputBuffer;
    context[2] = active;

    data.resize(BYTES_TO_READ/2);
    qDebug() << "RTL-SDR init...";
    bool success = initSDR();

    if(!success){
        qDebug() << "RTL-SDR init failed";
        return;
    }
    qDebug() << "RTL-SDR init done";

    qDebug() << "Setting RTL-SDR values";
    success = setUserValues();
    if(!success){
        qDebug() << "Setting RTL-SDR values failed";
    }
    qDebug() << "RTL-SDR Values set";

    int result = rtlsdr_read_async(dev, rtlsdr_callback, this->context, 0, BYTES_TO_READ);
    if(result < 0){
        qDebug() << "RTL-SDR read failed";
    }

    rtlsdr_close(dev);
    qDebug() << "RTL-SDR Thread end";
}

