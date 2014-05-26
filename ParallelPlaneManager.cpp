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

ParallelPlaneManager::ParallelPlaneManager():
    data_mutex_(QMutex::Recursive)
{
    QSettings settings("massheatmap.ini",QSettings::IniFormat);
    QString pcaFileName = settings.value("data_file","pca.csv").toString();
    QString covFileName = settings.value("coverage_file","./coverage_data/pack_test.csv").toString();
    use_cached_database_ = settings.value("use_cached_database","true").toBool();

    LoadDatabase(pcaFileName,covFileName);
}

ParallelPlaneManager::ParallelPlaneManager(const int& width, const int& height):
    width_(width),
    height_(height),
    data_mutex_(QMutex::Recursive)
{
    QSettings settings("massheatmap.ini",QSettings::IniFormat);
    QString pcaFileName = settings.value("data_file","pca.csv").toString();
    QString covFileName = settings.value("coverage_file","./coverage_data/pack_test.csv").toString();
    use_cached_database_ = settings.value("use_cached_database","true").toBool();

    LoadDatabase(pcaFileName,covFileName);
}

ParallelPlaneManager::~ParallelPlaneManager(){
    planes_.clear();
}

void ParallelPlaneManager::Redraw(){
    QMutexLocker locker(&data_mutex_);
    UpdateActiveSubjects();
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

    {   // create a new plane
        std::unique_ptr<ParallelPlane> pl(new ParallelPlane(&db_));
        pl->SetAxes(axis0,axis1);
        connect(pl.get(),SIGNAL(filterChanged()),this,SLOT(Redraw()));
        planes_.push_back(std::move(pl));
    }
}

void ParallelPlaneManager::AddNewPlane(const QString &axis0, const QString &axis1, const bool& rebuild){
    AddNewPlane(db_.get_header_index(axis0),db_.get_header_index(axis1),rebuild);
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
        for(size_t i = 0; i < db_.NumRows(); i++){
            bool active = true;
            for(size_t p = 0; p < planes_.size(); ++p){
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
