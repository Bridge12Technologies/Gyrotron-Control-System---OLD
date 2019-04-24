#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <string>
#include "device.h"
#include "labjack.h"
#include "pid.h"
#include <fstream>
#include "qcustomplot.h"
#include <QApplication>
#include "write.h"
#include <algorithm>
#include <QString>
#include <iostream> // for cout
#include <fstream> // for string streams (used to parse text file)
#include <sstream> // for parsing comma-delimited output from DXM
#include <unistd.h>
#include <math.h>
#include <iomanip> // used to set precision of a number in string stream
#include <sys/stat.h> // used to query file size

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
    void on_dxm_hv_button_clicked();
    void on_dxm_reset_button_clicked();
    void on_slm_hv_button_clicked();
    void on_slm_reset_button_clicked();
    void on_rigol1_hv_button_clicked();
    void on_rigol2_hv_button_clicked();
    void on_ramp_up_button_clicked();
    void on_ramp_down_button_clicked();
    void on_tabWidget_tabBarClicked(int index);
    void on_beam_curr_pid_button_clicked();
    void on_power_pid_button_clicked();
    void rigol1_set_curr_true();
    void rigol1_set_volt_true();
    void rigol2_set_curr_true();
    void rigol2_set_volt_true();
    void on_beam_kp_write_returnPressed();
    void on_beam_ki_write_returnPressed();
    void on_beam_kd_write_returnPressed();
    void on_ramp_sp_write_returnPressed();
    void on_ramp_time_write_returnPressed();
    void on_freq_pid_button_clicked();
    void on_fkp_write_returnPressed();
    void on_fki_write_returnPressed();
    void on_fkd_write_returnPressed();
    void on_clear_status_button_clicked();
    void dxm_recon();
    void slm_recon();
    void rigol_recon();
    void fms_recon();
    void spc_recon();
    void ljReconnect();
    void on_dxm_recon_button_clicked();
    void on_slm_recon_button_clicked();
    void on_rigol_recon_button_clicked();
    void on_spc_recon_button_clicked();
    void on_lj_recon_button_clicked();
    void on_reconfig_button_clicked();
    void on_log_rate_write_returnPressed();
    void on_beam_volt_button_clicked();
    void on_beam_curr_button_clicked();
    void on_anode_curr_button_clicked();
    void on_anode_volt_button_clicked();
    void on_power_button_clicked();
    void on_freq_button_clicked();
    void on_fms_recon_button_clicked();
    void on_shim_curr_button_clicked();
    void on_shim_volt_button_clicked();
    void on_trim_curr_button_clicked();
    void on_trim_volt_button_clicked();
    void on_fil_curr_button_clicked();

private:
    Ui::MainWindow *ui;
    QTimer data_timer;

    QFont main_font{"Open Sans", 12};
    QFont plot_axis_font{"Open Sans", 11};
    QFont plot_label_font{"Open Sans", 11, QFont::Bold};
    QFont plot_big_label_font{"Open Sans", 12, QFont::Bold};
    QFont small_button_font{"Open Sans", 11};
    QFont button_font{"Open Sans", 17};

    labjack daq;

    PID *pid, *fpid;

    time_t t;
    struct tm * now;

    int log_day_tracker, dat_day_tracker, init_error_code{0};
    int dxm_err_count{0}, slm_err_count{0}, rigol_err_count{0}, lj_err_count{0}, spc_err_count{0}, fms_err_count{0};
    int status, expected_devs{0}, num_data_logs{0}, num_action_logs{0}, lj_recon_count{-1};
    double refresh_rate, log_rate, pressure{0}, plot_span, freq, freq_sp{0}, beam_volt_sp{0}, anode_volt_sp{0}, anode_curr_sp{0};
    double key, ain0, ain1, ain2, beam2fil_scale, dxm_curr_limit, ramp_rate{0}, ramp_sp{0}, ramp_time{0}, press_max, press_min;
    double fil_curr, fil_curr_sp{0}, dxm_curr, dxm_volt, beam_curr_sp{-1}, diode_volt, shim_volt_sp{0}, shim_curr_sp{0}, trim_curr_sp{0}, trim_volt_sp{0};
    double slm_volt, slm_curr, kp{0}, ki{0}, kd{0}, power, power_set, power_sp{-1}, power_limit, power_calibrate;
    double fkp{0}, fki{0}, fkd{0}, dxm_volt_ref, e_ramp_time, e_ramp_rate, fatal_press, press_bound1, press_bound2, press_bound3, err_limit1, err_limit2;
    bool e_ramping{false}, dxm_hv_blocked{false}, dxm_hv_title_on{false}, slm_hv_title_on{false}, reset_pid_time{false};
    bool running{false}, dxm_hv_on{false}, slm_hv_on{false}, rigol1_hv_on{false}, rigol2_hv_on{false}, beam_curr_pid_on{false}, power_pid_on{false};
    bool freq_pid_on{false}, ramping_up{false}, ramping_down{false}, dxm_fault_triggered{false}, slm_fault_triggered{false};
    bool dxm_enabled{true}, slm_enabled{true}, rigol1_enabled{true}, rigol2_enabled{true}, labjack_enabled{true}, spc_enabled{true}, relaxing{false}, starting_prog{true};
    bool logging{false}, toggle_rigol, fms_enabled, rigol1_enabled_init, rigol2_enabled_init;
    bool dxm_arc{false}, dxm_over_temp{false}, dxm_over_volt{false}, dxm_under_volt{false}, dxm_over_curr{false}, dxm_under_curr{false};
    bool slm_arc{false}, slm_over_temp{false}, slm_over_volt{false}, slm_under_volt{false}, slm_over_curr{false}, slm_under_curr{false};
    std::string recv_str, out_str, log_file_name, dat_file_name, data_log_file_name, action_log_file_name, folder_name, press_str{"----------"};
    QString temp_str, white_box, grey_box, red_box;
    char timestamp_full[20], timestamp[10];

    QString fil_curr_st, beam_volt_st, beam_curr_st, anode_volt_st, anode_curr_st, shim_curr_st, shim_volt_st, trim_curr_st, trim_volt_st, freq_st, power_st;

    std::ifstream in_stream;
    std::fstream data_log_stream, action_log_stream;

    QCPItemText *press_plot_label, *beam_plot_label, *power_plot_label, *power_plot_label2;

    //std::vector<double> press_data, time_data;
    QVector<double> press_data, time_data, beam_data, power_sp_data, power_time_data, beam_sp_data, beam_time_data, power_data;
    std::vector<double> power_array;

    bool fil_curr_active{false}, beam_volt_active{false}, beam_curr_active{false}, anode_curr_active{false}, anode_volt_active{false};
    bool power_active{false}, freq_active{false}, shim_curr_active{false}, shim_volt_active{false}, trim_curr_active{false}, trim_volt_active{false};

    QString green_press_frame{"background: #46812B; color: white; border: none; border-radius: 75px; padding-top: 3px;"};
    QString yellow_press_frame{"background: #EDD32F; color: white; border: none; border-radius: 75px; padding-top: 3px;"};
    QString red_press_frame{"background: rgb(209,96,85); color: white; border: none; border-radius: 75px; padding-top: 3px;"};

    QString green_edit_button{"QPushButton:hover:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #2F5B12, stop: 1 #2F5B12);}"
                              "QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B); border: none;"
                              "border-radius: 18px; color: rgb(30,30,30); color: white; height: 36px; width: 36px;}"
                              "QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);}"
                              "QPushButton:disabled {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));}"};
    QString orange_edit_button{"QPushButton:hover:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #D15F1B, stop: 1 #D15F1B);}"
                               "QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F37021, stop: 1 #F37021); border: none;"
                               "border-radius: 18px; color: rgb(30,30,30); padding-top:-3px; height: 36px; width: 36px;}"
                               "QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E8691E, stop: 1 #E8691E);}"
                               "QPushButton:disabled {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));}"};
    QString orange_pid_button{"QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F37021, stop: 1 #F37021); border: none;"
                              "border-radius: 18px; color: rgb(30,30,30); padding-top:-3px; height: 36px; width: 100px; }"};

    QString clear_edit{"height: 34px; border-radius: 17px; border: none; background: none;"};
    QString active_edit{"height: 34px; border-radius: 17px; border: 1px solid rgb(220, 220, 220); background: rgb(240,240,240);"};
    QString error_edit{"height: 34px; border-radius: 17px; border: 1px solid rgb(186, 84, 74); background: rgb(209, 96, 85);"};

    QString green_ramp_button{"QPushButton:hover:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));}"
                              "QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B); border: none; color: white; border-radius: 23px;}"
                              "QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);}"
                              "QPushButton:disabled {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));}"};
    QString orange_ramp_button{"QPushButton:hover:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #D15F1B, stop: 1 #D15F1B);}"
                               "QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F37021, stop: 1 #F37021); border: none; color: rgb(30,30,30); border-radius: 23px;}"
                               "QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E8691E, stop: 1 #E8691E);}"
                               "QPushButton:disabled {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));}"};

    QString green_auto_button{"QPushButton:hover:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));}"
                              "QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B); border: none; color: white; border-radius: 23px;}"
                              "QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);}"
                              "QPushButton:disabled {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));}"};
    QString orange_auto_button{"QPushButton:hover:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #D15F1B, stop: 1 #D15F1B);}"
                               "QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F37021, stop: 1 #F37021); border: none; color: rgb(30,30,30); border-radius: 23px;}"
                               "QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E8691E, stop: 1 #E8691E);}"
                               "QPushButton:disabled {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));}"};

    QString green_hv_button{"QPushButton:hover:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(47, 91, 18), stop: 1 rgb(47, 91, 18));}"
                            "QPushButton { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #46812B, stop: 1 #46812B); border: none; color: white; border-radius: 50px; padding-top: 3px;}"
                            "QPushButton:hover { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #407527, stop: 1 #407527);}"
                            "QPushButton:disabled { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));}"};
    QString orange_hv_button{"QPushButton:hover:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #D15F1B, stop: 1 #D15F1B);}"
                             "QPushButton { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F37021, stop: 1 #F37021); border: none; color: rgb(30,30,30); border-radius: 50px; padding-top: 3px;}"
                             "QPushButton:hover { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E8691E, stop: 1 #E8691E);}"
                             "QPushButton:disabled { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(181, 181, 181), stop: 1 rgb(181, 181, 181));}"};

    void shutdown();
    void closeEvent (QCloseEvent *event);
    void init_fail_dialog(int error_code);
    void initialize();
    void probe();
    int ethprobe(ethdevice *dev, std::string test_cmd, std::string expected);
    int serprobe(serdevice *dev, std::string test_cmd, std::string expected);
    void parse_config();
    void check_faults();
    void init_plot();
    void new_data_log();
    void new_action_log();
    void data_log_write(std::string dlog_input, bool prepend_time = true);
    void action_log_write(std::string alog_input, bool prepend_time = true);
    long get_file_size(std::string filename);
    void set_button_green(QPushButton *button, bool turning_green, QString new_text);
    void soft_kill();
    void ethReconnect(ethdevice *dev);
    void serReconnect(serdevice *dev);
    void rigol1_set_curr(bool toggle_back);
    void rigol1_set_volt(bool toggle_back);
    void rigol2_set_curr(bool toggle_back);
    void rigol2_set_volt(bool toggle_back);
};

#endif // MAINWINDOW_H
