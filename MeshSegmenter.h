#ifndef MESHSEGMENTER_H
#define MESHSEGMENTER_H

#include <QString>
#include <QVector>
#include <vector>
#include <map>
#include "qcustomplot.h"
class MeshSegmenter : public QObject
{
    Q_OBJECT
public:
    MeshSegmenter(QCustomPlot *customPlot);

    void Load(const QString& file_name);
    void InitializeWidget();
public slots:
    void Update(osg::ref_ptr<osg::Vec4Array> colors);

private:
    std::vector<int> index_map_;
    std::map<int,int> segment_map_;
    std::map<int,int> segment_max_map_;
    std::map<int,QString> segment_name_map_;
    QCustomPlot *customPlot_;
    QVector<double> ticks_;
    QCPBars *coverage_;
    QVector<double> coverage_data_;
};

#endif // MESHSEGMENTER_H
