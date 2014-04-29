#ifndef DRAWABLEMESH_H
#define DRAWABLEMESH_H

#include <QThread>
#include <QMutex>
#include "CudaWrapper.h"
#include "Database.h"
#include "MeshSegmenter.h"

class DrawableMesh;
// Drawable mesh worker
//
// Threaded handler for asynchronous calls to CUDA to update the
// colored heat map render.
class DrawableWorker : public QObject {
    Q_OBJECT;
public:
    explicit DrawableWorker(DrawableMesh *mesh,Database*db,QObject *parent=0);

signals:
    void colorsUpdated();

public slots:
    // Called in a loop to check if color data needs to be refreshed by CUDA.  Should never be called by the user.
    void checkWork();

    // Request an update from CUDA (only most recent request is honored)
    void requestWork();

    // Kill worker
    void abort();
private:
    void doWork();

    DrawableMesh *mesh_;
    Database *db_;
    bool running_;
    bool abort_;
    bool working_;
    bool request_work_;

    QMutex data_mutex_;

    // Wrapper around CUDA
    CudaWrapper processor_;
};

class DrawableMesh : public QObject
{
    Q_OBJECT;
public:
    DrawableMesh(Database*db);
    ~DrawableMesh(){}

    void Load(const QString file_name);
    void Save(const QString file_name);

    void dbgChangeColor();
    osg::ref_ptr<osg::Node> get_node(){return node_;}
    osg::ref_ptr<osg::Geode> get_geode(){return geode_;}
    osg::ref_ptr<osg::Geometry> get_geometry(){return geom_;}
    osg::ref_ptr<osg::Vec4Array> get_colors(){return colors_;}

    void set_mesh_segmenter(MeshSegmenter *mesh_segmenter){
        mesh_segmenter_ = mesh_segmenter;
    }

    QMutex *get_mutex(){return &data_mutex_;}

public slots:
    void colorsUpdated();
    void requestWork();
private:
    QMutex data_mutex_;
    QThread *worker_thread_;
    DrawableWorker *worker_;
    Database *db_;
    MeshSegmenter *mesh_segmenter_;

    osg::ref_ptr<osg::Node> node_;
    osg::ref_ptr<osg::Geode> geode_;
    osg::ref_ptr<osg::Geometry> geom_;

    osg::ref_ptr<osg::Vec4Array> colors_;
};

#endif // DRAWABLEMESH_H
