#ifndef STABLE_H
#define STABLE_H

#if defined __cplusplus
    #include <QMainWindow>
    #include <QTimer>
    #include <QApplication>
    #include <QGridLayout>
    #include <QFile>
    #include <QString>
    #include <QStringList>
    #include <QDebug>
    #include <QMutex>
    #include <QMutexLocker>

    #include <map>
    #include <vector>
    #include <climits>
    #include <limits>
    #include <algorithm>

    #include <osg/Array>
    #include <osg/Point>
    #include <osg/Geometry>
    #include <osg/Image>
    #include <osg/MatrixTransform>
    #include <osg/TextureRectangle>
    #include <osgViewer/CompositeViewer>
    #include <osgViewer/ViewerEventHandlers>
    #include <osgGA/TrackballManipulator>
    #include <osgDB/ReadFile>
    #include <osgQt/GraphicsWindowQt>
    #include <osg/ShapeDrawable>
    #include <osg/Sequence>
    #include <osg/PolygonMode>
    #include <osg/io_utils>

    #include <osgText/Font>
    #include <osgText/Text>
    #undef min
    #undef max
#endif //__cplusplus

#endif // STABLE_H
