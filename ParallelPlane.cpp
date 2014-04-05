#include "ParallelPlane.h"
#include <osg/Node>

osg::Geometry* myCreateTexturedQuadGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip)
{
    bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
    if (option_flip) flip = !flip;

    if (useTextureRectangle)
    {
        osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
                                           osg::Vec3(width,0.0f,0.0f),
                                           xyPlane ? osg::Vec3(0.0f,height,0.0f) : osg::Vec3(0.0f,0.0f,height),
                                           0.0f, flip ? image->t() : 0.0, image->s(), flip ? 0.0 : image->t());

        osg::TextureRectangle* texture = new osg::TextureRectangle(image);
        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);


        pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                                                                        texture,
                                                                        osg::StateAttribute::ON);

        return pictureQuad;
    }
    else
    {
        osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
                                           osg::Vec3(width,0.0f,0.0f),
                                           xyPlane ? osg::Vec3(0.0f,height,0.0f) : osg::Vec3(0.0f,0.0f,height),
                                           0.0f, flip ? 1.0f : 0.0f , 1.0f, flip ? 0.0f : 1.0f);

        osg::Texture2D* texture = new osg::Texture2D(image);
        texture->setResizeNonPowerOfTwoHint(false);
        texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);


        pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                    texture,
                    osg::StateAttribute::ON);

        return pictureQuad;
    }
}

ParallelPlane::ParallelPlane(osg::Geode *node,Database *db):
    node_(node),
    db_(db)
{
    image_ = osgDB::readImageFile("gradient.bmp");
    osg::ref_ptr<osg::Drawable> drawable = myCreateTexturedQuadGeometry(osg::Vec3(0,0,0),1,1,image_,false,true,false);
    node_->addDrawable(drawable);
}


osg::Vec3f ParallelPlane::Domain(const int& row){
    float data[2];

    // get data for row[datum][axes_[0] and row[datum][axes_[1]]
    data[0] = (*db_)(row,axes_[0]);
    data[1] = (*db_)(row,axes_[1]);

    osg::Vec3f ret(
        (data[0] - db_->Min(axes_[0]))/(db_->Max(axes_[0])-db_->Min(axes_[0])),     //x
        (data[1] - db_->Min(axes_[1]))/(db_->Max(axes_[1])-db_->Min(axes_[1])),     //y
        0.0f                                                                       //z
    );

    return ret;
}
osg::Vec4f ParallelPlane::Color(const int& row){
    float data[2];

    // get data for row[datum][axes_[0] and row[datum][axes_[1]]
    data[0] = (*db_)(row,axes_[0]);
    data[1] = (*db_)(row,axes_[1]);

    osg::Vec2f coord(
        (data[0] - db_->Min(axes_[0]))/(db_->Max(axes_[0])-db_->Min(axes_[0])),    //u
        (data[1] - db_->Min(axes_[1]))/(db_->Max(axes_[1])-db_->Min(axes_[1]))     //v
    );
    return image_->getColor(coord);
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

osg::Vec3f ParallelPlane::GetLocalPosition(){
    return osg::Vec3f(0,0,0);
}
