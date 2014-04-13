#include "CudaWrapper.h"
#include <CudaTest/CudaKernel.hpp>
#include <QElapsedTimer>
#include <QColor>
CudaWrapper::CudaWrapper()
{
    //cuda_main(PINNED);

    //replace direct CUDA calls with calls to Thrust (impl'd as TBB right now)

}
#include <QLinearGradient>

#include "HeatColor.h"
void CudaWrapper::Reduce(osg::ref_ptr<osg::Vec4Array> colors_,const std::vector<int> active_subjects,const unsigned int& R, const unsigned int&C,const float *data,int size){
    if(R != colors_->getNumElements()){
        qDebug() << QString("[ERROR] - Reduction color array size must match row count").arg(QString::number(R),QString::number(colors_->getNumElements()));
        return;
    }
    const std::vector<int> inactive_subjects;
    //void color_pack(const std::vector<int> indices,const unsigned int& R, const unsigned int& C,float *colors,float *data);
    float *colors = new float[colors_->getNumElements()];

    printf("debug: ");
    for(int i = 0; i < 20; i++){
        printf("%f ",data[i]);
    }
    printf("\n");
    color_pack(inactive_subjects,R,C,colors,data);

    qDebug() << "Color[0]" << colors[0];
    float minc = colors[0];
    float maxc = colors[0];
    for(int c = 1; c < colors_->getNumElements(); ++c){
        minc = std::min(colors[c],minc);

        if(maxc < 1.0)
        maxc = std::max(colors[c],maxc);
    }

     //colorbrewer2 single hue gradient.  #fcfbfd(whitish) -> #3f007d(dark purple)
    HeatColor gradient_hue("fcfbfd","3f007d");

    for(int c = 0; c < colors_->getNumElements(); ++c){
        float value = (colors[c]-minc)/(maxc-minc);/// temp_scalar;//(26.0f * 1.5f);
        value = std::max(0.0f, std::min(value, 1.0f));

        if(value < 0.05)
            (*colors_)[c] = osg::Vec4f(0.5f,0.5f,0.5f,1.0f);
        else
            (*colors_)[c] = gradient_hue.GetColor(value);   //TODO:  hardcoded normalization based on 26 subjects with a per cell max of 1.5
    }
    delete [] colors;
}

