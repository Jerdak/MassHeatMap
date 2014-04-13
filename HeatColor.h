#ifndef HEATCOLOR_H
#define HEATCOLOR_H

// Gradient color class
//
// Quickly computes the interpolated value between 2 colors.
class HeatColor
{
public:
    HeatColor(int rs, int bs, int gs, int re, int be, int ge);
    HeatColor(QString start, QString end);

    // Get interpolated color, value range = [0,1], return values between [0,1]
    osg::Vec4f GetColor(const float& value);

    // Set start color, color in hex format (no leading '#')
    void SetStart(QString start);

    // Set start color, colors range from [0,255]
    void SetStart(int r, int g, int b);

    // Set end color, color in hex format (no leading '#')
    void SetEnd(QString end);

    // Set end color, colors range from [0,255]
    void SetEnd(int r, int g, int b);

private:
    osg::Vec3f start_;
    osg::Vec3f end_;
};

#endif // HEATCOLOR_H
