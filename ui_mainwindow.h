/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *data_tab;
    QGridLayout *gridLayout_10;
    QFrame *press_frame;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_11;
    QLabel *press_label;
    QLabel *label_14;
    QGroupBox *rigol1_group;
    QGridLayout *gridLayout_4;
    QLabel *label_5;
    QSpacerItem *verticalSpacer_12;
    QLabel *label_20;
    QLineEdit *shim_volt_edit;
    QSpacerItem *horizontalSpacer_12;
    QSpacerItem *horizontalSpacer_13;
    QPushButton *shim_curr_button;
    QPushButton *shim_volt_button;
    QSpacerItem *horizontalSpacer_14;
    QLineEdit *shim_curr_edit;
    QSpacerItem *verticalSpacer_13;
    QPushButton *rigol1_hv_button;
    QSpacerItem *verticalSpacer_14;
    QGroupBox *dxm_group;
    QGridLayout *gridLayout_2;
    QLabel *label_8;
    QLineEdit *beam_volt_edit;
    QSpacerItem *verticalSpacer_2;
    QLabel *dxm_curr_label;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QLineEdit *body_read;
    QLineEdit *fil_curr_edit;
    QPushButton *dxm_hv_button;
    QSpacerItem *horizontalSpacer_3;
    QLineEdit *collector_read;
    QPushButton *ramp_up_button;
    QLabel *label_3;
    QLabel *label_9;
    QLabel *label;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QPushButton *fil_curr_button;
    QPushButton *beam_volt_button;
    QPushButton *beam_curr_button;
    QPushButton *ramp_down_button;
    QSpacerItem *verticalSpacer_3;
    QLineEdit *beam_curr_edit;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *verticalSpacer_4;
    QGroupBox *fms_group;
    QGridLayout *gridLayout_8;
    QSpacerItem *horizontalSpacer_11;
    QLineEdit *freq_edit;
    QSpacerItem *verticalSpacer_10;
    QLabel *label_18;
    QSpacerItem *horizontalSpacer_10;
    QPushButton *freq_button;
    QSpacerItem *verticalSpacer_11;
    QGroupBox *pid_group;
    QGridLayout *gridLayout_9;
    QSpacerItem *verticalSpacer_15;
    QSpacerItem *verticalSpacer_18;
    QSpacerItem *horizontalSpacer_15;
    QSpacerItem *verticalSpacer_16;
    QPushButton *power_pid_button;
    QPushButton *beam_curr_pid_button;
    QPushButton *freq_pid_button;
    QSpacerItem *verticalSpacer_17;
    QSpacerItem *horizontalSpacer_16;
    QGroupBox *rigol2_group;
    QGridLayout *gridLayout_5;
    QPushButton *trim_volt_button;
    QPushButton *trim_curr_button;
    QLabel *label_7;
    QLabel *label_23;
    QLineEdit *trim_volt_edit;
    QSpacerItem *verticalSpacer_22;
    QSpacerItem *verticalSpacer_5;
    QSpacerItem *verticalSpacer_21;
    QPushButton *rigol2_hv_button;
    QSpacerItem *horizontalSpacer_18;
    QSpacerItem *horizontalSpacer_19;
    QLineEdit *trim_curr_edit;
    QSpacerItem *horizontalSpacer_17;
    QGroupBox *lj_curr_group;
    QGridLayout *gridLayout_6;
    QPushButton *power_button;
    QLabel *label_17;
    QSpacerItem *horizontalSpacer_8;
    QLineEdit *power_edit;
    QSpacerItem *verticalSpacer_7;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *verticalSpacer_6;
    QGroupBox *slm_group;
    QGridLayout *gridLayout_3;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *anode_volt_button;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *verticalSpacer_8;
    QLabel *label_4;
    QLabel *label_13;
    QPushButton *anode_curr_button;
    QLineEdit *anode_volt_edit;
    QLineEdit *anode_curr_edit;
    QSpacerItem *verticalSpacer_19;
    QPushButton *slm_hv_button;
    QSpacerItem *verticalSpacer_9;
    QWidget *plot_tab;
    QGridLayout *gridLayout_7;
    QSpacerItem *verticalSpacer_23;
    QCustomPlot *power_plot;
    QCustomPlot *press_plot;
    QCustomPlot *beam_plot;
    QSpacerItem *horizontalSpacer_37;
    QWidget *log_tab;
    QGridLayout *gridLayout_13;
    QSpacerItem *horizontalSpacer_20;
    QSpacerItem *horizontalSpacer_21;
    QSpacerItem *verticalSpacer_25;
    QGroupBox *log_group;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *log_box;
    QSpacerItem *verticalSpacer_28;
    QWidget *admin_tab;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer_20;
    QSpacerItem *verticalSpacer_24;
    QGroupBox *pid_const_group;
    QGridLayout *gridLayout_22;
    QSpacerItem *horizontalSpacer_41;
    QLineEdit *beam_kd_write;
    QLabel *label_35;
    QLabel *label_16;
    QLabel *label_25;
    QLineEdit *fki_write;
    QLabel *label_27;
    QLineEdit *beam_ki_write;
    QLineEdit *fkp_write;
    QLineEdit *fkd_write;
    QLabel *label_26;
    QLineEdit *beam_kp_write;
    QSpacerItem *horizontalSpacer_42;
    QGroupBox *reconnect_group;
    QGridLayout *gridLayout_11;
    QPushButton *spc_recon_button;
    QPushButton *rigol_recon_button;
    QPushButton *dxm_recon_button;
    QPushButton *slm_recon_button;
    QPushButton *fms_recon_button;
    QPushButton *lj_recon_button;
    QSpacerItem *verticalSpacer_26;
    QSpacerItem *horizontalSpacer_44;
    QGroupBox *ramp_group;
    QGridLayout *gridLayout_17;
    QLineEdit *ramp_sp_write;
    QSpacerItem *horizontalSpacer_39;
    QLabel *label_28;
    QLabel *label_29;
    QLineEdit *ramp_time_write;
    QSpacerItem *horizontalSpacer_40;
    QSpacerItem *horizontalSpacer_45;
    QGroupBox *misc_group;
    QGridLayout *gridLayout_12;
    QPushButton *reconfig_button;
    QLineEdit *log_rate_write;
    QLabel *label_32;
    QPushButton *clear_status_button;
    QLabel *label_33;
    QSpacerItem *horizontalSpacer_43;
    QSpacerItem *verticalSpacer_27;
    QWidget *faults_tab;
    QTreeWidget *treeWidget;
    QPushButton *dxm_reset_button;
    QPushButton *slm_reset_button;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1180, 775);
        MainWindow->setMinimumSize(QSize(1180, 775));
        MainWindow->setMaximumSize(QSize(1180, 775));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(240, 240, 240, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush2);
        QBrush brush3(QColor(127, 127, 127, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush3);
        QBrush brush4(QColor(170, 170, 170, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush4);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush2);
        QBrush brush5(QColor(255, 255, 220, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush5);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        QBrush brush6(QColor(0, 0, 0, 128));
        brush6.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush6);
#endif
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        QBrush brush7(QColor(0, 0, 0, 128));
        brush7.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush7);
#endif
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        QBrush brush8(QColor(0, 0, 0, 128));
        brush8.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush8);
#endif
        MainWindow->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("Open Sans"));
        font.setPointSize(12);
        MainWindow->setFont(font);
        MainWindow->setStyleSheet(QString::fromUtf8("background-color: rgb(240,240,240);\n"
"color: black;"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionQuit->setFont(font);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, 22, -1, -1);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Open Sans"));
        font1.setPointSize(11);
        tabWidget->setFont(font1);
        tabWidget->setStyleSheet(QString::fromUtf8("QTabWidget::tab-bar\n"
"{\n"
"	alignment: center;\n"
"}\n"
"QTabWidget::pane\n"
"{\n"
"	border: 0px;\n"
"}\n"
"\n"
"/* Style the tab using the tab sub-control. Note that it reads QTabBar _not_ QTabWidget */\n"
"QTabBar::tab {\n"
"/*margin-bottom: 1px;*/\n"
"background: white;\n"
"/*border: 1px solid #46812B;*/\n"
"border: 1px solid rgb(220,220,220);\n"
"padding: 0px 25px;\n"
"height: 36px !important;\n"
"}\n"
"QTabBar::tab:selected {\n"
"background: #46812B;\n"
"color:white;\n"
"border: none;\n"
"top: 1px;\n"
"}\n"
"QTabBar::tab:!selected:hover {\n"
"background: rgb(220,220,220);\n"
"}\n"
"QTabBar::tab:!selected\n"
"{\n"
"background: white;\n"
"/*border-bottom: 1px solid rgb(180,180,180);*/\n"
"}\n"
"\n"
"QTabBar::tab:last {\n"
"border-top-right-radius: 18px;\n"
"border-bottom-right-radius: 18px;\n"
"}\n"
"QTabBar::tab:first {\n"
"border-top-left-radius: 18px;\n"
"border-bottom-left-radius: 18px;\n"
"}"));
        data_tab = new QWidget();
        data_tab->setObjectName(QString::fromUtf8("data_tab"));
        gridLayout_10 = new QGridLayout(data_tab);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        gridLayout_10->setHorizontalSpacing(20);
        gridLayout_10->setContentsMargins(30, -1, 30, -1);
        press_frame = new QFrame(data_tab);
        press_frame->setObjectName(QString::fromUtf8("press_frame"));
        press_frame->setEnabled(true);
        press_frame->setMinimumSize(QSize(150, 150));
        press_frame->setMaximumSize(QSize(150, 150));
        press_frame->setStyleSheet(QString::fromUtf8("QFrame { background: #46812B; color: white; border: none; border-radius: 75px; }\n"
"QFrame:disabled { background: rgb(180,180,180); }"));
        press_frame->setFrameShape(QFrame::StyledPanel);
        press_frame->setFrameShadow(QFrame::Raised);
        verticalLayout_5 = new QVBoxLayout(press_frame);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(-1, 20, -1, 20);
        label_11 = new QLabel(press_frame);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Open Sans"));
        font2.setBold(false);
        font2.setWeight(50);
        label_11->setFont(font2);
        label_11->setStyleSheet(QString::fromUtf8("background: none;"));
        label_11->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_11, 0, Qt::AlignHCenter);

        press_label = new QLabel(press_frame);
        press_label->setObjectName(QString::fromUtf8("press_label"));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Open Sans"));
        font3.setPointSize(20);
        font3.setBold(true);
        font3.setWeight(75);
        press_label->setFont(font3);
        press_label->setStyleSheet(QString::fromUtf8("QLabel { color: white; background: none; }\n"
"QLabel:disabled { color: rgb(180,180,180); }"));
        press_label->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(press_label, 0, Qt::AlignHCenter);

        label_14 = new QLabel(press_frame);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Open Sans"));
        font4.setPointSize(10);
        label_14->setFont(font4);
        label_14->setStyleSheet(QString::fromUtf8("background: none;"));
        label_14->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_14, 0, Qt::AlignHCenter);


        gridLayout_10->addWidget(press_frame, 0, 2, 1, 1, Qt::AlignHCenter);

        rigol1_group = new QGroupBox(data_tab);
        rigol1_group->setObjectName(QString::fromUtf8("rigol1_group"));
        rigol1_group->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(rigol1_group->sizePolicy().hasHeightForWidth());
        rigol1_group->setSizePolicy(sizePolicy);
        rigol1_group->setMinimumSize(QSize(500, 170));
        rigol1_group->setMaximumSize(QSize(100000, 100000));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Open Sans"));
        font5.setPointSize(11);
        font5.setBold(true);
        font5.setWeight(75);
        rigol1_group->setFont(font5);
        rigol1_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: white;\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_4 = new QGridLayout(rigol1_group);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setHorizontalSpacing(15);
        label_5 = new QLabel(rigol1_group);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QFont font6;
        font6.setFamily(QString::fromUtf8("Open Sans"));
        font6.setPointSize(9);
        label_5->setFont(font6);
        label_5->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_5, 3, 1, 1, 1, Qt::AlignRight);

        verticalSpacer_12 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_12, 0, 2, 1, 1);

        label_20 = new QLabel(rigol1_group);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setFont(font6);
        label_20->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_20->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_20, 1, 1, 1, 1, Qt::AlignRight);

        shim_volt_edit = new QLineEdit(rigol1_group);
        shim_volt_edit->setObjectName(QString::fromUtf8("shim_volt_edit"));
        shim_volt_edit->setMinimumSize(QSize(100, 36));
        shim_volt_edit->setMaximumSize(QSize(100, 36));
        QFont font7;
        font7.setFamily(QString::fromUtf8("Open Sans"));
        font7.setPointSize(12);
        font7.setBold(true);
        font7.setWeight(75);
        shim_volt_edit->setFont(font7);
        shim_volt_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        shim_volt_edit->setAlignment(Qt::AlignCenter);
        shim_volt_edit->setReadOnly(true);

        gridLayout_4->addWidget(shim_volt_edit, 3, 2, 1, 1, Qt::AlignHCenter);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_12, 1, 0, 1, 1);

        horizontalSpacer_13 = new QSpacerItem(45, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_13, 1, 4, 1, 1);

        shim_curr_button = new QPushButton(rigol1_group);
        shim_curr_button->setObjectName(QString::fromUtf8("shim_curr_button"));
        shim_curr_button->setMinimumSize(QSize(36, 36));
        shim_curr_button->setMaximumSize(QSize(100000, 36));
        QFont font8;
        font8.setFamily(QString::fromUtf8("Open Sans"));
        font8.setPointSize(17);
        shim_curr_button->setFont(font8);
        shim_curr_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_4->addWidget(shim_curr_button, 1, 3, 1, 1, Qt::AlignLeft);

        shim_volt_button = new QPushButton(rigol1_group);
        shim_volt_button->setObjectName(QString::fromUtf8("shim_volt_button"));
        shim_volt_button->setMinimumSize(QSize(36, 36));
        shim_volt_button->setMaximumSize(QSize(100000, 36));
        shim_volt_button->setFont(font8);
        shim_volt_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_4->addWidget(shim_volt_button, 3, 3, 1, 1, Qt::AlignLeft);

        horizontalSpacer_14 = new QSpacerItem(45, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_14, 1, 6, 1, 1);

        shim_curr_edit = new QLineEdit(rigol1_group);
        shim_curr_edit->setObjectName(QString::fromUtf8("shim_curr_edit"));
        shim_curr_edit->setMinimumSize(QSize(100, 36));
        shim_curr_edit->setMaximumSize(QSize(100, 36));
        shim_curr_edit->setFont(font7);
        shim_curr_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        shim_curr_edit->setAlignment(Qt::AlignCenter);
        shim_curr_edit->setReadOnly(true);

        gridLayout_4->addWidget(shim_curr_edit, 1, 2, 1, 1, Qt::AlignHCenter);

        verticalSpacer_13 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_13, 2, 2, 1, 1);

        rigol1_hv_button = new QPushButton(rigol1_group);
        rigol1_hv_button->setObjectName(QString::fromUtf8("rigol1_hv_button"));
        rigol1_hv_button->setMinimumSize(QSize(100, 100));
        rigol1_hv_button->setMaximumSize(QSize(100, 100));
        rigol1_hv_button->setFont(font5);
        rigol1_hv_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 50px;\n"
"	padding-top: 3px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_4->addWidget(rigol1_hv_button, 1, 5, 3, 1);

        verticalSpacer_14 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_14, 4, 2, 1, 1);

        gridLayout_4->setRowStretch(0, 8);
        gridLayout_4->setRowStretch(2, 1);
        gridLayout_4->setRowStretch(4, 8);

        gridLayout_10->addWidget(rigol1_group, 3, 0, 2, 1);

        dxm_group = new QGroupBox(data_tab);
        dxm_group->setObjectName(QString::fromUtf8("dxm_group"));
        dxm_group->setEnabled(true);
        sizePolicy.setHeightForWidth(dxm_group->sizePolicy().hasHeightForWidth());
        dxm_group->setSizePolicy(sizePolicy);
        dxm_group->setMinimumSize(QSize(600, 210));
        dxm_group->setMaximumSize(QSize(10000, 100000));
        QFont font9;
        font9.setFamily(QString::fromUtf8("Open Sans"));
        font9.setPointSize(11);
        font9.setBold(true);
        font9.setWeight(75);
        font9.setKerning(true);
        dxm_group->setFont(font9);
        dxm_group->setAutoFillBackground(false);
        dxm_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: white;\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        dxm_group->setFlat(false);
        dxm_group->setCheckable(false);
        gridLayout_2 = new QGridLayout(dxm_group);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(15);
        gridLayout_2->setContentsMargins(0, -1, -1, -1);
        label_8 = new QLabel(dxm_group);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setMaximumSize(QSize(16777215, 20));
        label_8->setFont(font6);
        label_8->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));

        gridLayout_2->addWidget(label_8, 3, 5, 1, 1, Qt::AlignRight);

        beam_volt_edit = new QLineEdit(dxm_group);
        beam_volt_edit->setObjectName(QString::fromUtf8("beam_volt_edit"));
        beam_volt_edit->setMinimumSize(QSize(100, 36));
        beam_volt_edit->setMaximumSize(QSize(100, 36));
        beam_volt_edit->setFont(font7);
        beam_volt_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        beam_volt_edit->setAlignment(Qt::AlignCenter);
        beam_volt_edit->setReadOnly(true);

        gridLayout_2->addWidget(beam_volt_edit, 3, 2, 1, 1, Qt::AlignHCenter|Qt::AlignVCenter);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 0, 3, 1, 1);

        dxm_curr_label = new QLabel(dxm_group);
        dxm_curr_label->setObjectName(QString::fromUtf8("dxm_curr_label"));
        dxm_curr_label->setEnabled(true);
        dxm_curr_label->setFont(font6);
        dxm_curr_label->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        dxm_curr_label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(dxm_curr_label, 5, 1, 1, 1, Qt::AlignRight|Qt::AlignVCenter);

        horizontalSpacer = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 3, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 3, 9, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 6, 3, 1, 1);

        body_read = new QLineEdit(dxm_group);
        body_read->setObjectName(QString::fromUtf8("body_read"));
        body_read->setMinimumSize(QSize(100, 36));
        body_read->setMaximumSize(QSize(100, 36));
        body_read->setFont(font7);
        body_read->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        body_read->setAlignment(Qt::AlignCenter);
        body_read->setReadOnly(true);

        gridLayout_2->addWidget(body_read, 5, 6, 1, 1, Qt::AlignLeft);

        fil_curr_edit = new QLineEdit(dxm_group);
        fil_curr_edit->setObjectName(QString::fromUtf8("fil_curr_edit"));
        fil_curr_edit->setMinimumSize(QSize(100, 36));
        fil_curr_edit->setMaximumSize(QSize(100, 36));
        fil_curr_edit->setFont(font7);
        fil_curr_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        fil_curr_edit->setAlignment(Qt::AlignCenter);
        fil_curr_edit->setReadOnly(true);

        gridLayout_2->addWidget(fil_curr_edit, 1, 2, 1, 1, Qt::AlignHCenter|Qt::AlignVCenter);

        dxm_hv_button = new QPushButton(dxm_group);
        dxm_hv_button->setObjectName(QString::fromUtf8("dxm_hv_button"));
        dxm_hv_button->setMinimumSize(QSize(100, 100));
        dxm_hv_button->setMaximumSize(QSize(100, 100));
        dxm_hv_button->setFont(font5);
        dxm_hv_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 50px;\n"
"	padding-top: 3px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_2->addWidget(dxm_hv_button, 1, 8, 5, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 3, 4, 1, 1);

        collector_read = new QLineEdit(dxm_group);
        collector_read->setObjectName(QString::fromUtf8("collector_read"));
        collector_read->setEnabled(true);
        collector_read->setMinimumSize(QSize(100, 36));
        collector_read->setMaximumSize(QSize(100, 36));
        collector_read->setFont(font7);
        collector_read->setFocusPolicy(Qt::NoFocus);
        collector_read->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        collector_read->setAlignment(Qt::AlignCenter);
        collector_read->setReadOnly(true);

        gridLayout_2->addWidget(collector_read, 3, 6, 1, 1, Qt::AlignLeft);

        ramp_up_button = new QPushButton(dxm_group);
        ramp_up_button->setObjectName(QString::fromUtf8("ramp_up_button"));
        ramp_up_button->setEnabled(true);
        ramp_up_button->setMinimumSize(QSize(130, 46));
        ramp_up_button->setMaximumSize(QSize(130, 46));
        QFont font10;
        font10.setFamily(QString::fromUtf8("Open Sans"));
        font10.setPointSize(10);
        font10.setBold(true);
        font10.setWeight(75);
        ramp_up_button->setFont(font10);
        ramp_up_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 23px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_2->addWidget(ramp_up_button, 1, 5, 1, 1, Qt::AlignRight);

        label_3 = new QLabel(dxm_group);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font6);
        label_3->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_3, 3, 1, 1, 1, Qt::AlignRight|Qt::AlignVCenter);

        label_9 = new QLabel(dxm_group);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setFont(font6);
        label_9->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));

        gridLayout_2->addWidget(label_9, 5, 5, 1, 1, Qt::AlignRight);

        label = new QLabel(dxm_group);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(125, 34));
        label->setMaximumSize(QSize(125, 34));
        QFont font11;
        font11.setFamily(QString::fromUtf8("Open Sans"));
        font11.setPointSize(9);
        font11.setKerning(true);
        label->setFont(font11);
        label->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label, 1, 1, 1, 1, Qt::AlignRight|Qt::AlignVCenter);

        frame = new QFrame(dxm_group);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(100, 0));
        frame->setMaximumSize(QSize(100, 16777215));
        frame->setStyleSheet(QString::fromUtf8("border: none; background: none;"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        fil_curr_button = new QPushButton(frame);
        fil_curr_button->setObjectName(QString::fromUtf8("fil_curr_button"));
        fil_curr_button->setMinimumSize(QSize(36, 36));
        fil_curr_button->setMaximumSize(QSize(100000, 36));
        fil_curr_button->setFont(font8);
        fil_curr_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        verticalLayout_2->addWidget(fil_curr_button, 0, Qt::AlignLeft|Qt::AlignVCenter);

        beam_volt_button = new QPushButton(frame);
        beam_volt_button->setObjectName(QString::fromUtf8("beam_volt_button"));
        beam_volt_button->setMinimumSize(QSize(36, 36));
        beam_volt_button->setMaximumSize(QSize(100000, 36));
        beam_volt_button->setFont(font8);
        beam_volt_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        verticalLayout_2->addWidget(beam_volt_button, 0, Qt::AlignLeft|Qt::AlignVCenter);

        beam_curr_button = new QPushButton(frame);
        beam_curr_button->setObjectName(QString::fromUtf8("beam_curr_button"));
        beam_curr_button->setEnabled(false);
        beam_curr_button->setMinimumSize(QSize(36, 36));
        beam_curr_button->setMaximumSize(QSize(100000, 36));
        beam_curr_button->setFont(font8);
        beam_curr_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        verticalLayout_2->addWidget(beam_curr_button, 0, Qt::AlignLeft|Qt::AlignVCenter);


        gridLayout_2->addWidget(frame, 1, 3, 5, 1);

        ramp_down_button = new QPushButton(dxm_group);
        ramp_down_button->setObjectName(QString::fromUtf8("ramp_down_button"));
        ramp_down_button->setEnabled(true);
        ramp_down_button->setMinimumSize(QSize(130, 46));
        ramp_down_button->setMaximumSize(QSize(130, 46));
        ramp_down_button->setFont(font10);
        ramp_down_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 23px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_2->addWidget(ramp_down_button, 1, 6, 1, 1, Qt::AlignLeft);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_3, 2, 1, 1, 1);

        beam_curr_edit = new QLineEdit(dxm_group);
        beam_curr_edit->setObjectName(QString::fromUtf8("beam_curr_edit"));
        beam_curr_edit->setEnabled(true);
        beam_curr_edit->setMinimumSize(QSize(100, 36));
        beam_curr_edit->setMaximumSize(QSize(100, 36));
        beam_curr_edit->setFont(font7);
        beam_curr_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        beam_curr_edit->setAlignment(Qt::AlignCenter);
        beam_curr_edit->setReadOnly(true);

        gridLayout_2->addWidget(beam_curr_edit, 5, 2, 1, 1, Qt::AlignHCenter|Qt::AlignVCenter);

        horizontalSpacer_4 = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 3, 7, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_4, 4, 1, 1, 1);

        gridLayout_2->setRowStretch(0, 8);
        gridLayout_2->setRowStretch(2, 1);
        gridLayout_2->setRowStretch(4, 1);
        gridLayout_2->setRowStretch(6, 8);
        gridLayout_2->setColumnStretch(0, 8);
        gridLayout_2->setColumnStretch(4, 1);
        gridLayout_2->setColumnStretch(7, 8);
        gridLayout_2->setColumnStretch(9, 8);

        gridLayout_10->addWidget(dxm_group, 0, 0, 1, 2);

        fms_group = new QGroupBox(data_tab);
        fms_group->setObjectName(QString::fromUtf8("fms_group"));
        fms_group->setEnabled(true);
        fms_group->setMinimumSize(QSize(280, 110));
        fms_group->setFont(font5);
        fms_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: white;\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_8 = new QGridLayout(fms_group);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        gridLayout_8->setHorizontalSpacing(15);
        horizontalSpacer_11 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_11, 1, 4, 1, 1);

        freq_edit = new QLineEdit(fms_group);
        freq_edit->setObjectName(QString::fromUtf8("freq_edit"));
        freq_edit->setMinimumSize(QSize(100, 36));
        freq_edit->setMaximumSize(QSize(100, 36));
        freq_edit->setFont(font7);
        freq_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        freq_edit->setAlignment(Qt::AlignCenter);
        freq_edit->setReadOnly(true);

        gridLayout_8->addWidget(freq_edit, 1, 2, 1, 1);

        verticalSpacer_10 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer_10, 0, 2, 1, 1);

        label_18 = new QLabel(fms_group);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setMaximumSize(QSize(16777215, 1000));
        label_18->setFont(font6);
        label_18->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_18->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(label_18, 1, 1, 1, 1);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_10, 1, 0, 1, 1);

        freq_button = new QPushButton(fms_group);
        freq_button->setObjectName(QString::fromUtf8("freq_button"));
        freq_button->setMinimumSize(QSize(36, 36));
        freq_button->setMaximumSize(QSize(100000, 36));
        freq_button->setFont(font8);
        freq_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_8->addWidget(freq_button, 1, 3, 1, 1, Qt::AlignLeft);

        verticalSpacer_11 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer_11, 2, 2, 1, 1);


        gridLayout_10->addWidget(fms_group, 1, 1, 1, 1);

        pid_group = new QGroupBox(data_tab);
        pid_group->setObjectName(QString::fromUtf8("pid_group"));
        pid_group->setEnabled(true);
        pid_group->setMinimumSize(QSize(175, 210));
        QFont font12;
        font12.setFamily(QString::fromUtf8("Open Sans"));
        font12.setBold(true);
        font12.setWeight(75);
        pid_group->setFont(font12);
        pid_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: white;\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_9 = new QGridLayout(pid_group);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_9->setContentsMargins(-1, 33, -1, 20);
        verticalSpacer_15 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_9->addItem(verticalSpacer_15, 0, 1, 1, 1);

        verticalSpacer_18 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_9->addItem(verticalSpacer_18, 6, 1, 1, 1);

        horizontalSpacer_15 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer_15, 3, 0, 1, 1);

        verticalSpacer_16 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_9->addItem(verticalSpacer_16, 2, 1, 1, 1);

        power_pid_button = new QPushButton(pid_group);
        power_pid_button->setObjectName(QString::fromUtf8("power_pid_button"));
        power_pid_button->setEnabled(false);
        power_pid_button->setMinimumSize(QSize(110, 46));
        power_pid_button->setMaximumSize(QSize(110, 46));
        QFont font13;
        font13.setFamily(QString::fromUtf8("Open Sans"));
        font13.setPointSize(9);
        font13.setBold(true);
        font13.setWeight(75);
        power_pid_button->setFont(font13);
        power_pid_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 23px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_9->addWidget(power_pid_button, 1, 1, 1, 1);

        beam_curr_pid_button = new QPushButton(pid_group);
        beam_curr_pid_button->setObjectName(QString::fromUtf8("beam_curr_pid_button"));
        beam_curr_pid_button->setEnabled(false);
        beam_curr_pid_button->setMinimumSize(QSize(110, 46));
        beam_curr_pid_button->setMaximumSize(QSize(110, 46));
        beam_curr_pid_button->setFont(font13);
        beam_curr_pid_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 23px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_9->addWidget(beam_curr_pid_button, 3, 1, 1, 1);

        freq_pid_button = new QPushButton(pid_group);
        freq_pid_button->setObjectName(QString::fromUtf8("freq_pid_button"));
        freq_pid_button->setEnabled(false);
        freq_pid_button->setMinimumSize(QSize(110, 46));
        freq_pid_button->setMaximumSize(QSize(110, 46));
        freq_pid_button->setFont(font13);
        freq_pid_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 23px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_9->addWidget(freq_pid_button, 5, 1, 1, 1);

        verticalSpacer_17 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_9->addItem(verticalSpacer_17, 4, 1, 1, 1);

        horizontalSpacer_16 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer_16, 3, 2, 1, 1);

        gridLayout_9->setRowStretch(0, 3);
        gridLayout_9->setRowStretch(2, 1);
        gridLayout_9->setRowStretch(4, 1);
        gridLayout_9->setRowStretch(6, 2);

        gridLayout_10->addWidget(pid_group, 1, 2, 2, 1);

        rigol2_group = new QGroupBox(data_tab);
        rigol2_group->setObjectName(QString::fromUtf8("rigol2_group"));
        rigol2_group->setEnabled(true);
        sizePolicy.setHeightForWidth(rigol2_group->sizePolicy().hasHeightForWidth());
        rigol2_group->setSizePolicy(sizePolicy);
        rigol2_group->setMinimumSize(QSize(500, 170));
        rigol2_group->setMaximumSize(QSize(10000, 10000));
        rigol2_group->setFont(font5);
        rigol2_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: white;\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_5 = new QGridLayout(rigol2_group);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setHorizontalSpacing(15);
        trim_volt_button = new QPushButton(rigol2_group);
        trim_volt_button->setObjectName(QString::fromUtf8("trim_volt_button"));
        trim_volt_button->setMinimumSize(QSize(36, 36));
        trim_volt_button->setMaximumSize(QSize(100000, 36));
        trim_volt_button->setFont(font8);
        trim_volt_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_5->addWidget(trim_volt_button, 3, 3, 1, 1);

        trim_curr_button = new QPushButton(rigol2_group);
        trim_curr_button->setObjectName(QString::fromUtf8("trim_curr_button"));
        trim_curr_button->setMinimumSize(QSize(36, 36));
        trim_curr_button->setMaximumSize(QSize(100000, 36));
        trim_curr_button->setFont(font8);
        trim_curr_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_5->addWidget(trim_curr_button, 1, 3, 1, 1);

        label_7 = new QLabel(rigol2_group);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setFont(font6);
        label_7->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_7->setAlignment(Qt::AlignCenter);

        gridLayout_5->addWidget(label_7, 3, 1, 1, 1, Qt::AlignRight);

        label_23 = new QLabel(rigol2_group);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setFont(font6);
        label_23->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_23->setAlignment(Qt::AlignCenter);

        gridLayout_5->addWidget(label_23, 1, 1, 1, 1, Qt::AlignRight);

        trim_volt_edit = new QLineEdit(rigol2_group);
        trim_volt_edit->setObjectName(QString::fromUtf8("trim_volt_edit"));
        trim_volt_edit->setMinimumSize(QSize(100, 36));
        trim_volt_edit->setMaximumSize(QSize(100, 36));
        trim_volt_edit->setFont(font7);
        trim_volt_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        trim_volt_edit->setAlignment(Qt::AlignCenter);
        trim_volt_edit->setReadOnly(true);

        gridLayout_5->addWidget(trim_volt_edit, 3, 2, 1, 1, Qt::AlignHCenter);

        verticalSpacer_22 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_22, 4, 2, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_5, 0, 2, 1, 1);

        verticalSpacer_21 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_21, 2, 2, 1, 1);

        rigol2_hv_button = new QPushButton(rigol2_group);
        rigol2_hv_button->setObjectName(QString::fromUtf8("rigol2_hv_button"));
        rigol2_hv_button->setMinimumSize(QSize(100, 100));
        rigol2_hv_button->setMaximumSize(QSize(100, 100));
        rigol2_hv_button->setFont(font5);
        rigol2_hv_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 50px;\n"
"	padding-top: 3px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_5->addWidget(rigol2_hv_button, 1, 5, 3, 1);

        horizontalSpacer_18 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_18, 1, 4, 1, 1);

        horizontalSpacer_19 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_19, 1, 6, 1, 1);

        trim_curr_edit = new QLineEdit(rigol2_group);
        trim_curr_edit->setObjectName(QString::fromUtf8("trim_curr_edit"));
        trim_curr_edit->setMinimumSize(QSize(100, 36));
        trim_curr_edit->setMaximumSize(QSize(100, 36));
        trim_curr_edit->setFont(font7);
        trim_curr_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        trim_curr_edit->setAlignment(Qt::AlignCenter);
        trim_curr_edit->setReadOnly(true);

        gridLayout_5->addWidget(trim_curr_edit, 1, 2, 1, 1, Qt::AlignHCenter);

        horizontalSpacer_17 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_17, 1, 0, 1, 1);

        gridLayout_5->setRowStretch(0, 8);
        gridLayout_5->setRowStretch(2, 1);
        gridLayout_5->setRowStretch(4, 8);

        gridLayout_10->addWidget(rigol2_group, 3, 1, 2, 2);

        lj_curr_group = new QGroupBox(data_tab);
        lj_curr_group->setObjectName(QString::fromUtf8("lj_curr_group"));
        lj_curr_group->setEnabled(true);
        sizePolicy.setHeightForWidth(lj_curr_group->sizePolicy().hasHeightForWidth());
        lj_curr_group->setSizePolicy(sizePolicy);
        lj_curr_group->setMinimumSize(QSize(280, 110));
        lj_curr_group->setMaximumSize(QSize(100000, 100000));
        lj_curr_group->setFont(font5);
        lj_curr_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: white;\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_6 = new QGridLayout(lj_curr_group);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setHorizontalSpacing(15);
        power_button = new QPushButton(lj_curr_group);
        power_button->setObjectName(QString::fromUtf8("power_button"));
        power_button->setMinimumSize(QSize(36, 36));
        power_button->setMaximumSize(QSize(100000, 36));
        power_button->setFont(font8);
        power_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_6->addWidget(power_button, 1, 3, 1, 1, Qt::AlignLeft);

        label_17 = new QLabel(lj_curr_group);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setMaximumSize(QSize(16777215, 1000));
        label_17->setFont(font6);
        label_17->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_17->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(label_17, 1, 1, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer_8, 1, 0, 1, 1);

        power_edit = new QLineEdit(lj_curr_group);
        power_edit->setObjectName(QString::fromUtf8("power_edit"));
        power_edit->setMinimumSize(QSize(100, 36));
        power_edit->setMaximumSize(QSize(100, 36));
        power_edit->setFont(font7);
        power_edit->setFocusPolicy(Qt::NoFocus);
        power_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        power_edit->setAlignment(Qt::AlignCenter);
        power_edit->setReadOnly(true);

        gridLayout_6->addWidget(power_edit, 1, 2, 1, 1);

        verticalSpacer_7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_6->addItem(verticalSpacer_7, 2, 2, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer_9, 1, 4, 1, 1);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_6->addItem(verticalSpacer_6, 0, 2, 1, 1);


        gridLayout_10->addWidget(lj_curr_group, 2, 1, 1, 1);

        slm_group = new QGroupBox(data_tab);
        slm_group->setObjectName(QString::fromUtf8("slm_group"));
        slm_group->setEnabled(true);
        sizePolicy.setHeightForWidth(slm_group->sizePolicy().hasHeightForWidth());
        slm_group->setSizePolicy(sizePolicy);
        slm_group->setMinimumSize(QSize(500, 170));
        slm_group->setMaximumSize(QSize(100000, 100000));
        slm_group->setFont(font5);
        slm_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: white;\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_3 = new QGridLayout(slm_group);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(15);
        gridLayout_3->setContentsMargins(9, -1, -1, -1);
        horizontalSpacer_6 = new QSpacerItem(45, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_6, 1, 4, 1, 1);

        anode_volt_button = new QPushButton(slm_group);
        anode_volt_button->setObjectName(QString::fromUtf8("anode_volt_button"));
        anode_volt_button->setMinimumSize(QSize(36, 36));
        anode_volt_button->setMaximumSize(QSize(100000, 36));
        anode_volt_button->setFont(font8);
        anode_volt_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_3->addWidget(anode_volt_button, 1, 3, 1, 1, Qt::AlignLeft);

        horizontalSpacer_7 = new QSpacerItem(45, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_7, 1, 6, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_5, 1, 0, 1, 1);

        verticalSpacer_8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_8, 4, 2, 1, 1);

        label_4 = new QLabel(slm_group);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font6);
        label_4->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_4->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_4, 1, 1, 1, 1, Qt::AlignRight);

        label_13 = new QLabel(slm_group);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setFont(font6);
        label_13->setStyleSheet(QString::fromUtf8("QLabel{ background: none; color: rgb(30,30,30); }\n"
"QLabel:disabled{ color: rgb(180,180,180); }"));
        label_13->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_13, 3, 1, 1, 1, Qt::AlignRight);

        anode_curr_button = new QPushButton(slm_group);
        anode_curr_button->setObjectName(QString::fromUtf8("anode_curr_button"));
        anode_curr_button->setMinimumSize(QSize(36, 36));
        anode_curr_button->setMaximumSize(QSize(100000, 36));
        anode_curr_button->setFont(font8);
        anode_curr_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2f5b12, stop: 1 #2f5b12);\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	border-radius: 18px;\n"
"	color: white;\n"
"	height: 36px;\n"
"	width: 36px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_3->addWidget(anode_curr_button, 3, 3, 1, 1, Qt::AlignLeft);

        anode_volt_edit = new QLineEdit(slm_group);
        anode_volt_edit->setObjectName(QString::fromUtf8("anode_volt_edit"));
        anode_volt_edit->setMinimumSize(QSize(100, 36));
        anode_volt_edit->setMaximumSize(QSize(100, 36));
        anode_volt_edit->setFont(font7);
        anode_volt_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        anode_volt_edit->setAlignment(Qt::AlignCenter);
        anode_volt_edit->setReadOnly(true);

        gridLayout_3->addWidget(anode_volt_edit, 1, 2, 1, 1, Qt::AlignHCenter);

        anode_curr_edit = new QLineEdit(slm_group);
        anode_curr_edit->setObjectName(QString::fromUtf8("anode_curr_edit"));
        anode_curr_edit->setMinimumSize(QSize(100, 36));
        anode_curr_edit->setMaximumSize(QSize(100, 36));
        anode_curr_edit->setFont(font7);
        anode_curr_edit->setStyleSheet(QString::fromUtf8("QLineEdit { height: 34px; border-radius: 17px; border: none; background: white; color: black; }\n"
"QLineEdit:disabled { color: white; }\n"
""));
        anode_curr_edit->setAlignment(Qt::AlignCenter);
        anode_curr_edit->setReadOnly(true);

        gridLayout_3->addWidget(anode_curr_edit, 3, 2, 1, 1, Qt::AlignHCenter);

        verticalSpacer_19 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_19, 0, 2, 1, 1);

        slm_hv_button = new QPushButton(slm_group);
        slm_hv_button->setObjectName(QString::fromUtf8("slm_hv_button"));
        slm_hv_button->setMinimumSize(QSize(100, 100));
        slm_hv_button->setMaximumSize(QSize(100, 100));
        slm_hv_button->setFont(font5);
        slm_hv_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 50px;\n"
"	padding-top: 3px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_3->addWidget(slm_hv_button, 1, 5, 3, 1);

        verticalSpacer_9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_9, 2, 2, 1, 1);

        gridLayout_3->setRowStretch(0, 8);
        gridLayout_3->setRowStretch(2, 1);
        gridLayout_3->setRowStretch(4, 8);

        gridLayout_10->addWidget(slm_group, 1, 0, 2, 1);

        gridLayout_10->setRowStretch(0, 3);
        gridLayout_10->setRowStretch(1, 1);
        gridLayout_10->setRowStretch(2, 1);
        gridLayout_10->setRowStretch(3, 1);
        gridLayout_10->setRowStretch(4, 3);
        gridLayout_10->setColumnStretch(0, 5);
        gridLayout_10->setColumnStretch(1, 3);
        gridLayout_10->setColumnStretch(2, 1);
        tabWidget->addTab(data_tab, QString());
        plot_tab = new QWidget();
        plot_tab->setObjectName(QString::fromUtf8("plot_tab"));
        plot_tab->setFont(font);
        gridLayout_7 = new QGridLayout(plot_tab);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_7->setContentsMargins(35, -1, 35, -1);
        verticalSpacer_23 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_7->addItem(verticalSpacer_23, 0, 0, 1, 2);

        power_plot = new QCustomPlot(plot_tab);
        power_plot->setObjectName(QString::fromUtf8("power_plot"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(2);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(power_plot->sizePolicy().hasHeightForWidth());
        power_plot->setSizePolicy(sizePolicy1);
        power_plot->setStyleSheet(QString::fromUtf8(""));

        gridLayout_7->addWidget(power_plot, 3, 0, 1, 2);

        press_plot = new QCustomPlot(plot_tab);
        press_plot->setObjectName(QString::fromUtf8("press_plot"));
        sizePolicy1.setHeightForWidth(press_plot->sizePolicy().hasHeightForWidth());
        press_plot->setSizePolicy(sizePolicy1);
        press_plot->setStyleSheet(QString::fromUtf8(""));

        gridLayout_7->addWidget(press_plot, 1, 0, 1, 2);

        beam_plot = new QCustomPlot(plot_tab);
        beam_plot->setObjectName(QString::fromUtf8("beam_plot"));
        sizePolicy1.setHeightForWidth(beam_plot->sizePolicy().hasHeightForWidth());
        beam_plot->setSizePolicy(sizePolicy1);
        beam_plot->setStyleSheet(QString::fromUtf8(""));

        gridLayout_7->addWidget(beam_plot, 2, 0, 1, 2);

        horizontalSpacer_37 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_37, 2, 2, 1, 1);

        tabWidget->addTab(plot_tab, QString());
        log_tab = new QWidget();
        log_tab->setObjectName(QString::fromUtf8("log_tab"));
        gridLayout_13 = new QGridLayout(log_tab);
        gridLayout_13->setSpacing(6);
        gridLayout_13->setContentsMargins(11, 11, 11, 11);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        horizontalSpacer_20 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_13->addItem(horizontalSpacer_20, 1, 0, 1, 1);

        horizontalSpacer_21 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_13->addItem(horizontalSpacer_21, 1, 2, 1, 1);

        verticalSpacer_25 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_13->addItem(verticalSpacer_25, 2, 1, 1, 1);

        log_group = new QGroupBox(log_tab);
        log_group->setObjectName(QString::fromUtf8("log_group"));
        log_group->setFont(font12);
        log_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: white;\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        verticalLayout_3 = new QVBoxLayout(log_group);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(16, 32, 0, 0);
        log_box = new QTextEdit(log_group);
        log_box->setObjectName(QString::fromUtf8("log_box"));
        log_box->setFont(font);
        log_box->setStyleSheet(QString::fromUtf8("border: none; background: rgb(255,255,255,0);"));

        verticalLayout_3->addWidget(log_box);


        gridLayout_13->addWidget(log_group, 1, 1, 1, 1);

        verticalSpacer_28 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_13->addItem(verticalSpacer_28, 0, 1, 1, 1);

        gridLayout_13->setRowStretch(0, 1);
        gridLayout_13->setRowStretch(1, 10);
        gridLayout_13->setRowStretch(2, 1);
        gridLayout_13->setColumnStretch(0, 1);
        gridLayout_13->setColumnStretch(1, 15);
        gridLayout_13->setColumnStretch(2, 1);
        tabWidget->addTab(log_tab, QString());
        admin_tab = new QWidget();
        admin_tab->setObjectName(QString::fromUtf8("admin_tab"));
        admin_tab->setFont(font);
        gridLayout = new QGridLayout(admin_tab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalSpacer_20 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_20, 0, 1, 1, 1);

        verticalSpacer_24 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_24, 4, 1, 1, 1);

        pid_const_group = new QGroupBox(admin_tab);
        pid_const_group->setObjectName(QString::fromUtf8("pid_const_group"));
        pid_const_group->setFont(font5);
        pid_const_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: rgb(250,250,250);\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_22 = new QGridLayout(pid_const_group);
        gridLayout_22->setSpacing(6);
        gridLayout_22->setContentsMargins(11, 11, 11, 11);
        gridLayout_22->setObjectName(QString::fromUtf8("gridLayout_22"));
        gridLayout_22->setHorizontalSpacing(10);
        gridLayout_22->setVerticalSpacing(20);
        gridLayout_22->setContentsMargins(15, 36, 25, 29);
        horizontalSpacer_41 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_22->addItem(horizontalSpacer_41, 1, 0, 1, 1);

        beam_kd_write = new QLineEdit(pid_const_group);
        beam_kd_write->setObjectName(QString::fromUtf8("beam_kd_write"));
        beam_kd_write->setMinimumSize(QSize(100, 0));
        beam_kd_write->setMaximumSize(QSize(100, 16777215));
        beam_kd_write->setFont(font);
        beam_kd_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        beam_kd_write->setAlignment(Qt::AlignCenter);

        gridLayout_22->addWidget(beam_kd_write, 3, 2, 1, 1);

        label_35 = new QLabel(pid_const_group);
        label_35->setObjectName(QString::fromUtf8("label_35"));
        label_35->setMaximumSize(QSize(16777215, 34));
        label_35->setFont(font6);
        label_35->setStyleSheet(QString::fromUtf8("/*QLabel:disabled{ color: rgb(180,180,180); }*/\n"
"background: rgb(250,250,250);\n"
"color: rgb(30,30,30);"));
        label_35->setAlignment(Qt::AlignCenter);

        gridLayout_22->addWidget(label_35, 0, 3, 1, 1);

        label_16 = new QLabel(pid_const_group);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setFont(font1);
        label_16->setStyleSheet(QString::fromUtf8("/*QLabel:disabled{ color: rgb(180,180,180); }*/\n"
"background: rgb(250,250,250);\n"
"color: rgb(30,30,30);"));

        gridLayout_22->addWidget(label_16, 1, 1, 1, 1, Qt::AlignRight);

        label_25 = new QLabel(pid_const_group);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setFont(font1);
        label_25->setStyleSheet(QString::fromUtf8("/*QLabel:disabled{ color: rgb(180,180,180); }*/\n"
"background: rgb(250,250,250);\n"
"color: rgb(30,30,30);"));

        gridLayout_22->addWidget(label_25, 2, 1, 1, 1, Qt::AlignRight);

        fki_write = new QLineEdit(pid_const_group);
        fki_write->setObjectName(QString::fromUtf8("fki_write"));
        fki_write->setMinimumSize(QSize(100, 0));
        fki_write->setMaximumSize(QSize(100, 16777215));
        fki_write->setFont(font);
        fki_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        fki_write->setAlignment(Qt::AlignCenter);

        gridLayout_22->addWidget(fki_write, 2, 3, 1, 1);

        label_27 = new QLabel(pid_const_group);
        label_27->setObjectName(QString::fromUtf8("label_27"));
        label_27->setMaximumSize(QSize(16777215, 34));
        label_27->setFont(font6);
        label_27->setStyleSheet(QString::fromUtf8("/*QLabel:disabled{ color: rgb(180,180,180); }*/\n"
"background: rgb(250,250,250);\n"
"color: rgb(30,30,30);"));

        gridLayout_22->addWidget(label_27, 0, 2, 1, 1);

        beam_ki_write = new QLineEdit(pid_const_group);
        beam_ki_write->setObjectName(QString::fromUtf8("beam_ki_write"));
        beam_ki_write->setMinimumSize(QSize(100, 0));
        beam_ki_write->setMaximumSize(QSize(100, 16777215));
        beam_ki_write->setFont(font);
        beam_ki_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        beam_ki_write->setAlignment(Qt::AlignCenter);

        gridLayout_22->addWidget(beam_ki_write, 2, 2, 1, 1);

        fkp_write = new QLineEdit(pid_const_group);
        fkp_write->setObjectName(QString::fromUtf8("fkp_write"));
        fkp_write->setMinimumSize(QSize(100, 0));
        fkp_write->setMaximumSize(QSize(100, 16777215));
        fkp_write->setFont(font);
        fkp_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        fkp_write->setAlignment(Qt::AlignCenter);

        gridLayout_22->addWidget(fkp_write, 1, 3, 1, 1);

        fkd_write = new QLineEdit(pid_const_group);
        fkd_write->setObjectName(QString::fromUtf8("fkd_write"));
        fkd_write->setMinimumSize(QSize(100, 0));
        fkd_write->setMaximumSize(QSize(100, 16777215));
        fkd_write->setFont(font);
        fkd_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        fkd_write->setAlignment(Qt::AlignCenter);

        gridLayout_22->addWidget(fkd_write, 3, 3, 1, 1);

        label_26 = new QLabel(pid_const_group);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        label_26->setFont(font1);
        label_26->setStyleSheet(QString::fromUtf8("/*QLabel:disabled{ color: rgb(180,180,180); }*/\n"
"background: rgb(250,250,250);\n"
"color: rgb(30,30,30);"));

        gridLayout_22->addWidget(label_26, 3, 1, 1, 1, Qt::AlignRight);

        beam_kp_write = new QLineEdit(pid_const_group);
        beam_kp_write->setObjectName(QString::fromUtf8("beam_kp_write"));
        beam_kp_write->setMinimumSize(QSize(100, 0));
        beam_kp_write->setMaximumSize(QSize(100, 16777215));
        beam_kp_write->setFont(font);
        beam_kp_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        beam_kp_write->setAlignment(Qt::AlignCenter);

        gridLayout_22->addWidget(beam_kp_write, 1, 2, 1, 1);

        horizontalSpacer_42 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_22->addItem(horizontalSpacer_42, 1, 4, 1, 1);

        gridLayout_22->setRowStretch(0, 1);
        gridLayout_22->setRowStretch(1, 1);
        gridLayout_22->setRowStretch(2, 1);
        gridLayout_22->setRowStretch(3, 1);

        gridLayout->addWidget(pid_const_group, 1, 1, 3, 1);

        reconnect_group = new QGroupBox(admin_tab);
        reconnect_group->setObjectName(QString::fromUtf8("reconnect_group"));
        reconnect_group->setFont(font5);
        reconnect_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: rgb(250,250,250);\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_11 = new QGridLayout(reconnect_group);
        gridLayout_11->setSpacing(6);
        gridLayout_11->setContentsMargins(11, 11, 11, 11);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        gridLayout_11->setHorizontalSpacing(10);
        gridLayout_11->setVerticalSpacing(20);
        gridLayout_11->setContentsMargins(15, 40, 15, 28);
        spc_recon_button = new QPushButton(reconnect_group);
        spc_recon_button->setObjectName(QString::fromUtf8("spc_recon_button"));
        spc_recon_button->setMinimumSize(QSize(100, 40));
        spc_recon_button->setMaximumSize(QSize(100, 40));
        spc_recon_button->setFont(font13);
        spc_recon_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	color: white;\n"
"	border: none;\n"
"	border-radius: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_11->addWidget(spc_recon_button, 1, 0, 1, 1);

        rigol_recon_button = new QPushButton(reconnect_group);
        rigol_recon_button->setObjectName(QString::fromUtf8("rigol_recon_button"));
        rigol_recon_button->setMinimumSize(QSize(100, 40));
        rigol_recon_button->setMaximumSize(QSize(100, 40));
        rigol_recon_button->setFont(font13);
        rigol_recon_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	color: white;\n"
"	border: none;\n"
"	border-radius: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_11->addWidget(rigol_recon_button, 0, 2, 1, 1);

        dxm_recon_button = new QPushButton(reconnect_group);
        dxm_recon_button->setObjectName(QString::fromUtf8("dxm_recon_button"));
        dxm_recon_button->setMinimumSize(QSize(100, 40));
        dxm_recon_button->setMaximumSize(QSize(100, 40));
        dxm_recon_button->setFont(font13);
        dxm_recon_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	color: white;\n"
"	border: none;\n"
"	border-radius: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_11->addWidget(dxm_recon_button, 0, 0, 1, 1);

        slm_recon_button = new QPushButton(reconnect_group);
        slm_recon_button->setObjectName(QString::fromUtf8("slm_recon_button"));
        slm_recon_button->setMinimumSize(QSize(100, 40));
        slm_recon_button->setMaximumSize(QSize(100, 40));
        slm_recon_button->setFont(font13);
        slm_recon_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	color: white;\n"
"	border: none;\n"
"	border-radius: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_11->addWidget(slm_recon_button, 0, 1, 1, 1);

        fms_recon_button = new QPushButton(reconnect_group);
        fms_recon_button->setObjectName(QString::fromUtf8("fms_recon_button"));
        fms_recon_button->setMinimumSize(QSize(100, 40));
        fms_recon_button->setMaximumSize(QSize(100, 40));
        fms_recon_button->setFont(font13);
        fms_recon_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	color: white;\n"
"	border: none;\n"
"	border-radius: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_11->addWidget(fms_recon_button, 1, 1, 1, 1);

        lj_recon_button = new QPushButton(reconnect_group);
        lj_recon_button->setObjectName(QString::fromUtf8("lj_recon_button"));
        lj_recon_button->setMinimumSize(QSize(100, 40));
        lj_recon_button->setMaximumSize(QSize(100, 40));
        lj_recon_button->setFont(font13);
        lj_recon_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	color: white;\n"
"	border: none;\n"
"	border-radius: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_11->addWidget(lj_recon_button, 1, 2, 1, 1);


        gridLayout->addWidget(reconnect_group, 5, 1, 1, 1);

        verticalSpacer_26 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_26, 6, 1, 1, 1);

        horizontalSpacer_44 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_44, 1, 4, 1, 1);

        ramp_group = new QGroupBox(admin_tab);
        ramp_group->setObjectName(QString::fromUtf8("ramp_group"));
        ramp_group->setFont(font5);
        ramp_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: rgb(250,250,250);\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_17 = new QGridLayout(ramp_group);
        gridLayout_17->setSpacing(6);
        gridLayout_17->setContentsMargins(11, 11, 11, 11);
        gridLayout_17->setObjectName(QString::fromUtf8("gridLayout_17"));
        gridLayout_17->setHorizontalSpacing(10);
        gridLayout_17->setVerticalSpacing(20);
        gridLayout_17->setContentsMargins(15, 40, 15, 28);
        ramp_sp_write = new QLineEdit(ramp_group);
        ramp_sp_write->setObjectName(QString::fromUtf8("ramp_sp_write"));
        ramp_sp_write->setMinimumSize(QSize(100, 0));
        ramp_sp_write->setMaximumSize(QSize(100, 16777215));
        ramp_sp_write->setFont(font);
        ramp_sp_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        ramp_sp_write->setAlignment(Qt::AlignCenter);

        gridLayout_17->addWidget(ramp_sp_write, 0, 2, 1, 1);

        horizontalSpacer_39 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_17->addItem(horizontalSpacer_39, 0, 0, 1, 1);

        label_28 = new QLabel(ramp_group);
        label_28->setObjectName(QString::fromUtf8("label_28"));
        label_28->setFont(font6);
        label_28->setStyleSheet(QString::fromUtf8("/*QLabel:disabled{ color: rgb(180,180,180); }*/\n"
"background: rgb(250,250,250);\n"
"color: rgb(30,30,30);"));
        label_28->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_17->addWidget(label_28, 0, 1, 1, 1);

        label_29 = new QLabel(ramp_group);
        label_29->setObjectName(QString::fromUtf8("label_29"));
        label_29->setFont(font6);
        label_29->setStyleSheet(QString::fromUtf8("/*QLabel:disabled{ color: rgb(180,180,180); }*/\n"
"background: rgb(250,250,250);\n"
"color: rgb(30,30,30);"));
        label_29->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_17->addWidget(label_29, 1, 1, 1, 1);

        ramp_time_write = new QLineEdit(ramp_group);
        ramp_time_write->setObjectName(QString::fromUtf8("ramp_time_write"));
        ramp_time_write->setMinimumSize(QSize(100, 0));
        ramp_time_write->setMaximumSize(QSize(100, 16777215));
        ramp_time_write->setFont(font);
        ramp_time_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        ramp_time_write->setAlignment(Qt::AlignCenter);

        gridLayout_17->addWidget(ramp_time_write, 1, 2, 1, 1);

        horizontalSpacer_40 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_17->addItem(horizontalSpacer_40, 0, 3, 1, 1);


        gridLayout->addWidget(ramp_group, 1, 3, 1, 1);

        horizontalSpacer_45 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_45, 1, 2, 1, 1);

        misc_group = new QGroupBox(admin_tab);
        misc_group->setObjectName(QString::fromUtf8("misc_group"));
        misc_group->setFont(font5);
        misc_group->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"/*border: 1px solid rgb(180, 180, 180);*/\n"
"border: none;\n"
"background: rgb(250,250,250);\n"
"border-radius: 20px;\n"
"margin-top: 12px;\n"
"border: 1px solid rgb(220,220,220);\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"subcontrol-origin: margin;\n"
"/*left: 20px;*/\n"
"padding: 0px 3px;\n"
"left: 12px;\n"
"top: 18px;\n"
"color: rgb(85, 87, 83);\n"
"}\n"
"\n"
"QGroupBox::title:disabled {\n"
"color: rgb(180,180,180);\n"
"}"));
        gridLayout_12 = new QGridLayout(misc_group);
        gridLayout_12->setSpacing(6);
        gridLayout_12->setContentsMargins(11, 11, 11, 11);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        gridLayout_12->setHorizontalSpacing(10);
        gridLayout_12->setVerticalSpacing(13);
        gridLayout_12->setContentsMargins(15, 40, 15, 15);
        reconfig_button = new QPushButton(misc_group);
        reconfig_button->setObjectName(QString::fromUtf8("reconfig_button"));
        reconfig_button->setMinimumSize(QSize(100, 40));
        reconfig_button->setMaximumSize(QSize(100, 40));
        reconfig_button->setFont(font13);
        reconfig_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	color: white;\n"
"	border: none;\n"
"	border-radius: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_12->addWidget(reconfig_button, 1, 0, 1, 1);

        log_rate_write = new QLineEdit(misc_group);
        log_rate_write->setObjectName(QString::fromUtf8("log_rate_write"));
        log_rate_write->setMinimumSize(QSize(100, 0));
        log_rate_write->setMaximumSize(QSize(100, 16777215));
        log_rate_write->setFont(font);
        log_rate_write->setStyleSheet(QString::fromUtf8("/*background-color: rgb(220,220,220);*/\n"
"QLineEdit {\n"
"	height: 34px;\n"
"	border-radius: 17px;\n"
"	border: 1px solid rgb(220, 220, 220);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	border: none;\n"
"}"));
        log_rate_write->setAlignment(Qt::AlignCenter);

        gridLayout_12->addWidget(log_rate_write, 1, 1, 1, 1);

        label_32 = new QLabel(misc_group);
        label_32->setObjectName(QString::fromUtf8("label_32"));
        label_32->setFont(font6);
        label_32->setStyleSheet(QString::fromUtf8("/*QLabel:disabled{ color: rgb(180,180,180); }*/\n"
"background: rgb(250,250,250);\n"
"color: rgb(30,30,30);"));
        label_32->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_32, 0, 1, 1, 1, Qt::AlignHCenter|Qt::AlignBottom);

        clear_status_button = new QPushButton(misc_group);
        clear_status_button->setObjectName(QString::fromUtf8("clear_status_button"));
        clear_status_button->setMinimumSize(QSize(100, 40));
        clear_status_button->setMaximumSize(QSize(100, 40));
        clear_status_button->setFont(font13);
        clear_status_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	color: white;\n"
"	border: none;\n"
"	border-radius: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));

        gridLayout_12->addWidget(clear_status_button, 0, 0, 1, 1);

        label_33 = new QLabel(misc_group);
        label_33->setObjectName(QString::fromUtf8("label_33"));
        QFont font14;
        font14.setFamily(QString::fromUtf8("Open Sans"));
        label_33->setFont(font14);
        label_33->setStyleSheet(QString::fromUtf8("color: rgb(180,180,180);\n"
"background: none;"));

        gridLayout_12->addWidget(label_33, 2, 0, 1, 2);


        gridLayout->addWidget(misc_group, 5, 3, 1, 1);

        horizontalSpacer_43 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_43, 1, 0, 1, 1);

        verticalSpacer_27 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_27, 2, 3, 1, 1);

        tabWidget->addTab(admin_tab, QString());
        faults_tab = new QWidget();
        faults_tab->setObjectName(QString::fromUtf8("faults_tab"));
        treeWidget = new QTreeWidget(faults_tab);
        treeWidget->headerItem()->setText(0, QString());
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setFont(0, font);
        treeWidget->setHeaderItem(__qtreewidgetitem);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/circle_green"), QSize(), QIcon::Normal, QIcon::Off);
        QFont font15;
        font15.setFamily(QString::fromUtf8("Open Sans"));
        font15.setPointSize(14);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem(treeWidget);
        __qtreewidgetitem1->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem1->setFont(0, font15);
        __qtreewidgetitem1->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem(__qtreewidgetitem1);
        __qtreewidgetitem2->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem2->setFont(0, font15);
        __qtreewidgetitem2->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem3 = new QTreeWidgetItem(__qtreewidgetitem1);
        __qtreewidgetitem3->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem3->setFont(0, font15);
        __qtreewidgetitem3->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem4 = new QTreeWidgetItem(__qtreewidgetitem1);
        __qtreewidgetitem4->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem4->setFont(0, font15);
        __qtreewidgetitem4->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem5 = new QTreeWidgetItem(__qtreewidgetitem1);
        __qtreewidgetitem5->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem5->setFont(0, font15);
        __qtreewidgetitem5->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem6 = new QTreeWidgetItem(__qtreewidgetitem1);
        __qtreewidgetitem6->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem6->setFont(0, font15);
        __qtreewidgetitem6->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem7 = new QTreeWidgetItem(__qtreewidgetitem1);
        __qtreewidgetitem7->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem7->setFont(0, font15);
        __qtreewidgetitem7->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem8 = new QTreeWidgetItem(treeWidget);
        __qtreewidgetitem8->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem8->setFont(0, font15);
        __qtreewidgetitem8->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem9 = new QTreeWidgetItem(__qtreewidgetitem8);
        __qtreewidgetitem9->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem9->setFont(0, font15);
        __qtreewidgetitem9->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem10 = new QTreeWidgetItem(__qtreewidgetitem8);
        __qtreewidgetitem10->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem10->setFont(0, font15);
        __qtreewidgetitem10->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem11 = new QTreeWidgetItem(__qtreewidgetitem8);
        __qtreewidgetitem11->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem11->setFont(0, font15);
        __qtreewidgetitem11->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem12 = new QTreeWidgetItem(__qtreewidgetitem8);
        __qtreewidgetitem12->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem12->setFont(0, font15);
        __qtreewidgetitem12->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem13 = new QTreeWidgetItem(__qtreewidgetitem8);
        __qtreewidgetitem13->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem13->setFont(0, font15);
        __qtreewidgetitem13->setIcon(0, icon);
        QTreeWidgetItem *__qtreewidgetitem14 = new QTreeWidgetItem(__qtreewidgetitem8);
        __qtreewidgetitem14->setFlags(Qt::ItemIsEnabled);
        __qtreewidgetitem14->setFont(0, font15);
        __qtreewidgetitem14->setIcon(0, icon);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setGeometry(QRect(392, 136, 256, 500));
        treeWidget->setMinimumSize(QSize(0, 500));
        treeWidget->setFont(font);
        treeWidget->setFocusPolicy(Qt::NoFocus);
        treeWidget->setFrameShape(QFrame::NoFrame);
        treeWidget->setAlternatingRowColors(false);
        treeWidget->header()->setVisible(false);
        dxm_reset_button = new QPushButton(faults_tab);
        dxm_reset_button->setObjectName(QString::fromUtf8("dxm_reset_button"));
        dxm_reset_button->setGeometry(QRect(750, 140, 150, 46));
        dxm_reset_button->setMinimumSize(QSize(150, 46));
        dxm_reset_button->setMaximumSize(QSize(150, 46));
        dxm_reset_button->setFont(font5);
        dxm_reset_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 23px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));
        slm_reset_button = new QPushButton(faults_tab);
        slm_reset_button->setObjectName(QString::fromUtf8("slm_reset_button"));
        slm_reset_button->setGeometry(QRect(750, 310, 150, 46));
        slm_reset_button->setMinimumSize(QSize(150, 46));
        slm_reset_button->setMaximumSize(QSize(150, 46));
        slm_reset_button->setFont(font5);
        slm_reset_button->setStyleSheet(QString::fromUtf8("QPushButton:hover:pressed {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));\n"
"}\n"
"\n"
"QPushButton {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B);\n"
"	border: none;\n"
"	color: white;\n"
"	border-radius: 23px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));\n"
"}"));
        tabWidget->addTab(faults_tab, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "Ion Pump", nullptr));
        press_label->setText(QApplication::translate("MainWindow", "----------", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "Torr", nullptr));
        rigol1_group->setTitle(QApplication::translate("MainWindow", "Shim Coil", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Voltage (V)", nullptr));
        label_20->setText(QApplication::translate("MainWindow", "Current (A)", nullptr));
        shim_volt_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        shim_curr_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        shim_volt_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        shim_curr_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        rigol1_hv_button->setText(QApplication::translate("MainWindow", "ENABLE\n"
"SHIM", nullptr));
        dxm_group->setTitle(QApplication::translate("MainWindow", "Cathode", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "Collector Current (mA)", nullptr));
        beam_volt_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        dxm_curr_label->setText(QApplication::translate("MainWindow", "Beam Current (mA)", nullptr));
        body_read->setText(QApplication::translate("MainWindow", "-------", nullptr));
        fil_curr_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        dxm_hv_button->setText(QApplication::translate("MainWindow", "ENABLE\n"
"HV", nullptr));
        collector_read->setText(QApplication::translate("MainWindow", "-------", nullptr));
        ramp_up_button->setText(QApplication::translate("MainWindow", "\342\206\221 RAMP UP", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Beam Voltage Limit (kV)", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "Body Current (mA)", nullptr));
        label->setText(QApplication::translate("MainWindow", "Filament Current (A)", nullptr));
        fil_curr_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        beam_volt_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        beam_curr_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        ramp_down_button->setText(QApplication::translate("MainWindow", "RAMP DOWN \342\206\223", nullptr));
        beam_curr_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        fms_group->setTitle(QApplication::translate("MainWindow", "FMS", nullptr));
        freq_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        label_18->setText(QApplication::translate("MainWindow", "Frequency (GHz)", nullptr));
        freq_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        pid_group->setTitle(QApplication::translate("MainWindow", "Automatic Control", nullptr));
        power_pid_button->setText(QApplication::translate("MainWindow", "POWER", nullptr));
        beam_curr_pid_button->setText(QApplication::translate("MainWindow", "CURRENT", nullptr));
        freq_pid_button->setText(QApplication::translate("MainWindow", "FREQUENCY", nullptr));
        rigol2_group->setTitle(QApplication::translate("MainWindow", "Cavity Trim Coil", nullptr));
        trim_volt_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        trim_curr_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "Voltage (V)", nullptr));
        label_23->setText(QApplication::translate("MainWindow", "Current (A)", nullptr));
        trim_volt_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        rigol2_hv_button->setText(QApplication::translate("MainWindow", "ENABLE\n"
"CTC", nullptr));
        trim_curr_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        lj_curr_group->setTitle(QApplication::translate("MainWindow", "RF Monitor", nullptr));
        power_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        label_17->setText(QApplication::translate("MainWindow", "Power (W)", nullptr));
        power_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        slm_group->setTitle(QApplication::translate("MainWindow", "Mod Anode", nullptr));
        anode_volt_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Voltage (kV)", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "Current Limit (mA)", nullptr));
        anode_curr_button->setText(QApplication::translate("MainWindow", "\342\234\216", nullptr));
        anode_volt_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        anode_curr_edit->setText(QApplication::translate("MainWindow", "--------", nullptr));
        slm_hv_button->setText(QApplication::translate("MainWindow", "ENABLE\n"
"HV", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(data_tab), QApplication::translate("MainWindow", "Data", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(plot_tab), QApplication::translate("MainWindow", "Plot", nullptr));
        log_group->setTitle(QApplication::translate("MainWindow", "Log Stream", nullptr));
        log_box->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Open Sans'; font-size:12pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(log_tab), QApplication::translate("MainWindow", "Log", nullptr));
        pid_const_group->setTitle(QApplication::translate("MainWindow", "PID", nullptr));
        label_35->setText(QApplication::translate("MainWindow", "Frequency\n"
"PID Constants", nullptr));
        label_16->setText(QApplication::translate("MainWindow", "Kp", nullptr));
        label_25->setText(QApplication::translate("MainWindow", "Ki", nullptr));
        label_27->setText(QApplication::translate("MainWindow", "Beam Current\n"
"PID Constants", nullptr));
        label_26->setText(QApplication::translate("MainWindow", "Kd", nullptr));
        reconnect_group->setTitle(QApplication::translate("MainWindow", "Reconnect Devices", nullptr));
        spc_recon_button->setText(QApplication::translate("MainWindow", "SPCe", nullptr));
        rigol_recon_button->setText(QApplication::translate("MainWindow", "RIGOL", nullptr));
        dxm_recon_button->setText(QApplication::translate("MainWindow", "DXM", nullptr));
        slm_recon_button->setText(QApplication::translate("MainWindow", "SLM", nullptr));
        fms_recon_button->setText(QApplication::translate("MainWindow", "FMS", nullptr));
        lj_recon_button->setText(QApplication::translate("MainWindow", "LABJACK", nullptr));
        ramp_group->setTitle(QApplication::translate("MainWindow", "Ramp Rate", nullptr));
        label_28->setText(QApplication::translate("MainWindow", "Ramp SP (A)", nullptr));
        label_29->setText(QApplication::translate("MainWindow", "Ramp Time (min)", nullptr));
        ramp_time_write->setText(QString());
        misc_group->setTitle(QApplication::translate("MainWindow", "Misc", nullptr));
        reconfig_button->setText(QApplication::translate("MainWindow", "PARSE CONFIG", nullptr));
        log_rate_write->setText(QString());
        log_rate_write->setPlaceholderText(QString());
        label_32->setText(QApplication::translate("MainWindow", "Log Rate (sec)", nullptr));
        clear_status_button->setText(QApplication::translate("MainWindow", "CLR STATUS", nullptr));
        label_33->setText(QApplication::translate("MainWindow", "Note: blank = log everything", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(admin_tab), QApplication::translate("MainWindow", "Admin", nullptr));

        const bool __sortingEnabled = treeWidget->isSortingEnabled();
        treeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->topLevelItem(0);
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Cathode Sum Fault", nullptr));
        QTreeWidgetItem *___qtreewidgetitem1 = ___qtreewidgetitem->child(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "Arc", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = ___qtreewidgetitem->child(1);
        ___qtreewidgetitem2->setText(0, QApplication::translate("MainWindow", "Over Temperature", nullptr));
        QTreeWidgetItem *___qtreewidgetitem3 = ___qtreewidgetitem->child(2);
        ___qtreewidgetitem3->setText(0, QApplication::translate("MainWindow", "Over Voltage", nullptr));
        QTreeWidgetItem *___qtreewidgetitem4 = ___qtreewidgetitem->child(3);
        ___qtreewidgetitem4->setText(0, QApplication::translate("MainWindow", "Under Voltage", nullptr));
        QTreeWidgetItem *___qtreewidgetitem5 = ___qtreewidgetitem->child(4);
        ___qtreewidgetitem5->setText(0, QApplication::translate("MainWindow", "Over Current", nullptr));
        QTreeWidgetItem *___qtreewidgetitem6 = ___qtreewidgetitem->child(5);
        ___qtreewidgetitem6->setText(0, QApplication::translate("MainWindow", "Under Current", nullptr));
        QTreeWidgetItem *___qtreewidgetitem7 = treeWidget->topLevelItem(1);
        ___qtreewidgetitem7->setText(0, QApplication::translate("MainWindow", "Mod Anode Sum Fault", nullptr));
        QTreeWidgetItem *___qtreewidgetitem8 = ___qtreewidgetitem7->child(0);
        ___qtreewidgetitem8->setText(0, QApplication::translate("MainWindow", "Arc", nullptr));
        QTreeWidgetItem *___qtreewidgetitem9 = ___qtreewidgetitem7->child(1);
        ___qtreewidgetitem9->setText(0, QApplication::translate("MainWindow", "Over Temperature", nullptr));
        QTreeWidgetItem *___qtreewidgetitem10 = ___qtreewidgetitem7->child(2);
        ___qtreewidgetitem10->setText(0, QApplication::translate("MainWindow", "Over Voltage", nullptr));
        QTreeWidgetItem *___qtreewidgetitem11 = ___qtreewidgetitem7->child(3);
        ___qtreewidgetitem11->setText(0, QApplication::translate("MainWindow", "Under Voltage", nullptr));
        QTreeWidgetItem *___qtreewidgetitem12 = ___qtreewidgetitem7->child(4);
        ___qtreewidgetitem12->setText(0, QApplication::translate("MainWindow", "Over Current", nullptr));
        QTreeWidgetItem *___qtreewidgetitem13 = ___qtreewidgetitem7->child(5);
        ___qtreewidgetitem13->setText(0, QApplication::translate("MainWindow", "Under Current", nullptr));
        treeWidget->setSortingEnabled(__sortingEnabled);

        dxm_reset_button->setText(QApplication::translate("MainWindow", "RESET FAULTS", nullptr));
        slm_reset_button->setText(QApplication::translate("MainWindow", "RESET FAULTS", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(faults_tab), QApplication::translate("MainWindow", "Faults", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
