#include "ParallelPlaneManager.h"

ParallelPlaneManager::ParallelPlaneManager(osg::ref_ptr<osg::Node> root_node):
    root_node_(root_node)
{
    LoadDatabase("test_nominal_scrub.csv"); //TODO:  remove hardcoded database data
}

ParallelPlaneManager::~ParallelPlaneManager(){
    planes_.clear();
}

void ParallelPlaneManager::dbgDatabaseLoad(){
    Database db;
    db.LoadData("test_nominal_scrub.csv");
    db.dbgData(10);

    QString min_range = "";
    QString max_range = "";
    for(int c = 0; c < db.NumColumns(); c++){
        min_range += QString::number(db.Min(c)) + " ";
        max_range += QString::number(db.Max(c)) + " ";
    }
    qDebug() << "Min Range: " << min_range;
    qDebug() << "Max Range: " << max_range;
}

void ParallelPlaneManager::LoadDatabase(const QString& database_name){
    db_.Clear();
    db_.LoadData(database_name);
}

void ParallelPlaneManager::AddNewPlane(const int &axis0, const int &axis1){
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

    // make sure to attached new transform node to root
    root_node_->asTransform()->asMatrixTransform()->addChild(transform);
    // add geometry node to the transform
    transform->addChild(geode.get());

    {   // create a new plane
        std::unique_ptr<ParallelPlane> pl(new ParallelPlane(geode,transform,&db_));
        pl->SetAxes(axis0,axis1);
        planes_.push_back(std::move(pl));
    }
}

void ParallelPlaneManager::BuildSpacing(const int& width, const int& height){
    float step = width/(float)planes_.size();
    float offset = 0;
    for(int i = 0; i < planes_.size(); ++i){
        planes_[i]->SetPosition(osg::Vec3f(0,0,offset));
        offset -= step;
    }

}
