#include "CudaWrapper.h"
#include "HeatColor.h"

#include <CudaTest/CudaKernel.hpp>
#include <QElapsedTimer>
#include <QColor>
#include <QLinearGradient>
#include <QSettings>


CudaWrapper::CudaWrapper():
    valid_data_thresh_(0.01f),
    colors_(NULL)
{
    QSettings settings("massheatmap.ini",QSettings::IniFormat);
    valid_data_thresh_ = settings.value("valid_data_threshold","0.05").toFloat();
}
CudaWrapper::~CudaWrapper(){

    if(colors_)delete [] colors_;
}

void CudaWrapper::NormalizedHistogram(float *colors, const int& color_size, float &minc, float &maxc, const int& size){
    int valid_size = 0;

    memset(hist_,0.0f,sizeof(float)*CUDA_HISTOGRAM_BINS);

    for(int c = 0; c < color_size; ++c){
        colors[c] /= size;
    }
   // Metrics(colors,color_size,minc,maxc);
    minc = 0.0f;
    maxc = 0.05f;
    for(int c = 0; c < color_size; ++c){
        float normalized_value = (colors[c]-minc)/(maxc-minc);

        // count # of colors contributing to histogram
        if(validColor(normalized_value))valid_size+=1;
    }

    {   // build histogram of cumulative probabilities
        float step = 1.0f / (float)valid_size;
        for(int c = 0; c < color_size; ++c){
            float normalized_value = (colors[c]-minc)/(maxc-minc);
            int bin = normalized_value * (CUDA_HISTOGRAM_BINS-1);
            bin = std::max(0, std::min(bin, CUDA_HISTOGRAM_BINS-1));    //clamp [0,CUDA_HISTOGRAM_BINS-1]

            // make sure to only bin valid data
            if(validColor(normalized_value)) hist_[bin] += step;
        }
    }
    {   // build a lookup_ table in range [0,CUDA_HISTOGRAM_BINS]
        float total = 0.0f;
        for(int i = 0; i < CUDA_HISTOGRAM_BINS; i++){
            total += hist_[i];
            lookup_[i] =  total * (float)(CUDA_HISTOGRAM_BINS - 1.0f) + 0.5f;
        }
    }
}
void CudaWrapper::Metrics(float *colors, const int& color_size, float &minc, float &maxc){
    for(int c = 1; c < color_size; ++c){
        minc = std::min(colors[c],minc);
        maxc = std::max(colors[c],maxc);
    }
}

bool CudaWrapper::validColor(const float& f){
    return (f > valid_data_thresh_);
}

void CudaWrapper::PreprocessPack(const int& color_size,const std::vector<int>& inactive_subjects,const unsigned int& R, const unsigned int&C,const float *data,int size){
    if(R != color_size){
        qDebug() << QString("[ERROR] - Reduction color array size must match row count").arg(QString::number(R),QString::number(color_size));
        return;
    }
    if(!colors_)colors_ = new float[color_size];

    // Call CUDA
    color_pack(inactive_subjects,R,C,colors_,data);
}

void CudaWrapper::Reduce(osg::ref_ptr<osg::Vec4Array> colors,const std::vector<int>& inactive_subjects,const unsigned int& R, const unsigned int&C,const float *data,int size){
    if(R != colors->getNumElements()){
        qDebug() << QString("[ERROR] - Reduction color array size must match row count").arg(QString::number(R),QString::number(colors->getNumElements()));
        return;
    }
    float minc,maxc;
    if(!colors_)colors_ = new float[colors->getNumElements()];

    // Call CUDA
    color_pack(inactive_subjects,R,C,colors_,data);

    // Generate a normalization histogram
    NormalizedHistogram(colors_,colors->getNumElements(),minc,maxc,C-inactive_subjects.size());

    //Colorbrewer2 single hue gradient.  #fcfbfd(whitish) -> #3f007d(dark purple)
    HeatColor gradient_hue("fcfbfd","3f007d");

    for(int c = 0; c < colors->getNumElements(); ++c){
        float normalized_value = (colors_[c]-minc)/(maxc-minc);      //color value in [0,1]
        int bin = normalized_value * (CUDA_HISTOGRAM_BINS-1);       //bin in         [0,CUDA_HISTOGRAM_BINS-1]
        bin = std::max(0, std::min(bin, CUDA_HISTOGRAM_BINS-1));    //clamp bin to   [0,CUDA_HISTOGRAM_BINS-1]

        float delta = lookup_[bin] / (float)CUDA_HISTOGRAM_BINS;     //delta value in [0,1]

        // For valid colors: Linearly interpolate between end points of gradient_hue.
        if(validColor(normalized_value))
            (*colors)[c] = gradient_hue.GetColor(normalized_value);
        else
            (*colors)[c] = osg::Vec4f(0.5f,0.5f,0.5f,1.0f);
    }
}

