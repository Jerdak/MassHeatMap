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
    void AddNewPlane(const QString &axis0, const QString &axis1, const bool& rebuild=false);

    // Load database data (MUST BE DONE BEFORE CALLING AddNewPlane())
    void LoadDatabase(const QString& database_name,const QString& pack_name);

    int get_width() { return width_;}
    int get_height() { return height_;}

    void set_width(const int& width) { width_ = width; BuildSpacing();}
    void set_height(const int& height) { height_ = height; BuildSpacing();}

    Database* get_database() {return &db_;}
    ParallelPlane* get_plane(const size_t& idx){return planes_[idx].get();}
    size_t size(){return planes_.size();}

    static void dbgDatabaseLoad();

    std::vector<int> get_active_subjects();
    std::vector<int> get_inactive_subjects();
signals:
    void ActiveSubjectsChanged();

public slots:
    // Build spacing between parallel planes
    void BuildSpacing();

    // Redraw parallel planes
    void Redraw();

private:
    void UpdateActiveSubjects();
    void RedrawPoints(const int& index,osg::Vec3Array* vertices,osg::Vec4Array* colors);
    void RedrawLines(const int& index0,const int& index1,osg::Vec3Array* vertices,osg::Vec4Array* colors);
    void InitializeSceneGraph();

private:

    bool use_cached_database_;

    // Lockable mutex to control threaded plane access
    QMutex data_mutex_;

    // Width of plane view window
    int width_;

    // Height of plane view window
    int height_;

    // Subjects not filtered out by any plane
    std::vector<int> active_subjects_;

    // Subjects filtered out by at least 1 plane
    std::vector<int> inactive_subjects_;

    // Raw data
    Database db_;

    // Root parallel plane node
    osg::ref_ptr<osg::Node> root_node_;

    // Root parallel plane geometry node
    osg::ref_ptr<osg::Geode> geode_;

    // All the planes
    std::vector<std::unique_ptr<ParallelPlane> > planes_;
};

#endif // PARALLELPLANEMANAGER_H
