/********************************************************************************
** Form generated from reading UI file 'DrawWindow.ui'
**
** Created: Tue Apr 29 02:51:14 2014
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DRAWWINDOW_H
#define UI_DRAWWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DrawWindow
{
public:
    QWidget *centralwidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *DrawWindow)
    {
        if (DrawWindow->objectName().isEmpty())
            DrawWindow->setObjectName(QString::fromUtf8("DrawWindow"));
        DrawWindow->resize(800, 600);
        centralwidget = new QWidget(DrawWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(60, 30, 401, 261));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        DrawWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DrawWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        DrawWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(DrawWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        DrawWindow->setStatusBar(statusbar);

        retranslateUi(DrawWindow);

        QMetaObject::connectSlotsByName(DrawWindow);
    } // setupUi

    void retranslateUi(QMainWindow *DrawWindow)
    {
        DrawWindow->setWindowTitle(QApplication::translate("DrawWindow", "PCA Photo Booth", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DrawWindow: public Ui_DrawWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DRAWWINDOW_H
