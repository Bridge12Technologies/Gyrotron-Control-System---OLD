#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    showFullScreen();

    if(gyro.gui_debug_mode)
        init_gui();
    else {
        int init_stat = gyro.init(); // parse config, spawn threads, connect/probe devices, and exec pre-funcs
        if(init_stat < 0)
            init_fail_dialog(init_stat);
        else
        {
            init_gui();
            init_fields(); // initialize SmartLineEdits
            admin_mode = gyro.admin_mode_config(); // get admin mode setting from config file
            if(!admin_mode)
                ui->admin_tab->setVisible(false);
        }
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::shutdown()
{
    gyro.log_event("exiting program");
    data_timer.stop();
    if(!gyro.gui_debug_mode) gyro.stop();
    QApplication::quit();
}

void MainWindow::init_gui()
{
    QWidget *boxes[27] = {ui->status_group,ui->state_group,ui->beam_params_group,ui->fms_group,ui->power_group,
                         ui->pid_group,ui->plot_group,ui->fil_curr_group,ui->beam_volt_group,ui->freq_group,ui->curr_state_group,ui->auto_state_group,
                         ui->time_span_group,ui->faults_group,ui->log_group,ui->beam_pid_group,ui->power_pid_group,ui->freq_group,ui->ramp_rate_group,
                         ui->log_rate_group,ui->misc_group,ui->reconnect_group,ui->console_group,ui->settings_frame,ui->clock_frame,ui->storage_frame,
                         ui->plot_select_group};
    for(auto box : boxes) { box->installEventFilter(this); }

    // apply drop shadows to group boxes
    std::vector<QWidget*> groups{ui->state_group,ui->status_group,ui->beam_params_group,ui->fms_group,ui->power_group,ui->pid_group,
                                   ui->plot_group,ui->fil_curr_group,ui->beam_volt_group,ui->freq_group,ui->curr_state_group,ui->auto_state_group,ui->time_span_group,
                                   ui->beam_pid_group,ui->power_pid_group,ui->freq_pid_group,ui->ramp_rate_group,ui->log_rate_group,ui->misc_group,ui->reconnect_group,ui->console_group,
                                   ui->faults_group,ui->log_group,ui->settings_frame,ui->clock_frame,ui->storage_frame,ui->plot_select_group};
    for(QWidget* group : groups)
    {
        shadows.push_back(new QGraphicsDropShadowEffect);
        shadows.back()->setBlurRadius(25);
        shadows.back()->setXOffset(0);
        shadows.back()->setYOffset(0);
        shadows.back()->setColor(QColor(30,30,30,35));
        group->setGraphicsEffect(shadows.back());
    }

    // hide beam parameter group initially
    ui->beam_params_group->setVisible(false);
    ui->settings_frame->setVisible(false);
    ui->storage_frame->setVisible(false);
    ui->clock_frame->setVisible(false);
    ui->show_more_layout->setContentsMargins(0,0,0,0);
    ui->control_grid->setVerticalSpacing(60);
    ui->control_grid->setHorizontalSpacing(60);

    init_plots();

    QPalette palette;
    palette.setColor(QPalette::Highlight, ui->fault_list->palette().color(QPalette::Base));
    palette.setColor(QPalette::HighlightedText, ui->fault_list->palette().color(QPalette::Text));
    ui->fault_list->setPalette(palette);
    ui->fault_list->setViewMode(QListView::ListMode);

    if(!gyro.gui_debug_mode)
    {
        connect(&blink_timer, SIGNAL(timeout()), this, SLOT(blink_status())); // enable faults blink effect

        connect(&data_timer, SIGNAL(timeout()), this, SLOT(realtime_slot())); // enable the realtime slot
        data_timer.start(0); // start timer to begin calling realtime_slot repeatedly
    }
}

void MainWindow::init_plots()
{
    // align all left/right plot margins
    QCPMarginGroup *group = new QCPMarginGroup(ui->power_plot); // previously used the press_plot
    ui->press_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ui->beam_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ui->power_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);

    ui->press_plot->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->beam_plot->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->power_plot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->press_plot, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(press_context_menu(const QPoint &)));
    connect(ui->beam_plot, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(beam_context_menu(const QPoint &)));
    connect(ui->power_plot, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(power_context_menu(const QPoint &)));

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
    ui->press_plot->yAxis->setLabel("Vacuum (Torr)");
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

    ui->plot_splitter->setVisible(false);
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
    ui->gtc_volt_edit->assign_items(nullptr, gyro.gtc_volt_sp_ptr(), gyro.gtc_volt_ptr(), 3);
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

void MainWindow::set_blink_enabled(bool enable)
{
    blink_enabled = enable;
    if(!blink_enabled && blink_timer.isActive())
        set_blink_on(false);
}

void MainWindow::set_blink_on(bool enable)
{
    if(enable && blink_enabled)
    {
        if(ui->stackedWidget->currentIndex() == 2)
            ui->status_tab->setStyleSheet(tab_blink_off_selected);
        else
            ui->status_tab->setStyleSheet(tab_blink_off_unselected);
        blink_timer.start(blink_duration);
    }
    else
    {
        blink_timer.stop();
        if(ui->stackedWidget->currentIndex() == 2)
            ui->status_tab->setStyleSheet(tab_selected);
        else
            ui->status_tab->setStyleSheet(tab_unselected);
    }
}

void MainWindow::blink_status()
{
    if(ui->stackedWidget->currentIndex() != 2 && blink_enabled)
    {
        if(blink_on)
        {
            ui->status_tab->setStyleSheet(tab_blink_off_unselected);
            blink_on = false;
        }
        else
        {
            ui->status_tab->setStyleSheet(tab_blink_on);
            blink_on = true;
        }
    }
}

void MainWindow::detect_state_change(bool manual_update)
{
    int current_state = gyro.get_state();
    bool e_ramping = gyro.is_e_ramping();
    bool ramping_up = gyro.is_ramping_up();
    bool ramping_down = gyro.is_ramping_down();

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
        ui->state_mini_label->setText(QString::number(100*(1-(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT))) + "%");
    }
    else if(current_state != last_known_state || manual_update)
    {
        switch(current_state)
        {
        case 0:
            ui->state_label->setText("CONTROL\nPOWER ON");
            ui->small_state_label->setText("CTRL PW ON");
            ui->state_frame->setStyleSheet(grey_state_bubble);
            ui->prev_state_label->setEnabled(false);
            ui->next_state_label->setEnabled(true);
            ui->next_state_label->setText("Warm Up →");
            ui->prev_state_button->setStyleSheet(prev_style);
            ui->prev_state_button->setText("");
            ui->next_state_button->setStyleSheet(next_style);
            ui->next_state_button->setText("");
            ui->state_mini_label->setText("MW Off");
            break;
        case 1:
            ui->prev_state_label->setEnabled(true);
            ui->next_state_label->setEnabled(true);
            if(gyro.is_paused())
            {
                ui->state_frame->setStyleSheet(grey_state_bubble);

                if(ramping_up)
                {
                    ui->state_label->setText("WARM UP\n(PAUSED)");
                    ui->small_state_label->setText("WARM UP");
                    ui->prev_state_button->setStyleSheet(prev_style);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(next_style);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Cool Down");
                    ui->next_state_label->setText("Resume Warm Up →");
                    ui->state_mini_label->setText(QString::number(100*(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT)) + "%");
                }
                else if(ramping_down)
                {
                    ui->state_label->setText("COOL DOWN\n(PAUSED)");
                    ui->small_state_label->setText("COOL DOWN");
                    ui->prev_state_button->setStyleSheet(prev_style);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(next_style);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Resume Cool Down");
                    ui->next_state_label->setText("Warm Up →");
                    ui->state_mini_label->setText(QString::number(100*(1-(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT))) + "%");
                }
            }
            else
            {
                ui->state_frame->setStyleSheet(orange_state_bubble);

                if(ramping_up)
                {
                    ui->state_label->setText("WARM UP");
                    ui->small_state_label->setText("WARM UP");
                    ui->prev_state_button->setStyleSheet(pause_style);
                    ui->prev_state_button->setText("II");
                    ui->next_state_button->setStyleSheet(next_style);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Pause");
                    ui->next_state_label->setText("HV Standby →");
                    ui->state_mini_label->setText(QString::number(100*(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT)) + "%");
                }
                else if(ramping_down)
                {
                    ui->state_label->setText("COOL DOWN");
                    ui->small_state_label->setText("COOL DOWN");
                    ui->prev_state_button->setStyleSheet(prev_style);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(pause_style);
                    ui->next_state_button->setText("II");
                    ui->prev_state_label->setText("← Control Power On");
                    ui->next_state_label->setText("Pause →");
                    ui->state_mini_label->setText(QString::number(100*(1-(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT))) + "%");
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
            ui->state_frame->setStyleSheet(grey_state_bubble);
            ui->prev_state_label->setText("← Cool Down");
            ui->next_state_label->setText("MW On →");
            ui->state_mini_label->setText("MW Off");
            break;
        case 3:
            ui->state_label->setText("MW ON ⚡");
            ui->small_state_label->setText("MW ON");
            ui->prev_state_label->setEnabled(true);
            ui->next_state_label->setEnabled(false);
            ui->state_frame->setStyleSheet(green_state_bubble);
            ui->prev_state_label->setText("← HV Standby");
            ui->state_mini_label->setText(QString::number(100*(gyro.get_power()/gyro.POWER_LIMIT)) + "% power");
            break;
        }
    }
    // enable/disable state machine arrows based on system status
    ui->prev_state_button->setEnabled(current_state > 0 && !ramping_down && !e_ramping);
    ui->next_state_button->setEnabled(gyro.all_clear() && current_state < 3 && !ramping_up && !e_ramping);
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
    QVector<double> beam_sp_data = QVector<double>::fromStdVector(gyro.get_beam_sp_data());
    QVector<double> beam_time_data = QVector<double>::fromStdVector(gyro.get_beam_time_data());
    QVector<double> power_data = QVector<double>::fromStdVector(gyro.get_power_data());
    QVector<double> power_sp_data = QVector<double>::fromStdVector(gyro.get_power_sp_data());
    QVector<double> power_time_data = QVector<double>::fromStdVector(gyro.get_power_time_data());

    // update plot labels
    if(gyro.spc_available())
        press_plot_label->setText(QString::number(gyro.get_pressure()) + QString(" Torr"));
    else
        press_plot_label->setText("D/C");
    if(gyro.cath_available())
        beam_plot_label->setText(QString::number(gyro.get_beam_curr(),'f',2) + QString(" mA"));
    else
        beam_plot_label->setText("D/C");
    if(gyro.rsi_available())
        power_plot_label->setText(QString::number(gyro.get_power()) + QString(" W"));
    else
        power_plot_label->setText("D/C");

    // update beam plot, with bounds being nearest multiple of 5 above/below
    ui->beam_plot->graph(0)->setData(beam_time_data, beam_data, true);
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
    ui->press_plot->graph(0)->setData(press_time_data, press_data, true);
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
    ui->power_plot->graph(0)->setData(power_time_data, power_data, true);
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
    static double last_key = 0; // used to track differences in time
    static double last_clock_tick = 0; // used to update clock
    static double last_storage_check = 0; // used to update storage widget
    key = timer.elapsed()/1000.0; // used to track time

    if(key-last_clock_tick > 30) // update clock every 30 seconds
    {
        dateTime = dateTime.currentDateTime();
        if(use_24hr_format)
            ui->clock_label->setText(dateTime.time().toString("H:mm"));
        else
            ui->clock_label->setText(dateTime.time().toString("h:mm ap"));
    }

    if(key-last_storage_check > 300) // check storage every 5 minutes
    {
        std::filesystem::space_info hdd = std::filesystem::space("/");
        std::uintmax_t total = hdd.capacity;
        std::uintmax_t remaining = hdd.free;
        int percent = int(round((total-remaining)/total));
        ui->percent_full_label->setText(QString::number(percent) + "%");
        int gb_remaining = int(round(remaining/1e9));
        if(gb_remaining < 1)
        {
            ui->space_left_label->setText("<1 GB");
            ui->space_left_label->setStyleSheet("border: none; background: transparent; color: #D16055;");
            ui->percent_full_label->setStyleSheet("border: none; background: transparent; color: #D16055;");
        }
        else if(gb_remaining < 1000)
        {
            ui->space_left_label->setText(QString::number(gb_remaining) + " GB");
            ui->space_left_label->setStyleSheet("border: none; background: transparent; color: rgb(85,87,83);");
            ui->percent_full_label->setStyleSheet("border: none; background: transparent; color: rgb(85,87,83);");
        }
        else
        {
            ui->space_left_label->setText(QString::number(gb_remaining/1000) + " TB");
            ui->space_left_label->setStyleSheet("border: none; background: transparent; color: rgb(85,87,83);");
            ui->percent_full_label->setStyleSheet("border: none; background: transparent; color: rgb(85,87,83);");
        }
    }

    if (key-last_key > refresh_rate) // frequency of reiteration is refresh_rate in seconds
    {
        check_connections(); qApp->processEvents();
        detect_state_change(); qApp->processEvents();
        for(auto field : smart_edits) { field->update(); } qApp->processEvents();
        update_labels(); qApp->processEvents();
        update_indicators(); qApp->processEvents();
        if(ui->stackedWidget->currentIndex() == 1) { update_plots(); } qApp->processEvents();
        ui->log_box->append(QString::fromStdString(gyro.get_event_history())); qApp->processEvents();
        update_faults(); qApp->processEvents();
        update_pid_display(); qApp->processEvents();
        last_key = key;
    }
}

void MainWindow::update_pid_display()
{
    // if any PID is on update the filament current button
    if(gyro.beam_pid_is_on() || gyro.power_pid_is_on() || gyro.freq_pid_is_on())
    {
        ui->fil_curr_button->setText("set: " + QString::number(gyro.get_fil_curr_sp()));

        if(gyro.freq_pid_is_on())
            ui->beam_volt_button->setText("set: " + QString::number(gyro.get_beam_volt_sp()));
    }
}

bool MainWindow::lists_equal(std::vector<QListWidgetItem*> list1, std::vector<QListWidgetItem*> list2)
{
    if(list1.size() == 0 || list2.size() == 0 || list1.size() != list2.size())
        return false;
    for(unsigned long i = 0; i < list1.size(); i++)
    {
        if(list1.at(i)->text() != list2.at(i)->text())
            return false;
    }
    return true;
}

void MainWindow::update_faults()
{
    std::vector<std::string> warnings = gyro.get_warnings();
    std::vector<std::string> errors = gyro.get_errors();
    int fault_status = gyro.get_fault_status();
    std::vector<QListWidgetItem*> temp_fault_items;

    if(fault_status == -2 && last_fault_status != -2)
    {
        set_blink_on(true);
        if(ui->stackedWidget->currentIndex() == 2)
            ui->status_tab->setStyleSheet(tab_blink_off_selected);
        else
            ui->status_tab->setStyleSheet(tab_blink_off_unselected);
    }
    else if(fault_status > -2 && last_fault_status == -2)
    {
        set_blink_on(false);
        if(ui->stackedWidget->currentIndex() == 2)
            ui->status_tab->setStyleSheet(tab_selected);
        else
            ui->status_tab->setStyleSheet(tab_unselected);
    }

    // update faults list
    if(fault_status == 0 && fault_status != last_fault_status)
    {
        ui->fault_list->clear();
        fault_list_items.clear();
        ui->fault_list->setStyleSheet(empty_faults_list);
    }
    else
    {
        if(fault_status < 0)
            ui->fault_list->setStyleSheet(faults_list);

        for(auto error : errors) {
            temp_fault_items.push_back(new QListWidgetItem(error_icon, QString("     ") + QString::fromStdString(error), ui->fault_list));
            //ui->fault_list->addItem(fault_list_items.back());
        }
        for(auto warning : warnings) {
            temp_fault_items.push_back(new QListWidgetItem(warning_icon, QString("     ") + QString::fromStdString(warning), ui->fault_list));
            //ui->fault_list->addItem(fault_list_items.back());
        }
        //if(!lists_equal(temp_fault_items,fault_list_items))
        //{
            //ui->fault_list->clear();
            //fault_list_items = temp_fault_items;
            //for(auto item : temp_fault_items) { ui->fault_list->addItem(item); }
        //}
        //if(ui->fault_list->count() > 0) ui->fault_list->clear();
        while(ui->fault_list->count() > 0) ui->fault_list->takeItem(0);
        fault_list_items = temp_fault_items;
        for(auto item : temp_fault_items) { ui->fault_list->addItem(item); }
    }
    last_fault_status = fault_status;
    //for(auto ptr : temp_fault_items) delete ptr;
}

void MainWindow::update_indicators()
{
    if(gyro.rsi_available())
    {
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
    }
    switch(gyro.get_fault_status())
    {
    case 0: ui->faults_indicator->setStyleSheet(green_status_bubble); ui->fault_status->setText("OK"); break;
    case -1: ui->faults_indicator->setStyleSheet(yellow_status_bubble); ui->fault_status->setText("WARN ×" + QString::number(gyro.get_num_warnings())); break;
    case -2: ui->faults_indicator->setStyleSheet(red_status_bubble); ui->fault_status->setText("ERR ×" + QString::number(gyro.get_num_errors())); break;
    }
    if(gyro.spc_available())
    {
        ui->press_label->setText(QString::number(gyro.get_pressure()));
        switch(gyro.get_press_status())
        {
        case 0: ui->press_indicator->setStyleSheet(green_status_bubble); break;
        case -1: ui->press_indicator->setStyleSheet(yellow_status_bubble); break;
        case -2: ui->press_indicator->setStyleSheet(red_status_bubble); break;
        }
    }
}

void MainWindow::update_labels()
{
    if(gyro.spc_available())
        ui->press_label->setText(QString::number(gyro.get_pressure()));
    if(gyro.rsi_available())
    {
        ui->collector_read->setText(QString::number(gyro.get_collector_curr()));
        ui->body_read->setText(QString::number(gyro.get_body_curr()));
    }
    else
    {
        ui->collector_read->setText("N/A");
        ui->body_read->setText("N/A");
    }
    if(gyro.cath_available())
    {
        ui->fil_curr_label->setText(QString::number(gyro.get_fil_curr()));
        ui->beam_volt_label->setText(QString::number(gyro.get_beam_volt()));
    }
    if(gyro.fms_available())
        ui->freq_label->setText(QString::number(gyro.get_freq()));
}

void MainWindow::check_connections()
{
    if(gyro.cath_is_connected() && gyro.cath_is_enabled())
    {
        ui->fil_curr_group->setEnabled(true);
        ui->beam_volt_group->setEnabled(true);
        ui->fil_curr_button->setVisible(true);
        ui->beam_volt_button->setVisible(true);
        ui->beam_curr_button->setVisible(true);
    }
    else
    {
        ui->fil_curr_edit->setText("N/A");
        ui->beam_curr_edit->setText("N/A");
        ui->beam_volt_edit->setText("N/A");
        ui->fil_curr_group->setEnabled(false);
        ui->beam_volt_group->setEnabled(false);
        ui->fil_curr_button->setVisible(false);
        ui->beam_volt_button->setVisible(false);
        ui->beam_curr_button->setVisible(false);
    }
    if(!gyro.gtc_is_connected() || !gyro.gtc_is_enabled())
    {
        ui->gtc_curr_edit->setText("N/A");
        ui->gtc_volt_edit->setText("N/A");
    }
    if(gyro.rsi_is_connected() && gyro.rsi_is_enabled())
    {
        ui->power_group->setEnabled(true);
        ui->power_button->setVisible(true);
        ui->temp_indicator->setEnabled(true);
        ui->flow_indicator->setEnabled(true);
    }
    else
    {
        ui->power_group->setEnabled(false);
        ui->power_button->setVisible(false);
        ui->temp_indicator->setEnabled(false);
        ui->temp_status->setText("N/A");
        ui->flow_indicator->setEnabled(false);
        ui->flow_status->setText("N/A");
    }
    if(gyro.spc_is_connected() && gyro.spc_is_enabled())
    {
        ui->press_indicator->setEnabled(true);
        ui->press_label->setText("N/A");
    }
    else
    {
        ui->press_indicator->setEnabled(false);
        ui->press_label->setText("N/A");
    }
    if(gyro.fms_is_connected() && gyro.fms_is_enabled())
    {
        ui->fms_group->setEnabled(true);
        ui->freq_group->setEnabled(true);
        ui->freq_button->setVisible(true);
    }
    else
    {
        ui->fms_group->setEnabled(false);
        ui->freq_group->setEnabled(false);
        ui->freq_button->setVisible(false);
    }

    if(ui->beam_params_group->isEnabled() && gyro.get_state() == 3 && !gyro.power_pid_is_on() && !gyro.freq_pid_is_on() && ui->pid_dropdown->currentIndex() == 2)
        ui->enable_button->setEnabled(false);
    else if(ui->beam_params_group->isEnabled() && gyro.get_state() == 3 && ui->power_group->isEnabled() && !gyro.beam_pid_is_on() && !gyro.freq_pid_is_on() && ui->pid_dropdown->currentIndex() == 0)
        ui->enable_button->setEnabled(false);
    else if(ui->beam_params_group->isEnabled() && gyro.get_state() == 3 && ui->freq_group->isEnabled() && !gyro.beam_pid_is_on() && !gyro.power_pid_is_on() && ui->pid_dropdown->currentIndex() == 1)
        ui->enable_button->setEnabled(false);
    else ui->enable_button->setEnabled(true);
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    ui->plot_splitter->setVisible(false);
    gui.question_dialog("Are you sure you want to quit?\n",
                        [=](){shutdown();},[=](){event->ignore(); ui->plot_splitter->setVisible(true);});
}

void MainWindow::on_reconfig_button_clicked() { gyro.extract_config(); }
/*
void MainWindow::on_beam_pid_button_clicked()
{
    gyro.toggle_beam_pid();

    if(gyro.beam_pid_is_on())
    {
        ui->enable_button->setStyleSheet(gui.orange_button(36));
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
        ui->enable_button->setStyleSheet(gui.grey_button(36));
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
        ui->enable_button->setStyleSheet(gui.orange_button(36));
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
        ui->enable_button->setStyleSheet(gui.grey_button(36));
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
        ui->enable_button->setStyleSheet(gui.orange_button(36));
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
        ui->enable_button->setStyleSheet(gui.grey_button(36));
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
*/
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
        {
            gui.error_dialog(QString::fromStdString("Error setting filament current! (" + to_str(stat) + ")"),false);
        }
    }
}

void MainWindow::on_beam_volt_button_clicked()
{
    double entry = ui->beam_volt_edit->toggle_active();
    if(entry >= 0)
    {
        int stat = gyro.set_beam_volt(entry);
        if(stat < 0)
            gui.error_dialog(QString::fromStdString("Error setting beam voltage! (" + to_str(stat) + ")"),false);
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
        gui.error_dialog("Failed to change state! See log for more info.\n",false);
    else
        detect_state_change(true);
}

void MainWindow::on_next_state_button_clicked()
{
    if(gyro.increment_state() < 0)
        gui.error_dialog("Failed to change state! See log for more info.\n",false);
    else
        detect_state_change(true);
}

void MainWindow::on_gtc_curr_button_clicked()
{
    double entry = ui->gtc_curr_edit->toggle_active();
    if(entry >= 0)
    {
        int stat = gyro.set_gtc_curr_limit(entry);
        if(stat < 0)
            gui.error_dialog(QString::fromStdString("Error setting GTC current! (" + to_str(stat) + ")"),false);
    }
}

void MainWindow::on_control_tab_clicked()
{
    ui->control_tab->setStyleSheet(tab_selected);
    ui->plot_tab->setStyleSheet(tab_unselected);
    (gyro.get_fault_status() == 0) ? ui->status_tab->setStyleSheet(tab_unselected) : ui->status_tab->setStyleSheet(tab_blink_off_unselected);
    ui->admin_tab->setStyleSheet(tab_unselected);
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_plot_tab_clicked()
{
    ui->control_tab->setStyleSheet(tab_unselected);
    ui->plot_tab->setStyleSheet(tab_selected);
    (gyro.get_fault_status() == 0) ? ui->status_tab->setStyleSheet(tab_unselected) : ui->status_tab->setStyleSheet(tab_blink_off_unselected);
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
    (gyro.get_fault_status() == 0) ? ui->status_tab->setStyleSheet(tab_selected) : ui->status_tab->setStyleSheet(tab_blink_off_selected);
    ui->admin_tab->setStyleSheet(tab_unselected);
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_admin_tab_clicked()
{
    ui->control_tab->setStyleSheet(tab_unselected);
    ui->plot_tab->setStyleSheet(tab_unselected);
    (gyro.get_fault_status() == 0) ? ui->status_tab->setStyleSheet(tab_unselected) : ui->status_tab->setStyleSheet(tab_blink_off_unselected);
    ui->admin_tab->setStyleSheet(tab_selected);
    ui->stackedWidget->setCurrentIndex(3);
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

void MainWindow::on_time_span_slider_valueChanged(int value)
{
    double secs{-1};
    QString str;

    switch(value)
    {
    case 0: secs = 60; str = "1 min"; break;
    case 10: secs = 300; str = "5 min"; break;
    case 20: secs = 600; str = "10 min"; break;
    case 30: secs = 900; str = "15 min"; break;
    case 40: secs = 1200; str = "20 min"; break;
    case 50: secs = 1500; str = "25 min"; break;
    case 60: secs = 1800; str = "30 min"; break;
    case 70: secs = 2400; str = "40 min"; break;
    case 80: secs = 3000; str = "50 min"; break;
    case 90: secs = 3600; str = "1 hr"; break;
    }

    if(secs > 0)
    {
        gyro.set_plot_span(secs);
        update_plots();
        ui->time_span_group->setTitle("Time Span: " + str);
    }
}

void MainWindow::on_ramp_rate_slider_valueChanged(int value)
{
    double secs{-1};
    QString str;

    switch(value)
    {
    case 0: secs = 60; str = "1 min"; break;
    case 10: secs = 300; str = "5 min"; break;
    case 20: secs = 600; str = "10 min"; break;
    case 30: secs = 900; str = "15 min"; break;
    case 40: secs = 1200; str = "20 min"; break;
    case 50: secs = 1500; str = "25 min"; break;
    case 60: secs = 1800; str = "30 min"; break;
    case 70: secs = 2400; str = "40 min"; break;
    case 80: secs = 3000; str = "50 min"; break;
    case 90: secs = 3600; str = "1 hr"; break;
    }

    if(secs > 0)
    {
        gyro.set_ramp_time(secs);
        ui->time_span_group->setTitle("Ramp Rate: " + str);
    }
}

void MainWindow::on_log_rate_slider_valueChanged(int value)
{
    double secs{-1};
    QString str;

    switch(value)
    {
    case 0: secs = 0; str = "ASAP"; break;
    case 10: secs = 300; str = "5 min"; break;
    case 20: secs = 600; str = "10 min"; break;
    case 30: secs = 900; str = "15 min"; break;
    case 40: secs = 1200; str = "20 min"; break;
    case 50: secs = 1500; str = "25 min"; break;
    case 60: secs = 1800; str = "30 min"; break;
    case 70: secs = 2400; str = "40 min"; break;
    case 80: secs = 3000; str = "50 min"; break;
    case 90: secs = 3600; str = "1 hr"; break;
    }

    if(secs >= 0)
    {
        gyro.set_rec_rate(secs);
        ui->time_span_group->setTitle("Data Log Rate: " + str);
    }
}

void MainWindow::on_cath_recon_button_clicked()
{
    if(!cath_recon_blocked && !all_recon_blocked)
    {
        if(gyro.cath_is_connected() && !gyro.cath_is_enabled())
            gyro.enable_cath();
        else if(!gyro.cath_is_connected())
        {
            ui->cath_recon_button->setText("...");
            cath_recon_blocked = true;
            int stat = gyro.connect_cath();
            if(stat < 0)
            {
                ui->cath_recon_button->setText("❌");
                ui->cath_recon_button->setStyleSheet(red_recon_button);
            }
            else
            {
                ui->cath_recon_button->setText("✔");
                ui->cath_recon_button->setStyleSheet(red_recon_button);
            }
            QTimer::singleShot(3000, this, SLOT(reset_cath_recon()));
        }
    }
}

void MainWindow::on_gtc_recon_button_clicked()
{
    if(!gtc_recon_blocked && !all_recon_blocked)
    {
        if(gyro.gtc_is_connected() && !gyro.gtc_is_enabled())
            gyro.enable_gtc();
        else if(!gyro.gtc_is_connected())
        {
            ui->gtc_recon_button->setText("...");
            gtc_recon_blocked = true;
            int stat = gyro.connect_gtc();
            if(stat < 0)
            {
                ui->gtc_recon_button->setText("❌");
                ui->gtc_recon_button->setStyleSheet(red_recon_button);
            }
            else
            {
                ui->gtc_recon_button->setText("✔");
                ui->gtc_recon_button->setStyleSheet(red_recon_button);
            }
            QTimer::singleShot(3000, this, SLOT(reset_gtc_recon()));
        }
    }
}

void MainWindow::on_spc_recon_button_clicked()
{
    if(!spc_recon_blocked && !all_recon_blocked)
    {
        if(gyro.spc_is_connected() && !gyro.spc_is_enabled())
            gyro.enable_spc();
        else if(!gyro.spc_is_connected())
        {
            ui->spc_recon_button->setText("...");
            spc_recon_blocked = true;
            int stat = gyro.connect_spc();
            if(stat < 0)
            {
                ui->spc_recon_button->setText("❌");
                ui->spc_recon_button->setStyleSheet(red_recon_button);
            }
            else
            {
                ui->spc_recon_button->setText("✔");
                ui->spc_recon_button->setStyleSheet(red_recon_button);
            }
            QTimer::singleShot(3000, this, SLOT(reset_spc_recon()));
        }
    }
}

void MainWindow::on_fms_recon_button_clicked()
{
    if(!fms_recon_blocked && !all_recon_blocked)
    {
        if(gyro.fms_is_connected() && !gyro.fms_is_enabled())
            gyro.enable_fms();
        else if(!gyro.fms_is_connected())
        {
            ui->fms_recon_button->setText("...");
            fms_recon_blocked = true;
            int stat = gyro.connect_fms();
            if(stat < 0)
            {
                ui->fms_recon_button->setText("❌");
                ui->fms_recon_button->setStyleSheet(red_recon_button);
            }
            else
            {
                ui->fms_recon_button->setText("✔");
                ui->fms_recon_button->setStyleSheet(red_recon_button);
            }
            QTimer::singleShot(3000, this, SLOT(reset_fms_recon()));
        }
    }
}

void MainWindow::on_rsi_recon_button_clicked()
{
    if(!rsi_recon_blocked && !all_recon_blocked)
    {
        if(gyro.rsi_is_connected() && !gyro.rsi_is_enabled())
            gyro.enable_rsi();
        else if(!gyro.rsi_is_connected())
        {
            ui->rsi_recon_button->setText("...");
            rsi_recon_blocked = true;
            int stat = gyro.connect_rsi();
            if(stat < 0)
            {
                ui->rsi_recon_button->setText("❌");
                ui->rsi_recon_button->setStyleSheet(red_recon_button);
            }
            else
            {
                ui->rsi_recon_button->setText("✔");
                ui->rsi_recon_button->setStyleSheet(red_recon_button);
            }
            QTimer::singleShot(3000, this, SLOT(reset_rsi_recon()));
        }
    }
}

void MainWindow::on_all_recon_button_clicked()
{
    if(!all_recon_blocked)
    {
        ui->all_recon_button->setText("...");
        all_recon_blocked = true;

        if(!gyro.cath_is_connected()) { QTimer::singleShot(0, this, SLOT(on_cath_recon_button_clicked())); }
        if(!gyro.gtc_is_connected()) { QTimer::singleShot(0, this, SLOT(on_gtc_recon_button_clicked())); }
        if(!gyro.spc_is_connected()) { QTimer::singleShot(0, this, SLOT(on_spc_recon_button_clicked())); }
        if(!gyro.fms_is_connected()) { QTimer::singleShot(0, this, SLOT(on_fms_recon_button_clicked())); }
        if(!gyro.rsi_is_connected()) { QTimer::singleShot(0, this, SLOT(on_rsi_recon_button_clicked())); }

        QTimer::singleShot(3000, this, SLOT(reset_all_recon()));
    }
}

void MainWindow::reset_cath_recon()
{
    cath_recon_blocked = false;
    ui->cath_recon_button->setText("CATHODE");
    ui->cath_recon_button->setStyleSheet(recon_button_normal);
}
void MainWindow::reset_gtc_recon()
{
    gtc_recon_blocked = false;
    ui->gtc_recon_button->setText("GTC");
    ui->gtc_recon_button->setStyleSheet(recon_button_normal);
}
void MainWindow::reset_spc_recon()
{
    spc_recon_blocked = false;
    ui->spc_recon_button->setText("IPC");
    ui->spc_recon_button->setStyleSheet(recon_button_normal);
}
void MainWindow::reset_rsi_recon()
{
    rsi_recon_blocked = false;
    ui->rsi_recon_button->setText("RSI");
    ui->rsi_recon_button->setStyleSheet(recon_button_normal);
}
void MainWindow::reset_fms_recon()
{
    fms_recon_blocked = false;
    ui->fms_recon_button->setText("FMS");
    ui->fms_recon_button->setStyleSheet(recon_button_normal);
}
void MainWindow::reset_all_recon()
{
    all_recon_blocked = false;
    ui->all_recon_button->setText("ALL");
}

void MainWindow::console_print(QString qstr)
{
    qstr = ">> " + qstr;
    ui->console_text->append(qstr);
}

void MainWindow::on_console_edit_returnPressed()
{
    std::string resp;
    int index = ui->console_dropdown->currentIndex();
    QString qcmd = ui->console_edit->text();
    ui->console_edit->clear();
    qApp->setOverrideCursor(Qt::WaitCursor);

    switch(index)
    {
    case 0: resp = gyro.cath_io(qcmd.toStdString()); break;
    case 1: resp = gyro.gtc_io(qcmd.toStdString()); break;
    case 2: resp = gyro.spc_io(qcmd.toStdString()); break;
    case 3: resp = gyro.rsi_io(qcmd.toStdString()); break;
    case 4: resp = gyro.fms_io(qcmd.toStdString()); break;
    }

    console_print(QString::fromStdString(resp));
    qApp->restoreOverrideCursor();
}

void find_swap(std::string &line, std::string title, QLineEdit *field)
{
    if(line.find(title) != std::string::npos)
    {
        std::string old_val = line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1));
        line.replace(line.find(old_val),old_val.length(),field->text().toStdString());
    }
}

void MainWindow::on_save_pid_button_clicked()
{
    ui->save_pid_button->setEnabled(false);
    qApp->setOverrideCursor(Qt::WaitCursor);
    std::string line;
    std::ifstream in_file("../config");
    std::ofstream out_file("../config.temp");

    while(in_file >> line)
    {
        find_swap(line,"Beam Current PID Proportional Constant Kp",ui->beam_kp_edit);
        find_swap(line,"Beam Current PID Integral Constant Ki",ui->beam_ki_edit);
        find_swap(line,"Beam Current PID Derivative Constant Kd",ui->beam_kd_edit);
        find_swap(line,"Power Current PID Proportional Constant Kp",ui->power_kp_edit);
        find_swap(line,"Power Current PID Integral Constant Ki",ui->power_ki_edit);
        find_swap(line,"Power Current PID Derivative Constant Kd",ui->power_kd_edit);
        find_swap(line,"Frequency Current PID Proportional Constant Kp",ui->freq_kp_edit);
        find_swap(line,"Frequency Current PID Integral Constant Ki",ui->freq_ki_edit);
        find_swap(line,"Frequency Current PID Derivative Constant Kd",ui->freq_kd_edit);
        line += "\n";
        out_file << line;
    }

    in_file.close();
    out_file.close();
    remove("../config");
    rename("../config_temp","../config");
    ui->save_pid_button->setEnabled(true);
    qApp->restoreOverrideCursor();
}

void MainWindow::clear_press_data() { gyro.clear_press_data(); }
void MainWindow::clear_beam_data() { gyro.clear_beam_data(); }
void MainWindow::clear_power_data() { gyro.clear_power_data(); }

void MainWindow::press_context_menu(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addAction("Clear Vacuum Data", this, SLOT(clear_press_data()));
    contextMenu.exec(QCursor::pos());
    (void)pos;
}

void MainWindow::beam_context_menu(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addAction("Clear Beam Data", this, SLOT(clear_beam_data()));
    contextMenu.exec(QCursor::pos());
    (void)pos;
}

void MainWindow::power_context_menu(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addAction("Clear Power Data", this, SLOT(clear_power_data()));
    contextMenu.exec(QCursor::pos());
    (void)pos;
}

void MainWindow::update_margins()
{
    if(!this->isMaximized())
    {
        ui->state_group->setStyleSheet(state_group_min);
        ui->control_page->layout()->setSpacing(5);
        ui->control_page->layout()->setContentsMargins(25,25,25,5);
        ui->plot_page->layout()->setSpacing(20);
        ui->plot_page->layout()->setContentsMargins(25,25,25,25);
        ui->status_page->layout()->setSpacing(20);
        ui->status_page->layout()->setContentsMargins(25,25,25,25);
        ui->admin_page->layout()->setSpacing(20);
        ui->admin_page->layout()->setContentsMargins(25,25,25,25);
    }
    else
    {
        ui->state_group->setStyleSheet(state_group_max);
        ui->control_page->layout()->setSpacing(50);
        ui->control_page->layout()->setContentsMargins(50,50,50,50);
        ui->plot_page->layout()->setSpacing(50);
        ui->plot_page->layout()->setContentsMargins(50,50,50,50);
        ui->status_page->layout()->setSpacing(50);
        ui->status_page->layout()->setContentsMargins(50,50,50,50);
        ui->admin_page->layout()->setSpacing(50);
        ui->admin_page->layout()->setContentsMargins(50,50,50,50);
    }
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
void MainWindow::resizeEvent(QResizeEvent* evt)
{ QMainWindow::resizeEvent(evt); /*update_margins();*/ }

void MainWindow::on_close_button_clicked() { this->close(); }
void MainWindow::on_minimize_button_clicked() { this->showMinimized(); }
/*
void MainWindow::on_maximize_button_clicked()
{
    if(this->isMaximized()) this->showNormal();
    else this->showMaximized();
    update_margins();
}
*/
void MainWindow::on_settings_button_clicked()
{
    settings_window settings_popup(&admin_mode,ui->admin_tab,ui->stackedWidget,
                                   &blink_enabled,[=](bool e){set_blink_enabled(e);},this);
    settings_popup.exec();
}

void MainWindow::on_more_button_clicked()
{
    if(ui->beam_params_group->isVisible())
    {
        ui->beam_params_group->setVisible(false);
        ui->settings_frame->setVisible(false);
        ui->storage_frame->setVisible(false);
        ui->clock_frame->setVisible(false);
        ui->more_button->setText("MORE ▼");
        ui->show_more_layout->setContentsMargins(0,0,0,0);
        ui->control_grid->setVerticalSpacing(60);
        ui->control_grid->setHorizontalSpacing(60);
    }
    else
    {
        ui->beam_params_group->setVisible(true);
        ui->settings_frame->setVisible(true);
        ui->storage_frame->setVisible(true);
        ui->clock_frame->setVisible(true);
        ui->more_button->setText("LESS ▲");
        ui->show_more_layout->setContentsMargins(0,0,0,20);
        ui->control_grid->setVerticalSpacing(40);
        ui->control_grid->setHorizontalSpacing(40);
    }
}
