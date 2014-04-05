#ifndef PARALLELPLANEMANAGER_H
#define PARALLELPLANEMANAGER_H

#include "Database.h"
#include "ParallelPlane.h"

class ParallelPlaneManager
{
public:
    ParallelPlaneManager(osg::ref_ptr<osg::Node> root_node);
    ~ParallelPlaneManager();

    // Add a new plane to the plane manager
    void AddNewPlane(const int& axis0, const int& axis1);

    // Compute the spacing between all parallel planes and apply settings
    // to planes
    void BuildSpacing(const int& width, const int& height);

    // Load database data (MUST BE DONE BEFORE CALLING AddNewPlane())
    void LoadDatabase(const QString& database_name);

    Database* get_database() {return &db_;}
    ParallelPlane* get_plane(const size_t& idx){return planes_[idx].get();}

    static void dbgDatabaseLoad();
private:
    Database db_;
    osg::ref_ptr<osg::Node> root_node_;
    std::vector<std::unique_ptr<ParallelPlane>> planes_;
};

#endif // PARALLELPLANEMANAGER_H
