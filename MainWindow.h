#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ParallelPlane.h"
#include "Database.h"
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow, public osgViewer::CompositeViewer
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    QWidget* addViewWidget( osgQt::GraphicsWindowQt* gw, osg::Node* scene );
    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );
    virtual void paintEvent( QPaintEvent* event ){
        Q_UNUSED(event);
        frame();
    }
    void TestDrawPoints();
private:
    Ui::MainWindow *ui;
    QTimer _timer;
    ParallelPlane *pl_;
    Database db_;
    osg::Geode *geode_;
};

#endif // MAINWINDOW_H
