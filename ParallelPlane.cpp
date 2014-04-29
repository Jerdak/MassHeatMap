#include "ParallelPlane.h"
#include <osg/Node>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <QSettings>
#include <QString>
#include <cmath>
#include <QTransform>
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

        osg::StateSet* stateset = new osg::StateSet;
       // stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

        pictureQuad->setStateSet(stateset);
        pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                    texture,
                    osg::StateAttribute::ON);

        return pictureQuad;
    }
}


ParallelPlane::ParallelPlane(osg::Geode *geode,osg::MatrixTransform *transform,Database *db):
    geode_(geode),
    db_(db),
    transform_(transform),
    filter_radius_(-1.0f),
    filter_width_(-1.0f),
    filter_height_(-1.0f),
    filter_angle_(0.0f),
    filter_position_(osg::Vec3f(0,0,0)),
    is_filtered_(false)
{
    QSettings settings("massheatmap.ini",QSettings::IniFormat);
    QString imageFileName = settings.value("gradient_image","gradient3.bmp").toString();
    QString colorImageFileName = settings.value("color_image","gradient2.bmp").toString();

    image_ = osgDB::readImageFile(imageFileName.toStdString());
    image_color_ = osgDB::readImageFile(colorImageFileName.toStdString());

    osg::ref_ptr<osg::Drawable> drawable = myCreateTexturedQuadGeometry(osg::Vec3(0,0,0),1,1,image_,false,true,false);
    osgText::Font* font = osgText::readFontFile("./fonts/arial_bold.ttf");
    osg::Vec4 layoutColor(0.0f,0.0f,0.0f,1.0f);
    float layoutCharacterSize = 0.05f;
    {
        text_ = new osgText::Text;
        text_->setFont(font);
        text_->setColor(layoutColor);
        text_->setCharacterSize(layoutCharacterSize);
        text_->setPosition(osg::Vec3(0.02,0.1,0.01f));

        // right to left layouts would be used for hebrew or arabic fonts.
        text_->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text_->setFontResolution(20,20);
        geode_->addDrawable(text_);
    }
    geode_->addDrawable(drawable);

}

float ParallelPlane::Variance(){
    float var0 = db_->get_eigen_value(axes_[0]);
    float var1 = db_->get_eigen_value(axes_[1]);

    if(var0<0 || var1<0)return -1;
    else return var0+var1;
}

void ParallelPlane::UpdateText(){

    float var = Variance();

    QString var_text = (var<0)?"NA":QString("Variance:%1").arg(var);
    text_->setPosition(osg::Vec3(0.02,0.1,0.01f));
    QString txt = QString("XAxis: %1\nYAxis: %2\n%3").arg(db_->get_header(axes_[0])).arg(db_->get_header(axes_[1])).arg(var_text);
    printf("Text: %s\n",txt.toStdString().c_str());
    text_->setText(txt.toStdString());

}

void ParallelPlane::SetPosition(osg::Vec3f pos){
    osg::Matrix t;
    t.makeTranslate(pos);

    osg::Matrix m = transform_->getMatrix();
    m.setTrans(osg::Vec3f(0,0,0));  //remove old translation but leave rotation intact
    transform_->setMatrix(m*t);
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
    return image_color_->getColor(coord);
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

    UpdateText();
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
