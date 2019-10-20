#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QTime>
#include <QVector>
#include <vector>
#include <filesystem>
//#include "lib/face/face.h"
#include "lib/qcustomplot.h"
//#include "gyrotron.h"
#include "settings_window.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void realtime_slot();
    void on_reconfig_button_clicked();
    void on_beam_volt_button_clicked();
    void on_beam_curr_button_clicked();
    void on_power_button_clicked();
    void on_freq_button_clicked();
    void on_gtc_curr_button_clicked();
    void on_fil_curr_button_clicked();
    void on_prev_state_button_clicked();
    void on_next_state_button_clicked();
    void on_beam_kp_button_clicked();
    void on_beam_ki_button_clicked();
    void on_beam_kd_button_clicked();
    void on_power_kp_button_clicked();
    void on_power_ki_button_clicked();
    void on_power_kd_button_clicked();
    void on_freq_kp_button_clicked();
    void on_freq_ki_button_clicked();
    void on_freq_kd_button_clicked();
    void on_close_button_clicked();
    void on_minimize_button_clicked();
    //void on_maximize_button_clicked();
    void on_control_tab_clicked();
    void on_plot_tab_clicked();
    void on_admin_tab_clicked();
    void on_status_tab_clicked();
    void on_time_span_slider_valueChanged(int value);
    void on_ramp_rate_slider_valueChanged(int value);
    void on_log_rate_slider_valueChanged(int value);
    void on_cath_recon_button_clicked();
    void on_gtc_recon_button_clicked();
    void on_spc_recon_button_clicked();
    void on_fms_recon_button_clicked();
    void on_rsi_recon_button_clicked();
    void on_all_recon_button_clicked();
    void reset_cath_recon();
    void reset_gtc_recon();
    void reset_spc_recon();
    void reset_rsi_recon();
    void reset_fms_recon();
    void reset_all_recon();
    void on_console_edit_returnPressed();
    void on_save_pid_button_clicked();
    void blink_status();
    void plot1_context_menu(const QPoint &pos);
    void plot2_context_menu(const QPoint &pos);
    void plot3_context_menu(const QPoint &pos);
    void on_settings_button_clicked();
    void on_more_button_clicked();
    void on_enable_button_clicked();
    void on_plot1_dropdown_currentIndexChanged(const QString &arg1);
    void on_plot2_dropdown_currentIndexChanged(const QString &arg1);
    void on_plot3_dropdown_currentIndexChanged(const QString &arg1);
    void clear_plot1_data();
    void clear_plot2_data();
    void clear_plot3_data();

private:
    void shutdown();
    void closeEvent (QCloseEvent *event);
    void init_fail_dialog(int err_code);
    void init_gui(); // init plots, apply drop shadows, start data timer, grey out disabled devices
    void init_fields(); // init entry fields, attach to buttons and validators
    void check_faults();
    void init_plots();
    void set_button_green(QPushButton *button, bool turning_green, QString new_text);
    void soft_kill();
    void set_state_transition(double percent);
    bool valid_check(QString qstr, double max_val, double min_val = 0);
    void detect_state_change(bool manual_update = false); // apply stylesheet changes based on state
    void update_plots();
    void check_connections(); // check device connection and PID status then enable/disable groups/buttons accordingly
    void console_print(QString qstr);
    void update_labels();
    void update_indicators();
    void update_faults();
    void update_pid_display();
    void set_blink_on(bool enable);
    void set_blink_enabled(bool enable);
    bool lists_equal(std::vector<QListWidgetItem*> list1, std::vector<QListWidgetItem*> list2);
    void update_margins();
    void set_ui_expanded(bool expand);

    Ui::MainWindow *ui;
    Face gui{this};
    Gyrotron gyro;
    std::vector<QGraphicsDropShadowEffect*> shadows;
    QTimer data_timer, blink_timer;
    QDateTime dateTime; // for system clock
    int blink_duration{500};
    bool blink_on{false}, blink_enabled{true}, use_24hr_format{false};
    QPoint oldPos;
    std::vector<SmartLineEdit*> smart_edits;
    int last_known_state{0}, last_fault_status{0}, resize_tracker{0};
    bool window_locked{false}, admin_mode{true};
    std::atomic<bool> cath_recon_blocked{false}, gtc_recon_blocked{false}, spc_recon_blocked{false};
    std::atomic<bool> rsi_recon_blocked{false}, fms_recon_blocked{false}, all_recon_blocked{false};

    std::vector<QListWidgetItem*> fault_list_items;

    QIcon warning_icon{":/images/warning_yellow.png"};
    QIcon error_icon{":/images/error_red.png"};

    QString tab_selected{"QPushButton{color: rgb(85,87,83); border: none; background: rgb(240,240,240);}"};
    QString tab_unselected{"QPushButton { color: rgb(85,87,83); border: none; background: rgb(220,220,220); }"
                           "QPushButton:hover { background: rgb(225,225,225); }"
                           "QPushButton:hover:pressed { background: rgb(240,240,240); }"};
    QString tab_blink_on{"QPushButton { color: #D16055; text-decoration: underline; border: none; background: #dbc8c8; }"
                         "QPushButton:hover { background: rgb(225,225,225); }"
                         "QPushButton:hover:pressed { background: rgb(240,240,240); }"};
    QString tab_blink_off_selected{"QPushButton { color: #D16055; text-decoration: underline; border: none; background: rgb(240,240,240); }"
                          "QPushButton:hover { background: rgb(225,225,225); }"
                          "QPushButton:hover:pressed { background: rgb(240,240,240); }"};
    QString tab_blink_off_unselected{"QPushButton { color: #D16055; text-decoration: underline; border: none; background: rgb(220,220,220); }"
                          "QPushButton:hover { background: rgb(225,225,225); }"
                          "QPushButton:hover:pressed { background: rgb(240,240,240); }"};

    QString empty_faults_list{"background: white; border: none; background-image: url(:/images/no_faults.png);"
                              "background-position: center; background-repeat: no-repeat;"};
    QString faults_list{"background: white; border: none; background-image: none;"};

    QString recon_button_normal{"QPushButton{background-color: rgb(85,87,83); border: none; color: white; border-radius: 22px;}"
                                "QPushButton:hover:pressed{background-color: rgb(48,49,47);}"
                                "QPushButton:hover{background-color: rgb(66,68,65);}"
                                "QPushButton:disabled{background-color: rgb(200,200,200);}"};
    QString green_recon_button{"QPushButton{background-color: #46812B; border: none; color: white; border-radius: 22px;}"};
    QString red_recon_button{"QPushButton{background-color: #D16055; border: none; color: white; border-radius: 22px;}"};

    QString state_group_min{"QGroupBox { border: none; background: white; border-radius: 20px; border: 1px solid rgb(220,220,220); width: 695px; }"
                            "QGroupBox::title { subcontrol-origin: margin; padding: 0px 3px; left: 12px; top: 6px; color: rgb(85, 87, 83); }"
                            "QGroupBox::title:disabled { color: rgb(180,180,180); }"};
    QString state_group_max{"QGroupBox { border: none; background: white; border-radius: 20px; border: 1px solid rgb(220,220,220); width: 950px; }"
                            "QGroupBox::title { subcontrol-origin: margin; padding: 0px 3px; left: 12px; top: 6px; color: rgb(85, 87, 83); }"
                            "QGroupBox::title:disabled { color: rgb(180,180,180); }"};

    QString lg_grey_state_frame{"QFrame { background-color: rgb(85,87,83); color: white; border: none; width: 400px; height: 400px; border-radius: 200px; }"};
    QString sm_grey_state_frame{"QFrame { background-color: rgb(85,87,83); color: white; border: none; width: 320px; height: 320px; border-radius: 160px; }"};
    QString lg_green_state_frame{"QFrame { background-color: #46812B; color: white; border: none; width: 400px; height: 400px; border-radius: 200px; }"};
    QString sm_green_state_frame{"QFrame { background-color: #46812B; color: white; border: none; width: 320px; height: 320px; border-radius: 160px; }"};
    QString lg_orange_state_frame{"QFrame { background-color: #F37021; color: white; border: none; width: 400px; height: 400px; border-radius: 200px; }"};
    QString sm_orange_state_frame{"QFrame { background-color: #F37021; color: white; border: none; width: 320px; height: 320px; border-radius: 160px; }"};
    QString lg_red_state_frame{"QFrame { background-color: #D16055; color: white; border: none; width: 400px; height: 400px; border-radius: 200px; }"};
    QString sm_red_state_frame{"QFrame { background-color: #D16055; color: white; border: none; width: 320px; height: 320px; border-radius: 160px; }"};

    QString sm_prev_arrow{"QPushButton { width: 216px; height: 216px; border: none; background: none; border-image: url(:/images/back_small.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:hover { border-image: url(:/images/back_small_hover.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:hover:pressed { border-image: url(:/images/back_small_press.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:disabled { border-image: url(:/images/back_small_disabled.png) 0 0 0 0 stretch stretch; }"};
    QString lg_prev_arrow{"QPushButton { width: 270px; height: 270px; border: none; background: none; border-image: url(:/images/back.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:hover { border-image: url(:/images/back_hover.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:hover:pressed { border-image: url(:/images/back_press.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:disabled { border-image: url(:/images/back_disabled.png) 0 0 0 0 stretch stretch; }"};
    QString sm_next_arrow{"QPushButton { width: 216px; height: 216px; border: none; background: none; border-image: url(:/images/forward_small.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:hover { border-image: url(:/images/forward_small_hover.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:hover:pressed { border-image: url(:/images/forward_small_press.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:disabled { border-image: url(:/images/forward_small_disabled.png) 0 0 0 0 stretch stretch; }"};
    QString lg_next_arrow{"QPushButton { width: 270px; height: 270px; border: none; background: none; border-image: url(:/images/forward.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:hover { border-image: url(:/images/forward_hover.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:hover:pressed { border-image: url(:/images/forward_press.png) 0 0 0 0 stretch stretch; }"
                          "QPushButton:disabled { border-image: url(:/images/forward_disabled.png) 0 0 0 0 stretch stretch; }"};

    QString lg_pause_bubble{"QPushButton { width: 200px; height: 200px; border: none; background: #46812B; border-radius: 100px; margin: 10px; color: white; }"
                            "QPushButton:hover { background: #407527; }"
                            "QPushButton:hover:pressed { background: #2f5b12; }"};
    QString sm_pause_bubble{"QPushButton { width: 160px; height: 160px; border: none; background: #46812B; border-radius: 80px; margin: 10px; color: white; }"
                            "QPushButton:hover { background: #407527; }"
                            "QPushButton:hover:pressed { background: #2f5b12; }"};

    QString lg_green_bubble{"QFrame { height: 180px; width: 180px; background: #46812B; color: white; border: none; border-radius: 90px; }"
                            "QFrame:disabled { background: rgb(220,220,220); }"};
    QString sm_green_bubble{"QFrame { height: 120px; width: 180px; background: #46812B; color: white; border: none; border-radius: 60px; }"
                            "QFrame:disabled { background: rgb(220,220,220); }"};
    QString lg_yellow_bubble{"QFrame { height: 180px; width: 180px; background: #E89831; color: white; border: none; border-radius: 90px; }"
                            "QFrame:disabled { background: rgb(220,220,220); }"};
    QString sm_yellow_bubble{"QFrame { height: 120px; width: 180px; background: #E89831; color: white; border: none; border-radius: 60px; }"
                            "QFrame:disabled { background: rgb(220,220,220); }"};
    QString lg_red_bubble{"QFrame { height: 180px; width: 180px; background: #D16055; color: white; border: none; border-radius: 90px; }"
                            "QFrame:disabled { background: rgb(220,220,220); }"};
    QString sm_red_bubble{"QFrame { height: 120px; width: 180px; background: #D16055; color: white; border: none; border-radius: 60px; }"
                            "QFrame:disabled { background: rgb(220,220,220); }"};

    bool showing_more{false};
    int init_error_code{0};
    double key, plot_span{0}, refresh_rate{0.5};
    std::string press_str{"----------"};
    char timestamp_full[20], timestamp[10];
    double time_span{900}; // default 15 mins (in sync with gyrotron default)

    QCPItemText *plot1_label, *plot2_label, *plot3_label;

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
    void resizeEvent(QResizeEvent* evt);
};

#endif // MAINWINDOW_H
