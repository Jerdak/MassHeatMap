#include "ParallelPlane.h"
#include <osg/Node>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <QSettings>
#include <QString>
#include <cmath>
#include <QTransform>


ParallelPlane::ParallelPlane(Database *db):
    db_(db),
    filter_radius_(-1.0f),
    filter_width_(-1.0f),
    filter_height_(-1.0f),
    filter_angle_(0.0f),
    filter_position_(osg::Vec3f(0,0,0)),
    is_filtered_(false)
{
    QSettings settings("massheatmap.ini",QSettings::IniFormat);

}

float ParallelPlane::Variance(){
    float var0 = db_->get_eigen_value(axes_[0]);
    float var1 = db_->get_eigen_value(axes_[1]);

    if(var0<0 || var1<0)return -1;
    else return var0+var1;
}


osg::Vec3f ParallelPlane::Domain(const int& row){
    float data[2];

    // get data for row[datum][axes_[0] and row[datum][axes_[1]]
    data[0] = (*db_)(row,axes_[0]);
    data[1] = (*db_)(row,axes_[1]);

    // normalize
    osg::Vec3f ret(
        (data[0] - db_->Min(axes_[0]))/(db_->Max(axes_[0])-db_->Min(axes_[0])),     //x
        (data[1] - db_->Min(axes_[1]))/(db_->Max(axes_[1])-db_->Min(axes_[1])),     //y
        0.0f                                                                        //z, OSG treats Z as up.
    );

    return ret;
}

void ParallelPlane::SetAxes(const size_t& axis0, const size_t& axis1){
    if(axis0 >= db_->NumColumns()){
        qDebug() << "Cannot set parallel plane axis to <" << axis0 << ">, out of range";
        return;
    }
    if(axis1 >= db_->NumColumns()){
        qDebug() << "Cannot set parallel plane axis to <" << axis1 << ">, out of range";
        return;
    }
    axes_[0] = axis0;
    axes_[1] = axis1;
}
void ParallelPlane::set_filtered(const bool& b){
    is_filtered_ = b;

    emit filterChanged();

}
void ParallelPlane::SetFilter(const osg::Vec3f& center, const float& height, const float &width, const float &angle){
    filter_height_ = height;
    filter_width_  = width;
    filter_position_ = center;
    filter_angle_ = angle;
    is_filtered_ = true;

    emit filterChanged();
}
void ParallelPlane::SetFilter(const osg::Vec3f& p, const float& radius){
    filter_radius_ = radius;
    filter_position_ = p;
    is_filtered_ = true;

    emit filterChanged();
}
osg::Vec3f ParallelPlane::GetLocalPosition(){
    return osg::Vec3f(0,0,0);
}


bool ParallelPlane::InRange(const int& row){
    return InFilter(row);
}

bool ParallelPlane::InFilterNew(const int& row){
    if(!is_filtered_)return true;

    QTransform trans[3];
    osg::Vec3f tmp = Domain(row);
    QPointF p = QPointF(tmp.x(),tmp.y());


    trans[0].scale(filter_width_,filter_height_);
    trans[1].rotate(filter_angle_);
    trans[2].translate(filter_position_.x(),filter_position_.y());

    {   // apply inversion transforms manually, Qt's transform class doesn't apply transforms in sequence
        p = p * trans[2].inverted() * trans[1].inverted() * trans[0].inverted();    // point in ellipse space
    }

    QLineF pd(p,QPointF(0,0));

    // assume unit ellipse w/ radius/height = 1.0
    return (pd.length() <= 1.0);
}

bool ParallelPlane::InFilter(const int& row){
    return InFilterNew(row);
    if(filter_radius_ < 0)return true;  //hacky way to avoid having another flag

    osg::Vec3f tmp = Domain(row) - filter_position_;
    return tmp.length() <= filter_radius_;
}
