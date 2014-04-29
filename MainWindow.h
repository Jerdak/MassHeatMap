#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ParallelPlaneManager.h"
#include "Database.h"
#include "DrawableMesh.h"
#include "qcustomplot.h"
#include "MeshSegmenter.h"
namespace Ui {
    class MainWindow;
}
class OptionWindow;
class DrawWindow;
class MainWindow : public QMainWindow, public osgViewer::CompositeViewer
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    QWidget* addParallelPlaneWidget( osgQt::GraphicsWindowQt* gw);
    QWidget* addModellerWidget( osgQt::GraphicsWindowQt* gw, osg::ref_ptr<osg::Node> scene );

    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );
    virtual void paintEvent( QPaintEvent* event ){
        Q_UNUSED(event);
        frame();
    }
    void testBarChartDemo(QCustomPlot *customPlot);

private:
    Ui::MainWindow *ui;
    QTimer _timer;
    std::unique_ptr<DrawableMesh> mesh_;
    std::unique_ptr<ParallelPlaneManager> plane_manager_;
    osg::MatrixTransform *parent_transform_;
    OptionWindow *option_window_;
    DrawWindow *draw_window_;
    MeshSegmenter *mesh_seg;
private slots:
    void close();
};

#endif // MAINWINDOW_H
