#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QTime>
#include <QVector>
#include <vector>
#include "lib/face/face.h"
#include "lib/qcustomplot.h"
#include "gyrotron.h"

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
    void on_status_group_clicked();
    void on_close_button_clicked();
    void on_minimize_button_clicked();
    void on_control_tab_clicked();
    void on_plot_tab_clicked();
    void on_admin_tab_clicked();
    void on_fault_tab_clicked();

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
    void detect_state_change(int current_state, bool e_ramping, bool manual_update = false);
    void update_plots();

    Ui::MainWindow *ui;
    Face gui{this};
    Gyrotron gyro;
    std::vector<QGraphicsDropShadowEffect*> shadows;
    QTimer data_timer;
    QPoint oldPos;

    std::vector<SmartLineEdit*> smart_edits;
    int last_known_state{0}, resize_tracker{0};

    std::vector<QListWidgetItem*> fault_list_items;

    QIcon warning_icon{"images/warning_yellow.png"};
    QIcon error_icon{"images/error_red.png"};

    QString tab_selected{"QPushButton{color: rgb(85,87,83); border: none; background: rgb(240,240,240);}"};
    QString tab_unselected{"QPushButton { color: rgb(85,87,83); border: none; background: rgb(220,220,220); }"
                           "QPushButton:hover { background: rgb(225,225,225); }"
                           "QPushButton:hover:pressed { background: rgb(240,240,240); }"};

    QString green_state_bubble{"QFrame { background-color: #46812B; color: white; border: none; border-radius: 100px; }"};
    QString orange_state_bubble{"QFrame { background-color: #F37021; color: white; border: none; border-radius: 100px; }"};
    QString red_state_bubble{"QFrame { background-color: #D16055; color: white; border: none; border-radius: 100px; }"};

    QString green_status_bubble{"QFrame { background: #46812B; color: white; border: none; border-radius: 37px; }"
                                "QFrame:disabled { background: rgb(180,180,180); }"};
    QString yellow_status_bubble{"QFrame { background: #E89831; color: white; border: none; border-radius: 37px; }"
                                "QFrame:disabled { background: rgb(180,180,180); }"};
    QString red_status_bubble{"QFrame { background: #D16055; color: white; border: none; border-radius: 37px; }"
                                "QFrame:disabled { background: rgb(180,180,180); }"};

    QString empty_faults_list{"background: white; border: none; background-image: url(:/images/no_faults.png);"
                              "background-position: center; background-repeat: no-repeat;"};
    QString faults_list{"background: white; border: none; background-image: none;"};

    double refresh_rate{0.61}, log_rate{0.61}, plot_span{1200}, e_ramp_rate{0.5};
    double press_bound1{8e-8}, press_bound2{4e-7}, press_bound3{1e-6}, fatal_press{5e-6};
    double err_limit1{10}, err_limit2{30};
    bool paused{false};
    int log_day_tracker, dat_day_tracker, init_error_code{0}, state{0};
    int dxm_err_count{0}, rigol_err_count{0}, lj_err_count{0}, spc_err_count{0}, fms_err_count{0};
    int status, expected_devs{0}, num_data_logs{0}, num_action_logs{0}, lj_recon_count{-1};
    double pressure{0}, freq, freq_sp{0}, beam_volt_sp{0}, anode_volt_sp{0}, anode_curr_sp{0};
    double key, ain0, ain1, ain2, beam2fil_scale, dxm_curr_limit, ramp_rate{0}, ramp_sp{0}, ramp_time{0}, press_max, press_min;
    double fil_curr, fil_curr_sp{0}, dxm_curr, dxm_volt, beam_curr_sp{-1}, diode_volt, gtc_volt_sp{0}, gtc_curr_sp{0}, trim_curr_sp{0}, trim_volt_sp{0};
    double power, power_set, power_sp{-1}, power_limit, power_calibrate, e_ramp_time;
    bool e_ramping{false}, cath_hv_blocked{false}, reset_pid_time{false};
    bool running{false}, cath_hv_on{false}, gtc_hv_on{false}, beam_curr_pid_on{false}, power_pid_on{false};
    bool freq_pid_on{false}, ramping_up{false}, ramping_down{false}, cath_fault_triggered{false};
    bool relaxing{false};
    std::string recv_str, out_str, log_file_name, dat_file_name, data_log_file_name, action_log_file_name, folder_name, press_str{"----------"};
    QString temp_str, white_box, grey_box, red_box;
    char timestamp_full[20], timestamp[10];

    QString fil_curr_st, beam_volt_st, beam_curr_st, anode_volt_st, anode_curr_st, shim_curr_st, shim_volt_st, trim_curr_st, trim_volt_st, freq_st, power_st;

    QCPItemText *press_plot_label, *beam_plot_label, *power_plot_label, *power_plot_label2;

    //std::vector<double> press_data, time_data;
    QVector<double> press_data, time_data, beam_data, power_sp_data, power_time_data, beam_sp_data, beam_time_data, power_data;
    std::vector<double> power_array;

    bool fil_curr_active{false}, beam_volt_active{false}, beam_curr_active{false}, anode_curr_active{false}, anode_volt_active{false}, ramp_paused{false};
    bool power_active{false}, freq_active{false}, shim_curr_active{false}, shim_volt_active{false}, trim_curr_active{false}, trim_volt_active{false};


protected:
    void mousePressEvent(QMouseEvent *evt) { oldPos = evt->globalPos(); }
    void mouseMoveEvent(QMouseEvent *evt)
    { const QPoint delta = evt->globalPos() - oldPos; move(x()+delta.x(), y()+delta.y()); oldPos = evt->globalPos(); }
};

#endif // MAINWINDOW_H
