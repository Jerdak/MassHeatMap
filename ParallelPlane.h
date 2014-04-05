#ifndef PARALLELPLANE_H
#define PARALLELPLANE_H

#include <osg/Vec3f>
#include <osg/Vec4f>
#include "Database.h"

class ParallelPlane
{
public:
    ParallelPlane(osg::Geode *node, Database *db);

    osg::Vec3f GetLocalPosition();

    osg::Vec3f Domain(const int& row);
    osg::Vec4f Color(const int& row);
    osg::Vec3f ReverseDomain(const osg::Vec3f& v);

    void SetAxes(const size_t& axis0, const size_t& axis1);
private:
    int width_;
    int height_;

    int axes_[2];
    osg::Node *scene_;
    osg::Geode *node_;
    osg::Image* image_;
    Database *db_;
};

#endif // PARALLELPLANE_H
