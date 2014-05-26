#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "OptionWindow.h"
#include "DrawWindow.h"

#include <iostream>
#include <osg/LineWidth>
#include <osg/Node>
#include <QTimer>
#include "Database.h"
#include "ParallelPlaneManager.h"
#include <osgViewer/CompositeViewer>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("massheatmap.ini",QSettings::IniFormat);
    QString hue_image_name = settings.value("hue_image","../images/hue_range.png").toString();
    QString mesh_base_name = settings.value("mesh_base","../data/male_apose_closed.ply").toString();
    QString mesh_segmented_name = settings.value("mesh_segmented","../data/male_apose_closed_segments.obj").toString();

    plane_manager_ = std::move(std::unique_ptr<ParallelPlaneManager>(new ParallelPlaneManager(5,1)));

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    ui->widget->Initialize(plane_manager_.get());
    ui->statusBar->showMessage("Building planes");

    plane_manager_->AddNewPlane("pca0","pca1",true);
    plane_manager_->AddNewPlane("pca2","pca3");

    plane_manager_->AddNewPlane("weightkg","stature");//plane_manager_->AddNewPlane(95,96);
    plane_manager_->AddNewPlane("chestcircumference","span",true);
    plane_manager_->Redraw();

    ui->statusBar->showMessage("Scene Render Complete.");

    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(close()));
    _timer.start( 10 );
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::close(){
   QApplication::quit();
}

void MainWindow::testBarChartDemo(QCustomPlot *customPlot)
{
  // create empty bar chart objects:
  QCPBars *coverage = new QCPBars(customPlot->xAxis, customPlot->yAxis);
  QCPBars *protection = new QCPBars(customPlot->xAxis, customPlot->yAxis);
  customPlot->addPlottable(coverage);
  customPlot->addPlottable(protection);
  // set names and colors:
  QPen pen;
  pen.setWidthF(1.2);
  protection->setName("Protection");
  pen.setColor(QColor(1, 92, 191));
  protection->setPen(pen);
  protection->setBrush(QColor(1, 92, 191, 50));
  coverage->setName("Coverage");
  pen.setColor(QColor(150, 222, 0));
  coverage->setPen(pen);
  coverage->setBrush(QColor(150, 222, 0, 70));
  // stack bars ontop of each other:
  coverage->moveAbove(protection);

  // prepare x axis with country labels:
  QVector<double> ticks;
  QVector<QString> labels;
  ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;
  labels << "Head" << "Torso" << "Arms" << "Legs" << "Hands" << "Feet";
  customPlot->xAxis->setAutoTicks(false);
  customPlot->xAxis->setAutoTickLabels(false);
  customPlot->xAxis->setTickVector(ticks);
  customPlot->xAxis->setTickVectorLabels(labels);
  customPlot->xAxis->setTickLabelRotation(60);
  customPlot->xAxis->setSubTickCount(0);
  customPlot->xAxis->setTickLength(0, 4);
  customPlot->xAxis->grid()->setVisible(true);
  customPlot->xAxis->setRange(0, 8);

  // prepare y axis:
  customPlot->yAxis->setRange(0, 12.1);
  customPlot->yAxis->setPadding(5); // a bit more space to the left border
  customPlot->yAxis->setLabel("Percent of max");
  customPlot->yAxis->grid()->setSubGridVisible(true);
  QPen gridPen;
  gridPen.setStyle(Qt::SolidLine);
  gridPen.setColor(QColor(0, 0, 0, 25));
  customPlot->yAxis->grid()->setPen(gridPen);
  gridPen.setStyle(Qt::DotLine);
  customPlot->yAxis->grid()->setSubGridPen(gridPen);

  // Add data:
  QVector<double> protectionData, coverageData;
  protectionData << 0.68*10.5 << 0.62*5.5 << 0.82*5.5 << 0.50*5.8 << 0.59*5.2 << 0.00*4.2 << 0.37*11.2;
  coverageData   << 0.06*10.5 << 0.05*5.5 << 0.04*5.5 << 0.06*5.8 << 0.02*5.2 << 0.07*4.2 << 0.25*11.2;
  protection->setData(ticks, protectionData);
  coverage->setData(ticks, coverageData);

  // setup legend:
  customPlot->legend->setVisible(true);
  customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
  customPlot->legend->setBrush(QColor(255, 255, 255, 200));
  QPen legendPen;
  legendPen.setColor(QColor(130, 130, 130, 200));
  customPlot->legend->setBorderPen(legendPen);
  QFont legendFont = font();
  legendFont.setPointSize(10);
  customPlot->legend->setFont(legendFont);
  customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
