/********************************************************************************
** Form generated from reading UI file 'OptionWindow.ui'
**
** Created: Thu Apr 24 12:30:51 2014
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONWINDOW_H
#define UI_OPTIONWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OptionWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QTextEdit *tbPositionX;
    QTextEdit *tbPositionY;
    QTextEdit *tbRadius;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *OptionWindow)
    {
        if (OptionWindow->objectName().isEmpty())
            OptionWindow->setObjectName(QString::fromUtf8("OptionWindow"));
        OptionWindow->resize(800, 600);
        centralwidget = new QWidget(OptionWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 781, 101));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        tbPositionX = new QTextEdit(verticalLayoutWidget);
        tbPositionX->setObjectName(QString::fromUtf8("tbPositionX"));

        verticalLayout->addWidget(tbPositionX);

        tbPositionY = new QTextEdit(verticalLayoutWidget);
        tbPositionY->setObjectName(QString::fromUtf8("tbPositionY"));

        verticalLayout->addWidget(tbPositionY);

        tbRadius = new QTextEdit(verticalLayoutWidget);
        tbRadius->setObjectName(QString::fromUtf8("tbRadius"));

        verticalLayout->addWidget(tbRadius);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(660, 120, 121, 31));
        OptionWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(OptionWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        OptionWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(OptionWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        OptionWindow->setStatusBar(statusbar);

        retranslateUi(OptionWindow);

        QMetaObject::connectSlotsByName(OptionWindow);
    } // setupUi

    void retranslateUi(QMainWindow *OptionWindow)
    {
        OptionWindow->setWindowTitle(QApplication::translate("OptionWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("OptionWindow", "PushButton", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class OptionWindow: public Ui_OptionWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONWINDOW_H
