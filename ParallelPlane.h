#ifndef PARALLELPLANE_H
#define PARALLELPLANE_H

#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osgText/Text>
#include "Database.h"

class ParallelPlane : public QObject
{
    Q_OBJECT
signals:
    void filterChanged();
public:
    ParallelPlane(Database *db);
    ~ParallelPlane() { qDebug() << "~ParallelPlane"; }

    osg::Vec3f GetLocalPosition();

    // Return 3D position of a data point in ParallelPlane coordinate space
    osg::Vec3f Domain(const int& row);

    // Set data axes for data set
    void SetAxes(const size_t& axis0, const size_t& axis1);

    void SetFilter(const osg::Vec3f& p, const float& radius);
    void SetFilter(const osg::Vec3f& center, const float& height, const float& width, const float& angle);

    float Variance();
    osg::Vec3f  get_filter_position(){ return filter_position_;}
    float  get_filter_width() { return filter_width_; }
    float  get_filter_height(){ return filter_height_;}
    float  get_filter_angle(){ return filter_angle_;}

    bool IsFiltered() { return is_filtered_;}
    void set_filtered(const bool& b);

    bool InRange(const int& row);
    bool InFilterNew(const int& row);
    bool InFilter(const int& row);


private:

     bool is_filtered_;

    int index_;
    int width_;
    int height_;

    // Data axes
    int axes_[2];

    // Raw data database.
    Database *db_;

    osg::Vec3f filter_position_;
    float filter_radius_;
    float filter_width_;
    float filter_height_;
    float filter_angle_; //degrees
};

#endif // PARALLELPLANE_H
