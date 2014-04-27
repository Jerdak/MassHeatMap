#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "OptionWindow.h"
#include "DrawWindow.h"

#include <iostream>
#include <osg/LineWidth>
#include <osg/Node>
#include <QTimer>
#include "Database.h"
#include <osgViewer/CompositeViewer>
#include <osg/MatrixTransform>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    parent_transform_ = new osg::MatrixTransform();

    plane_manager_ = std::move(std::unique_ptr<ParallelPlaneManager>(new ParallelPlaneManager(parent_transform_,5,1)));
    mesh_ = std::move(std::unique_ptr<DrawableMesh>(new DrawableMesh(plane_manager_->get_database())));
    mesh_->Load("male_apose_closed.ply");
    connect(plane_manager_.get(),SIGNAL(ActiveSubjectsChanged()),mesh_.get(),SLOT(requestWork()));

    QWidget* widget1 = addParallelPlaneWidget( createGraphicsWindow(0,0,100,100) );
    QWidget* widget2 = addModellerWidget( createGraphicsWindow(0,0,100,100), mesh_->get_node() );

    // USE SINGLE THREADED.  OSG does not play nice with Qt Threads, setting this to anythign else results
    // in numerous crashes in OSG's draw methods.
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);
    ui->setupUi(this);

    ui->gridLayout->addWidget(widget1,0,0);
    ui->gridLayout_2->addWidget(widget2,0,0);
    ui->statusBar->showMessage("Building planes");

    plane_manager_->AddNewPlane("pca0","pca1",true);
    plane_manager_->AddNewPlane("pca2","pca3");

    plane_manager_->AddNewPlane("weightkg","stature");//plane_manager_->AddNewPlane(95,96);
    plane_manager_->AddNewPlane("chestcircumference","span",true);
    plane_manager_->Redraw();

    ui->statusBar->showMessage("Scene Render Complete.");
    testBarChartDemo(ui->plotWidget);
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(close()));

    draw_window_ = new DrawWindow(plane_manager_.get(),this);
    draw_window_->show();
    _timer.start( 10 );
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::close(){
   QApplication::quit();
}
QWidget* MainWindow::addParallelPlaneWidget( osgQt::GraphicsWindowQt* gw)
{
    osgViewer::View* view = new osgViewer::View;

    addView( view );

    osg::Camera* camera = view->getCamera();
    camera->setGraphicsContext( gw );

    const osg::GraphicsContext::Traits* traits = gw->getTraits();

    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 1000.0f );


    //TODO: orthographic camera (osg TrackballManipulator doesn't support ortho zoom so we'll need a custom camera
    //camera->setProjectionMatrixAsOrtho(0.0f, 100,0.0f,100, 1.0f, 10000.0f );

    {   // Manually rotate parallel plane widget to correct orientation (TODO:  Fix this hack with something cleaner)
        osg::Matrix m1 = osg::Matrix::rotate(-1.57,osg::Vec3f(1,0,0));
        osg::Matrix m2 = osg::Matrix::rotate(3.14,osg::Vec3f(0,0,1));
        osg::Matrix m3 = osg::Matrix::rotate(-1.57,osg::Vec3f(0,0,1));
        parent_transform_->setMatrix(m2*m1*m3);
    }
    view->setSceneData(parent_transform_);
    view->addEventHandler( new osgViewer::StatsHandler );
    view->setCameraManipulator( new osgGA::TrackballManipulator);

    return gw->getGLWidget();
}

QWidget* MainWindow::addModellerWidget( osgQt::GraphicsWindowQt* gw, osg::ref_ptr<osg::Node> scene)
{
    osgViewer::View* view = new osgViewer::View;
    addView( view );
    osg::Camera* camera = view->getCamera();
    camera->setGraphicsContext( gw );

    const osg::GraphicsContext::Traits* traits = gw->getTraits();

    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 1000.0f );

    //TODO: orthographic camera (osg TrackballManipulator doesn't support ortho zoom so we'll need a custom camera
    //camera->setProjectionMatrixAsOrtho(0.0f, 100,0.0f,100, 1.0f, 10000.0f );

    {   // Manually rotate model widget to correct orientation (TODO:  Fix this hack with something cleaner)
        osg::MatrixTransform *t = new osg::MatrixTransform();
        osg::Matrix m1 = osg::Matrix::rotate(1.57,osg::Vec3f(1,0,0));
        t->setMatrix(m1);
        t->addChild(scene);

        view->setSceneData(t);
    }
    view->addEventHandler( new osgViewer::StatsHandler );
    view->setCameraManipulator( new osgGA::TrackballManipulator );

    return gw->getGLWidget();
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
osgQt::GraphicsWindowQt* MainWindow::createGraphicsWindow( int x, int y, int w, int h, const std::string& name, bool windowDecoration )
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    return new osgQt::GraphicsWindowQt(traits.get());
}
