#ifndef DRAWABLEMESH_H
#define DRAWABLEMESH_H

#include <QThread>
#include <QMutex>
#include "CudaWrapper.h"
#include "Database.h"

class DrawableMesh;
class DrawableWorker : public QObject {
    Q_OBJECT;
public:
    explicit DrawableWorker(DrawableMesh *mesh,Database*db,QObject *parent=0);
    void doWork();
signals:
    void colorsUpdated();

public slots:
    void checkWork();
    void requestWork();
    void abort();
private:
    DrawableMesh *mesh_;
    Database *db_;
    bool running_;
    bool abort_;
    bool work_;

    QMutex data_mutex_;
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

    QMutex *get_mutex(){return &data_mutex_;}

public slots:
    void colorsUpdated();
    void requestWork();
private:
    QMutex data_mutex_;
    QThread *worker_thread_;
    DrawableWorker *worker_;
    Database *db_;
    osg::ref_ptr<osg::Node> node_;
    osg::ref_ptr<osg::Geode> geode_;
    osg::ref_ptr<osg::Geometry> geom_;

    osg::ref_ptr<osg::Vec4Array> colors_;
};

#endif // DRAWABLEMESH_H
