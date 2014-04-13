#include "HeatColor.h"
#include <QString>
#include <QStringRef>

HeatColor::HeatColor(int rs, int bs, int gs, int re, int be, int ge){
    SetStart(rs,gs,bs);
    SetEnd(re,be,ge);
}

osg::Vec4f HeatColor::GetColor(const float& value){
    float r = (1.0f-value)*start_[0] + (value)*end_[0];
    float g = (1.0f-value)*start_[1] + (value)*end_[1];
    float b = (1.0f-value)*start_[2] + (value)*end_[2];
    printf("r: %f\n",r);
    printf("g: %f\n",g);
    printf("b: %f\n",b);
    return osg::Vec4f(r,g,b,1.0f);
}

HeatColor::HeatColor(QString start, QString end){
    SetStart(start);
    SetEnd(end);
}
void HeatColor::SetStart(QString start){
    QString tmp = start.toLower();
    int r = QStringRef(&tmp,0,2).toString().toUInt(0,16);
    int g = QStringRef(&tmp,2,2).toString().toUInt(0,16);
    int b = QStringRef(&tmp,4,2).toString().toUInt(0,16);

    SetStart(r,g,b);
}

void HeatColor::SetStart(int r, int g, int b){
    start_ = osg::Vec3f(r/256.0f,g/256.0f,b/256.0f);
}

void HeatColor::SetEnd(QString end){
    QString tmp = end.toLower();
    int r = QStringRef(&tmp,0,2).toString().toUInt(0,16);
    int g = QStringRef(&tmp,2,2).toString().toUInt(0,16);
    int b = QStringRef(&tmp,4,2).toString().toUInt(0,16);

    SetEnd(r,g,b);
}

void HeatColor::SetEnd(int r, int g, int b){
    end_ = osg::Vec3f(r/256.0f,g/256.0f,b/256.0f);
}
