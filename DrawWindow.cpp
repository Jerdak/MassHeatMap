#include "DrawWindow.h"
#include "ui_DrawWindow.h"
#include "ParallelPlaneManager.h"
#include "ParallelPlane.h"
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QRect>
#include <QLineF>
#include <QSettings>
#include <cmath>
DrawWindow::DrawWindow(ParallelPlaneManager *plane_manager,QWidget *parent) :
    QMainWindow(parent),
    plane_manager_(plane_manager),
    ui(new Ui::DrawWindow),
    left_mouse_down_(false),
    left_mouse_selection_(-1)
{
    ui->setupUi(this);
    this->setMouseTracking(true);

    QSettings settings("massheatmap.ini",QSettings::IniFormat);

    width_ = settings.value("draw_window_size","250").toInt();
    height_ = settings.value("draw_window_size","250").toInt();
    spacing_ = settings.value("draw_window_spacings","10").toInt();
    wheel_scale_ = settings.value("draw_window_wheel_scale","4").toFloat();

    QString tmp_img_name = settings.value("draw_image","gradient3.bmp").toString();
    QImage tmp_img(tmp_img_name);
    draw_image_ = tmp_img.scaled(width_,height_); //rescale image before applying it to the paint surface.
}

DrawWindow::~DrawWindow()
{
    delete ui;
}

void DrawWindow::leftMouseDown(QMouseEvent *event){
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
            break;
        }
    }
    if(left_mouse_selection_!=-1)left_mouse_down_ = true;
    ellipse_height_ = 5.0;
}

void DrawWindow::rightMouseDown(QMouseEvent *event){
    int mouse_selection = -1;
    for(int i = 0; i < rects_.size(); ++i){
        if(rects_[i].contains(event->x(),event->y())){
            int ex = event->x();
            int ey = event->y();
            left_mouse_point_ = QPoint(ex,ey);

            int rx = rects_[i].x();
            int ry = rects_[i].y();

            float x = (ex - rx) / (float)width_;
            float y = 1.0 - ((ey - ry) / (float)height_); // make sure to flip the y value
            mouse_selection = i;
            break;
        }
    }
    if(mouse_selection!=-1)plane_manager_->get_plane(mouse_selection)->set_filtered(false);
}

void DrawWindow::mousePressEvent(QMouseEvent *event){

    if(event->buttons() == Qt::RightButton)rightMouseDown(event);
    else if(event->buttons() == Qt::LeftButton)leftMouseDown(event);

}

void DrawWindow::wheelEvent(QWheelEvent * event){
    if(!left_mouse_down_)return;

    ellipse_height_ += event->delta()/wheel_scale_;
    updateCurrentFilter();
    repaint();
}

void DrawWindow::mouseReleaseEvent(QMouseEvent *event){
    printf("Release\n");
    left_mouse_down_ = false;
    left_mouse_selection_ = -1;

    repaint();
}

void DrawWindow::updateCurrentFilter(){
    if(!left_mouse_down_)return;

    int rx = rects_[left_mouse_selection_].x();
    int ry = rects_[left_mouse_selection_].y();

    //plane_manager_->get_plane(left_mouse_selection_)->SetFilter(osg::Vec3f(o,0.0),d.length());
    QPointF a((left_mouse_point_.x() - rx)/(float)width_,(left_mouse_point_.y() - ry)/(float)height_);
    QPointF b((current_mouse_point_.x() - rx)/(float)width_,(current_mouse_point_.y() - ry)/(float)height_);
    QLineF ab(a,b);
    QPointF center = ab.pointAt(0.5);

    float r = (-(float)ab.length()/2.0f);    // divide by width not necessary, value already normalized;
    float h = (-ellipse_height_/2.0f) / height_;

    plane_manager_->get_plane(left_mouse_selection_)->SetFilter(osg::Vec3f(center.x(),1.0f-center.y(),0.0f),-h,-r,ab.angle());
}

void DrawWindow::mouseMoveEvent(QMouseEvent *event){
    if(!left_mouse_down_)return;

    // verify the mouse is still inside the clicked parallel plane, ignore if not.
    if(!rects_[left_mouse_selection_].contains(event->x(),event->y()))return;

    current_mouse_point_ = QPoint(event->x(),event->y());

    updateCurrentFilter();
    this->repaint();
}
void DrawWindow::paintActiveFilter(QPaintEvent *event, QPainter &painter){
    if(!left_mouse_down_)return;
    QTransform trans;

    QLineF a(left_mouse_point_,current_mouse_point_);
    QPointF c = a.pointAt(0.5);
    trans.translate(c.x(),c.y());
    trans.rotate(-a.angle());
    painter.setTransform(trans);
    painter.setBrush(QBrush(QColor(0,255,0,150)));
    painter.drawEllipse(-(float)a.length()/2.0f,-ellipse_height_/2.0f,(float)a.length(),ellipse_height_);

    float area_ellipse = 3.141592654 * (float)a.length()/2.0f * ellipse_height_/2.0f;
    float area_image = width_ * height_;
    //TODO: replace with percent selection based on number of filtered subjects
    QString txt =  QString("Area: %1\%").arg(area_ellipse/area_image*100);

    painter.drawText(0,0,txt);
}

void DrawWindow::paintActiveSubjects(QPaintEvent *event, QPainter &painter){
    int x = 0;
    int y = 0;

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(255,0,0,255));
    painter.setBrush(brush);
    std::vector<int> subjects = plane_manager_->get_active_subjects();
    for(int i = 0 ; i < subjects.size(); ++i){
        int subject = subjects[i];
        x = 0;
        y = 0;
        for(int p = 0; p < plane_manager_->size(); p++){
            ParallelPlane* pl = plane_manager_->get_plane(p);
            osg::Vec3f pt = pl->ReverseDomain(subject);
            int dx = pt.x() * width_ + x;
            int dy = (1-pt.y()) * height_ + y;//flip, qt images rendered from top left.
            painter.drawEllipse(dx,dy,5,5);
            if(x + 2*width_ + spacing_ > ui->centralwidget->width()){
                y += height_ + spacing_;
                x = 0;
            } else {
                x += width_ + spacing_;
            }

        }
    }
}

void DrawWindow::paintInactiveSubjects(QPaintEvent *event, QPainter &painter){
    int x = 0;
    int y = 0;

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(150,150,150,150));
    painter.setBrush(brush);
    std::vector<int> subjects = plane_manager_->get_inactive_subjects();
    for(int i = 0 ; i < subjects.size(); ++i){
        int subject = subjects[i];
        x = 0;
        y = 0;
        for(int p = 0; p < plane_manager_->size(); p++){
            ParallelPlane* pl = plane_manager_->get_plane(p);
            osg::Vec3f pt = pl->ReverseDomain(subject);
            int dx = pt.x() * width_ + x;
            int dy = (1-pt.y()) * height_ + y;//flip, qt images rendered from top left.
            painter.drawEllipse(dx,dy,5,5);
            if(x + 2*width_ + spacing_ > ui->centralwidget->width()){
                y += height_ + spacing_;
                x = 0;
            } else {
                x += width_ + spacing_;
            }

        }
    }
}
void DrawWindow::paintPlanes(QPaintEvent *event, QPainter &painter){
    int x = 0;
    int y = 0;

    for(int i = 0; i < plane_manager_->size(); i++){
        ParallelPlane* pl = plane_manager_->get_plane(i);
        painter.drawImage(x,y,draw_image_,0,0,width_,height_);
        rects_.push_back( QRect(x,y,width_,height_) );

        if(x + 2*width_ + spacing_ > ui->centralwidget->width()){
            y += height_ + spacing_;
            x = 0;
        } else {
            x += width_ + spacing_;
        }
    }
}
void DrawWindow::paintFilters(QPaintEvent *event, QPainter &painter){
    int x = 0;
    int y = 0;
    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(255,255,255,75));
    painter.setBrush(brush);
    for(int p = 0; p < plane_manager_->size(); p++){
        if(p!=left_mouse_selection_){
            if(plane_manager_->get_plane(p)->IsFiltered()){

                osg::Vec3f pos = plane_manager_->get_plane(p)->get_filter_position();
                QPointF c(pos.x(),pos.y());
                float r = plane_manager_->get_plane(p)->get_filter_width();
                float h = plane_manager_->get_plane(p)->get_filter_height();
                float a = plane_manager_->get_plane(p)->get_filter_angle();

                float cx = c.x() * width_ + x;
                float cy = (1-c.y()) * height_ + y;
                QTransform trans;
                trans.translate(cx,cy);
                trans.rotate(-a);
                painter.setTransform(trans);
                painter.drawEllipse(-r*width_,-h*height_,r*width_*2,h*height_*2);
            }
        }

        if(x + 2*width_ + spacing_ > ui->centralwidget->width()){
            y += height_ + spacing_;
            x = 0;
        } else {
            x += width_ + spacing_;
        }
    }
}
void DrawWindow::paintText(QPaintEvent *event, QPainter &painter){
    int x = 0;
    int y = 0;
    painter.resetMatrix();
    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(0,0,0,255));
    painter.setBrush(brush);
    for(int p = 0; p < plane_manager_->size(); p++){

        painter.drawText(x,y+10, QString("Variance: %1").arg(plane_manager_->get_plane(p)->Variance()));
        if(x + 2*width_ + spacing_ > ui->centralwidget->width()){
            y += height_ + spacing_;
            x = 0;
        } else {
            x += width_ + spacing_;
        }
    }
}

void DrawWindow::paintEvent(QPaintEvent *event){
    if(!ui)return;

    rects_.clear();
    ui->centralwidget->width();

    QBrush  background = QBrush(QColor(67, 67, 67));
    QPainter painter;
    painter.begin(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(event->rect(), background);

        paintPlanes(event,painter);
        paintActiveSubjects(event,painter);
        paintInactiveSubjects(event,painter);
        paintFilters(event,painter);
        paintActiveFilter(event,painter);
        paintText(event,painter);
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
