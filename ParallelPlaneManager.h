#ifndef PARALLELPLANEMANAGER_H
#define PARALLELPLANEMANAGER_H

#include "Database.h"
#include "ParallelPlane.h"

class ParallelPlaneManager: public QObject
{
    Q_OBJECT
public:
    ParallelPlaneManager(osg::ref_ptr<osg::Node> root_node);
    ParallelPlaneManager(osg::ref_ptr<osg::Node> root_node,const int& width, const int& height);
    ~ParallelPlaneManager();

    // Add a new plane to the plane manager
    void AddNewPlane(const int& axis0, const int& axis1, const bool& rebuild=false);

    // Load database data (MUST BE DONE BEFORE CALLING AddNewPlane())
    void LoadDatabase(const QString& database_name,const QString& pack_name);

    int get_width() { return width_;}
    int get_height() { return height_;}

    void set_width(const int& width) { width_ = width; BuildSpacing();}
    void set_height(const int& height) { height_ = height; BuildSpacing();}

    Database* get_database() {return &db_;}
    ParallelPlane* get_plane(const size_t& idx){return planes_[idx].get();}

    static void dbgDatabaseLoad();

signals:
    void ActiveSubjectsChanged();

public slots:
    void BuildSpacing();
    void Redraw();

private:
    void UpdateActiveSubjects();
    void RedrawPoints(const int& index,osg::Vec3Array* vertices,osg::Vec4Array* colors);
    void RedrawLines(const int& index0,const int& index1,osg::Vec3Array* vertices,osg::Vec4Array* colors);
    void InitializeSceneGraph();

private:
    int width_;
    int height_;

    std::vector<int> active_subjects_;
    Database db_;
    osg::ref_ptr<osg::Node> root_node_;
    osg::ref_ptr<osg::Geode> geode_;
    std::vector<std::unique_ptr<ParallelPlane> > planes_;
};

#endif // PARALLELPLANEMANAGER_H
