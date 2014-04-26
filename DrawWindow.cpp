#include "DrawWindow.h"
#include "ui_DrawWindow.h"
#include "ParallelPlaneManager.h"
#include "ParallelPlane.h"
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QRect>
#include <QLineF>

#include <cmath>
DrawWindow::DrawWindow(ParallelPlaneManager *plane_manager,QWidget *parent) :
    QMainWindow(parent),
    plane_manager_(plane_manager),
    ui(new Ui::DrawWindow)
{
    ui->setupUi(this);
    this->setMouseTracking(true);

    width_ = 250;
    height_ = 250;
    spacing_ = 10;

    left_mouse_down_ = false;

}

DrawWindow::~DrawWindow()
{
    delete ui;
}

void DrawWindow::mousePressEvent(QMouseEvent *event){

    for(int i = 0; i < rects_.size(); ++i){
        if(rects_[i].contains(event->x(),event->y())){
            int ex = event->x();
            int ey = event->y();
            left_mouse_point_ = QPoint(ex,ey);

            int rx = rects_[i].x();
            int ry = rects_[i].y();

            float x = (ex - rx) / (float)width_;
            float y = 1.0 - ((ey - ry) / (float)height_); // make sure to flip the y value
            left_mouse_selection_ = i;
           // plane_manager_->get_plane(i)->SetFilter(osg::Vec3f(x,y,0.0),0.25f);
        }
    }
    left_mouse_down_ = true;
    ellipse_height_ = 5.0;
}

void DrawWindow::wheelEvent(QWheelEvent * event){
    ellipse_height_ += event->delta()/4.0f;
    repaint();
}

void DrawWindow::mouseReleaseEvent(QMouseEvent *event){
    printf("Release\n");
    left_mouse_down_ = false;

}
void DrawWindow::mouseMoveEvent(QMouseEvent *event){
    if(!left_mouse_down_)return;

    // verify the mouse is still inside the clicked parallel plane, ignore if not.
    if(!rects_[left_mouse_selection_].contains(event->x(),event->y()))return;

    int ex = event->x();
    int ey = event->y();
    int rx = rects_[left_mouse_selection_].x();
    int ry = rects_[left_mouse_selection_].y();

    current_mouse_point_ = QPoint(ex,ey);
    // Get normalized x/y coords for left mouse down position (origin)
    float xo = (left_mouse_point_.x() - rx) / (float)width_;
    float yo = 1.0 - ((left_mouse_point_.y() - ry) / (float)height_); // make sure to flip the y value

    // Get normalized x/y coords for current left mouse position
    float xc = (ex - rx) / (float)width_;
    float yc = 1.0 - ((ey - ry) / (float)height_); // make sure to flip the y value

    osg::Vec2f o(xo,yo);    //original
    osg::Vec2f c(xc,yc);    //current
    osg::Vec2f d = c-o;     //direction
    //plane_manager_->get_plane(left_mouse_selection_)->SetFilter(osg::Vec3f(o,0.0),d.length());
    QLineF a(left_mouse_point_,current_mouse_point_);
    QPointF center = a.pointAt(0.5);
    QTransform trans;
    trans.translate(center.x(),center.y());
   // trans.rotate(-a.angle());
//   / center = QPointF(-(float)a.length()/2.0f,-ellipse_height_/2.0f) * trans;
    center = QPointF(-(float)a.length()/2.0f,-ellipse_height_/2.0f);
    xo = (center.x() - rx) / (float)width_;
    yo = (center.y() - ry) / (float)height_;

    printf("xo: %f, yo: %f\n",xo,yo);
   // painter.setTransform(trans);
  // painter.drawEllipse(-(float)a.length()/2.0f,-ellipse_height_/2.0f,(float)a.length(),ellipse_height_);
    plane_manager_->get_plane(left_mouse_selection_)->SetFilter(osg::Vec3f(xo,yo,0.0f),ellipse_height_,(float)a.length(),-a.angle());
    this->repaint();
}

void DrawWindow::paintEvent(QPaintEvent *event){
    if(!ui)return;

    rects_.clear();
    ui->centralwidget->width();

    QBrush  background = QBrush(QColor(64, 32, 64));
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), background);
    int x = 0;
    int y = 0;

    QImage img("gradient3.bmp");
    QImage simg = img.scaled(width_,height_); //rescale image before applying it to the paint surface.

    //printf("Window width = %d\n",ui->centralwidget->width());
    for(int i = 0; i < plane_manager_->size(); i++){
        ParallelPlane* pl = plane_manager_->get_plane(i);
        painter.drawImage(x,y,simg,0,0,width_,height_);
        rects_.push_back( QRect(x,y,width_,height_) );
        //printf("(%d,%d)->(%d,%d)\n",x,y,width_,height_);

        if(x + 2*width_ + spacing_ > ui->centralwidget->width()){
            y += height_ + spacing_;
            x = 0;
        } else {
            x += width_ + spacing_;
        }
    }

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    std::vector<int>& subjects = plane_manager_->get_active_subjects();
    for(int i = 0 ; i < subjects.size(); ++i){
        int subject = subjects[i];
        int x = 0;
        int y = 0;
        for(int p = 0; p < plane_manager_->size(); p++){
            ParallelPlane* pl = plane_manager_->get_plane(p);
            osg::Vec3f pt = pl->ReverseDomain(subject);
            int dx = pt.x() * width_ + x;
            int dy = (1-pt.y()) * height_ + y;
            //dy = height_ - dy; //flip, qt images rendered from top left.
            painter.drawEllipse(dx,dy,5,5);
            if(p == plane_manager_->size()-1){
              //printf("(%d,%d)->(%d,%d)\n",dx,dy,width_,height_);
            }
            if(x + 2*width_ + spacing_ > ui->centralwidget->width()){
                y += height_ + spacing_;
                x = 0;
            } else {
                x += width_ + spacing_;
            }

        }
    }

    if(left_mouse_down_){
        QTransform trans;

        QLineF a(left_mouse_point_,current_mouse_point_);
        QPointF c = a.pointAt(0.5);
        trans.translate(c.x(),c.y());
        trans.rotate(-a.angle());
        painter.setTransform(trans);
        painter.drawEllipse(-(float)a.length()/2.0f,-ellipse_height_/2.0f,(float)a.length(),ellipse_height_);
    }
    painter.end();
//    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
//         gradient.setColorAt(0.0, Qt::white);
//         gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));
//    QPainter painter;
//    QBrush  background = QBrush(QColor(64, 32, 64));
//    QBrush  circleBrush = QBrush(gradient);
//    QPen circlePen = QPen(Qt::black);
//    circlePen.setWidth(1);
//    QPen textPen = QPen(Qt::white);
//    QFont textFont; textFont.setPixelSize(50);
//    painter.begin(this);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.fillRect(event->rect(), background);
//    //painter.translate(100, 100);

//    painter.save();
//    painter.setBrush(circleBrush);
//    painter.setPen(circlePen);
//    painter.drawImage(0,0,QImage("gradient2.bmp"),200,200);
//    int elapsed = 1;
//    painter.rotate(elapsed * 0.030);

//    qreal r = elapsed/1000.0;
//    int n = 30;
//    for (int i = 0; i < n; ++i) {
//        painter.rotate(30);
//        qreal radius = 0 + 120.0*((i+r)/n);
//        qreal circleRadius = 1 + ((i+r)/n)*20;
//        painter.drawEllipse(QRectF(radius, -circleRadius,
//                                    circleRadius*2, circleRadius*2));
//    }
//    painter.restore();

//    painter.setPen(textPen);
//    painter.setFont(textFont);
//    painter.drawText(QRect(-50, -50, 100, 100), Qt::AlignCenter, "Qt");
//    painter.end();
//    printf("Redraw\n");
}
