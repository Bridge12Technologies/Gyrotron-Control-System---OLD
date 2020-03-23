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
    void plot1_context_menu(const QPoint &pos);
    void plot2_context_menu(const QPoint &pos);
    void plot3_context_menu(const QPoint &pos);
    void on_settings_button_clicked();
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
    void toggle_lg_display(bool show_large);
    void update_state_diagram();

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

    std::function<QString(QString,int,int,int)> indicator_style = [](QString color, int height, int width, int font_size) {
        QString style = "QLabel{ background: " + color + "; color: white; border: none; border-radius: " + QString::number(int(height/2)) + "px; font-size: ";
        style += QString::number(font_size) + "pt; max-height: " + QString::number(height) + "px; min-height: " + QString::number(height);
        style += "px; min-width: " + QString::number(width) + "px; max-width: " + QString::number(width) + "px;}";
        style += "QLabel:disabled{ color: #bdbdbd; background: rgb(220,220,220); }";
        return style;};

    QString med_green_indicator{indicator_style("#2FA84F",60,150,16)};
    QString med_yellow_indicator{indicator_style("#FFC107",60,150,16)};
    QString med_red_indicator{indicator_style("#F44336",60,150,16)};
    QString lg_green_indicator{indicator_style("#2FA84F",78,195,18)};
    QString lg_yellow_indicator{indicator_style("#FFC107",78,195,18)};
    QString lg_red_indicator{indicator_style("#F44336",78,195,18)};

    QString tab_selected{"QPushButton { color: rgb(85,87,83); border: none; background: #e6e6e6; }"};
    QString tab_unselected{"QPushButton { color: rgb(85,87,83); border: none; background: rgb(210,210,210); }"
                           "QPushButton:hover { background: rgb(215,215,215); }"
                           "QPushButton:hover:pressed { background: rgb(220,220,220); }"};
    QString tab_blink_on{"QPushButton { color: #D16055; text-decoration: underline; border: none; background: #dbc8c8; }"
                         "QPushButton:hover { background: rgb(225,225,225); }"
                         "QPushButton:hover:pressed { background: rgb(240,240,240); }"};
    QString tab_blink_off{"QPushButton { color: #D16055; text-decoration: underline; border: none; background: rgb(220,220,220); }"
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

    std::function<QString(QString,int,int)> state_style = [](QString color, int height, int width) {
        QString style = "#state_frame { background-color: " + color + "; color: white; border: none; border-radius: ";
        style += QString::number(int(height/2)) + "px; max-height: " + QString::number(height) + "px; min-height: " + QString::number(height);
        style += "px; max-width: " + QString::number(width) + "px; min-width: " + QString::number(width) + "px;}";
        return style;};

    QString lg_blue_state{state_style("#448AFF",182,455)};
    QString lg_orange_state{state_style("#F57C00",182,455)};
    QString lg_purple_state{state_style("#9C27B0",182,455)};
    QString lg_green_state{state_style("#2FA84F",182,455)};
    QString lg_red_state{state_style("#F44336",182,455)};
    QString med_blue_state{state_style("#448AFF",130,325)};
    QString med_orange_state{state_style("#F57C00",130,325)};
    QString med_purple_state{state_style("#9C27B0",130,325)};
    QString med_green_state{state_style("#2FA84F",130,325)};
    QString med_red_state{state_style("#F44336",130,325)};

    std::function<QString(QString,int)> arrow_style = [](QString direction, int size) {
      QString style = "QPushButton { border: none; background: none; border-image: url(:/images/" + direction + ".png) 0 0 0 0 stretch stretch; ";
      style += "max-height: " + QString::number(size) + "px; min-height: " + QString::number(size) +
               "px; min-width: " + QString::number(size) + "px; max_width: " + QString::number(size) + "px;}" +
               "QPushButton:hover { border-image: url(:/images/" + direction + "_hover.png) 0 0 0 0 stretch stretch; }"
               "QPushButton:hover:pressed { border-image: url(:/images/" + direction + "_press.png) 0 0 0 0 stretch stretch; }"
               "QPushButton:disabled { border-image: url(:/images/" + direction + "_disabled.png) 0 0 0 0 stretch stretch; }";
    return style;};

    QString med_prev_arrow{arrow_style("back",100)};
    QString lg_prev_arrow{arrow_style("back",140)};
    QString med_next_arrow{arrow_style("forward",100)};
    QString lg_next_arrow{arrow_style("forward",140)};

    QString lg_pause_bubble{"QPushButton { max-width: 150px; min-width: 150px; max-height: 150px; min-height: 150px;"
                            "border: none; background: #555753; border-radius: 75px; margin: 10px; color: white; }"
                            "QPushButton:hover { background: #424441; } QPushButton:hover:pressed { background: #30312F; }"};
    QString med_pause_bubble{"QPushButton { max-width: 100px; min-width: 100px; max-height: 100px; min-height: 100px;"
                            "border: none; background: #555753; border-radius: 50px; margin: 10px; color: white; }"
                            "QPushButton:hover { background: #424441; } QPushButton:hover:pressed { background: #30312F; }"};

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
