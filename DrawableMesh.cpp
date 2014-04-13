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
    db_(db)
{
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
       // processor_.dbgColor(mesh_->get_colors());
        processor_.Reduce(
                    mesh_->get_colors(),
                    db_->get_active_subjects(),
                    db_->NumPackRows(),
                    db_->NumPackColumns(),
                    db_->get_coverage_data(),
                    db_->NumPackRows()*db_->NumPackColumns());
//        QElapsedTimer time;time.start();
//        osg::ref_ptr<osg::Vec4Array> colors = mesh_->get_colors();
//        int high = 10;
//        int low = 1;

//        int step = qrand() % ((high + 1) - low) + low;
//        float r =  qrand() % ((high + 1) - low) + low;
//        float g =  qrand() % ((high + 1) - low) + low;
//        float b =  qrand() % ((high + 1) - low) + low;

//        r/=10.0f;
//        g/=10.0f;
//        b/=10.0f;
//        for(int i = 0; i < colors->getNumElements(); ++i){
//            if(i%step==0) (*colors)[i] = osg::Vec4f(r,g,b,1);
//            else {
//                (*colors)[i] = osg::Vec4f(0.5f,0.5f,0.5f,1.0f);
//            }
//        }
        //int difference = time.elapsed();
    }

    emit colorsUpdated();

    // loop back around :)
   // QTimer::singleShot(100,this,SLOT(doWork()));
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
    qDebug() << "Work requested on drawable mesh";
    QTimer::singleShot(1,worker_,SLOT(doWork()));
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
        geom_->setUseVertexBufferObjects(false);    //set to false to allow runtime color manipulation
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
    worker_->doWork();
}

void DrawableMesh::Save(const QString file_name){

}