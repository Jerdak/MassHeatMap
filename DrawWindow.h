#ifndef DRAWWINDOW_H
#define DRAWWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QRect>
#include <vector>

namespace Ui {
class DrawWindow;
}

class ParallelPlaneManager;
class DrawWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit DrawWindow(ParallelPlaneManager *plane_manager,QWidget *parent = 0);
    ~DrawWindow();
protected:
    void updateCurrentFilter();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);

    void leftMouseDown(QMouseEvent *event);
    void rightMouseDown(QMouseEvent *event);

    void paintActiveFilter(QPaintEvent *event, QPainter &painter);
    void paintActiveSubjects(QPaintEvent *event, QPainter &painter);
    void paintInactiveSubjects(QPaintEvent *event, QPainter &painter);
    void paintPlanes(QPaintEvent *event, QPainter &painter);
    void paintFilters(QPaintEvent *event, QPainter &painter);
private:
    float ellipse_height_;
    QPoint left_mouse_point_;
    QPoint current_mouse_point_;
    bool left_mouse_down_;
    int left_mouse_selection_;

    QWidget *canvas_;
    Ui::DrawWindow *ui;
    ParallelPlaneManager *plane_manager_;
    std::vector<QRect> rects_;
    int width_;
    int height_;
    int spacing_;
};

#endif // DRAWWINDOW_H
