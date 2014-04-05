#include "MainWindow.h"
#include "ui_MainWindow.h"


#include <iostream>
#include <osg/LineWidth>
#include "Database.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //db_.LoadData("test_nominal_scrub.csv");

    parent_transform_ = new osg::MatrixTransform();
    plane_manager_ = std::move(std::unique_ptr<ParallelPlaneManager>(new ParallelPlaneManager(parent_transform_)));

    QWidget* widget1 = addParallelPlaneWidget( createGraphicsWindow(0,0,100,100) );
    QWidget* widget2 = addModellerWidget( createGraphicsWindow(0,0,100,100), osgDB::readNodeFile("brain.ply") );
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);
    ui->setupUi(this);

    ui->gridLayout->addWidget(widget1,0,0);
    ui->gridLayout_2->addWidget(widget2,0,0);
    ui->statusBar->showMessage("Building planes");

    plane_manager_->AddNewPlane(76,92);
    plane_manager_->AddNewPlane(3,15);
    plane_manager_->AddNewPlane(67,83);
    ui->statusBar->showMessage("Respacing");

    plane_manager_->BuildSpacing(5,1);
    ui->statusBar->showMessage("Drawing points");

    TestDrawPoints(0);
    TestDrawPoints(1);


    ui->statusBar->showMessage("Scene Render Complete.");
    testBarChartDemo(ui->plotWidget);
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

void MainWindow::TestDrawPoints(const size_t& idx){
    //TODO:  Right now points/lines are drawn relative
    //to the geode that created them (ie: local coordinates)
    //This makes it a challenge to draw lines correctly
    //because the endpoint will be offset by the parallel plane's
    //position.  EIther we need to subject the current plane's transform
    //from the end point or points/lines shouldn't be drawn in local coordinates

    ParallelPlane *pl = plane_manager_->get_plane(idx);
    ParallelPlane *pl2 = plane_manager_->get_plane(idx+1);

    Database *db = plane_manager_->get_database();
    osg::Geode *geode = pl->get_geode();
    osg::Geode *geode2 = pl2->get_geode();

    {
        // create Geometry object to store all the vertices and points primitive.
        osg::Geometry* pointsGeom = new osg::Geometry();

        // create a Vec3Array and add to it all my coordinates.
        // Like all the *Array variants (see include/osg/Array) , Vec3Array is derived from both osg::Array
        // and std::vector<>.  osg::Array's are reference counted and hence sharable,
        // which std::vector<> provides all the convenience, flexibility and robustness
        // of the most popular of all STL containers.
        osg::Vec3Array* vertices = new osg::Vec3Array;

        for(int i = 0; i < db->NumRows(); i++){
            vertices->push_back(pl->Domain(i));
        }
        // pass the created vertex array to the points geometry object.
        pointsGeom->setVertexArray(vertices);

        osg::Vec4Array* colors = new osg::Vec4Array;
        for(int i = 0; i < db->NumRows(); i++){
            colors->push_back(pl->Color(i));
        }
        pointsGeom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);

        osg::StateSet* stateset = new osg::StateSet;
        stateset->setAttributeAndModes(new osg::Point(3.0f),osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        pointsGeom->setStateSet(stateset);
        pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

        // add the points geometry to the geode.
        geode->addDrawable(pointsGeom);
    }
    {
        osg::Geometry* linesGeom = new osg::Geometry();
        osg::Vec3Array* vertices = new osg::Vec3Array();
        for(int i = 0; i < db->NumRows(); i++){
            vertices->push_back(pl->Domain(i));
            vertices->push_back(pl2->ReverseDomain(i));
        }

        osg::Vec4Array* colors = new osg::Vec4Array();
        for(int i = 0; i < db->NumRows(); i++){
            colors->push_back(pl->Color(i));
            colors->push_back(pl->Color(i));
        }

        linesGeom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
        linesGeom->setVertexArray(vertices);

        linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,vertices->getNumElements()));

        osg::StateSet* stateset = new osg::StateSet;
        osg::LineWidth* linewidth = new osg::LineWidth();
        linewidth->setWidth(4.0f);
        stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        linesGeom->setStateSet(stateset);

        // add the points geomtry to the geode.
        geode->addDrawable(linesGeom);
    }
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

    view->setSceneData(parent_transform_);
    view->addEventHandler( new osgViewer::StatsHandler );
    view->setCameraManipulator( new osgGA::TrackballManipulator );

    return gw->getGLWidget();
}
QWidget* MainWindow::addModellerWidget( osgQt::GraphicsWindowQt* gw, osg::Node* scene)
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

    view->setSceneData(scene);
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
