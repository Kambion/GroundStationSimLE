#define _USE_MATH_DEFINES
#include "sdrworker.h"
#include <QDebug>
#include <QLibrary>
#include <QString>
#include <fstream>
#include <math.h>
#include <complex>
#include <vector>

std::vector<std::complex<double>> data;


void bb_digital_filter(double *b, double *a, double *x, double *y, double *Z, int len_b, uint32_t len_x, int stride_X, int stride_Y)
{
    double *ptr_x = x, *ptr_y = y;
    double *ptr_Z;
    double *ptr_b = (double*)b;
    double *ptr_a = (double*)a;
    double *xn, *yn;
    const double a0 = *((double *)a);
    int n;
    uint32_t k;

    /* normalize the filter coefs only once. */
    for (n = 0; n < len_b; ++n) {
        ptr_b[n] /= a0;
        ptr_a[n] /= a0;
    }

    for (k = 0; k < len_x; k++) {
        ptr_b = (double *)b;   /* Reset a and b pointers */
        ptr_a = (double *)a;
        xn = (double *)ptr_x;
        yn = (double *)ptr_y;
        if (len_b > 1) {
            ptr_Z = ((double *)Z);
            *yn = *ptr_Z + *ptr_b  * *xn;   /* Calculate first delay (output) */
            ptr_b++;
            ptr_a++;
            /* Fill in middle delays */
            for (n = 0; n < len_b - 2; n++) {
                *ptr_Z =
                    ptr_Z[1] + *xn * (*ptr_b) - *yn * (*ptr_a);
                ptr_b++;
                ptr_a++;
                ptr_Z++;
            }
            /* Calculate last delay */
            *ptr_Z = *xn * (*ptr_b) - *yn * (*ptr_a);
        }
        else {
            *yn = *xn * (*ptr_b);
        }

        ptr_y += stride_Y;      /* Move to next input/output point */
        ptr_x += stride_X;
    }

}
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
std::vector<std::complex<double>> sosfilt(double sos[4][6], std::vector<std::complex<double>>& x, std::complex<double> zi[4][2]){
    //TODO
    //TODO
    //TODO
    //TODO
    //TODO
}


//template <typename T>
std::vector<std::complex<double>> sosfiltfilt(double sos[4][6], std::vector<std::complex<double>>& x){
    int edge = 27;
    std::vector<std::complex<double>> ext = validate_pad(x, edge);

    std::complex<double> zi[4][2] = {{ 2.55361726e-06, -2.07641258e-06},
                       { 3.14494957e-04, -2.63952402e-04},
                        { 2.04415018e-02, -1.81862975e-02},
                        { 9.73501515e-01, -9.34387200e-01}};

    std::complex<double> x_0 = ext.front();
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 2; j++){
            zi[i][j] *= x_0;
        }
    }
    std::vector<std::complex<double>> y = sosfilt(sos, x, zi);
}

//template <typename T>
std::vector<std::complex<double>> decimate(std::vector<std::complex<double>>& x, int q){
    int n = 8;
    //some magic numbers :) (from python)
    //bwFM = 200000;
    //decRate = 1800000/bwFM
    //sos = cheby1(8, 0.05, 0.8 / decRate, output='sos')
    double sos[4][6] = {{ 8.13160323e-09,  1.62632065e-08,  8.13160323e-09,  1.00000000e+00, -1.80102222e+00,  8.13719178e-01},
                        {1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.80826915e+00,  8.40588279e-01},
                        {1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.83026610e+00,  8.91360262e-01},
                        {1.00000000e+00,  2.00000000e+00,  1.00000000e+00,  1.00000000e+00, -1.87714628e+00,  9.60659875e-01}};

    std::vector<std::complex<double>> y = sosfiltfilt(sos, x);
}



void demod(uint32_t len){
    double bwFM = 200000;
    int decRate = SAMPLE_RATE/bwFM;

    data = decimate(data, decRate);


    //############################
    //nie wiem czy to o to chodzi w decimate(), raczej trzeba to zmienić
    // x3 = sig.decimate(x2, decRate)

    /*for(int i = 0; i < len; i+=decRate){
        x2[i/decRate] = x1[i];
    }*/



    //int length = ceil(len/decRate);
    //int newFs = SAMPLE_RATE/decRate;
    //############################



    //############################
    //no i ten fragment też nie wiadomo czy działa jak trzeba
    //y4 = x3[1:] * np.conj(x3[:-1])
    //x4 = np.angle(y4)
    ///std::complex<double> tmp;
    //for(int i = 1; i < length; i++){
        //tmp = x2[i-1];
        //tmp.imag(-1*tmp.imag());
        //y4[i-1] = std::arg(x2[i]*tmp);
    //}
    //length--;
    //############################



    //double d = newFs*75e-6;
    //double x = exp(-1/d);
    //double b[1] = {1-x};
    //double a[2] = {1, -x};

    //double delay[2] = { 0 ,0 };

    //############################
    //zajebałem z jakiegoś Githuba, działa zajebiście jak porównywałem z sig.lfilter() z scipy'a
    //x5 = sig.lfilter(b,a,x4)
    //for(int i = 0; i<length; i++){
       // bb_digital_filter(b, a, &y4[i], &x4[i], delay, 1, 1, 1, 1);
    //}
    //############################




    //############################
    //znowu decimate(), tak jak wyżej pewnie do zmiany
    //x6 = sig.decimate(x5, dec_audio)
    //double audioFreq = 44100;
    ////int dec_audio = int(newFs/audioFreq);
    //for(int i = 0; i < length; i+=dec_audio){
        ///x5[i/decRate] = x4[i];
    //}
    ///length = ceil(length/decRate);
    //############################

    ///std::ofstream file("demod_output.txt", std::ios::app);
    //for(int i = 0; i < length; i++){
        //file << x5[i] << '\n';
    //}
    //file.close();
}


static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx){
    for(uint32_t i = 0, q = 1; i < len; i+=2, q+=2){
        data[i/2].real(buf[i]/(255.0/2) - 1);
        data[i/2].imag(buf[q]/(255.0/2) - 1);
    }
    data.resize(6284667);
    std::ifstream file("output.txt");
    double tempReal;
    double tempImag;
    for(int i = 0; i < 6284667; i++){
        std::string temp;
        getline(file, temp);
        sscanf(temp.c_str(), "%lf%lfj", &tempReal, &tempImag);
        data[i].real(tempReal);
        data[i].imag(tempImag);
    }
    file.close();
    demod(6284667);

    //demod(len/2);
    rtlsdr_cancel_async(dev);
}


SDRWorker::SDRWorker(CAudioBuffer *audioBuffer, bool* active){
    this->audioBuffer = audioBuffer;
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

    result = rtlsdr_set_tuner_gain_mode(dev, 0);
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

    int result = rtlsdr_read_async(dev, rtlsdr_callback, NULL, 0, BYTES_TO_READ);
    if(result < 0){
        qDebug() << "RTL-SDR read failed";
    }

    rtlsdr_close(dev);
    qDebug() << "RTL-SDR Thread end";
}

