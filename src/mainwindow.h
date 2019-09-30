#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QTime>
#include <QVector>
#include <vector>
#include "lib/face/face.h"
#include "lib/qcustomplot.h"
#include "gyrotron.h"

#define WM_NCHITTEST                    0x0084


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
    void on_beam_pid_button_clicked();
    void on_power_pid_button_clicked();
    void on_freq_pid_button_clicked();
    void on_reconfig_button_clicked();
    void on_beam_volt_button_clicked();
    void on_beam_curr_button_clicked();
    void on_power_button_clicked();
    void on_freq_button_clicked();
    void on_gtc_curr_button_clicked();
    void on_gtc_volt_button_clicked();
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
    void on_maximize_button_clicked();
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
    void press_context_menu(const QPoint &pos);
    void clear_press_data();
    void beam_context_menu(const QPoint &pos);
    void clear_beam_data();
    void power_context_menu(const QPoint &pos);
    void clear_power_data();

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
    void set_blink_enabled(bool enable);
    bool lists_equal(std::vector<QListWidgetItem*> list1, std::vector<QListWidgetItem*> list2);
    void update_margins();

    Ui::MainWindow *ui;
    Face gui{this};
    Gyrotron gyro;
    std::vector<QGraphicsDropShadowEffect*> shadows;
    QTimer data_timer, blink_timer;
    int blink_duration{500};
    bool blink_on{false};
    QPoint oldPos;
    std::vector<SmartLineEdit*> smart_edits;
    int last_known_state{0}, last_fault_status{0}, resize_tracker{0};
    bool window_locked{false};
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

    QString grey_state_bubble{"QFrame { background-color: rgb(85,87,83); color: white; border: none; border-radius: 100px; }"};
    QString green_state_bubble{"QFrame { background-color: #46812B; color: white; border: none; border-radius: 100px; }"};
    QString orange_state_bubble{"QFrame { background-color: #F37021; color: white; border: none; border-radius: 100px; }"};
    QString red_state_bubble{"QFrame { background-color: #D16055; color: white; border: none; border-radius: 100px; }"};

    QString green_status_bubble{"QFrame { background: #46812B; color: white; border: none; border-radius: 40px; }"
                                "QFrame:disabled { background: rgb(220,220,220); }"};
    QString yellow_status_bubble{"QFrame { background: #E89831; color: white; border: none; border-radius: 40px; }"
                                "QFrame:disabled { background: rgb(220,220,220); }"};
    QString red_status_bubble{"QFrame { background: #D16055; color: white; border: none; border-radius: 40px; }"
                                "QFrame:disabled { background: rgb(220,220,220); }"};

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

    int init_error_code{0};
    double key, plot_span{0}, refresh_rate{0.5};
    std::string press_str{"----------"};
    char timestamp_full[20], timestamp[10];
    double time_span{900}; // default 15 mins (in sync with gyrotron default)

    QCPItemText *press_plot_label, *beam_plot_label, *power_plot_label, *power_plot_label2;

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
    void resizeEvent(QResizeEvent* evt);
};

#endif // MAINWINDOW_H
