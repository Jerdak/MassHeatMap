#include "OptionWindow.h"
#include "ui_OptionWindow.h"

OptionWindow::OptionWindow(ParallelPlaneManager *plane_manager,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OptionWindow),
    plane_manager_(plane_manager)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(updatePlaneManager()));
}

OptionWindow::~OptionWindow()
{
    delete ui;
}

void OptionWindow::updatePlaneManager(){

    float x = ui->tbPositionX->toPlainText().toFloat();
    float y = ui->tbPositionY->toPlainText().toFloat();
    float radius = ui->tbRadius->toPlainText().toFloat();
    plane_manager_->get_plane(0)->SetFilter(osg::Vec3f(x,y,0.0),radius);
}
