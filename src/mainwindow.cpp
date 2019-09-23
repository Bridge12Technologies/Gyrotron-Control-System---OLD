#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    gui_debug_mode = true;

    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    if(gui_debug_mode)
        init_gui();
    else {
        int init_stat = gyro.init(); // parse config, spawn threads, connect/probe devices, and exec pre-funcs
        (init_stat < 0) ? init_fail_dialog(init_stat) : init_gui(); // either init gui next, or prompt error
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::shutdown()
{
    gyro.log_event("exiting program");
    data_timer.stop();
    if(!gui_debug_mode) gyro.stop();
    QApplication::quit();
}

void MainWindow::init_gui()
{
    QWidget *boxes[25] = {ui->status_group,ui->state_group,ui->press_group,ui->cathode_group,ui->fms_group,ui->power_group,ui->gtc_group,
                         ui->pid_group,ui->plot_group,ui->fil_curr_group,ui->beam_volt_group,ui->freq_group,ui->curr_state_group,ui->auto_state_group,
                         ui->time_span_group,ui->faults_group,ui->log_group,ui->beam_pid_group,ui->power_pid_group,ui->freq_group,ui->ramp_rate_group,
                         ui->log_rate_group,ui->misc_group,ui->reconnect_group,ui->console_group};
    for(auto box : boxes) { box->installEventFilter(this); }

    // apply drop shadows to group boxes
    std::vector<QGroupBox*> groups{ui->state_group,ui->status_group,ui->press_group,ui->cathode_group,ui->fms_group,ui->power_group,ui->pid_group,ui->gtc_group,
                                   ui->plot_group,ui->fil_curr_group,ui->beam_volt_group,ui->freq_group,ui->curr_state_group,ui->auto_state_group,ui->time_span_group,
                                   ui->beam_pid_group,ui->power_pid_group,ui->freq_pid_group,ui->ramp_rate_group,ui->log_rate_group,ui->misc_group,ui->reconnect_group,ui->console_group,
                                   ui->faults_group,ui->log_group};

    for(QGroupBox* group : groups)
    {
        shadows.push_back(new QGraphicsDropShadowEffect);
        shadows.back()->setBlurRadius(25);
        shadows.back()->setXOffset(0);
        shadows.back()->setYOffset(0);
        shadows.back()->setColor(QColor(30,30,30,35));
        group->setGraphicsEffect(shadows.back());
    }

    // disabled UI components if devices are disabled
    if(!gyro.cath_is_enabled())
    {
        ui->cathode_group->setEnabled(false);
        ui->pid_group->setEnabled(false);
    }
    if(!gyro.gtc_is_enabled()) { ui->gtc_group->setEnabled(false); }
    if(!gyro.rsi_is_enabled())
    {
        ui->power_group->setEnabled(false);
        ui->power_pid_button->setEnabled(false);
        ui->temp_indicator->setEnabled(false);
        ui->flow_indicator->setEnabled(false);
    }
    if(!gyro.spc_is_enabled()) { ui->press_group->setEnabled(false); }
    if(!gyro.fms_is_enabled()) { ui->fms_group->setEnabled(false); ui->freq_pid_button->setEnabled(false); }

    init_plots();

    // connect realtime slot to timer and start it
    connect(&data_timer, SIGNAL(timeout()), this, SLOT(realtime_slot())); // enable the realtime slot
    data_timer.start(0);
}

void MainWindow::init_plots()
{
    // align all left/right plot margins
    QCPMarginGroup *group = new QCPMarginGroup(ui->power_plot); // previously used the press_plot
    ui->press_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ui->beam_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ui->power_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);

    ui->press_plot->setBackground(QBrush(Qt::white));
    ui->press_plot->axisRect()->setBackground(Qt::white);
    //ui->press_plot->xAxis->setVisible(false);
    ui->press_plot->xAxis->setBasePen(Qt::NoPen);
    ui->press_plot->xAxis->setTickLabels(false);
    ui->press_plot->xAxis->setTickPen(Qt::NoPen);
    ui->press_plot->xAxis->setSubTickPen(Qt::NoPen);
    ui->press_plot->yAxis->setTickLabelFont(plot_axis_font);
    ui->press_plot->yAxis->setLabelFont(small_plot_label_font);
    ui->press_plot->yAxis->setLabelColor(QColor(85, 87, 83));
    ui->press_plot->yAxis->setBasePen(Qt::NoPen);
    ui->press_plot->yAxis->setTickPen(Qt::NoPen);
    ui->press_plot->yAxis->setSubTickPen(Qt::NoPen);
    ui->press_plot->yAxis->setTickLabelColor(QColor(85, 87, 83));
    ui->press_plot->yAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));
    ui->press_plot->yAxis->grid()->setSubGridPen(QPen(QColor(200,200,200),1,Qt::DotLine));
    ui->press_plot->xAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));

    ui->beam_plot->setBackground(QBrush(Qt::white));
    ui->beam_plot->axisRect()->setBackground(Qt::white);
    //ui->beam_plot->xAxis->setVisible(false);
    ui->beam_plot->xAxis->setBasePen(Qt::NoPen);
    ui->beam_plot->xAxis->setTickLabels(false);
    ui->beam_plot->xAxis->setTickPen(Qt::NoPen);
    ui->beam_plot->xAxis->setSubTickPen(Qt::NoPen);
    ui->beam_plot->yAxis->setTickLabelFont(plot_axis_font);
    ui->beam_plot->yAxis->setLabelFont(small_plot_label_font);
    ui->beam_plot->yAxis->setLabelColor(QColor(85, 87, 83));
    ui->beam_plot->yAxis->setBasePen(Qt::NoPen);
    ui->beam_plot->yAxis->setTickPen(Qt::NoPen);
    ui->beam_plot->yAxis->setSubTickPen(Qt::NoPen);
    ui->beam_plot->yAxis->setTickLabelColor(QColor(85, 87, 83));
    ui->beam_plot->yAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));
    ui->beam_plot->xAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));

    ui->power_plot->setBackground(QBrush(Qt::white));
    ui->power_plot->axisRect()->setBackground(Qt::white);
    ui->power_plot->xAxis->setTickLabelFont(plot_axis_font);
    ui->power_plot->xAxis->setLabelFont(small_plot_label_font);
    ui->power_plot->xAxis->setLabelColor(QColor(85, 87, 83));
    ui->power_plot->xAxis->setBasePen(Qt::NoPen);
    ui->power_plot->xAxis->setTickLabelColor(QColor(85, 87, 83));
    ui->power_plot->xAxis->setTickPen(Qt::NoPen);
    ui->power_plot->xAxis->setSubTickPen(Qt::NoPen);
    ui->power_plot->yAxis->setTickLabelFont(plot_axis_font);
    ui->power_plot->yAxis->setLabelFont(small_plot_label_font);
    ui->power_plot->yAxis->setLabelColor(QColor(85, 87, 83));
    ui->power_plot->yAxis->setBasePen(Qt::NoPen);
    ui->power_plot->yAxis->setTickPen(Qt::NoPen);
    ui->power_plot->yAxis->setSubTickPen(Qt::NoPen);
    ui->power_plot->yAxis->setTickLabelColor(QColor(85, 87, 83));
    ui->power_plot->yAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));
    ui->power_plot->xAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));

    // PRESSURE PLOT

    // setup pressure plot
    ui->press_plot->addGraph(); // light green line
    ui->press_plot->graph(0)->setPen(QPen(QColor(70, 129, 43)));
    ui->press_plot->yAxis->setLabel("Pressure (Torr)");
    ui->press_plot->yAxis->setNumberPrecision(10);

    // set axes
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->press_plot->xAxis->setTicker(timeTicker);
    ui->press_plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    ui->press_plot->yAxis->setTicker(logTicker);
    ui->press_plot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
    ui->press_plot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
    ui->press_plot->yAxis->grid()->setSubGridVisible(true);
    ui->press_plot->yAxis->setRange(1e-8, 2e-6); // default range
    ui->press_plot->xAxis->setPadding(10);
    ui->press_plot->yAxis->setPadding(10);

    // add label
    press_plot_label = new QCPItemText(ui->press_plot);
    press_plot_label->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
    press_plot_label->position->setType(QCPItemPosition::ptAxisRectRatio);
    press_plot_label->position->setCoords(0.99, 0.01);
    press_plot_label->setText("");
    press_plot_label->setFont(plot_big_label_font);

    //ui->press_plot->setInteraction(QCP::iRangeDrag, true); // enable plot dragging
    //ui->press_plot->setInteraction(QCP::iRangeZoom, true); // enable plot zoom

    // setup beam plot
    ui->beam_plot->addGraph(); // light green line
    ui->beam_plot->graph(0)->setPen(QPen(QColor(70, 129, 43)));
    ui->beam_plot->addGraph(); // gray line
    ui->beam_plot->graph(1)->setPen(QPen(QColor(180, 180, 180)));
    ui->beam_plot->yAxis->setLabel("Beam Current (mA)");

    // set axes
    ui->beam_plot->xAxis->setTicker(timeTicker);
    ui->beam_plot->yAxis->setRange(0, 30); // default range
    ui->beam_plot->xAxis->setPadding(10);
    ui->beam_plot->yAxis->setPadding(10);

    // add label
    beam_plot_label = new QCPItemText(ui->beam_plot);
    beam_plot_label->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
    beam_plot_label->position->setType(QCPItemPosition::ptAxisRectRatio);
    beam_plot_label->position->setCoords(0.99, 0.01);
    beam_plot_label->setText("");
    beam_plot_label->setFont(plot_big_label_font);

    //ui->beam_plot->setInteraction(QCP::iRangeDrag, true); // enable plot dragging
    //ui->beam_plot->setInteraction(QCP::iRangeZoom, true); // enable plot zoom

    // setup power plot
    ui->power_plot->addGraph(); // light green line -> power in W
    ui->power_plot->graph(0)->setPen(QPen(QColor(70, 129, 43)));
    ui->power_plot->addGraph(); // grey line -> power in W setpoint
    ui->power_plot->graph(1)->setPen(QPen(QColor(180, 180, 180)));
    ui->power_plot->xAxis->setLabel("Run Time (h:m:s)");
    ui->power_plot->yAxis->setLabel("Voltage (mV)");
    ui->power_plot->yAxis->setNumberPrecision(3);
    ui->press_plot->yAxis->setNumberFormat("g"); // display numbers in short scientific format or fixed, whichever is shorter

    // set axes
    ui->power_plot->xAxis->setTicker(timeTicker);
    ui->power_plot->yAxis->setRange(0, 5);
    ui->power_plot->xAxis->setPadding(10);
    ui->power_plot->yAxis->setPadding(10);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->power_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->power_plot->xAxis2, SLOT(setRange(QCPRange)));
    //connect(ui->power_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->power_plot->yAxis2, SLOT(setRange(QCPRange)));

    // add label for power in mV
    power_plot_label = new QCPItemText(ui->power_plot);
    power_plot_label->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
    power_plot_label->position->setType(QCPItemPosition::ptAxisRectRatio);
    power_plot_label->position->setCoords(0.99, 0.01);
    power_plot_label->setText("");
    power_plot_label->setFont(plot_big_label_font);

    // force redraw
    ui->beam_plot->replot();
    ui->press_plot->replot();
    ui->power_plot->replot();
}

void MainWindow::init_fail_dialog(int err_code)
{
    QString err_msg = "Failed to establish connection with ";
    QString msg_tail = "\n\nSee the event log for more detail.";

    switch(err_code)
    {
    case -1: err_msg += "cathode supply." + msg_tail; break;
    case -2: err_msg += "gun trim coil supply." + msg_tail; break;
    case -3: err_msg += "remote signal interface." + msg_tail; break;
    case -4: err_msg += "SPC pump controller." + msg_tail; break;
    case -5: err_msg += "frequency monitoring system." + msg_tail; break;
    default: err_msg = "Unrecognized error code: " + QString::number(err_code);
    }

    gui.error_dialog(err_msg);
    shutdown();
}

bool MainWindow::valid_check(QString qstr, double max_val, double min_val)
{
    if(qstr == "")
        return false;
    double val = qstr.toDouble();
    return (val >= 0 && val <= max_val && val >= min_val);
}

void MainWindow::init_fields()
{
    smart_edits.push_back(ui->fil_curr_edit);
    ui->fil_curr_edit->assign_items(ui->fil_curr_button, gyro.fil_curr_sp_ptr(), gyro.fil_curr_ptr(), 3);
    ui->fil_curr_edit->assign_validator([=](QString qstr){return valid_check(qstr,gyro.FIL_CURR_LIMIT);});

    smart_edits.push_back(ui->beam_volt_edit);
    ui->beam_volt_edit->assign_items(ui->beam_volt_button, gyro.beam_volt_sp_ptr(), gyro.beam_volt_ptr(), 2);
    ui->beam_volt_edit->assign_validator([=](QString qstr){return valid_check(qstr,gyro.BEAM_VOLT_LIMIT);});

    smart_edits.push_back(ui->beam_curr_edit);
    ui->beam_curr_edit->assign_items(ui->beam_curr_button, gyro.beam_curr_sp_ptr(), gyro.beam_curr_ptr(), 2);
    ui->beam_curr_edit->assign_validator([=](QString qstr){return valid_check(qstr,gyro.BEAM_CURR_LIMIT);});

    smart_edits.push_back(ui->freq_edit);
    ui->freq_edit->assign_items(ui->freq_button, gyro.freq_sp_ptr(), gyro.freq_ptr());
    ui->freq_edit->assign_validator([=](QString qstr){return valid_check(qstr,gyro.UPPER_FREQ_LIMIT,gyro.LOWER_FREQ_LIMIT);});

    smart_edits.push_back(ui->power_edit);
    ui->power_edit->assign_items(ui->power_button, gyro.power_sp_ptr(), gyro.power_ptr());
    ui->power_edit->assign_validator([=](QString qstr){return valid_check(qstr,gyro.POWER_LIMIT);});

    smart_edits.push_back(ui->gtc_curr_edit);
    ui->gtc_curr_edit->assign_items(ui->gtc_curr_button, gyro.gtc_curr_sp_ptr(), gyro.gtc_curr_ptr(), 3);
    ui->gtc_curr_edit->assign_validator([=](QString qstr){return valid_check(qstr,gyro.GTC_CURR_LIMIT);});

    smart_edits.push_back(ui->gtc_volt_edit);
    ui->gtc_volt_edit->assign_items(ui->gtc_volt_button, gyro.gtc_volt_sp_ptr(), gyro.gtc_volt_ptr(), 3);
    ui->gtc_volt_edit->assign_validator([=](QString qstr){return valid_check(qstr,gyro.GTC_VOLT_LIMIT);});

    smart_edits.push_back(ui->beam_kp_edit);
    ui->beam_kp_edit->assign_items(ui->beam_kp_button, gyro.beam_kp_ptr());
    smart_edits.push_back(ui->beam_ki_edit);
    ui->beam_ki_edit->assign_items(ui->beam_ki_button, gyro.beam_ki_ptr());
    smart_edits.push_back(ui->beam_kd_edit);
    ui->beam_kd_edit->assign_items(ui->beam_kd_button, gyro.beam_kd_ptr());

    smart_edits.push_back(ui->power_kp_edit);
    ui->power_kp_edit->assign_items(ui->power_kp_button, gyro.power_kp_ptr());
    smart_edits.push_back(ui->power_ki_edit);
    ui->power_ki_edit->assign_items(ui->power_ki_button, gyro.power_ki_ptr());
    smart_edits.push_back(ui->power_kd_edit);
    ui->power_kd_edit->assign_items(ui->power_kd_button, gyro.power_kd_ptr());

    smart_edits.push_back(ui->freq_kp_edit);
    ui->freq_kp_edit->assign_items(ui->freq_kp_button, gyro.freq_kp_ptr());
    smart_edits.push_back(ui->freq_ki_edit);
    ui->freq_ki_edit->assign_items(ui->freq_ki_button, gyro.freq_ki_ptr());
    smart_edits.push_back(ui->freq_kd_edit);
    ui->freq_kd_edit->assign_items(ui->freq_kd_button, gyro.freq_kd_ptr());
}

void MainWindow::detect_state_change(int current_state, bool e_ramping, bool manual_update)
{
    if(e_ramping)
    {
        ui->state_label->setText("EMERGENCY\nRAMP DOWN");
        ui->small_state_label->setText("E RAMP DOWN");
        ui->state_frame->setStyleSheet(red_state_bubble);
        ui->prev_state_label->setEnabled(false);
        ui->next_state_label->setEnabled(false);
        ui->prev_state_button->setStyleSheet(prev_style);
        ui->prev_state_button->setText("");
        ui->next_state_button->setStyleSheet(next_style);
        ui->next_state_button->setText("");
    }
    else if(current_state != last_known_state || manual_update)
    {
        switch(current_state)
        {
        case 0:
            ui->state_label->setText("CONTROL\nPOWER ON");
            ui->small_state_label->setText("CTRL PW ON");
            ui->state_frame->setStyleSheet(green_state_bubble);
            ui->prev_state_label->setEnabled(false);
            ui->next_state_label->setEnabled(true);
            ui->next_state_label->setText("Warm Up →");
            ui->prev_state_button->setStyleSheet(prev_style);
            ui->prev_state_button->setText("");
            ui->next_state_button->setStyleSheet(next_style);
            ui->next_state_button->setText("");
            break;
        case 1:
            ui->prev_state_label->setEnabled(true);
            ui->next_state_label->setEnabled(true);
            if(gyro.is_paused())
            {
                ui->state_frame->setStyleSheet(green_state_bubble);

                if(gyro.is_ramping_up())
                {
                    ui->state_label->setText("WARM UP\n(PAUSED)");
                    ui->small_state_label->setText("WARM UP");
                    ui->prev_state_button->setStyleSheet(prev_style);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(next_style);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Cool Down");
                    ui->next_state_label->setText("Resume Warm Up →");
                }
                else if(gyro.is_ramping_down())
                {
                    ui->state_label->setText("COOL DOWN\n(PAUSED)");
                    ui->small_state_label->setText("COOL DOWN");
                    ui->prev_state_button->setStyleSheet(prev_style);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(next_style);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Resume Cool Down");
                    ui->next_state_label->setText("Warm Up →");
                }
            }
            else
            {
                ui->state_frame->setStyleSheet(orange_state_bubble);

                if(gyro.is_ramping_up())
                {
                    ui->state_label->setText("WARM UP");
                    ui->small_state_label->setText("WARM UP");
                    ui->prev_state_button->setStyleSheet(pause_style);
                    ui->prev_state_button->setText("II");
                    ui->next_state_button->setStyleSheet(next_style);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Pause");
                    ui->next_state_label->setText("HV Standby →");
                }
                else if(gyro.is_ramping_down())
                {
                    ui->state_label->setText("COOL DOWN");
                    ui->small_state_label->setText("COOL DOWN");
                    ui->prev_state_button->setStyleSheet(prev_style);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(pause_style);
                    ui->next_state_button->setText("II");
                    ui->prev_state_label->setText("← Control Power On");
                    ui->next_state_label->setText("Pause →");
                }
            }
            break;
        case 2:
            ui->prev_state_button->setStyleSheet(prev_style);
            ui->prev_state_button->setText("");
            ui->next_state_button->setStyleSheet(next_style);
            ui->next_state_button->setText("");
            ui->prev_state_label->setEnabled(true);
            ui->next_state_label->setEnabled(true);
            ui->state_label->setText("HV STANDBY");
            ui->small_state_label->setText("HV STANDBY");
            ui->state_frame->setStyleSheet(green_state_bubble);
            ui->prev_state_label->setText("← Cool Down");
            ui->next_state_label->setText("MW On →");
            break;
        case 3:
            ui->state_label->setText("MW ON ⚡");
            ui->small_state_label->setText("MW ON");
            ui->prev_state_label->setEnabled(true);
            ui->next_state_label->setEnabled(false);
            ui->state_frame->setStyleSheet(green_state_bubble);
            ui->prev_state_label->setText("← HV Standby");
            break;
        }
    }
    last_known_state = current_state;
}

void MainWindow::update_plots()
{
    double beam_max, beam_min, beam_max_bound, beam_min_bound;
    double press_max, press_min, press_max_bound, press_min_bound;
    double power_max, power_min, power_max_bound, power_min_bound;

    QVector<double> press_data = QVector<double>::fromStdVector(gyro.get_press_data());
    QVector<double> press_time_data = QVector<double>::fromStdVector(gyro.get_press_time_data());
    QVector<double> beam_data = QVector<double>::fromStdVector(gyro.get_beam_data());
    QVector<double> beam_time_data = QVector<double>::fromStdVector(gyro.get_beam_time_data());
    QVector<double> power_data = QVector<double>::fromStdVector(gyro.get_power_data());
    QVector<double> power_time_data = QVector<double>::fromStdVector(gyro.get_power_time_data());

    // update beam plot, with bounds being nearest multiple of 5 above/below
    ui->beam_plot->graph(0)->setData(time_data, beam_data, true);
    ui->beam_plot->graph(1)->setData(beam_time_data, beam_sp_data, true);
    ui->beam_plot->xAxis->setRange(key, plot_span, Qt::AlignRight);
    if(resize_tracker == 0)
    {
        // calculate new bounds and re-apply them
        beam_max = *std::max_element(beam_data.constBegin(), beam_data.constEnd());
        beam_min = *std::min_element(beam_data.constBegin(), beam_data.constEnd());
        //beam_max_bound = (floor(beam_max/5)+1)*5;
        beam_max_bound = beam_max+((beam_max-beam_min)*0.1);
        //beam_min_bound = (floor(beam_min/5)-1)*5;
        beam_min_bound = beam_min-((beam_max-beam_min)*0.1);
        if(beam_min_bound < 0) beam_min_bound = 0;
        ui->beam_plot->yAxis->setRange(beam_min_bound,beam_max_bound);
        resize_tracker++;
    }

    // update pressure plot, with upper bound being nearest decade above/below
    ui->press_plot->graph(0)->setData(time_data, press_data, true);
    ui->press_plot->xAxis->setRange(key, plot_span, Qt::AlignRight);
    if(resize_tracker == 1)
    {
        // calculate new bounds and re-apply them
        press_max = *std::max_element(press_data.constBegin(), press_data.constEnd());
        press_min = *std::min_element(press_data.constBegin(), press_data.constEnd());
        press_max_bound = pow(10,ceil(log10 (press_max)));
        press_min_bound = pow(10,floor(log10 (press_min)));
        ui->press_plot->yAxis->setRange(press_min_bound,press_max_bound);
        resize_tracker++;
    }

    // update power plot, with bounds being nearest quarter watt above/below
    ui->power_plot->graph(0)->setData(time_data, power_data, true);
    ui->power_plot->graph(1)->setData(power_time_data, power_sp_data, true);
    ui->power_plot->xAxis->setRange(key, plot_span, Qt::AlignRight);
    if(resize_tracker == 2)
    {
        // calculate new bounds and re-apply them
        power_max = *std::max_element(power_data.constBegin(), power_data.constEnd());
        power_min = *std::min_element(power_data.constBegin(), power_data.constEnd());
        power_max_bound = (floor(power_max/5)+1)*5;
        power_min_bound = (floor(power_min/5)-1)*5;
        if(power_min_bound < 0) power_min_bound = 0;
        ui->power_plot->yAxis2->setRange(power_min_bound,power_max_bound);
        resize_tracker = 0;
    }

    if(ui->stackedWidget->currentIndex() == 1)
    {
        ui->press_plot->replot(); // update plot if visible
        ui->beam_plot->replot();
        ui->power_plot->replot();
    }
}

void MainWindow::realtime_slot()
{
    static QTime timer(QTime::currentTime()); // setup loop timer
    static double key = timer.elapsed()/1000.0; // used to track time
    static double last_key = 0; // used to track differences in time
    bool e_ramping, ramping_down, ramping_up;
    int current_state, fault_status;
    double pressure;
    std::vector<std::string> warnings, errors;

    if (key-last_key > refresh_rate) // frequency of reiteration is refresh_rate in seconds
    {
        current_state = gyro.get_state();
        e_ramping = gyro.is_e_ramping();
        ramping_up = gyro.is_ramping_up();
        ramping_down = gyro.is_ramping_down();
        pressure = gyro.get_pressure();
        fault_status = gyro.get_fault_status();
        fault_status = 0; // stand-in
        warnings = gyro.get_sys_warnings();
        errors = gyro.get_sys_errors();

        qApp->processEvents();

        // enable/disable state machine arrows based on system status
        ui->prev_state_button->setEnabled(current_state > 0 && !ramping_down && !e_ramping);
        ui->next_state_button->setEnabled(gyro.all_clear() && current_state < 3 && !ramping_up && !e_ramping);

        // apply stylesheet changes based on state
        detect_state_change(current_state,e_ramping);

        qApp->processEvents();

        // update smart line edits
        for(auto field : smart_edits) { field->update(); }

        qApp->processEvents();

        // update labels
        ui->press_label->setText(QString::number(pressure));
        ui->collector_read->setText(QString::number(gyro.get_collector_curr()));
        ui->body_read->setText(QString::number(gyro.get_body_curr()));
        ui->fil_curr_label->setText(QString::number(gyro.get_fil_curr()));
        ui->beam_volt_label->setText(QString::number(gyro.get_beam_volt()));
        ui->freq_label->setText(QString::number(gyro.get_freq()));

        qApp->processEvents();

        // update status bubbles and pressure group box
        switch(gyro.get_temp_status())
        {
        case 0: ui->temp_indicator->setStyleSheet(green_status_bubble); ui->temp_status->setText("OK"); break;
        case -1: ui->temp_indicator->setStyleSheet(yellow_status_bubble); ui->temp_status->setText("WARN"); break;
        case -2: ui->temp_indicator->setStyleSheet(red_status_bubble); ui->temp_status->setText("HIGH!"); break;
        }
        switch(gyro.get_flow_status())
        {
        case 0: ui->flow_indicator->setStyleSheet(green_status_bubble); ui->flow_status->setText("OK"); break;
        case -1: ui->flow_indicator->setStyleSheet(yellow_status_bubble); ui->flow_status->setText("WARN"); break;
        case -2: ui->flow_indicator->setStyleSheet(red_status_bubble); ui->flow_status->setText("LOW!"); break;
        }
        switch(fault_status)
        {
        case 0: ui->faults_indicator->setStyleSheet(green_status_bubble); ui->fault_status->setText("OK"); break;
        case -1: ui->faults_indicator->setStyleSheet(yellow_status_bubble); ui->fault_status->setText("WARN"); break;
        case -2: ui->faults_indicator->setStyleSheet(red_status_bubble); ui->fault_status->setText("ERR!"); break;
        }
        switch(gyro.get_press_status())
        {
        case 0: ui->press_group->setStyleSheet(group_style); ui->press_group->setTitle("Pressure"); break;
        case -1: ui->press_group->setStyleSheet(warn_group_style); ui->press_group->setTitle("Pressure (RELAXATION IN PROGRESS)"); break;
        case -2: ui->press_group->setStyleSheet(err_group_style); ui->press_group->setTitle("Pressure (FATAL)"); break;
        }

        qApp->processEvents();

        // update plot labels
        press_plot_label->setText(QString::number(pressure) + QString(" Torr"));
        beam_plot_label->setText(QString::number(gyro.get_beam_curr(),'f',2) + QString(" mA"));
        power_plot_label->setText(QString::number(gyro.get_power()) + QString(" W"));

        qApp->processEvents();

        // update plot data and scale the y-axes accordingly
        update_plots();

        qApp->processEvents();

        // update log stream
        ui->log_box->append(QString::fromStdString(gyro.get_event_history()));

        // update faults list
        if(fault_status == 0)
        {
            ui->fault_list->clear();
            fault_list_items.clear();
            ui->fault_list->setStyleSheet(empty_faults_list);
        }
        else
        {
            ui->fault_list->setStyleSheet(faults_list);
            for(auto error : errors) {
                fault_list_items.push_back(new QListWidgetItem(error_icon, QString::fromStdString(error), ui->fault_list));
                ui->fault_list->addItem(fault_list_items.back());
            }
            for(auto warning : warnings) {
                fault_list_items.push_back(new QListWidgetItem(warning_icon, QString::fromStdString(warning), ui->fault_list));
                ui->fault_list->addItem(fault_list_items.back());
            }
        }

        // if any PID is on update the filament current button
        if(gyro.beam_pid_is_on() || gyro.power_pid_is_on() || gyro.freq_pid_is_on())
        {
            ui->fil_curr_button->setText("set: " + QString::number(gyro.get_fil_curr_sp()));

            if(gyro.freq_pid_is_on())
                ui->beam_volt_button->setText("set: " + QString::number(gyro.get_beam_volt_sp()));
        }

        last_key = key;
    }
}

void MainWindow::closeEvent (QCloseEvent *event)
{ gui.question_dialog("Are you sure you want to quit?\n",[=](){shutdown();},[=](){event->ignore();}); }

void MainWindow::on_reconfig_button_clicked() { gyro.extract_config(); }

void MainWindow::on_beam_pid_button_clicked()
{
    gyro.toggle_beam_pid();

    if(gyro.beam_pid_is_on())
    {
        ui->beam_pid_button->setStyleSheet(gui.orange_button(46));
        ui->power_pid_button->setEnabled(false);
        ui->freq_pid_button->setEnabled(false);
        ui->beam_curr_button->setEnabled(true);
        ui->small_pid_label->setText("CURRENT");
        ui->fil_curr_button->setEnabled(false);
        ui->fil_curr_button->setText("set: ");
        ui->fil_curr_button->setFont(small_button_font);
        ui->fil_curr_button->setStyleSheet(orange_pid_button);
        gyro.log_event("automatic beam current control enabled");
    }
    else
    {
        ui->beam_pid_button->setStyleSheet(gui.green_button(46));
        ui->power_pid_button->setEnabled(true);
        ui->freq_pid_button->setEnabled(true);
        ui->beam_curr_button->setEnabled(false);
        ui->small_pid_label->setText("NONE");
        ui->fil_curr_button->setEnabled(true);
        ui->fil_curr_button->setText("✎");
        ui->fil_curr_button->setFont(button_font);
        ui->fil_curr_button->setStyleSheet(green_edit_button);
        gyro.log_event("automatic beam current control disabled");
    }
}

void MainWindow::on_power_pid_button_clicked()
{
    gyro.toggle_power_pid();

    if(gyro.power_pid_is_on())
    {
        ui->power_pid_button->setStyleSheet(gui.orange_button(46));
        ui->beam_pid_button->setEnabled(false);
        ui->freq_pid_button->setEnabled(false);
        ui->power_button->setEnabled(true);
        ui->small_pid_label->setText("POWER");
        ui->fil_curr_button->setEnabled(false);
        ui->fil_curr_button->setText("set: ");
        ui->fil_curr_button->setFont(small_button_font);
        ui->fil_curr_button->setStyleSheet(orange_pid_button);
        gyro.log_event("automatic power control enabled");
    }
    else
    {
        ui->power_pid_button->setStyleSheet(gui.green_button(46));
        ui->beam_pid_button->setEnabled(true);
        ui->freq_pid_button->setEnabled(true);
        ui->power_button->setEnabled(false);
        ui->small_pid_label->setText("NONE");
        ui->fil_curr_button->setEnabled(true);
        ui->fil_curr_button->setText("✎");
        ui->fil_curr_button->setFont(button_font);
        ui->fil_curr_button->setStyleSheet(green_edit_button);
        gyro.log_event("automatic power control disabled");
    }
}

void MainWindow::on_freq_pid_button_clicked()
{
    gyro.toggle_freq_pid();

    if(gyro.freq_pid_is_on())
    {
        ui->freq_pid_button->setStyleSheet(gui.orange_button(46));
        ui->beam_pid_button->setEnabled(false);
        ui->power_pid_button->setEnabled(false);
        ui->freq_button->setEnabled(true);
        ui->small_pid_label->setText("FREQUENCY");
        ui->fil_curr_button->setEnabled(false);
        ui->fil_curr_button->setText("set: ");
        ui->fil_curr_button->setFont(small_button_font);
        ui->fil_curr_button->setStyleSheet(orange_pid_button);
        ui->beam_volt_button->setEnabled(false);
        ui->beam_volt_button->setText("set: ");
        ui->beam_volt_button->setFont(small_button_font);
        ui->beam_volt_button->setStyleSheet(orange_pid_button);
        gyro.log_event("automatic frequency control enabled");
    }
    else
    {
        ui->freq_pid_button->setStyleSheet(gui.green_button(46));
        ui->beam_pid_button->setEnabled(true);
        ui->power_pid_button->setEnabled(true);
        ui->freq_button->setEnabled(false);
        ui->small_pid_label->setText("NONE");
        ui->fil_curr_button->setEnabled(true);
        ui->fil_curr_button->setText("✎");
        ui->fil_curr_button->setFont(button_font);
        ui->fil_curr_button->setStyleSheet(green_edit_button);
        ui->beam_volt_button->setEnabled(true);
        ui->beam_volt_button->setText("✎");
        ui->beam_volt_button->setFont(button_font);
        ui->beam_volt_button->setStyleSheet(green_edit_button);
        gyro.log_event("automatic frequency control disabled");
    }
}

void MainWindow::on_beam_kp_button_clicked()
{
    double entry = ui->beam_kp_edit->toggle_active();
    if(entry >= 0) gyro.set_beam_kp(entry);
}

void MainWindow::on_beam_ki_button_clicked()
{
    double entry = ui->beam_ki_edit->toggle_active();
    if(entry >= 0) gyro.set_beam_ki(entry);
}

void MainWindow::on_beam_kd_button_clicked()
{
    double entry = ui->beam_kd_edit->toggle_active();
    if(entry >= 0) gyro.set_beam_kd(entry);
}

void MainWindow::on_power_kp_button_clicked()
{
    double entry = ui->power_kp_edit->toggle_active();
    if(entry >= 0) gyro.set_power_kp(entry);
}

void MainWindow::on_power_ki_button_clicked()
{
    double entry = ui->power_ki_edit->toggle_active();
    if(entry >= 0) gyro.set_power_ki(entry);
}

void MainWindow::on_power_kd_button_clicked()
{
    double entry = ui->power_kd_edit->toggle_active();
    if(entry >= 0) gyro.set_power_kd(entry);
}

void MainWindow::on_freq_kp_button_clicked()
{
    double entry = ui->freq_kp_edit->toggle_active();
    if(entry >= 0) gyro.set_freq_kp(entry);
}

void MainWindow::on_freq_ki_button_clicked()
{
    double entry = ui->freq_ki_edit->toggle_active();
    if(entry >= 0) gyro.set_freq_ki(entry);
}

void MainWindow::on_freq_kd_button_clicked()
{
    double entry = ui->freq_kd_edit->toggle_active();
    if(entry >= 0) gyro.set_freq_kd(entry);
}

void MainWindow::on_fil_curr_button_clicked()
{
    double entry = ui->fil_curr_edit->toggle_active();
    if(entry >= 0)
    {
        int stat = gyro.set_fil_curr(entry);
        if(stat < 0)
            gui.error_dialog(QString::fromStdString("Error setting filament current! (" + to_str(stat) + ")"));
    }
}

void MainWindow::on_beam_volt_button_clicked()
{
    double entry = ui->beam_volt_edit->toggle_active();
    if(entry >= 0)
    {
        int stat = gyro.set_beam_volt(entry);
        if(stat < 0)
            gui.error_dialog(QString::fromStdString("Error setting beam voltage! (" + to_str(stat) + ")"));
    }
}

void MainWindow::on_beam_curr_button_clicked()
{
    double entry = ui->beam_curr_edit->toggle_active();
    if(entry >= 0) gyro.set_beam_curr(entry);
}

void MainWindow::on_power_button_clicked()
{
    double entry = ui->power_edit->toggle_active();
    if(entry >= 0) gyro.set_power(entry);
}

void MainWindow::on_freq_button_clicked()
{
    double entry = ui->freq_edit->toggle_active();
    if(entry >= 0) gyro.set_freq(entry);
}

void MainWindow::on_prev_state_button_clicked()
{
    if(gyro.decrement_state() < 0)
        gui.error_dialog("Failed to change state! See log for more info.\n");
    else
        detect_state_change(gyro.get_state(),gyro.is_e_ramping(),true);
}

void MainWindow::on_next_state_button_clicked()
{
    if(gyro.increment_state() < 0)
        gui.error_dialog("Failed to change state! See log for more info.\n");
    else
        detect_state_change(gyro.get_state(),gyro.is_e_ramping(),true);
}

void MainWindow::on_gtc_curr_button_clicked()
{
    double entry = ui->gtc_curr_edit->toggle_active();
    if(entry >= 0)
    {
        int stat = gyro.set_gtc_curr_limit(entry);
        if(stat < 0)
            gui.error_dialog(QString::fromStdString("Error setting GTC current! (" + to_str(stat) + ")"));
    }
}

void MainWindow::on_gtc_volt_button_clicked()
{
    double entry = ui->gtc_volt_edit->toggle_active();
    if(entry >= 0)
    {
        int stat = gyro.set_gtc_volt(entry);
        if(stat < 0)
            gui.error_dialog(QString::fromStdString("Error setting GTC voltage! (" + to_str(stat) + ")"));
    }
}

void MainWindow::on_close_button_clicked() { this->close(); }
void MainWindow::on_minimize_button_clicked() { this->showMinimized(); }

void MainWindow::on_control_tab_clicked()
{
    ui->control_tab->setStyleSheet(tab_selected);
    ui->plot_tab->setStyleSheet(tab_unselected);
    ui->status_tab->setStyleSheet(tab_unselected);
    ui->admin_tab->setStyleSheet(tab_unselected);
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_plot_tab_clicked()
{
    ui->control_tab->setStyleSheet(tab_unselected);
    ui->plot_tab->setStyleSheet(tab_selected);
    ui->status_tab->setStyleSheet(tab_unselected);
    ui->admin_tab->setStyleSheet(tab_unselected);
    ui->press_plot->replot();
    ui->beam_plot->replot();
    ui->power_plot->replot();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_status_tab_clicked()
{
    ui->control_tab->setStyleSheet(tab_unselected);
    ui->plot_tab->setStyleSheet(tab_unselected);
    ui->status_tab->setStyleSheet(tab_selected);
    ui->admin_tab->setStyleSheet(tab_unselected);
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_admin_tab_clicked()
{
    ui->control_tab->setStyleSheet(tab_unselected);
    ui->plot_tab->setStyleSheet(tab_unselected);
    ui->status_tab->setStyleSheet(tab_unselected);
    ui->admin_tab->setStyleSheet(tab_selected);
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::mousePressEvent(QMouseEvent *evt)
{
    oldPos = evt->globalPos();
}
void MainWindow::mouseMoveEvent(QMouseEvent *evt)
{
    const QPoint delta = evt->globalPos() - oldPos;
    if(!window_locked) move(x()+delta.x(), y()+delta.y());
    oldPos = evt->globalPos();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QString obj_str = obj->objectName();
    if(obj_str.contains("_group"))
    {
        if(event->type() == QEvent::Enter)
            window_locked = true;
        else if(event->type() == QEvent::Leave)
            window_locked = false;
    }
    return QWidget::eventFilter(obj, event);
}
