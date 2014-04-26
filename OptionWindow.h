#ifndef OPTIONWINDOW_H
#define OPTIONWINDOW_H

#include <QMainWindow>
#include "ParallelPlaneManager.h"
namespace Ui {
class OptionWindow;
}

class OptionWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit OptionWindow(ParallelPlaneManager *plane_manager,QWidget *parent = 0);
    ~OptionWindow();

public slots:
    void updatePlaneManager();
private:
    Ui::OptionWindow *ui;
    ParallelPlaneManager *plane_manager_;
};

#endif // OPTIONWINDOW_H
