#include "DrawableMesh.h"

#include <QTimer>
#include <QEventLoop>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <qglobal.h>
#include "CudaWrapper.h"
#include "Database.h"

DrawableWorker::DrawableWorker(DrawableMesh *mesh,Database*db,QObject *parent):
    QObject(parent),
    mesh_(mesh),
    abort_(false),
    running_(false),
    work_(false),
    db_(db),
    data_mutex_(QMutex::Recursive)
{
}

void DrawableWorker::requestWork(){
    QMutexLocker lock(&data_mutex_);
    work_ = true;
}

void DrawableWorker::checkWork(){
    {   //lock and abort
        QMutexLocker lock(&data_mutex_);
        if(abort_) {
            return;
        }
    }
    {   //lock and work (if work has been queued
        QMutexLocker lock(&data_mutex_);
        if(work_)doWork();
    }
    QTimer::singleShot(15,this,SLOT(checkWork()));
}

void DrawableWorker::doWork(){
    {   //lock and abort
        QMutexLocker lock(&data_mutex_);
        if(abort_) {
            return;
        }
    }

    {   //color
        QMutexLocker lock(mesh_->get_mutex());
        processor_.Reduce(
                    mesh_->get_colors(),
                    db_->get_inactive_subjects(),
                    db_->NumPackRows(),
                    db_->NumPackColumns(),
                    db_->get_coverage_data(),
                    db_->NumPackRows()*db_->NumPackColumns());
    }
    work_ = false;
    emit colorsUpdated();
}

void DrawableWorker::abort(){
    QMutexLocker locker(&data_mutex_);
    abort_ = false;
    qDebug() << "Request worker shutdown";
}

DrawableMesh::DrawableMesh(Database*db):
    db_(db)
{
    worker_thread_ = new QThread();
    worker_ = new DrawableWorker(this,db);
    worker_->moveToThread(worker_thread_);

    connect(worker_,SIGNAL(colorsUpdated()),this,SLOT(colorsUpdated()));
    connect(qApp,SIGNAL(aboutToQuit()),worker_,SLOT(abort()));

    worker_thread_->start();
}

void DrawableMesh::requestWork(){
    QTimer::singleShot(1,worker_,SLOT(requestWork()));
}

void DrawableMesh::colorsUpdated(){
    QMutexLocker lock(&data_mutex_);
    geom_->setColorArray(colors_, osg::Array::BIND_PER_VERTEX);
}

void DrawableMesh::dbgChangeColor(){
    return;

    QMutexLocker lock(&data_mutex_);
    osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geom_->getVertexArray());
    for(int i = 0; i < vertices->getNumElements(); ++i){
        if(i%10==0) (*colors_)[i] = osg::Vec4f(1,0,0,1);
        else {
            (*colors_)[i] = osg::Vec4f(0.5f,0.5f,0.5f,1.0f);
        }
    }
    geom_->setColorArray(colors_, osg::Array::BIND_PER_VERTEX);
}
#include <osg/Geometry>
void DrawableMesh::Load(const QString file_name){
    {
        QMutexLocker lock(&data_mutex_);
        node_ = osgDB::readNodeFile(file_name.toStdString().c_str());
        geode_ = node_->asGeode();
        if(!geode_){
            qDebug() << "Node is not a geode";
            return;
        }

        osg::Drawable *drawable = geode_->getDrawable(0);
        if(!drawable){
            qDebug() << "Geode does not have a drawable child(0)";
            return;
        }

        geom_ = drawable->asGeometry();
        geom_->setUseVertexBufferObjects(false);                        //set to false to allow runtime color manipulation
        geom_->setDataVariance(osg::Geometry::DataVariance::DYNAMIC);   //set to dynamic to optimize model for changing data

        if(!geom_){
            qDebug() << "Drawable is not a Geometry";
            return;
        }
        if(colors_){
            colors_->clear();
        }
        colors_= new osg::Vec4Array();
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geom_->getVertexArray());
        for(int i = 0; i < vertices->getNumElements(); ++i){
            colors_->push_back(osg::Vec4f(0.5f,0.5f,0.5f,1.0f));
        }
        geom_->setColorArray(colors_, osg::Array::BIND_PER_VERTEX);
    }

    // Start a single request to build first heatmap
    QTimer::singleShot(1,worker_,SLOT(requestWork()));
    QTimer::singleShot(1,worker_,SLOT(checkWork()));
}

void DrawableMesh::Save(const QString file_name){

}
