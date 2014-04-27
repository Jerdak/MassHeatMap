#include "ParallelPlaneManager.h"
#include "PrincipalComponentAnalyzer.h"
#include <Qobject>
#include <QSettings>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <QFileInfo>

ParallelPlaneManager::ParallelPlaneManager(osg::ref_ptr<osg::Node> root_node):
    root_node_(root_node),
    data_mutex_(QMutex::Recursive)
{
    QSettings settings("massheatmap.ini",QSettings::IniFormat);
    QString pcaFileName = settings.value("data_file","pca.csv").toString();
    QString covFileName = settings.value("coverage_file","./coverage_data/pack_test.csv").toString();
    use_cached_database_ = settings.value("use_cached_database","true").toBool();

    LoadDatabase(pcaFileName,covFileName);
    InitializeSceneGraph();
}

ParallelPlaneManager::ParallelPlaneManager(osg::ref_ptr<osg::Node> root_node,const int& width, const int& height):
    root_node_(root_node),
    width_(width),
    height_(height),
    data_mutex_(QMutex::Recursive)
{
    QSettings settings("massheatmap.ini",QSettings::IniFormat);
    QString pcaFileName = settings.value("data_file","pca.csv").toString();
    QString covFileName = settings.value("coverage_file","./coverage_data/pack_test.csv").toString();
    use_cached_database_ = settings.value("use_cached_database","true").toBool();

    LoadDatabase(pcaFileName,covFileName);
    InitializeSceneGraph();
}

ParallelPlaneManager::~ParallelPlaneManager(){
    root_node_->asTransform()->asMatrixTransform()->removeChild(geode_);
    planes_.clear();
}

void ParallelPlaneManager::InitializeSceneGraph(){
    geode_ = new osg::Geode();

    osg::StateSet* stateset = new osg::StateSet;
    stateset->setAttributeAndModes(new osg::Point(4.0f),osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    osg::ref_ptr<osg::Geometry> pointGeom = new osg::Geometry();
    pointGeom->setUseVertexBufferObjects(false);
    pointGeom->setStateSet(stateset);

    stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(4.0f);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    osg::ref_ptr<osg::Geometry> lineGeom = new osg::Geometry();
    lineGeom->setUseVertexBufferObjects(false);
    lineGeom->setStateSet(stateset);

    geode_->addDrawable(pointGeom);
    geode_->addDrawable(lineGeom);

    root_node_->asTransform()->asMatrixTransform()->addChild(geode_);
}


void ParallelPlaneManager::RedrawPoints(const int& index, osg::Vec3Array* vertices,osg::Vec4Array* colors){
    ParallelPlane *pl = planes_[index].get();



    for(auto subject = active_subjects_.begin(); subject != active_subjects_.end(); ++subject){
        vertices->push_back(pl->ReverseDomain((*subject))+osg::Vec3(0,0.0,0.01));
        vertices->push_back(pl->ReverseDomain((*subject))-osg::Vec3(0,0.0,0.01));
        colors->push_back(osg::Vec4f(0.9,0.9,0.9,1.0));//pl->Color((*subject)));
    }
}

void ParallelPlaneManager::RedrawLines(const int& index0,const int& index1,osg::Vec3Array* vertices,osg::Vec4Array* colors){
    ParallelPlane *pl0 = planes_[index0].get();
    ParallelPlane *pl1 = planes_[index1].get();

    //TODO: remove hardcoded "currently selected"
    ParallelPlane *current_plane = planes_[0].get();

    for(auto subject = active_subjects_.begin(); subject != active_subjects_.end(); ++subject){
        vertices->push_back(pl0->ReverseDomain((*subject)));
        vertices->push_back(pl1->ReverseDomain((*subject)));
        colors->push_back(current_plane->Color((*subject)));
        colors->push_back(current_plane->Color((*subject)));
    }
}

void ParallelPlaneManager::Redraw(){
    UpdateActiveSubjects();

    osg::Geometry* pointsGeom = geode_->getDrawable(0)->asGeometry();
    osg::Geometry* linesGeom = geode_->getDrawable(1)->asGeometry();

    {   //redraw points
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
        vertices->push_back(osg::Vec3f(0,0,0)); //origin
        vertices->push_back(osg::Vec3f(0,1,0)); //y+
        vertices->push_back(osg::Vec3f(1,0,0)); //x+

        colors->push_back(osg::Vec4f(1,0,0,1));
        colors->push_back(osg::Vec4f(0,1,0,1));
        colors->push_back(osg::Vec4f(0,0,1,1));
        for(int p = 0; p < planes_.size(); ++p){
            RedrawPoints(p,vertices,colors);
        }
        pointsGeom->setVertexArray(vertices);
        pointsGeom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);

        if(pointsGeom->getNumPrimitiveSets()==0){
            pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));
        } else {
            osg::DrawArrays *draw = (osg::DrawArrays*)pointsGeom->getPrimitiveSet(0);
            draw->setCount(vertices->size());
        }
    }
    {   //redraw lines
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
        for(int p = 0; p < planes_.size()-1; ++p){
            RedrawLines(p,p+1,vertices,colors);
        }
        linesGeom->setVertexArray(vertices);
        linesGeom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);

        if(linesGeom->getNumPrimitiveSets()==0){
            linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,vertices->size()));
        } else {
            osg::DrawArrays *draw = (osg::DrawArrays*)linesGeom->getPrimitiveSet(0);
            draw->setCount(vertices->size());
        }
    }
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

void ParallelPlaneManager::LoadDatabase(const QString& database_name,const QString& pack_name){
    QMutexLocker locker(&data_mutex_);
    bool load_raw = true;
    if(use_cached_database_){
        QFileInfo fi("db_cache.csv");
        if(fi.exists()){
            qDebug() << "Loading cached database from db_cache.csv\n";
            load_raw = false;
        } else {
            load_raw = true;
        }
    }

    db_.Clear();
    if(load_raw){
        qDebug() << "Loading raw database from" << database_name << "\n";
        Database db_msrs;
        db_msrs.LoadData(database_name);
        PrincipalComponentAnalyzer pca;
        Database db_pca = pca.GeneratePCATable(db_msrs);
        for(size_t c = 0; c < db_msrs.NumColumns();++c){
            db_pca.append_column(db_msrs.get_column(c),db_msrs.get_header(c),false);
        }
        db_pca.set_subjects(db_msrs.get_subjects());
        db_pca.BuildMetrics();

        db_pca.SaveData("db_cache.csv");
        db_.set_eigen_values(db_pca.get_eigen_values());
    }

    db_.LoadData("db_cache.csv");
    db_.LoadCoveragePack(pack_name);

    db_.dbgVerifyIntegrity();
    db_.BuildMetrics();
}

void ParallelPlaneManager::AddNewPlane(const int &axis0, const int &axis1, const bool& rebuild){
    QMutexLocker locker(&data_mutex_);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

    // make sure to attached new transform node to root
    root_node_->asTransform()->asMatrixTransform()->addChild(transform);
    // add geometry node to the transform
    transform->addChild(geode.get());

    {   // create a new plane
        std::unique_ptr<ParallelPlane> pl(new ParallelPlane(geode,transform,&db_));
        pl->SetAxes(axis0,axis1);
        connect(pl.get(),SIGNAL(filterChanged()),this,SLOT(Redraw()));
        planes_.push_back(std::move(pl));
    }

    if(rebuild)BuildSpacing();
}

void ParallelPlaneManager::AddNewPlane(const QString &axis0, const QString &axis1, const bool& rebuild){
    AddNewPlane(db_.get_header_index(axis0),db_.get_header_index(axis1),rebuild);
}

void ParallelPlaneManager::BuildSpacing(){
    QMutexLocker locker(&data_mutex_);
    float step = width_/(float)planes_.size();
    float offset = 0;
    for(int i = 0; i < planes_.size(); ++i){
        planes_[i]->SetPosition(osg::Vec3f(0,0,offset));
        offset -= step;
    }
}

std::vector<int> ParallelPlaneManager::get_active_subjects(){
    QMutexLocker locker(&data_mutex_);
    return active_subjects_;
}
std::vector<int> ParallelPlaneManager::get_inactive_subjects(){
    QMutexLocker locker(&data_mutex_);
    return inactive_subjects_;
}
void ParallelPlaneManager::UpdateActiveSubjects(){
    {
        QMutexLocker locker(&data_mutex_);
        active_subjects_.clear();
        inactive_subjects_.clear();
        for(int i = 0; i < db_.NumRows(); i++){
            bool active = true;
            for(int p = 0; p < planes_.size(); ++p){
                if(!planes_[p]->InRange(i)){
                    active = false;
                    inactive_subjects_.push_back(i);
                    break;
                }
            }
            if(active)active_subjects_.push_back(i);
        }
        db_.set_inactive_subjects(inactive_subjects_);
    }
    emit ActiveSubjectsChanged();
}
