#include "MainWindow.h"
#include "ui_MainWindow.h"


#include <iostream>
#include <osg/LineWidth>
#include "Database.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    db_.LoadData("test_nominal_scrub.csv");

    QWidget* widget1 = addViewWidget( createGraphicsWindow(0,0,100,100), osgDB::readNodeFile("fountain.osgt") );
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);
    ui->setupUi(this);
    //ui->dockWidget->setWidget(widget1);

    ui->gridLayout->addWidget(widget1,0,0);

    TestDrawPoints();
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );

//    Database db;
//    db.LoadData("test_nominal_scrub.csv");
//    db.dbgData(10);

//    QString min_range = "";
//    QString max_range = "";
//    for(int c = 0; c < db.NumColumns(); c++){
//        min_range += QString::number(db.Min(c)) + " ";
//        max_range += QString::number(db.Max(c)) + " ";
//    }
//    qDebug() << "Min Range: " << min_range;
//    qDebug() << "Max Range: " << max_range;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TestDrawPoints(){

    {
        // create Geometry object to store all the vertices and points primitive.
        osg::Geometry* pointsGeom = new osg::Geometry();

        // create a Vec3Array and add to it all my coordinates.
        // Like all the *Array variants (see include/osg/Array) , Vec3Array is derived from both osg::Array
        // and std::vector<>.  osg::Array's are reference counted and hence sharable,
        // which std::vector<> provides all the convenience, flexibility and robustness
        // of the most popular of all STL containers.
        osg::Vec3Array* vertices = new osg::Vec3Array;

        for(int i = 0; i < db_.NumRows(); i++){
            vertices->push_back(pl_->Domain(i));
        }
        // pass the created vertex array to the points geometry object.
        pointsGeom->setVertexArray(vertices);

        osg::Vec4Array* colors = new osg::Vec4Array;
        for(int i = 0; i < db_.NumRows(); i++){
            colors->push_back(pl_->Color(i));
        }
        pointsGeom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);

        osg::StateSet* stateset = new osg::StateSet;
        stateset->setAttributeAndModes(new osg::Point(3.0f),osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        pointsGeom->setStateSet(stateset);
        pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

        // add the points geometry to the geode.
        geode_->addDrawable(pointsGeom);
    }
    {
        osg::Geometry* linesGeom = new osg::Geometry();
        osg::Vec3Array* vertices = new osg::Vec3Array();
        for(int i = 0; i < db_.NumRows(); i++){
            vertices->push_back(pl_->Domain(i));
            vertices->push_back(pl_->Domain(i) + osg::Vec3f(0,0,-1));
        }

        osg::Vec4Array* colors = new osg::Vec4Array();
        for(int i = 0; i < db_.NumRows(); i++){
            colors->push_back(pl_->Color(i));
            colors->push_back(pl_->Color(i));
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
        geode_->addDrawable(linesGeom);

    }
}
QWidget* MainWindow::addViewWidget( osgQt::GraphicsWindowQt* gw, osg::Node* scene)
{
    Q_UNUSED(scene);

    osgViewer::View* view = new osgViewer::View;
    addView( view );

    osg::Camera* camera = view->getCamera();
    camera->setGraphicsContext( gw );

    const osg::GraphicsContext::Traits* traits = gw->getTraits();

    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

    osg::Geode *geode = new osg::Geode();
    geode_ = geode;
    pl_ = new ParallelPlane(geode,&db_);
    pl_->SetAxes(76,92);    //76 = statute, 92 = weight

//    {
//        osg::Geometry* linesGeom = new osg::Geometry();

//        // this time we'll prealloacte the vertex array to the size we
//        // need and then simple set them as array elements, 8 points
//        // makes 4 line segments.
//        osg::Vec3Array* vertices = new osg::Vec3Array(8);
//        (*vertices)[0].set(-1.13704, -2.15188e-09, 0.40373);
//        (*vertices)[1].set(-0.856897, -2.15188e-09, 0.531441);
//        (*vertices)[2].set(-0.889855, -2.15188e-09, 0.444927);
//        (*vertices)[3].set(-0.568518, -2.15188e-09, 0.40373);
//        (*vertices)[4].set(-1.00933, -2.15188e-09, 0.370773);
//        (*vertices)[5].set(-0.716827, -2.15188e-09, 0.292498);
//        (*vertices)[6].set(-1.07936, 9.18133e-09, 0.317217);
//        (*vertices)[7].set(-0.700348, 9.18133e-09, 0.362533);

//        osg::Vec4 color(1.0f,1.0f,0.0f,1.0f);


//        osg::Vec4Array* colors = new osg::Vec4Array(8);
//        (*colors)[0] = color;
//        (*colors)[1] = color;
//        (*colors)[2] = color;
//        (*colors)[3] = color;
//        (*colors)[4] = color;
//        (*colors)[5] = color;
//        (*colors)[6] = color;
//        (*colors)[7] = color;

//         linesGeom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
//        // pass the created vertex array to the points geometry object.
//        linesGeom->setVertexArray(vertices);

//        // set the colors as before, plus using the aobve
////        osg::Vec4Array* colors = new osg::Vec4Array;
////        colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
////        linesGeom->setColorArray(colors);
////        linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);


//        // set the normal in the same way color.
////        osg::Vec3Array* normals = new osg::Vec3Array;
////        normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
//        //linesGeom->setNormalArray(normals);
//        //linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);


//        // This time we simply use primitive, and hardwire the number of coords to use
//        // since we know up front,
//        linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,8));
//        osg::StateSet* stateset = new osg::StateSet;
//       osg::LineWidth* linewidth = new osg::LineWidth();
//       linewidth->setWidth(4.0f);
//       stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
//       stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
//           linesGeom->setStateSet(stateset);
//        //        osg::LineWidth* linewidth = new osg::LineWidth();
////        linewidth->setWidth(4.0f);
////        linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth,
////        osg::StateAttribute::ON);

//        // add the points geomtry to the geode.
//        geode->addDrawable(linesGeom);
//    }
    view->setSceneData(geode);
   // view->setSceneData( scene );
    view->addEventHandler( new osgViewer::StatsHandler );
    view->setCameraManipulator( new osgGA::TrackballManipulator );

    return gw->getGLWidget();
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
