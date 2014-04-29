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
    ParallelPlane(osg::Geode *geode, osg::MatrixTransform *transform, Database *db);
    ~ParallelPlane() { qDebug() << "~ParallelPlane"; }

    osg::Vec3f GetLocalPosition();

    // Return 3D position of a data point in ParallelPlane coordinate space
    osg::Vec3f Domain(const int& row);

    // Return 3D position of a data point in world space
    osg::Vec3f ReverseDomain(const int& row);

    // Get pixel color where data point falls on textured plane
    osg::Vec4f Color(const int& row);

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

    // Set position of parallel plane
    void SetPosition(osg::Vec3f pos);

    // Return local transformation of plane
    osg::MatrixTransform *get_transform(){return transform_;}

    // Return geometric node containing plane geometry.
    osg::Geode *get_geode() {return geode_;}

    bool InRange(const int& row);
    bool InFilterNew(const int& row);
    bool InFilter(const int& row);

    void UpdateText();

    QString get_text(){return QString(text_->getText().createUTF8EncodedString().c_str());}
private:

    // Flat texture creation utility (pulled from OpenSceneGraph examples)
    osg::Geometry* myCreateTexturedQuadGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane=false, bool option_flip=false);

    bool is_filtered_;

    int index_;
    int width_;
    int height_;

    // Data axes
    int axes_[2];

    // Transformation node controlling this parallel plane
    osg::MatrixTransform *transform_;

    // Geometrci data node containing plane geometry.
    osg::Geode *geode_;

    // Parallel plane texture image
    osg::Image* image_;

    // Parallel plane color image (paints lines and points)
    osg::Image* image_color_;

    osgText::Text* text_;
    // Raw data database.
    Database *db_;

    osg::Vec3f filter_position_;
    float filter_radius_;
    float filter_width_;
    float filter_height_;
    float filter_angle_; //degrees
};

#endif // PARALLELPLANE_H
