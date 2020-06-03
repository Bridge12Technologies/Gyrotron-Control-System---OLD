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

/**  */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /** This is the constantly recurring function that acts as the main loop for GUI updates */
    void realtime_slot();
    /**  */
    void on_reconfig_button_clicked();
    /** When the beam voltage submit button is clicked then check against validator and apply the value */
    void on_beam_volt_button_clicked();
    /** When the beam current submit button is clicked then check against validator and apply the value */
    void on_beam_curr_button_clicked();
    /** When the beam power submit button is clicked then check against validator and apply the value */
    void on_power_button_clicked();
    /** When the frequency submit button is clicked then check against validator and apply the value */
    void on_freq_button_clicked();
    /** When GTC current submit button is clicked then check against validator and apply the value */
    void on_gtc_curr_button_clicked();
    /** When filament current submit button is clicked then check against validator and apply the value */
    void on_fil_curr_button_clicked();
    /** When previous state arrow button is clicked then decrement current state */
    void on_prev_state_button_clicked();
    /** When next state arrow button is clicked then increment current state */
    void on_next_state_button_clicked();
    /** When beam current proportional control constant submit button is clicked then check against validator and apply the value */
    void on_beam_kp_button_clicked();
    /** When beam current integral control constant submit button is clicked then check against validator and apply the value */
    void on_beam_ki_button_clicked();
    /** When beam current derivative control constant submit button is clicked then check against validator and apply the value */
    void on_beam_kd_button_clicked();
    /** When power proportional control constant submit button is clicked then check against validator and apply the value */
    void on_power_kp_button_clicked();
    /** When power integral control constant submit button is clicked then check against validator and apply the value */
    void on_power_ki_button_clicked();
    /** When power derivative control constant submit button is clicked then check against validator and apply the value */
    void on_power_kd_button_clicked();
    /** When frequency proportional constrol constant submit button is clicked then check against validator and apply the value */
    void on_freq_kp_button_clicked();
    /** When frequency integral control constant submit button is clicked then check against validator and apply the value */
    void on_freq_ki_button_clicked();
    /** When frequency derivative control constant submit button is clicked then check against validator and apply the value */
    void on_freq_kd_button_clicked();
    /** When close button is clicked then prompt to confirm choice and if yes then exit the program */
    void on_close_button_clicked();
    /** Minimize window on click */
    void on_minimize_button_clicked();
    /** Maximize window on click or return to previous state if already maximized */
    void on_maximize_button_clicked();
    /** Switch to control tab on click */
    void on_control_tab_clicked();
    /** Switch to plot tab on click */
    void on_plot_tab_clicked();
    /** Switch to admin tab on click */
    void on_admin_tab_clicked();
    /** Switch to status tab on click */
    void on_status_tab_clicked();
    /** Update plot time span when slider is moved */
    void on_time_span_slider_valueChanged(int value);
    /** Update filament ramp rate when slider moved */
    void on_ramp_rate_slider_valueChanged(int value);
    /** Update log rate when slider moved */
    void on_log_rate_slider_valueChanged(int value);
    /** Manually trigger an attempt to reconnect to cathode supply and change button text to reflect success/failure */
    void on_cath_recon_button_clicked();
    /** Manually trigger an attempt to reconnect to GTC supply and change button text to reflect success/failure */
    void on_gtc_recon_button_clicked();
    /** Manually trigger an attempt to reconnect to ion pump controller and change button text to reflect success/failure */
    void on_spc_recon_button_clicked();
    /** Manually trigger an attempt to reconnect to the FMS and change button text to reflect success/failure */
    void on_fms_recon_button_clicked();
    /** Manually trigger an attempt to reconnect to the RSI and change button text to reflect success/failure */
    void on_rsi_recon_button_clicked();
    /** Manually trigger an attempt to reconnect to all peripheral devices and change button text to reflect success/failure */
    void on_all_recon_button_clicked();
    /** Reset the text and stylesheet on the reconnect button for the cathode supply */
    void reset_cath_recon();
    /** Reset the text and stylesheet on the reconnect button for the gun trim coil supply */
    void reset_gtc_recon();
    /** Reset the text and stylesheet on the reconnect button for the ion pump controller */
    void reset_spc_recon();
    /** Reset the text and stylesheet on the reconnect button for the RSI */
    void reset_rsi_recon();
    /** Reset the text and stylesheet on the reconnect button for the FMS */
    void reset_fms_recon();
    /** Reset the text and stylesheet on the reconnect button for all peripheral devices  */
    void reset_all_recon();
    /** When enter key pressed inside the console command entry field then format that command and send to specified device */
    void on_console_edit_returnPressed();
    /** Take all current PID constants and overwrite them to the config file */
    void on_save_pid_button_clicked();
    /** Check current tab and blink_enabled status to stop/start blinking effect as needed */
    void blink_status();
    /** Right clicking plot 1 will show context menu with option to clear the visible data */
    void plot1_context_menu(const QPoint &pos);
    /** Right clicking plot 2 will show context menu with option to clear the visible data */
    void plot2_context_menu(const QPoint &pos);
    /** Right clicking plot 3 will show context menu with option to clear the visible data */
    void plot3_context_menu(const QPoint &pos);
    /** Open the settings window */
    void on_settings_button_clicked();
    /** Enable/disable PID controller chosen in dropdown to the left */
    void on_enable_button_clicked();
    /** If this dropdown selection changes then change what gets plotted to plot 1 */
    void on_plot1_dropdown_currentIndexChanged(const QString &arg1);
    /** If this dropdown selection changes then change what gets plotted to plot 2 */
    void on_plot2_dropdown_currentIndexChanged(const QString &arg1);
    /** If this dropdown selection changes then change what gets plotted to plot 3 */
    void on_plot3_dropdown_currentIndexChanged(const QString &arg1);
    /** Clear visible data on plot 1 */
    void clear_plot1_data();
    /** Clear visible data on plot 2 */
    void clear_plot2_data();
    /** Clear visible data on plot 3 */
    void clear_plot3_data();

private:
    /** Stop realtime (GUI) loop, and call stop() to disconnect devices, close logs, and join all threads */
    void shutdown();
    /** Detect window close and prompt confirmation before exiting program */
    void closeEvent (QCloseEvent *event);
    /** Prompt user with an error window according to the provided initialization error code */
    void init_fail_dialog(int err_code);
    /** Init plots, apply drop shadows, start data timer, grey out disabled devices */
    void init_gui();
    /** Init entry fields, attach to buttons and validators */
    void init_fields();
    /** Gather all faults and post to faults list in status tab */
    void check_faults();
    /** Initialize and stylize all three plots */
    void init_plots();
    /** Set button stylesheet to green button */
    void set_button_green(QPushButton *button, bool turning_green, QString new_text);
    /** Validator that takes input value as QString and checks against double boundaries */
    bool valid_check(QString qstr, double max_val, double min_val = 0);
    /** Apply stylesheet changes to state widget based on current state */
    void update_state_widget(bool manual_update = false);
    /** Update plot data and adjust scaling as needed */
    void update_plots();
    /** Check device connection and PID status then enable/disable groups/buttons accordingly */
    void check_connections();
    /** Output QString to device console in the admin page */
    void console_print(QString qstr);
    /** Update labels with their respective monitor values */
    void update_labels();
    /** Update indicator stylesheets according to their assigned bools */
    void update_indicators();
    /** Update faults list */
    void update_faults();
    /** Update special setpoint label that displays automatically updated setpoint during a ramp operation */
    void update_pid_display();
    /** Turn blink on or off accoring to provided bool */
    void set_blink_on(bool enable);
    /** Enable/disable status tab blink for new faults */
    void set_blink_enabled(bool enable);
    /** Check if vectors of QListWidgetItems used to display faults are equivilant */
    bool lists_equal(std::vector<QListWidgetItem*> list1, std::vector<QListWidgetItem*> list2);
    /** Toggle large display mode for maximized interface */
    void toggle_lg_display(bool show_large);

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
    QIcon pointer{":/images/pointer.png"};
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

    QString lg_blue_state{state_style("#448AFF",195,488)};
    QString lg_orange_state{state_style("#F57C00",195,488)};
    QString lg_purple_state{state_style("#9C27B0",195,488)};
    QString lg_green_state{state_style("#2FA84F",195,488)};
    QString lg_red_state{state_style("#F44336",195,488)};
    QString med_blue_state{state_style("#448AFF",130,325)};
    QString med_orange_state{state_style("#F57C00",130,325)};
    QString med_purple_state{state_style("#9C27B0",130,325)};
    QString med_green_state{state_style("#2FA84F",130,325)};
    QString med_red_state{state_style("#F44336",130,325)};

    std::function<QString(QString,int)> arrow_style = [](QString direction, int size) {
      QString style = "QPushButton { border: none; background: none; border-image: url(:/images/" + direction + ".png) 0 0 0 0 stretch stretch; ";
      style += "max-height: " + QString::number(size) + "px; min-height: " + QString::number(size) +
               "px; min-width: " + QString::number(size) + "px; max-width: " + QString::number(size) + "px;}" +
               "QPushButton:hover { border-image: url(:/images/" + direction + "_hover.png) 0 0 0 0 stretch stretch; }"
               "QPushButton:hover:pressed { border-image: url(:/images/" + direction + "_press.png) 0 0 0 0 stretch stretch; }"
               "QPushButton:disabled { border-image: url(:/images/" + direction + "_disabled.png) 0 0 0 0 stretch stretch; }";
    return style;};

    QString med_prev_arrow{arrow_style("back",100)};
    QString lg_prev_arrow{arrow_style("back",150)};
    QString med_next_arrow{arrow_style("forward",100)};
    QString lg_next_arrow{arrow_style("forward",150)};

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
