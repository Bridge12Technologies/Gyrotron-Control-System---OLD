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
        shadows.back()->setColor(QColor(30,30,30,30));
        group->setGraphicsEffect(shadows.back());
    }

    set_ui_expanded(false);

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
    QCPMarginGroup *group = new QCPMarginGroup(ui->plot3); // previously used the plot1
    ui->plot1->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ui->plot2->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ui->plot3->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);

    ui->plot1->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->plot2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->plot3->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plot1, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(plot1_context_menu(const QPoint &)));
    connect(ui->plot2, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(plot2_context_menu(const QPoint &)));
    connect(ui->plot3, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(plot3_context_menu(const QPoint &)));

    ui->plot1->setBackground(QBrush(Qt::white));
    ui->plot1->axisRect()->setBackground(Qt::white);
    //ui->plot1->xAxis->setVisible(false);
    ui->plot1->xAxis->setBasePen(Qt::NoPen);
    ui->plot1->xAxis->setTickLabels(false);
    ui->plot1->xAxis->setTickPen(Qt::NoPen);
    ui->plot1->xAxis->setSubTickPen(Qt::NoPen);
    ui->plot1->yAxis->setTickLabelFont(plot_axis_font);
    ui->plot1->yAxis->setLabelFont(small_plot_label_font);
    ui->plot1->yAxis->setLabelColor(QColor(85, 87, 83));
    ui->plot1->yAxis->setBasePen(Qt::NoPen);
    ui->plot1->yAxis->setTickPen(Qt::NoPen);
    ui->plot1->yAxis->setSubTickPen(Qt::NoPen);
    ui->plot1->yAxis->setTickLabelColor(QColor(85, 87, 83));
    ui->plot1->yAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));
    ui->plot1->yAxis->grid()->setSubGridPen(QPen(QColor(200,200,200),1,Qt::DotLine));
    ui->plot1->xAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));

    ui->plot2->setBackground(QBrush(Qt::white));
    ui->plot2->axisRect()->setBackground(Qt::white);
    //ui->plot2->xAxis->setVisible(false);
    ui->plot2->xAxis->setBasePen(Qt::NoPen);
    ui->plot2->xAxis->setTickLabels(false);
    ui->plot2->xAxis->setTickPen(Qt::NoPen);
    ui->plot2->xAxis->setSubTickPen(Qt::NoPen);
    ui->plot2->yAxis->setTickLabelFont(plot_axis_font);
    ui->plot2->yAxis->setLabelFont(small_plot_label_font);
    ui->plot2->yAxis->setLabelColor(QColor(85, 87, 83));
    ui->plot2->yAxis->setBasePen(Qt::NoPen);
    ui->plot2->yAxis->setTickPen(Qt::NoPen);
    ui->plot2->yAxis->setSubTickPen(Qt::NoPen);
    ui->plot2->yAxis->setTickLabelColor(QColor(85, 87, 83));
    ui->plot2->yAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));
    ui->plot2->xAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));

    ui->plot3->setBackground(QBrush(Qt::white));
    ui->plot3->axisRect()->setBackground(Qt::white);
    ui->plot3->xAxis->setTickLabelFont(plot_axis_font);
    ui->plot3->xAxis->setLabelFont(small_plot_label_font);
    ui->plot3->xAxis->setLabelColor(QColor(85, 87, 83));
    ui->plot3->xAxis->setBasePen(Qt::NoPen);
    ui->plot3->xAxis->setTickLabelColor(QColor(85, 87, 83));
    ui->plot3->xAxis->setTickPen(Qt::NoPen);
    ui->plot3->xAxis->setSubTickPen(Qt::NoPen);
    ui->plot3->yAxis->setTickLabelFont(plot_axis_font);
    ui->plot3->yAxis->setLabelFont(small_plot_label_font);
    ui->plot3->yAxis->setLabelColor(QColor(85, 87, 83));
    ui->plot3->yAxis->setBasePen(Qt::NoPen);
    ui->plot3->yAxis->setTickPen(Qt::NoPen);
    ui->plot3->yAxis->setSubTickPen(Qt::NoPen);
    ui->plot3->yAxis->setTickLabelColor(QColor(85, 87, 83));
    ui->plot3->yAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));
    ui->plot3->xAxis->grid()->setPen(QPen(QColor(200,200,200),1,Qt::DotLine));

    // PRESSURE PLOT

    // setup pressure plot
    ui->plot1->addGraph(); // light green line
    ui->plot1->graph(0)->setPen(QPen(QColor(70, 129, 43)));
    ui->plot1->yAxis->setLabel("Vacuum (Torr)");
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->plot1->xAxis->setTicker(timeTicker);
    ui->plot1->yAxis->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    ui->plot1->yAxis->setTicker(logTicker);
    ui->plot1->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
    ui->plot1->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
    ui->plot1->yAxis->grid()->setSubGridVisible(true);
    ui->plot1->yAxis->setRange(1e-8, 2e-6); // default range
    ui->plot1->xAxis->setPadding(10);
    ui->plot1->yAxis->setPadding(10);

    // add label
    plot1_label = new QCPItemText(ui->plot1);
    plot1_label->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
    plot1_label->position->setType(QCPItemPosition::ptAxisRectRatio);
    plot1_label->position->setCoords(0.99, 0.01);
    plot1_label->setText("");
    plot1_label->setFont(plot_big_label_font);

    //ui->plot1->setInteraction(QCP::iRangeDrag, true); // enable plot dragging
    //ui->plot1->setInteraction(QCP::iRangeZoom, true); // enable plot zoom

    // setup beam plot
    ui->plot2->addGraph(); // light green line
    ui->plot2->graph(0)->setPen(QPen(QColor(70, 129, 43)));
    ui->plot2->addGraph(); // gray line
    ui->plot2->graph(1)->setPen(QPen(QColor(180, 180, 180)));
    ui->plot2->yAxis->setLabel("Beam Current (mA)");

    // set axes
    ui->plot2->xAxis->setTicker(timeTicker);
    ui->plot2->yAxis->setRange(0, 30); // default range
    ui->plot2->xAxis->setPadding(10);
    ui->plot2->yAxis->setPadding(10);

    // add label
    plot2_label = new QCPItemText(ui->plot2);
    plot2_label->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
    plot2_label->position->setType(QCPItemPosition::ptAxisRectRatio);
    plot2_label->position->setCoords(0.99, 0.01);
    plot2_label->setText("");
    plot2_label->setFont(plot_big_label_font);

    //ui->plot2->setInteraction(QCP::iRangeDrag, true); // enable plot dragging
    //ui->plot2->setInteraction(QCP::iRangeZoom, true); // enable plot zoom

    // setup power plot
    ui->plot3->addGraph(); // light green line -> power in W
    ui->plot3->graph(0)->setPen(QPen(QColor(70, 129, 43)));
    ui->plot3->addGraph(); // grey line -> power in W setpoint
    ui->plot3->graph(1)->setPen(QPen(QColor(180, 180, 180)));
    ui->plot3->xAxis->setLabel("Run Time (h:m:s)");
    ui->plot3->yAxis->setLabel("Voltage (mV)");
    ui->plot3->yAxis->setNumberPrecision(3);
    ui->plot3->yAxis->setNumberFormat("g"); // display numbers in short scientific format or fixed, whichever is shorter

    // set axes
    ui->plot3->xAxis->setTicker(timeTicker);
    ui->plot3->yAxis->setRange(0, 5);
    ui->plot3->xAxis->setPadding(10);
    ui->plot3->yAxis->setPadding(10);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->plot3->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot3->xAxis2, SLOT(setRange(QCPRange)));
    //connect(ui->plot3->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot3->yAxis2, SLOT(setRange(QCPRange)));

    // add label for power in mV
    plot3_label = new QCPItemText(ui->plot3);
    plot3_label->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
    plot3_label->position->setType(QCPItemPosition::ptAxisRectRatio);
    plot3_label->position->setCoords(0.99, 0.01);
    plot3_label->setText("");
    plot3_label->setFont(plot_big_label_font);

    // force redraw
    ui->plot2->replot();
    ui->plot1->replot();
    ui->plot3->replot();
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
        if(showing_more) ui->state_frame->setStyleSheet(sm_red_state_frame);
        else ui->state_frame->setStyleSheet(lg_red_state_frame);
        ui->prev_state_label->setEnabled(false);
        ui->next_state_label->setEnabled(false);
        ui->prev_state_button->setStyleSheet(showing_more ? sm_prev_arrow : lg_prev_arrow);
        ui->prev_state_button->setText("");
        ui->next_state_button->setStyleSheet(showing_more ? sm_next_arrow : lg_next_arrow);
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
            if(showing_more) ui->state_frame->setStyleSheet(sm_grey_state_frame);
            else ui->state_frame->setStyleSheet(lg_grey_state_frame);
            ui->prev_state_label->setEnabled(false);
            ui->next_state_label->setEnabled(true);
            ui->next_state_label->setText("Warm Up →");
            ui->prev_state_button->setStyleSheet(showing_more ? sm_prev_arrow : lg_prev_arrow);
            ui->prev_state_button->setText("");
            ui->next_state_button->setStyleSheet(showing_more ? sm_next_arrow : lg_next_arrow);
            ui->next_state_button->setText("");
            ui->state_mini_label->setText("MW Off");
            break;
        case 1:
            ui->prev_state_label->setEnabled(true);
            ui->next_state_label->setEnabled(true);
            if(gyro.is_paused())
            {
                if(showing_more) ui->state_frame->setStyleSheet(sm_grey_state_frame);
                else ui->state_frame->setStyleSheet(lg_grey_state_frame);

                if(ramping_up)
                {
                    ui->state_label->setText("WARM UP\n(PAUSED)");
                    ui->small_state_label->setText("WARM UP");
                    ui->prev_state_button->setStyleSheet(showing_more ? sm_prev_arrow : lg_prev_arrow);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(showing_more ? sm_next_arrow : lg_next_arrow);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Cool Down");
                    ui->next_state_label->setText("Resume Warm Up →");
                    ui->state_mini_label->setText(QString::number(100*(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT)) + "%");
                }
                else if(ramping_down)
                {
                    ui->state_label->setText("COOL DOWN\n(PAUSED)");
                    ui->small_state_label->setText("COOL DOWN");
                    ui->prev_state_button->setStyleSheet(showing_more ? sm_prev_arrow : lg_prev_arrow);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(showing_more ? sm_next_arrow : lg_next_arrow);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Resume Cool Down");
                    ui->next_state_label->setText("Warm Up →");
                    ui->state_mini_label->setText(QString::number(100*(1-(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT))) + "%");
                }
            }
            else
            {
                if(showing_more) ui->state_frame->setStyleSheet(sm_orange_state_frame);
                else ui->state_frame->setStyleSheet(lg_orange_state_frame);

                if(ramping_up)
                {
                    ui->state_label->setText("WARM UP");
                    ui->small_state_label->setText("WARM UP");
                    ui->prev_state_button->setStyleSheet(showing_more ? sm_pause_bubble : lg_pause_bubble);
                    ui->prev_state_button->setText("II");
                    ui->next_state_button->setStyleSheet(showing_more ? sm_next_arrow : lg_next_arrow);
                    ui->next_state_button->setText("");
                    ui->prev_state_label->setText("← Pause");
                    ui->next_state_label->setText("HV Standby →");
                    ui->state_mini_label->setText(QString::number(100*(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT)) + "%");
                }
                else if(ramping_down)
                {
                    ui->state_label->setText("COOL DOWN");
                    ui->small_state_label->setText("COOL DOWN");
                    ui->prev_state_button->setStyleSheet(showing_more ? sm_prev_arrow : lg_prev_arrow);
                    ui->prev_state_button->setText("");
                    ui->next_state_button->setStyleSheet(showing_more ? sm_pause_bubble : lg_pause_bubble);
                    ui->next_state_button->setText("II");
                    ui->prev_state_label->setText("← Control Power On");
                    ui->next_state_label->setText("Pause →");
                    ui->state_mini_label->setText(QString::number(100*(1-(gyro.get_fil_curr()/gyro.FIL_CURR_LIMIT))) + "%");
                }
            }
            break;
        case 2:
            ui->prev_state_button->setStyleSheet(showing_more ? sm_prev_arrow : lg_prev_arrow);
            ui->prev_state_button->setText("");
            ui->next_state_button->setStyleSheet(showing_more ? sm_next_arrow : lg_next_arrow);
            ui->next_state_button->setText("");
            ui->prev_state_label->setEnabled(true);
            ui->next_state_label->setEnabled(true);
            ui->state_label->setText("HV STANDBY");
            ui->small_state_label->setText("HV STANDBY");
            if(showing_more) ui->state_frame->setStyleSheet(sm_grey_state_frame);
            else ui->state_frame->setStyleSheet(lg_grey_state_frame);
            ui->prev_state_label->setText("← Cool Down");
            ui->next_state_label->setText("MW On →");
            ui->state_mini_label->setText("MW Off");
            break;
        case 3:
            ui->state_label->setText("MW ON ⚡");
            ui->small_state_label->setText("MW ON");
            ui->prev_state_label->setEnabled(true);
            ui->next_state_label->setEnabled(false);
            if(showing_more) ui->state_frame->setStyleSheet(sm_green_state_frame);
            else ui->state_frame->setStyleSheet(lg_green_state_frame);
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
    double plot_max, plot_min, plot_max_bound{0}, plot_min_bound{0};
    QVector<double> plot_data, plot_sp_data, plot_time_data;
    QCustomPlot* plot[3] = {ui->plot1,ui->plot2,ui->plot3};
    QCPItemText* label[3] = {plot1_label,plot2_label,plot3_label};
    QString selection[3] = {ui->plot1_dropdown->currentText(),
                            ui->plot1_dropdown->currentText(),
                            ui->plot1_dropdown->currentText()};

    for(int i=0; i<3; i++)
    {
        if(selection[i] == "VACUUM")
        {
            plot_data = QVector<double>::fromStdVector(gyro.get_press_data());
            plot_time_data = QVector<double>::fromStdVector(gyro.get_press_time_data());
            if(gyro.spc_available()) label[i]->setText(QString::number(gyro.get_pressure()) + QString(" Torr"));
            else label[i]->setText("D/C");
            plot_max = *std::max_element(plot_data.constBegin(), plot_data.constEnd());
            plot_min = *std::min_element(plot_data.constBegin(), plot_data.constEnd());
            plot_max_bound = pow(10,ceil(log10(plot_max)));
            plot_min_bound = pow(10,floor(log10(plot_min)));
        }
        else if(selection[i] == "BEAM CURRENT")
        {
            plot_data = QVector<double>::fromStdVector(gyro.get_beam_curr_data());
            plot_sp_data = QVector<double>::fromStdVector(gyro.get_beam_curr_sp_data());
            plot_time_data = QVector<double>::fromStdVector(gyro.get_beam_curr_time_data());
            if(gyro.cath_available()) label[i]->setText(QString::number(gyro.get_beam_curr(),'f',2) + QString(" mA"));
            else label[i]->setText("D/C");
            plot_max = *std::max_element(plot_data.constBegin(), plot_data.constEnd());
            plot_min = *std::min_element(plot_data.constBegin(), plot_data.constEnd());
            plot_max_bound = plot_max+((plot_max-plot_min)*0.1);
            plot_min_bound = plot_min-((plot_max-plot_min)*0.1);
            if(plot_min_bound < 0) plot_min_bound = 0;
        }
        else if(selection[i] == "BEAM VOLTAGE")
        {
            plot_data = QVector<double>::fromStdVector(gyro.get_beam_volt_data());
            plot_sp_data = QVector<double>::fromStdVector(gyro.get_beam_volt_sp_data());
            plot_time_data = QVector<double>::fromStdVector(gyro.get_beam_volt_time_data());
            if(gyro.cath_available()) label[i]->setText(QString::number(gyro.get_beam_volt(),'f',2) + QString(" V"));
            else label[i]->setText("D/C");
            plot_max = *std::max_element(plot_data.constBegin(), plot_data.constEnd());
            plot_min = *std::min_element(plot_data.constBegin(), plot_data.constEnd());
            plot_max_bound = plot_max+((plot_max-plot_min)*0.1);
            plot_min_bound = plot_min-((plot_max-plot_min)*0.1);
            if(plot_min_bound < 0) plot_min_bound = 0;
        }
        else if(selection[i] == "POWER")
        {
            plot_data = QVector<double>::fromStdVector(gyro.get_power_data());
            plot_sp_data = QVector<double>::fromStdVector(gyro.get_power_sp_data());
            plot_time_data = QVector<double>::fromStdVector(gyro.get_power_time_data());
            if(gyro.rsi_available()) label[i]->setText(QString::number(gyro.get_power()) + QString(" W"));
            else label[i]->setText("D/C");
            plot_max = *std::max_element(plot_data.constBegin(), plot_data.constEnd());
            plot_min = *std::min_element(plot_data.constBegin(), plot_data.constEnd());
            plot_max_bound = (floor(plot_max/5)+1)*5;
            plot_min_bound = (floor(plot_min/5)-1)*5;
            if(plot_min_bound < 0) plot_min_bound = 0;
        }
        else if(selection[i] == "FIL. CURRENT")
        {
            plot_data = QVector<double>::fromStdVector(gyro.get_fil_curr_data());
            plot_sp_data = QVector<double>::fromStdVector(gyro.get_fil_curr_sp_data());
            plot_time_data = QVector<double>::fromStdVector(gyro.get_fil_curr_time_data());
            if(gyro.cath_available()) label[i]->setText(QString::number(gyro.get_fil_curr(),'f',2) + QString(" mA"));
            else label[i]->setText("D/C");
            plot_max = *std::max_element(plot_data.constBegin(), plot_data.constEnd());
            plot_min = *std::min_element(plot_data.constBegin(), plot_data.constEnd());
            plot_max_bound = plot_max+((plot_max-plot_min)*0.1);
            plot_min_bound = plot_min-((plot_max-plot_min)*0.1);
            if(plot_min_bound < 0) plot_min_bound = 0;
        }
        else if(selection[i] == "FREQUENCY")
        {
            plot_data = QVector<double>::fromStdVector(gyro.get_freq_data());
            plot_sp_data = QVector<double>::fromStdVector(gyro.get_freq_sp_data());
            plot_time_data = QVector<double>::fromStdVector(gyro.get_freq_time_data());
            if(gyro.cath_available()) label[i]->setText(QString::number(gyro.get_freq(),'f',2) + QString(" GHz"));
            else label[i]->setText("D/C");
            plot_max = *std::max_element(plot_data.constBegin(), plot_data.constEnd());
            plot_min = *std::min_element(plot_data.constBegin(), plot_data.constEnd());
            plot_max_bound = plot_max+((plot_max-plot_min)*0.1);
            plot_min_bound = plot_min-((plot_max-plot_min)*0.1);
            if(plot_min_bound < 0) plot_min_bound = 0;
        }

        plot[i]->graph(0)->setData(plot_time_data, plot_data, true);
        if(selection[i] != "VACUUM") plot[i]->graph(1)->setData(plot_time_data, plot_sp_data, true);
        plot[i]->xAxis->setRange(key, plot_span, Qt::AlignRight);
        plot[i]->yAxis->setRange(plot_min_bound,plot_max_bound);
        if(ui->stackedWidget->currentIndex() == 1) plot[i]->replot();
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
           ui->percent_full_label->setStyleSheet("border: none; background: transparent; color: #D16055;");
        else
           ui->percent_full_label->setStyleSheet("border: none; background: transparent; color: rgb(85,87,83);");
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
        ui->temp_indicator->setEnabled(true);
        ui->flow_indicator->setEnabled(true);
        switch(gyro.get_temp_status())
        {
        case 0: ui->temp_indicator->setStyleSheet(showing_more ? sm_green_bubble : lg_green_bubble); ui->temp_status->setText("OK"); break;
        case -1: ui->temp_indicator->setStyleSheet(showing_more ? sm_yellow_bubble : lg_yellow_bubble); ui->temp_status->setText("WARN"); break;
        case -2: ui->temp_indicator->setStyleSheet(showing_more ? sm_red_bubble : lg_red_bubble); ui->temp_status->setText("HIGH!"); break;
        }
        switch(gyro.get_flow_status())
        {
        case 0: ui->flow_indicator->setStyleSheet(showing_more ? sm_green_bubble : lg_green_bubble); ui->flow_status->setText("OK"); break;
        case -1: ui->flow_indicator->setStyleSheet(showing_more ? sm_yellow_bubble : lg_yellow_bubble); ui->flow_status->setText("WARN"); break;
        case -2: ui->flow_indicator->setStyleSheet(showing_more ? sm_red_bubble : lg_red_bubble); ui->flow_status->setText("LOW!"); break;
        }
    }
    else
    {
        ui->temp_indicator->setEnabled(false);
        ui->flow_indicator->setEnabled(false);
        ui->temp_indicator->setStyleSheet(showing_more ? sm_green_bubble : lg_green_bubble);
        ui->flow_indicator->setStyleSheet(showing_more ? sm_green_bubble : lg_green_bubble);
        ui->temp_status->setText("N/A");
        ui->flow_status->setText("N/A");
    }
    switch(gyro.get_fault_status())
    {
    case 0: ui->faults_indicator->setStyleSheet(showing_more ? sm_green_bubble : lg_green_bubble); ui->fault_status->setText("OK"); break;
    case -1: ui->faults_indicator->setStyleSheet(showing_more ? sm_yellow_bubble : lg_yellow_bubble); ui->fault_status->setText("WARN ×" + QString::number(gyro.get_num_warnings())); break;
    case -2: ui->faults_indicator->setStyleSheet(showing_more ? sm_red_bubble : lg_red_bubble); ui->fault_status->setText("ERR ×" + QString::number(gyro.get_num_errors())); break;
    }
    if(gyro.spc_available())
    {
        ui->press_indicator->setEnabled(true);
        ui->press_label->setText(QString::number(gyro.get_pressure()));
        switch(gyro.get_press_status())
        {
        case 0: ui->press_indicator->setStyleSheet(showing_more ? sm_green_bubble : lg_green_bubble); break;
        case -1: ui->press_indicator->setStyleSheet(showing_more ? sm_yellow_bubble : lg_yellow_bubble); break;
        case -2: ui->press_indicator->setStyleSheet(showing_more ? sm_red_bubble : lg_red_bubble); break;
        }
    }
    else
    {
        ui->press_indicator->setEnabled(false);
        ui->press_indicator->setStyleSheet(showing_more ? sm_green_bubble : lg_green_bubble);
        ui->press_label->setText("N/A");
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
    if((!gyro.cath_is_connected() || !gyro.cath_is_enabled()) && (!gyro.gtc_is_connected() || !gyro.gtc_is_enabled()))
        ui->beam_params_group->setEnabled(false);
    else
        ui->beam_params_group->setEnabled(true);

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
        ui->gtc_curr_button->setVisible(false);
    }
    else
        ui->gtc_curr_button->setVisible(true);
    if(gyro.rsi_is_connected() && gyro.rsi_is_enabled())
    {
        ui->power_group->setEnabled(true);
        ui->power_button->setVisible(true);
    }
    else
    {
        ui->power_group->setEnabled(false);
        ui->power_button->setVisible(false);
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

void MainWindow::on_enable_button_clicked()
{
    if(gyro.pid_is_on())
    {
        gyro.turn_off_pid();
        ui->enable_button->setStyleSheet(gui.grey_button(60));
        ui->enable_button->setText("ENABLE");
        ui->pid_dropdown->setEnabled(true);
        ui->pid_group->setTitle("Auto Control");

        if(ui->pid_dropdown->currentText() == "POWER")
        {
            ui->power_button->setEnabled(false);
            ui->small_pid_label->setText("NONE");
            ui->fil_curr_button->setEnabled(true);
            ui->fil_curr_button->setText("✎");
            ui->fil_curr_button->setFont(button_font);
            ui->fil_curr_button->setStyleSheet(green_edit_button);
            gyro.log_event("automatic power control disabled");
        }
        else if(ui->pid_dropdown->currentText() == "CURRENT")
        {
            ui->beam_curr_button->setEnabled(false);
            ui->small_pid_label->setText("NONE");
            ui->fil_curr_button->setEnabled(true);
            ui->fil_curr_button->setText("✎");
            ui->fil_curr_button->setFont(button_font);
            ui->fil_curr_button->setStyleSheet(green_edit_button);
            gyro.log_event("automatic beam current control disabled");
        }
        else if(ui->pid_dropdown->currentText() == "FREQUENCY")
        {
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
    else
    {
        ui->enable_button->setStyleSheet(gui.orange_button(60));
        ui->enable_button->setText("DISABLE");
        ui->pid_dropdown->setEnabled(false);
        ui->pid_group->setTitle("Auto Control [ON]");

        if(ui->pid_dropdown->currentText() == "POWER")
        {
            gyro.toggle_power_pid(true);
            ui->power_button->setEnabled(true);
            ui->small_pid_label->setText("POWER");
            ui->fil_curr_button->setEnabled(false);
            ui->fil_curr_button->setText("set: ");
            ui->fil_curr_button->setFont(small_button_font);
            ui->fil_curr_button->setStyleSheet(orange_pid_button);
            gyro.log_event("automatic power control enabled");
        }
        else if(ui->pid_dropdown->currentText() == "CURRENT")
        {
            gyro.toggle_beam_pid(true);
            ui->beam_curr_button->setEnabled(true);
            ui->small_pid_label->setText("CURRENT");
            ui->fil_curr_button->setEnabled(false);
            ui->fil_curr_button->setText("set: ");
            ui->fil_curr_button->setFont(small_button_font);
            ui->fil_curr_button->setStyleSheet(orange_pid_button);
            gyro.log_event("automatic beam current control enabled");
        }
        else if(ui->pid_dropdown->currentText() == "FREQUENCY")
        {
            gyro.toggle_freq_pid(true);
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
    ui->plot1->replot();
    ui->plot2->replot();
    ui->plot3->replot();
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

void MainWindow::clear_plot1_data()
{
    if(ui->plot1_dropdown->currentText() == "VACUUM")
        gyro.clear_press_data();
    else if(ui->plot1_dropdown->currentText() == "BEAM CURRENT")
        gyro.clear_beam_curr_data();
    else if(ui->plot1_dropdown->currentText() == "BEAM VOLTAGE")
        gyro.clear_beam_volt_data();
    else if(ui->plot1_dropdown->currentText() == "POWER")
        gyro.clear_power_data();
    else if(ui->plot1_dropdown->currentText() == "FIL. CURRENT")
        gyro.clear_fil_curr_data();
    else if(ui->plot1_dropdown->currentText() == "FREQUENCY")
        gyro.clear_freq_data();
}

void MainWindow::clear_plot2_data()
{
    if(ui->plot2_dropdown->currentText() == "VACUUM")
        gyro.clear_press_data();
    else if(ui->plot2_dropdown->currentText() == "BEAM CURRENT")
        gyro.clear_beam_curr_data();
    else if(ui->plot2_dropdown->currentText() == "BEAM VOLTAGE")
        gyro.clear_beam_volt_data();
    else if(ui->plot2_dropdown->currentText() == "POWER")
        gyro.clear_power_data();
    else if(ui->plot2_dropdown->currentText() == "FIL. CURRENT")
        gyro.clear_fil_curr_data();
    else if(ui->plot2_dropdown->currentText() == "FREQUENCY")
        gyro.clear_freq_data();
}

void MainWindow::clear_plot3_data()
{
    if(ui->plot3_dropdown->currentText() == "VACUUM")
        gyro.clear_press_data();
    else if(ui->plot3_dropdown->currentText() == "BEAM CURRENT")
        gyro.clear_beam_curr_data();
    else if(ui->plot3_dropdown->currentText() == "BEAM VOLTAGE")
        gyro.clear_beam_volt_data();
    else if(ui->plot3_dropdown->currentText() == "POWER")
        gyro.clear_power_data();
    else if(ui->plot3_dropdown->currentText() == "FIL. CURRENT")
        gyro.clear_fil_curr_data();
    else if(ui->plot3_dropdown->currentText() == "FREQUENCY")
        gyro.clear_freq_data();
}

void MainWindow::plot1_context_menu(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addAction("Clear Vacuum Data", this, SLOT(clear_plot1_data()));
    contextMenu.exec(QCursor::pos());
    (void)pos;
}

void MainWindow::plot2_context_menu(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addAction("Clear Beam Data", this, SLOT(clear_plot2_data()));
    contextMenu.exec(QCursor::pos());
    (void)pos;
}

void MainWindow::plot3_context_menu(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addAction("Clear Power Data", this, SLOT(clear_plot3_data()));
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
                                   &blink_enabled,[=](bool e){set_blink_enabled(e);},
                                   &use_24hr_format, this);
    settings_popup.exec();
}

void MainWindow::set_ui_expanded(bool expand)
{
    if(expand)
    {
        showing_more = true;
        ui->beam_params_group->setVisible(true);
        ui->settings_frame->setVisible(true);
        ui->storage_frame->setVisible(true);
        ui->clock_frame->setVisible(true);
        ui->more_button->setText("LESS ▲");
        ui->show_more_layout->setContentsMargins(0,0,0,20);
        ui->control_grid->setVerticalSpacing(50);
        ui->control_grid->setHorizontalSpacing(50);
        ui->control_page->layout()->setContentsMargins(50,50,50,15);
        //ui->state_frame->setContentsMargins(9,40,9,40);
        ui->state_top_label->setStyleSheet("background: transparent; font-size: 14pt; color: white;");
        ui->state_mini_label->setStyleSheet("background: transparent; font-size: 14pt; color: white;");
        ui->state_label->setStyleSheet("QLabel { color: white; background: none; font-size: 20pt; }"
                                       "QLabel:disabled { color: rgb(180,180,180); }");
        //ui->temp_indicator->setContentsMargins(9,24,9,24);
        //ui->flow_indicator->setContentsMargins(9,24,9,24);
        //ui->faults_indicator->setContentsMargins(9,24,9,24);
        //ui->press_indicator->setContentsMargins(9,24,9,24);
        ui->temp_indicator->layout()->setSpacing(8);
        ui->flow_indicator->layout()->setSpacing(8);
        ui->faults_indicator->layout()->setSpacing(8);
        ui->press_indicator->layout()->setSpacing(8);
        ui->temp_indicator->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        ui->flow_indicator->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        ui->faults_indicator->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        ui->press_indicator->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        ui->state_frame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    }
    else
    {
        showing_more = false;
        ui->beam_params_group->setVisible(false);
        ui->settings_frame->setVisible(false);
        ui->storage_frame->setVisible(false);
        ui->clock_frame->setVisible(false);
        ui->more_button->setText("MORE ▼");
        ui->show_more_layout->setContentsMargins(0,0,0,18);
        ui->control_grid->setVerticalSpacing(60);
        ui->control_grid->setHorizontalSpacing(60);
        ui->control_page->layout()->setContentsMargins(60,60,60,15);
        //ui->state_frame->setContentsMargins(9,50,9,50);
        ui->state_top_label->setStyleSheet("background: transparent; font-size: 16pt; color: white; margin-top: 10px;");
        ui->state_mini_label->setStyleSheet("background: transparent; font-size: 16pt; color: white; margin-bottom: 10px;");
        ui->state_label->setStyleSheet("QLabel { color: white; background: none; font-size: 26pt; }"
                                       "QLabel:disabled { color: rgb(180,180,180); }");
        //ui->temp_indicator->setContentsMargins(9,30,9,40);
        //ui->flow_indicator->setContentsMargins(9,30,9,40);
        //ui->faults_indicator->setContentsMargins(9,30,9,40);
        //ui->press_indicator->setContentsMargins(9,30,9,40);
        ui->temp_indicator->layout()->setSpacing(0);
        ui->flow_indicator->layout()->setSpacing(0);
        ui->faults_indicator->layout()->setSpacing(0);
        ui->press_indicator->layout()->setSpacing(0);
        ui->temp_indicator->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
        ui->flow_indicator->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
        ui->faults_indicator->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
        ui->press_indicator->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
        ui->state_frame->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    }
    update_indicators();
    detect_state_change(true);
}

void MainWindow::on_more_button_clicked()
{
    if(showing_more)
        set_ui_expanded(false);
    else
        set_ui_expanded(true);
}

void MainWindow::on_plot1_dropdown_currentIndexChanged(const QString &item)
{
    if(item == "VACUUM")
    {
        ui->plot1->yAxis->setLabel("Vacuum (Torr)");
        ui->plot1->yAxis->setNumberFormat("eb");
        ui->plot1->yAxis->setNumberPrecision(0);
    }
    else if(item == "BEAM CURRENT")
    {
        ui->plot1->yAxis->setLabel("Beam Current (mA)");
        ui->plot1->yAxis->setNumberFormat("g");
        ui->plot1->yAxis->setNumberPrecision(3);
    }
    else if(item == "BEAM VOLTAGE")
    {
        ui->plot1->yAxis->setLabel("Beam Voltage (kV)");
        ui->plot1->yAxis->setNumberFormat("g");
        ui->plot1->yAxis->setNumberPrecision(3);
    }
    else if(item == "POWER")
    {
        ui->plot1->yAxis->setLabel("Power (W)");
        ui->plot1->yAxis->setNumberFormat("g");
        ui->plot1->yAxis->setNumberPrecision(3);
    }
    else if(item == "FIL. CURRENT")
    {
        ui->plot1->yAxis->setLabel("Filament Current (mA)");
        ui->plot1->yAxis->setNumberFormat("g");
        ui->plot1->yAxis->setNumberPrecision(3);
    }
    else if(item == "FREQUENCY")
    {
        ui->plot1->yAxis->setLabel("Frequency (GHz)");
        ui->plot1->yAxis->setNumberFormat("g");
        ui->plot1->yAxis->setNumberPrecision(3);
    }
    update_plots();
}

void MainWindow::on_plot2_dropdown_currentIndexChanged(const QString &item)
{
    if(item == "VACUUM")
    {
        ui->plot2->yAxis->setLabel("Vacuum (Torr)");
        ui->plot2->yAxis->setNumberFormat("eb");
        ui->plot2->yAxis->setNumberPrecision(0);
    }
    else if(item == "BEAM CURRENT")
    {
        ui->plot2->yAxis->setLabel("Beam Current (mA)");
        ui->plot2->yAxis->setNumberFormat("g");
        ui->plot2->yAxis->setNumberPrecision(3);
    }
    else if(item == "BEAM VOLTAGE")
    {
        ui->plot2->yAxis->setLabel("Beam Voltage (kV)");
        ui->plot2->yAxis->setNumberFormat("g");
        ui->plot2->yAxis->setNumberPrecision(3);
    }
    else if(item == "POWER")
    {
        ui->plot2->yAxis->setLabel("Power (W)");
        ui->plot2->yAxis->setNumberFormat("g");
        ui->plot2->yAxis->setNumberPrecision(3);
    }
    else if(item == "FIL. CURRENT")
    {
        ui->plot2->yAxis->setLabel("Filament Current (mA)");
        ui->plot2->yAxis->setNumberFormat("g");
        ui->plot2->yAxis->setNumberPrecision(3);
    }
    else if(item == "FREQUENCY")
    {
        ui->plot2->yAxis->setLabel("Frequency (GHz)");
        ui->plot2->yAxis->setNumberFormat("g");
        ui->plot2->yAxis->setNumberPrecision(3);
    }
    update_plots();
}

void MainWindow::on_plot3_dropdown_currentIndexChanged(const QString &item)
{
    if(item == "VACUUM")
    {
        ui->plot3->yAxis->setLabel("Vacuum (Torr)");
        ui->plot3->yAxis->setNumberFormat("eb");
        ui->plot3->yAxis->setNumberPrecision(0);
    }
    else if(item == "BEAM CURRENT")
    {
        ui->plot3->yAxis->setLabel("Beam Current (mA)");
        ui->plot3->yAxis->setNumberFormat("g");
        ui->plot3->yAxis->setNumberPrecision(3);
    }
    else if(item == "BEAM VOLTAGE")
    {
        ui->plot3->yAxis->setLabel("Beam Voltage (kV)");
        ui->plot3->yAxis->setNumberFormat("g");
        ui->plot3->yAxis->setNumberPrecision(3);
    }
    else if(item == "POWER")
    {
        ui->plot3->yAxis->setLabel("Power (W)");
        ui->plot3->yAxis->setNumberFormat("g");
        ui->plot3->yAxis->setNumberPrecision(3);
    }
    else if(item == "FIL. CURRENT")
    {
        ui->plot3->yAxis->setLabel("Filament Current (mA)");
        ui->plot3->yAxis->setNumberFormat("g");
        ui->plot3->yAxis->setNumberPrecision(3);
    }
    else if(item == "FREQUENCY")
    {
        ui->plot3->yAxis->setLabel("Frequency (GHz)");
        ui->plot3->yAxis->setNumberFormat("g");
        ui->plot3->yAxis->setNumberPrecision(3);
    }
    update_plots();
}
