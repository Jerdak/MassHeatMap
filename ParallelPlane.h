#ifndef PARALLELPLANE_H
#define PARALLELPLANE_H

#include <osg/Vec3f>
#include <osg/Vec4f>
#include "Database.h"

class ParallelPlane
{
public:
    ParallelPlane(osg::Geode *geode, osg::MatrixTransform *transform, Database *db);
    ~ParallelPlane() { qDebug() << "~ParallelPlane"; }

    osg::Vec3f GetLocalPosition();

    osg::Vec3f Domain(const int& row);
    osg::Vec4f Color(const int& row);
    osg::Vec3f ReverseDomain(const int& row);

    void SetAxes(const size_t& axis0, const size_t& axis1);
    void SetPosition(osg::Vec3f pos);
    osg::MatrixTransform *get_transform(){return transform_;}
    osg::Geode *get_geode() {return geode_;}

private:
    osg::Geometry* myCreateTexturedQuadGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane=false, bool option_flip=false);

    int width_;
    int height_;

    int axes_[2];
    osg::MatrixTransform *transform_;
    osg::Geode *geode_;
    osg::Image* image_;
    Database *db_;
};

#endif // PARALLELPLANE_H
