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
    void on_close_button_clicked();
    void on_minimize_button_clicked();
    void on_control_tab_clicked();
    void on_plot_tab_clicked();
    void on_admin_tab_clicked();
    void on_status_tab_clicked();
    void on_maximize_button_clicked();

    void on_time_span_slider_valueChanged(int value);

    void on_ramp_rate_slider_valueChanged(int value);

    void on_log_rate_slider_valueChanged(int value);

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
    void check_connections(); // check device connection and PID status then enable/disable groups/buttons accordingly

    Ui::MainWindow *ui;
    Face gui{this};
    Gyrotron gyro;
    std::vector<QGraphicsDropShadowEffect*> shadows;
    QTimer data_timer;
    QPoint oldPos;
    bool gui_debug_mode;
    std::vector<SmartLineEdit*> smart_edits;
    int last_known_state{0}, resize_tracker{0};
    bool window_locked{false};

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

    int init_error_code{0};
    double key, plot_span{0}, refresh_rate{0.5};
    std::string press_str{"----------"};
    char timestamp_full[20], timestamp[10];
    double time_span{900}; // default 15 mins (in sync with gyrotron default)

    QCPItemText *press_plot_label, *beam_plot_label, *power_plot_label, *power_plot_label2;

    QVector<double> press_data, time_data, beam_data, power_sp_data, power_time_data, beam_sp_data, beam_time_data, power_data;
    std::vector<double> power_array;

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
};

#endif // MAINWINDOW_H
