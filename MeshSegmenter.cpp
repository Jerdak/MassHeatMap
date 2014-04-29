#include "MeshSegmenter.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QFont>

MeshSegmenter::MeshSegmenter(QCustomPlot *customPlot):
    customPlot_(customPlot)
{
}

void MeshSegmenter::Load(const QString& file_name){
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    int sindex = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        line = line.trimmed();

        if(line[0] == 'v' || line[0] == 'V'){
            QStringList tokens = line.split(' ');
            if(tokens.size() != 7){
                qDebug() << "[ERROR] - Segment line format incorrect: " << line << tokens.size() << "\n";
                continue;
            }
            int segment = (tokens[4].toFloat() * 255.0f + 0.5f);  // segmented data stores same index in red, blue, and green channels.
            if(segment_map_.find(segment) == segment_map_.end()){
                segment_map_[segment] = sindex++;
                segment_max_map_[segment] = 0;
            }
            segment_max_map_[segment] += 1;
            index_map_.push_back(segment);
        }
    }
    if(index_map_.size() != 10777){
        qDebug() << "[ERROR] - MeshSegment map != 10777 vertices: " << index_map_.size() << "\n";
        return;
    }
    {   // hardcoded segments
        segment_name_map_[255] = "right_arm";
        segment_name_map_[230] = "left_arm";
        segment_name_map_[189] = "neck";
        segment_name_map_[139] = "thorax";
        segment_name_map_[81] = "abdomen";
        segment_name_map_[31] = "pelvis";
    }
//    for(auto kvp = segment_name_map_.begin(); kvp != segment_name_map_.end(); kvp++){
//        int segment = kvp->first;
//        if(segment_map_.find(segment)==segment_map_.end()){
//            qDebug() << "Couldn't find segment " << segment << "," << kvp->second << "\n";
//        } else {
//            qDebug() << "Found segment " << segment << "," << kvp->second << "\n";
//        }
//    }
    InitializeWidget();
}

void MeshSegmenter::InitializeWidget(){
   coverage_ = new QCPBars(customPlot_->xAxis, customPlot_->yAxis);
   customPlot_->addPlottable(coverage_);

   QPen pen;
   pen.setWidthF(1.2);
   coverage_->setName("coverage_");
   pen.setColor(QColor(150, 222, 0));
   coverage_->setPen(pen);
   coverage_->setBrush(QColor(150, 222, 0, 70));

   QVector<double> ticks;
   QVector<QString> labels;
   int tick = 1;
   for(auto kvp = segment_name_map_.begin(); kvp != segment_name_map_.end(); kvp++){
        ticks << tick++;
        labels << kvp->second;
        coverage_data_ << 0.0;
   }

   customPlot_->xAxis->setAutoTicks(false);
   customPlot_->xAxis->setAutoTickLabels(false);
   customPlot_->xAxis->setTickVector(ticks);
   customPlot_->xAxis->setTickVectorLabels(labels);
   customPlot_->xAxis->setTickLabelRotation(60);
   customPlot_->xAxis->setSubTickCount(0);
   customPlot_->xAxis->setTickLength(0, 4);
   customPlot_->xAxis->grid()->setVisible(true);
   customPlot_->xAxis->setRange(0, tick);

   // prepare y axis:
   customPlot_->yAxis->setRange(0, 100);
   customPlot_->yAxis->setPadding(5); // a bit more space to the left border
   customPlot_->yAxis->setLabel("Percent of max");
   customPlot_->yAxis->grid()->setSubGridVisible(true);

   QPen gridPen;
   gridPen.setStyle(Qt::SolidLine);
   gridPen.setColor(QColor(0, 0, 0, 25));
   customPlot_->yAxis->grid()->setPen(gridPen);
   gridPen.setStyle(Qt::DotLine);
   customPlot_->yAxis->grid()->setSubGridPen(gridPen);


   coverage_->setData(ticks, coverage_data_);

   // setup legend:
   customPlot_->legend->setVisible(true);
   customPlot_->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
   customPlot_->legend->setBrush(QColor(255, 255, 255, 200));
   QPen legendPen;
   legendPen.setColor(QColor(130, 130, 130, 200));
   customPlot_->legend->setBorderPen(legendPen);
   QFont legendFont = customPlot_->font();
   legendFont.setPointSize(10);
   customPlot_->legend->setFont(legendFont);
   customPlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
   ticks_ = ticks;

}
#include <osg/Array>
#include <osg/ref_ptr>
void MeshSegmenter::Update(osg::ref_ptr<osg::Vec4Array> colors){


    // Add data:
    if(index_map_.size() != colors->size()){
        qDebug() << "[ERROR] - Mismatched index map and colors " << index_map_.size() << " != " << colors->size() <<"\n";
        return;
    }
    std::map<int,float> cov_map;
    for(int i = 0; i < colors->size(); ++i){
        osg::Vec4Array *cs = colors.get();
        osg::Vec4 c = (*cs)[i];
        int segment = index_map_[i];
        if(cov_map.find(segment)==cov_map.end())cov_map[segment] = 0.0;

        if(!(c.x()==0.5 && c.y()==0.5 && c.z()==0.5))cov_map[segment] += 1;
    }
    int ct = 0;
    for(auto kvp = segment_name_map_.begin(); kvp != segment_name_map_.end(); kvp++){
        int segment = kvp->first;
        coverage_data_[ct++] = cov_map[segment]/(float)segment_max_map_[segment] * 100.0f;
    }
    coverage_->setData(ticks_, coverage_data_);

    customPlot_->replot();
}
