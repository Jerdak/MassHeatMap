#include "ParallelPlane.h"
#include <osg/Node>
#include <osg/Matrix>
#include <osg/MatrixTransform>

osg::Geometry* ParallelPlane::myCreateTexturedQuadGeometry(
        const osg::Vec3& pos,       //Position of textured quads
        float width,                //Width of textured quads (not image width)
        float height,               //Height of textured quads (not image height)
        osg::Image* image,          //Textured quad image
        bool useTextureRectangle,   //Use texture rectangle which allows for asymmetric texture
        bool xyPlane,               //Render textured quads along x/y axes
        bool option_flip)           //Flip raw image
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
void ParallelPlane::SetPosition(osg::Vec3f pos){
    osg::Matrix t;
    t.makeTranslate(pos);

    osg::Matrix m = transform_->getMatrix();
    m.setTrans(osg::Vec3f(0,0,0));  //remove old translation but leave rotation intact
    transform_->setMatrix(m*t);
}

ParallelPlane::ParallelPlane(osg::Geode *geode,osg::MatrixTransform *transform,Database *db):
    geode_(geode),
    db_(db),
    transform_(transform),
    filter_radius_(-1.0f),
    filter_position_(osg::Vec3f(0,0,0))
{
    image_ = osgDB::readImageFile("gradient2.bmp");
    osg::ref_ptr<osg::Drawable> drawable = myCreateTexturedQuadGeometry(osg::Vec3(0,0,0),1,1,image_,false,true,false);

//    osg::Matrix t;
//    t.makeTranslate(osg::Vec3f(0,12,0));

  //  transform_->setMatrix(t);
   // transform_->addDrawable(geode_);
//    transform_->addChild(drawable.get());
   // geode_->asTransform()->asMatrixTransform()->setMatrix(t);
    geode_->addDrawable(drawable);
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

osg::Vec3f ParallelPlane::ReverseDomain(const int& row){
    osg::Matrix m = transform_->getMatrix();
    return Domain(row) * m;
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

void ParallelPlane::SetFilter(const osg::Vec3f& p, const float& radius){
    filter_radius_ = radius;
    filter_position_ = p;

    emit filterChanged();
}

osg::Vec3f ParallelPlane::GetLocalPosition(){
    return osg::Vec3f(0,0,0);
}


bool ParallelPlane::InRange(const int& row){
    return InFilter(row);
}

bool ParallelPlane::InFilter(const int& row){
    if(filter_radius_ < 0)return true;  //hacky way to avoid having another flag

    osg::Vec3f tmp = Domain(row) - filter_position_;
    return tmp.length() <= filter_radius_;
}
