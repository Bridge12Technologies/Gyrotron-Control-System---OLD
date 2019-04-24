#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QApplication::setFont(main_font);
    setWindowTitle("B12T Gyrotron Control System");

    rigol1_enabled = rigol2_enabled = true;
    labjack_enabled = true;
    spc_enabled = true;
    fms_enabled = false;

    initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::shutdown()
{
    // close all device connections cleanly before exiting

    cout << "good bye\n";
    action_log_write("exiting program");

    data_timer.stop();

    data_log_stream.close();
    action_log_stream.close();

    if(dxm_enabled)
    {
        status = eth_disconnect(&::ED_DXM);
        if(status < 0) cout << "could not disconnect from DXM\n";
    }
    if(slm_enabled)
    {
        status = eth_disconnect(&::ED_SLM);
        if(status < 0) cout << "could not disconnect from SLM\n";
    }
    if(rigol1_enabled || rigol2_enabled)
    {
        status = eth_disconnect(&::ED_RIGOL);
        if(status < 0) cout << "could not disconnect from Rigol\n";
    }
    if(spc_enabled)
    {
        status = eth_disconnect(&::ED_SPCE);
        if(status < 0) cout << "could not disconnect from SPCe\n";
    }
    if(labjack_enabled)
    {
        status = daq.lj_close();
        if(status < 0) cout << "could not disconnect from LabJack\n";
    }
    if(fms_enabled)
    {
        status = eth_disconnect(&::ED_FMS);
        if(status < 0) cout << "could not disconnect from FMS\n";
    }
}

void MainWindow::initialize()
{
    cout << "initializing...\n";

    QGraphicsDropShadowEffect *effect1 = new QGraphicsDropShadowEffect;
    effect1->setBlurRadius(15); effect1->setXOffset(0); effect1->setYOffset(0); effect1->setColor(QColor(210,210,210));
    ui->dxm_group->setGraphicsEffect(effect1);

    QGraphicsDropShadowEffect *effect2 = new QGraphicsDropShadowEffect;
    effect2->setBlurRadius(15); effect2->setXOffset(0); effect2->setYOffset(0); effect2->setColor(QColor(210,210,210));
    ui->slm_group->setGraphicsEffect(effect2);

    QGraphicsDropShadowEffect *effect3 = new QGraphicsDropShadowEffect;
    effect3->setBlurRadius(15); effect3->setXOffset(0); effect3->setYOffset(0); effect3->setColor(QColor(210,210,210));
    ui->fms_group->setGraphicsEffect(effect3);

    QGraphicsDropShadowEffect *effect4 = new QGraphicsDropShadowEffect;
    effect4->setBlurRadius(15); effect4->setXOffset(0); effect4->setYOffset(0); effect4->setColor(QColor(210,210,210));
    ui->lj_curr_group->setGraphicsEffect(effect4);

    QGraphicsDropShadowEffect *effect5 = new QGraphicsDropShadowEffect;
    effect5->setBlurRadius(15); effect5->setXOffset(0); effect5->setYOffset(0); effect5->setColor(QColor(210,210,210));
    ui->pid_group->setGraphicsEffect(effect5);

    QGraphicsDropShadowEffect *effect6 = new QGraphicsDropShadowEffect;
    effect6->setBlurRadius(15); effect6->setXOffset(0); effect6->setYOffset(0); effect6->setColor(QColor(210,210,210));
    ui->rigol1_group->setGraphicsEffect(effect6);

    QGraphicsDropShadowEffect *effect7 = new QGraphicsDropShadowEffect;
    effect7->setBlurRadius(15); effect7->setXOffset(0); effect7->setYOffset(0); effect7->setColor(QColor(210,210,210));
    ui->rigol2_group->setGraphicsEffect(effect7);

    QGraphicsDropShadowEffect *effect8 = new QGraphicsDropShadowEffect;
    effect8->setBlurRadius(15); effect8->setXOffset(0); effect8->setYOffset(0); effect8->setColor(QColor(210,210,210));
    ui->pid_const_group->setGraphicsEffect(effect8);

    QGraphicsDropShadowEffect *effect9 = new QGraphicsDropShadowEffect;
    effect9->setBlurRadius(15); effect9->setXOffset(0); effect9->setYOffset(0); effect9->setColor(QColor(210,210,210));
    ui->ramp_group->setGraphicsEffect(effect9);

    QGraphicsDropShadowEffect *effect10 = new QGraphicsDropShadowEffect;
    effect10->setBlurRadius(15); effect10->setXOffset(0); effect10->setYOffset(0); effect10->setColor(QColor(210,210,210));
    ui->reconnect_group->setGraphicsEffect(effect10);

    QGraphicsDropShadowEffect *effect11 = new QGraphicsDropShadowEffect;
    effect11->setBlurRadius(15); effect11->setXOffset(0); effect11->setYOffset(0); effect11->setColor(QColor(210,210,210));
    ui->misc_group->setGraphicsEffect(effect11);

    QGraphicsDropShadowEffect *effect12 = new QGraphicsDropShadowEffect;
    effect12->setBlurRadius(15); effect12->setXOffset(0); effect12->setYOffset(0); effect12->setColor(QColor(180,180,180));
    ui->press_frame->setGraphicsEffect(effect12);

    QGraphicsDropShadowEffect *effect13 = new QGraphicsDropShadowEffect;
    effect13->setBlurRadius(15); effect13->setXOffset(0); effect13->setYOffset(0); effect13->setColor(QColor(210,210,210));
    ui->log_group->setGraphicsEffect(effect13);

    statusBar()->clearMessage();

    if(!rigol1_enabled) ui->rigol1_group->setEnabled(false);
    if(!rigol2_enabled) ui->rigol2_group->setEnabled(false);
    if(!labjack_enabled) ui->lj_curr_group->setEnabled(false);
    if(!spc_enabled) ui->press_frame->setEnabled(false);
    if(!fms_enabled)
    {
        ui->fms_group->setEnabled(false);
        ui->freq_pid_button->setEnabled(false);
    }

    // store copies of enabled bools so they can be restored properly upon device reconnect
    rigol1_enabled_init = rigol1_enabled;
    rigol2_enabled_init = rigol2_enabled;

    // initialize variables
    refresh_rate = 0.61; // once proper threading is implemented this needs to drop
    log_rate = refresh_rate;
    plot_span = 1200; //default plot span, in seconds
    e_ramp_rate = 0.5;
    err_limit1 = 10;
    err_limit2 = 30;
    press_bound1 = 8e-8;
    press_bound2 = 4e-7;
    press_bound3 = 1e-6;
    fatal_press = 5e-6;

    // parse the config file
    parse_config();

    // create logs folder for this session
    t = time(nullptr);   // get time now
    now = localtime( & t );
    strftime(timestamp_full, sizeof(timestamp_full), "%Y-%m-%d %R", now);
    folder_name = timestamp_full;
    replace( folder_name.begin(), folder_name.end(), ' ', '@'); // replace the space with an @ to prevent system command errors
    replace( folder_name.begin(), folder_name.end(), ':', '_'); // replace the replace colon with underscore to make files/folders readable on Windows
    string folder_cmd = "mkdir ./logs/" + folder_name + "/";
    exec_cmd(folder_cmd.c_str());

    new_data_log();
    new_action_log();

    // generate presure plot
    init_plot();

    // connect realtime slot to timer and start it
    connect(&data_timer, SIGNAL(timeout()), this, SLOT(realtime_slot())); // enable the realtime slot
    data_timer.start(0);

    ui->treeWidget->topLevelItem(0)->setExpanded(true);
    ui->treeWidget->topLevelItem(1)->setExpanded(true);

    probe();
}

void MainWindow::init_plot()
{
    // align all left/right plot margins
    QCPMarginGroup *group = new QCPMarginGroup(ui->power_plot); // previously used the press_plot
    ui->press_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ui->beam_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    ui->power_plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);

    ui->press_plot->setBackground(QBrush(QColor(240,240,240)));
    ui->press_plot->axisRect()->setBackground(Qt::white);
    //ui->press_plot->setFont(plotFont);
    ui->press_plot->xAxis->setTickLabelFont(plot_axis_font);
    ui->press_plot->xAxis->setLabelFont(plot_label_font);
    ui->press_plot->xAxis->setLabelColor(QColor(30,30,30));
    ui->press_plot->xAxis->setBasePen(QPen(QColor(30,30,30)));
    ui->press_plot->xAxis->setTickLabelColor(QColor(30,30,30));
    ui->press_plot->yAxis->setTickLabelFont(plot_axis_font);
    ui->press_plot->yAxis->setLabelFont(plot_label_font);
    ui->press_plot->yAxis->setLabelColor(QColor(30,30,30));
    ui->press_plot->yAxis->setBasePen(QPen(QColor(30,30,30)));
    ui->press_plot->yAxis->setTickLabelColor(QColor(30,30,30));

    ui->beam_plot->setBackground(QBrush(QColor(240,240,240)));
    ui->beam_plot->axisRect()->setBackground(Qt::white);
    //ui->beam_plot->setFont(plotFont);
    ui->beam_plot->xAxis->setTickLabelFont(plot_axis_font);
    ui->beam_plot->xAxis->setLabelFont(plot_label_font);
    ui->beam_plot->xAxis->setLabelColor(QColor(30,30,30));
    ui->beam_plot->xAxis->setBasePen(QPen(QColor(30,30,30)));
    ui->beam_plot->xAxis->setTickLabelColor(QColor(30,30,30));
    ui->beam_plot->yAxis->setTickLabelFont(plot_axis_font);
    ui->beam_plot->yAxis->setLabelFont(plot_label_font);
    ui->beam_plot->yAxis->setLabelColor(QColor(30,30,30));
    ui->beam_plot->yAxis->setBasePen(QPen(QColor(30,30,30)));
    ui->beam_plot->yAxis->setTickLabelColor(QColor(30,30,30));

    ui->power_plot->setBackground(QBrush(QColor(240,240,240)));
    ui->power_plot->axisRect()->setBackground(Qt::white);
    //ui->power_plot->setFont(plotFont);
    ui->power_plot->xAxis->setTickLabelFont(plot_axis_font);
    ui->power_plot->xAxis->setLabelFont(plot_label_font);
    ui->power_plot->xAxis->setLabelColor(QColor(30,30,30));
    ui->power_plot->xAxis->setBasePen(QPen(QColor(30,30,30)));
    ui->power_plot->xAxis->setTickLabelColor(QColor(30,30,30));
    ui->power_plot->yAxis->setTickLabelFont(plot_axis_font);
    ui->power_plot->yAxis->setLabelFont(plot_label_font);
    ui->power_plot->yAxis->setLabelColor(QColor(30,30,30));
    ui->power_plot->yAxis->setBasePen(QPen(QColor(30,30,30)));
    ui->power_plot->yAxis->setTickLabelColor(QColor(30,30,30));

    // PRESSURE PLOT

    // setup pressure plot
    ui->press_plot->addGraph(); // light green line
    ui->press_plot->graph(0)->setPen(QPen(QColor(70, 129, 43)));
    ui->press_plot->xAxis->setLabel(""); // Run Time (h:m:s)
    ui->press_plot->yAxis->setLabel("Pressure (Torr)");
    ui->press_plot->yAxis->setNumberPrecision(10);
    ui->press_plot->axisRect()->setupFullAxesBox();

    // set axes
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->press_plot->xAxis->setTicker(timeTicker);
    ui->press_plot->xAxis2->setTicker(timeTicker);
    ui->press_plot->axisRect()->setupFullAxesBox();
    ui->press_plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->press_plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    ui->press_plot->yAxis->setTicker(logTicker);
    ui->press_plot->yAxis2->setTicker(logTicker);
    ui->press_plot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
    ui->press_plot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
    ui->press_plot->yAxis->grid()->setSubGridVisible(true);
    ui->press_plot->yAxis->setRange(1e-8, 2e-6); // default range
    ui->press_plot->xAxis->setPadding(10);
    ui->press_plot->yAxis->setPadding(10);
    ui->press_plot->xAxis2->setPadding(10);
    ui->press_plot->yAxis2->setPadding(10);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->press_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->press_plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->press_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->press_plot->yAxis2, SLOT(setRange(QCPRange)));

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
    ui->beam_plot->addGraph(); // light green line
    ui->beam_plot->graph(1)->setPen(QPen(QColor(180, 180, 180)));
    ui->beam_plot->xAxis->setLabel("");
    ui->beam_plot->yAxis->setLabel("Beam Current (mA)");
    //ui->beam_plot->yAxis->setNumberPrecision(2);
    ui->beam_plot->axisRect()->setupFullAxesBox();

    // set axes
    ui->beam_plot->xAxis->setTicker(timeTicker);
    ui->beam_plot->xAxis2->setTicker(timeTicker);
    ui->beam_plot->axisRect()->setupFullAxesBox();
    //ui->beam_plot->yAxis->setSubTickCount(8);
    ui->beam_plot->yAxis->setRange(0, 30); // default range
    ui->beam_plot->xAxis->setPadding(10);
    ui->beam_plot->yAxis->setPadding(10);
    ui->beam_plot->xAxis2->setPadding(10);
    ui->beam_plot->yAxis2->setPadding(10);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->beam_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->beam_plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->beam_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->beam_plot->yAxis2, SLOT(setRange(QCPRange)));

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
    ui->power_plot->axisRect()->setupFullAxesBox();

    // set axes
    ui->power_plot->xAxis->setTicker(timeTicker);
    ui->power_plot->xAxis2->setTicker(timeTicker);
    ui->power_plot->axisRect()->setupFullAxesBox();
    //ui->beam_plot->yAxis->setSubTickCount(8);
    ui->power_plot->yAxis->setRange(0, 5); // default range
    ui->power_plot->xAxis->setPadding(10);
    ui->power_plot->yAxis->setPadding(10);
    ui->power_plot->xAxis2->setPadding(10);
    ui->power_plot->yAxis2->setPadding(10);

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

    // start with tab widget on the data page
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::probe()
{
    // connect to and test all devices
    int temp_int;
    int devcount = 0;
    bool all_set = true;

    // connect to labjack using API wrapper call
    if(labjack_enabled)
    {
        cout << "probing LabJack...";
        if(daq.lj_init() < 0)
        {
            cout << "could not connect to LabJack\n";
            labjack_enabled = false;
            ui->lj_curr_group->setEnabled(false);
        }
        devcount++;
        cout << "done\n";
    }

    // establish connection and open socket for all other devices
    if(dxm_enabled)
    {
        status = eth_connect(&::ED_DXM);
        if(status < 0)
        {
            cout << "could not connect to DXM\n";
            init_error_code += 1;
            all_set = false;
        }
    }

    if(slm_enabled)
    {
        status = eth_connect(&::ED_SLM);
        if(status < 0)
        {
            cout << "could not connect to SLM\n";
            init_error_code += 2;
            all_set = false;
        }
    }

    if(rigol1_enabled || rigol2_enabled)
    {
        status = eth_connect(&::ED_RIGOL);
        if(status < 0)
        {
            cout << "could not connect to Rigol\n";
            rigol1_enabled = rigol2_enabled = false;
            ui->rigol1_group->setEnabled(false);
            ui->rigol2_group->setEnabled(false);
            expected_devs--;
        }
    }

    if(spc_enabled)
    {
        status = eth_connect(&::ED_SPCE);
        if(status < 0)
        {
            cout << "could not connect to SPCe\n";
            spc_enabled = false;
            ui->press_frame->setEnabled(false);
            expected_devs--;
        }
    }

    if(fms_enabled)
    {
        status = eth_connect(&::ED_FMS);
        if(status < 0)
        {
            cout << "could not connect to FMS\n";
            fms_enabled = false;
            ui->fms_group->setEnabled(false);
            ui->freq_pid_button->setEnabled(false);
            expected_devs--;
        }
    }

    // begin probing routine to check that all devices are responding properly
    if(all_set)
    {
        if(dxm_enabled)
        {
            cout << "probing DXM...";
            out_str = "99,1,";
            out_str = "\x02" + out_str + "\x03";
            temp_int = ethprobe(&::ED_DXM,out_str,"$");
            if(temp_int == 0)
            {
                init_error_code -= 1;
                cout << "failed\n";
            }
            else
                cout << "done\n";
        }
        if(slm_enabled)
        {
            cout << "probing SLM...";
            out_str = "99,1,";
            out_str = "\x02" + out_str + "\x03";
            temp_int = ethprobe(&::ED_SLM,out_str,"$");
            if(temp_int == 0)
            {
                init_error_code -= 2;
                cout << "failed\n";
            }
            else
                cout << "done\n";
        }
        if(spc_enabled)
        {
            cout << "probing SPCe...";
            temp_int = ethprobe(&::ED_SPCE,"spc 01\r\n","00");
            if(temp_int == 0)
            {
                spc_enabled = false;
                ui->press_frame->setEnabled(false);
                cout << "failed\n";
            }
            else
                cout << "done\n";
        }
        if(rigol1_enabled || rigol2_enabled)
        {
            cout << "probing Rigol...";
            temp_int = ethprobe(&::ED_RIGOL,"*IDN?\r\n","DP821A");
            if(temp_int == 0)
            {
                rigol1_enabled = rigol2_enabled = false;
                ui->rigol1_group->setEnabled(false);
                ui->rigol2_group->setEnabled(false);
                cout << "failed\n";
            }
            else
                cout << "done\n";
        }
        if(fms_enabled)
        {
            cout << "probing FMS...";
            temp_int = ethprobe(&::ED_FMS,"#0","0");
            if(temp_int == 0)
            {
                fms_enabled = false;
                ui->fms_group->setEnabled(false);
                ui->freq_pid_button->setEnabled(false);
                cout << "failed\n";
            }
            else
                cout << "done\n";
        }

        if(init_error_code == 0) // found everything
        {
            ui->statusBar->showMessage("All devices connected. Ready to begin.");
            cout << "probe done, read loop started\n";
            ui->statusBar->clearMessage();
            running = true;
        }
        else // found too much, indicating logic error
        {
            init_fail_dialog(init_error_code);
        }
    }
    else
        init_fail_dialog(init_error_code);
}

void MainWindow::init_fail_dialog(int error_code)
{
    QString err_message;

    if(error_code == -3) // probe fail
        err_message = "Cathode & Mode Anode supplies failed probe.";
    else if(error_code == -2)
        err_message = "Mode Anode supply failed probe.";
    else if(error_code == -1)
        err_message = "Cathode supply failed probe.";
    else if(error_code == 1)
        err_message = "Cathode supply failed to connect.\nPlease check the connection and try again.";
    else if(error_code == 2)
        err_message = "Mod Anode supply failed to connect.\nPlease check the connection and try again.";
    else if(error_code == 3)
        err_message = "Cathode & Mode Anode supplies failed to connect.\nPlease check their connections and try again.";
    else
        err_message = "Unknown error.\nPlease check all devices are connected and powered on.";

    QMessageBox::critical( this, tr("Initialization Error"), err_message, QMessageBox::Ok);
    shutdown();
    QApplication::quit();
}

int MainWindow::ethprobe(ethdevice *dev, string test_cmd, string expected)
{
    // probe ethernet device looking for expected feedback
    // returns 1 if successful, otherwise 0

    recv_str = eth_IO(test_cmd, dev);
    if(recv_str.find("!ERR") == string::npos) // check for errors
    {
        if(recv_str.find(expected) != string::npos) // check for expected output
            return 1;
        else
            out_str = dev->getname() + " unexpected return: " + recv_str;
    }
    else
    {
        out_str = dev->getname() + " could not be reached.";
        cout << dev->getname() << " returned \"" << recv_str << "\"\n";
    }

    ui->statusBar->showMessage(QString::fromUtf8(out_str.c_str()));
    return 0;
}

void MainWindow::realtime_slot()
{
    // this function constantly reiterates while program is open

    // setup the timer to manage the data processing
    static QTime timer(QTime::currentTime());
    key = timer.elapsed()/1000.0; // used to track time
    static double last_key = 0, last_pid_key = 0, last_log_key = 0; // used to track differences in time
    static int resize_tracker = 0;
    static bool toggle_rigol = false;
    static double power_avg = 0;
    int temp_int;
    double convert, dt, press_max, press_min, press_max_bound, press_min_bound, temp_num;
    double beam_max, beam_min, beam_max_bound, beam_min_bound, dxm_curr_adjust, power_adjust, freq_adjust;
    double diode_max, diode_min, diode_max_bound, diode_min_bound;
    char comma;
    double *point;
    istringstream ss;

    if (key-last_key > refresh_rate && running) // frequency of reiteration is refresh_rate in seconds
    {
        // switch to appropriate channel on rigol first
        // then send read commands last to allow for small delay between writes
        if(rigol1_enabled && rigol2_enabled)
        {
            if(toggle_rigol)
                eth_write(":INST CH1\r\n", &::ED_RIGOL);
            else
                eth_write(":INST CH2\r\n", &::ED_RIGOL);

            qApp->processEvents();
        }
        else if(rigol1_enabled) toggle_rigol = true;
        else if(rigol2_enabled) toggle_rigol = false;
        if(starting_prog) // if only single channel enabled, make sure it is selected at start of read loop
        {
            if(rigol1_enabled && !rigol2_enabled)
            {
                eth_write(":INST CH1\r\n", &::ED_RIGOL);
            }
            else if(rigol2_enabled && !rigol1_enabled)
            {
                eth_write(":INST CH2\r\n", &::ED_RIGOL);
            }

            usleep(10000);

            //eth_write(":OUTP:OVP CH2,ON\r\n", &::ED_RIGOL); // put ch2 in CC mode
            eth_write(":VOLT:PROT:STAT ON\r\n", &::ED_RIGOL);

            if(!dxm_enabled) starting_prog = false; // only toggle this is dxm doesn't need it too
        }

        if(dxm_enabled)
        {
            if(starting_prog)
            {
                // write initial beam current limit
                convert = (dxm_curr_limit/30)*4095;
                out_str = "11," + to_string(int(convert)) + ",";
                out_str = '\x02' + out_str + '\x03';
                recv_str = eth_IO(out_str, &::ED_DXM);
                if(recv_str.find("!ERR") != string::npos) // check for errors
                    ui->statusBar->showMessage("ERROR could not write to DXM to set current limit");
                dxm_curr_adjust = 0;
                power_adjust = 0;
                starting_prog = false;
            }

            // read DXM filament current and beam current/voltage in one command
            out_str = "19,";
            out_str = "\x02" + out_str + "\x03";
DXM_CMD1:
            //cout << "SENT (dxm): \"" << out_str << "\"\n";
            recv_str = eth_IO(out_str, &::ED_DXM);
            recv_str.erase(std::remove(recv_str.begin(), recv_str.end(), '\x02'), recv_str.end());
            recv_str.erase(std::remove(recv_str.begin(), recv_str.end(), '\x03'), recv_str.end());
            //cout << "RECIEVED: \"" << recv_str << "\"\n";
            if(recv_str.find("!ERR") == string::npos && recv_str.length() > 4) // check for errors
            {
                dxm_err_count = 0;
                // reset text boxes to normal
                if(!fil_curr_active)
                    ui->fil_curr_edit->setStyleSheet(clear_edit);
                if(!beam_volt_active)
                    ui->beam_volt_edit->setStyleSheet(clear_edit);
                if(!beam_curr_active)
                    ui->beam_curr_edit->setStyleSheet(clear_edit);
                recv_str.erase(0,3); // erase first 3 chars (command echoed back)

                // parse remaining measurement values, delimited by comma
                ss.clear();
                ss.str(recv_str);
                ss >> temp_num;
                dxm_volt = (temp_num/4095)*20;
                ss >> comma;
                ss >> temp_num;
                dxm_curr = (temp_num/4095)*30;
                ss >> comma;
                ss >> temp_num;
                //cout << "read filament bit value=" << temp_num << endl;
                fil_curr = (temp_num/4095)*5;
                ss >> comma;

                //cout << "read filament current=" << fil_curr << endl;

                if(fil_curr < 0.005 && !ramping_down)
                {
                    ui->ramp_down_button->setEnabled(false);
                    ui->ramp_up_button->setEnabled(true);
                }
                else if(fil_curr > 3.295 && !ramping_up)
                {
                    ui->ramp_up_button->setEnabled(false);
                    ui->ramp_down_button->setEnabled(true);
                }
                else
                {
                    ui->ramp_up_button->setEnabled(true);
                    ui->ramp_down_button->setEnabled(true);
                }

                // print to text boxes
                if(!beam_volt_active)
                    ui->beam_volt_edit->setText(QString::number(dxm_volt,'f',2));
                if(!beam_curr_active)
                    ui->beam_curr_edit->setText(QString::number(dxm_curr,'f',2));
                if(!fil_curr_active)
                    ui->fil_curr_edit->setText(QString::number(fil_curr,'f',3));
                beam_plot_label->setText(QString::number(dxm_curr,'f',2) + QString(" mA"));
            }
            else
            {
                dxm_err_count++;
                if(dxm_err_count > 1)
                {
                    ui->fil_curr_edit->setStyleSheet(error_edit);
                    ui->beam_curr_edit->setStyleSheet(error_edit);
                    ui->beam_volt_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR could not read from DXM");
                    if(!beam_volt_active)
                        ui->beam_volt_edit->setText(QString::fromUtf8(recv_str.c_str()));
                    if(!beam_curr_active)
                        ui->beam_curr_edit->setText(QString::fromUtf8(recv_str.c_str()));
                    if(!fil_curr_active)
                        ui->fil_curr_edit->setText(QString::fromUtf8(recv_str.c_str()));
                    beam_plot_label->setText(QString::fromUtf8(recv_str.c_str()));
                    dxm_enabled = false;
                    ui->dxm_group->setEnabled(false);
                    ui->pid_group->setEnabled(false);
                    dxm_volt = -1;
                    dxm_curr = -1;
                    fil_curr = -1;
                    dxm_err_count = 0;
                    action_log_write("ERROR reading from DXM, attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(dxm_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "DXM read failed, retrying...\n";
                    action_log_write("DXM read failed, retrying");
                    usleep(10000);
                    goto DXM_CMD1;
                }
            }

            qApp->processEvents();

            if(dxm_enabled && (ramping_up || ramping_down || (beam_curr_pid_on && beam_curr_sp >= 0) || (power_pid_on && power_sp >= 0) || (freq_pid_on && freq_sp > 0) || e_ramping))
            {
                if(e_ramping)
                {
                    cout << "fil set = " << fil_curr_sp << ", dt = " << (key-last_key) << endl;
                    fil_curr_sp -= e_ramp_rate;
                    if(fil_curr_sp <= 0) // check if finished
                    {
                        e_ramping = false;
                        ui->ramp_up_button->setEnabled(true);
                        ui->ramp_down_button->setEnabled(true);
                        ui->dxm_hv_button->setEnabled(true);
                        ui->fil_curr_button->setEnabled(true);
                        ui->fil_curr_button->setText("✎");
                        ui->fil_curr_button->setFont(button_font);
                        ui->fil_curr_button->setStyleSheet(green_edit_button);
                        ui->fil_curr_edit->setReadOnly(true);
                        ui->beam_volt_button->setEnabled(true);
                        ui->beam_volt_button->setText("✎");
                        ui->beam_volt_button->setFont(button_font);
                        ui->beam_volt_edit->setReadOnly(true);
                        ui->beam_volt_button->setStyleSheet(green_edit_button);
                        cout << "emergency ramp finished\n";
                        action_log_write("Emergency ramp down finished");
                    }
                }
                else if(ramping_up && !relaxing)
                {
                    cout << "fil set = " << fil_curr_sp << ", dt = " << (key-last_key) << endl;
                    fil_curr_sp += ramp_rate; // increment filament current
                    if(fil_curr_sp >= ramp_sp) // check if finished
                    {
                        ramping_up = false;
                        ui->ramp_down_button->setEnabled(true);
                        ui->ramp_up_button->setStyleSheet(green_ramp_button);
                        ui->ramp_up_button->setText("↑ RAMP UP");
                        ui->fil_curr_button->setEnabled(true);
                        ui->fil_curr_button->setText("✎");
                        ui->fil_curr_button->setFont(button_font);
                        ui->fil_curr_edit->setReadOnly(true);
                        ui->fil_curr_button->setStyleSheet(green_edit_button);

                        if(dxm_hv_on)
                        {
                            ui->beam_curr_pid_button->setEnabled(true);
                            ui->power_pid_button->setEnabled(true);
                            if(fms_enabled) ui->freq_pid_button->setEnabled(true);
                        }

                        cout << "ramp up finished\n";
                        ui->statusBar->showMessage("Ramp up finished");
                        action_log_write("filament ramp up finished");
                    }
                }
                else if (ramping_down && !relaxing)
                {
                    cout << "fil set = " << fil_curr_sp << ", dt = " << (key-last_key) << endl;
                    fil_curr_sp -= ramp_rate; // decrement filament current
                    if(fil_curr_sp <= 0) // check if finished
                    {
                        ramping_down = false;
                        ui->ramp_up_button->setEnabled(true);
                        ui->ramp_down_button->setEnabled(false);
                        ui->ramp_down_button->setStyleSheet(green_ramp_button);
                        ui->ramp_down_button->setText("RAMP DOWN ↓");
                        ui->fil_curr_button->setEnabled(true);
                        ui->fil_curr_button->setText("✎");
                        ui->fil_curr_button->setFont(button_font);
                        ui->fil_curr_edit->setReadOnly(true);
                        ui->fil_curr_button->setStyleSheet(green_edit_button);

                        if(dxm_hv_on)
                        {
                            ui->beam_curr_pid_button->setEnabled(true);
                            ui->power_pid_button->setEnabled(true);
                            if(fms_enabled) ui->freq_pid_button->setEnabled(true);
                        }

                        cout << "ramp down finished\n";
                        ui->statusBar->showMessage("Ramp down finished");
                        action_log_write("filament ramp down finished");
                    }
                }
                else if(beam_curr_pid_on && beam_curr_sp >= 0 && !relaxing && !dxm_hv_blocked) // beam current PID control
                {
                    if(reset_pid_time) // set default dt if pid control is just starting/resuming
                    {
                        dt = refresh_rate;
                        fil_curr_sp = fil_curr;
                        reset_pid_time = false;
                    }
                    else
                        dt = key - last_pid_key; // otherwise calculate the dt precisely

                    if(dt <= 0) dt = refresh_rate;

                    // calculate neccisary adjustment from PID calculation
                    dxm_curr_adjust = pid->calculate(beam_curr_sp,dxm_curr,dt);
                    //fil_curr_sp += (double)(dxm_curr_adjust * beam2fil_scale); // scale the adjustment to beam current for filament

                    // be careful that this adjustment isn't problematic, especially prior to proper tuning
                    if((dxm_curr+dxm_curr_adjust) > 30)
                    {
                        dxm_curr_adjust = 30 - dxm_curr;
                    }
                    else if((dxm_curr+dxm_curr_adjust) < 0)
                    {
                        dxm_curr_adjust = dxm_curr * (-1);
                    }

                    fil_curr_sp += dxm_curr_adjust;

                    // adjust every time? even if system hasn't had adequate time to shift? consider this if logic errors arise.

                    cout << "MAIN: actual beam=" << dxm_curr << ", goal=" << beam_curr_sp << ", adjust=" << dxm_curr_adjust << ", fil=" << fil_curr << ", dt=" << dt << endl;
                    //cout << "dt=" << dt << ", actual fil=" << fil_curr << ", fil set=" << fil_curr_sp << endl;

                    last_pid_key = key;
                }
                else if(power_pid_on && power_sp >= 0 && power_array.size() >= 3 && !relaxing && !dxm_hv_blocked) // power PID control
                {
                    if(reset_pid_time) // set default dt if pid control is just starting/resuming
                    {
                        dt = refresh_rate;
                        fil_curr_sp = fil_curr;
                        reset_pid_time = false;
                    }
                    else
                        dt = key - last_pid_key; // otherwise calculate the dt precisely

                    if(dt <= 0) dt = refresh_rate;

                    // calculate neccisary adjustment from PID calculation
                    //power_adjust = pid->calculate(power_sp,power,dt);
                    power_adjust = pid->calculate(power_sp,power_avg,dt);
                    /*
                    // be careful that this adjustment isn't problematic, especially prior to proper tuning
                    if((power+power_adjust) > 5)
                    {
                        power_adjust = 5 - power;
                    }
                    else if((power+power_adjust) < 0)
                    {
                        power_adjust = power * (-1);
                    }
*/
                    //fil_curr_sp += power_adjust;
                    fil_curr_sp += power_adjust * 15; // trying out a scale to keep roughly the same PID constants for both controllers

                    cout << "MAIN: power=" << power << ", avg power=" << power_avg << ", goal=" << power_sp << ", adjust=" << power_adjust << ", fil=" << fil_curr << endl;

                    last_pid_key = key;
                }
                else if (freq_pid_on && freq_sp > 0 && !relaxing && !dxm_hv_blocked)
                {
                    if(reset_pid_time) // set default dt if pid control is just starting/resuming
                    {
                        dt = refresh_rate;
                        fil_curr_sp = fil_curr;
                        beam_volt_sp = dxm_volt;
                        reset_pid_time = false;
                    }
                    else
                        dt = key - last_pid_key; // otherwise calculate the dt precisely

                    if(dt <= 0) dt = refresh_rate;

                    freq_adjust = fpid->calculate(freq_sp,freq,dt);

                    fil_curr_sp += freq_adjust; // needs scale values which are TBD
                    beam_volt_sp += freq_adjust/2; // trying this

                    // need to also check that voltage isnt incremented more than 100V
                    if(beam_volt_sp > dxm_volt_ref + 0.1)
                        beam_volt_sp = dxm_volt_ref + 0.1;
                    else if(beam_volt_sp < dxm_volt_ref - 0.1)
                        beam_volt_sp = dxm_volt_ref - 0.1;

                    cout << "MAIN: actual freq=" << freq << ", goal=" << freq_sp << ", adjust=" << freq_adjust << ", dt=" << dt;
                    if(beam_volt_sp >= dxm_volt_ref + 0.1)
                        cout << ", top bound hit";
                    else if(beam_volt_sp <= dxm_volt_ref - 0.1)
                        cout << ", bot bound hit";
                    cout << endl;

                    last_pid_key = key;
                }

                // check to make sure filament current isnt set out of bounds
                if(fil_curr_sp > 3.3) fil_curr_sp = 3.3;
                else if (fil_curr_sp < 0) fil_curr_sp = 0;

                // check to make sure beam voltage is within bounds
                if(beam_volt_sp > 5) beam_volt_sp = 5;
                else if(beam_volt_sp < 0) beam_volt_sp = 0;

                qApp->processEvents();

                if(!relaxing && (ramping_up || ramping_down || (beam_curr_pid_on && beam_curr_sp >= 0) || (power_pid_on && power_sp >= 0) || (freq_pid_on && freq_sp > 0) || e_ramping))
                {
                    // set new filament current
                    ui->fil_curr_button->setText(QString("set: %1").arg(QString::number(fil_curr_sp, 'f', 3)));
                    convert = (fil_curr_sp/3.3)*4095;
                    cout << "writing filament current value " << (int(convert)) << endl;
                    out_str = "13," + to_string(int(convert)) + ",";
                    out_str = "\x02" + out_str + "\x03";
DXM_CMD2:
                    recv_str = eth_write(out_str, &::ED_DXM);
                    if(recv_str.find("!ERR") == string::npos) // check for errors
                    {
                        dxm_err_count = 0;
                        ui->fil_curr_edit->setStyleSheet(clear_edit);
                    }
                    else
                    {
                        dxm_err_count++;
                        if(dxm_err_count > 1)
                        {
                            ui->fil_curr_edit->setStyleSheet(error_edit);
                            ui->statusBar->showMessage("ERROR could not write to DXM (via PID)");
                            dxm_enabled = false;
                            ui->dxm_group->setEnabled(false);
                            ui->pid_group->setEnabled(false);
                            dxm_err_count = 0;
                            action_log_write("ERROR writing to DXM, attempting reconnect");
                            QTimer::singleShot(100, this, SLOT(dxm_recon())); // wait 0.1s then attempt reconnect to device
                        }
                        else
                        {
                            cout << "DXM read failed, retrying...\n";
                            usleep(10000);
                            goto DXM_CMD2;
                        }
                    }

                    qApp->processEvents();

                    if(freq_pid_on && !dxm_hv_blocked)
                    {
                        ui->beam_volt_button->setText(QString("set: %1").arg(QString::number(beam_volt_sp, 'f', 3)));
                        convert = (beam_volt_sp/20)*4095;
                        out_str = "10," + to_string(int(convert)) + ",";
                        out_str = "\x02" + out_str + "\x03";
DXM_CMD3:
                        recv_str = eth_write(out_str, &::ED_DXM);
                        if(recv_str.find("!ERR") == string::npos) // check for errors
                        {
                            dxm_err_count = 0;
                            ui->beam_volt_edit->setStyleSheet(clear_edit);
                            ui->beam_volt_button->setText(QString::number(beam_volt_sp));
                        }
                        else
                        {
                            dxm_err_count++;
                            if(dxm_err_count > 1)
                            {
                                ui->beam_volt_edit->setStyleSheet(error_edit);
                                ui->statusBar->showMessage("ERROR could not write to DXM");
                                dxm_enabled = false;
                                ui->dxm_group->setEnabled(false);
                                ui->pid_group->setEnabled(false);
                                dxm_err_count = 0;
                                action_log_write("ERROR writing to DXM, attempting reconnect");
                                QTimer::singleShot(100, this, SLOT(dxm_recon())); // wait 0.1s then attempt reconnect to device
                            }
                            else
                            {
                                cout << "DXM write failed, retrying...\n";
                                usleep(10000);
                                goto DXM_CMD3;
                            }
                        }
                    }
                }

                qApp->processEvents();
            }
        }

        if(slm_enabled)
        {
            // read SLM voltage and current in one command
            out_str = "19,";
            out_str = "\x02" + out_str + "\x03";
SLM_CMD1:
            //cout << "SENT (slm): \"" << out_str << "\"\n";
            recv_str = eth_IO(out_str, &::ED_SLM);
            recv_str.erase(std::remove(recv_str.begin(), recv_str.end(), '\x02'), recv_str.end());
            recv_str.erase(std::remove(recv_str.begin(), recv_str.end(), '\x03'), recv_str.end());
            //cout << "RECIEVED: \"" << recv_str << "\"\n";
            if(recv_str.find("!ERR") == string::npos && recv_str.length() > 4) // check for errors
            {
                slm_err_count = 0;
                // reset text boxes to normal
                if(!anode_volt_active)
                    ui->anode_volt_edit->setStyleSheet(clear_edit);
                if(!anode_curr_active)
                    ui->anode_curr_edit->setStyleSheet(clear_edit);
                recv_str.erase(0,3); // erase first 3 chars (command echoed back)

                // parse remaining measurement values, delimited by comma
                ss.clear();
                ss.str(recv_str);
                ss >> temp_num;
                slm_volt = (temp_num/4095)*5;
                ss >> comma;
                ss >> temp_num;
                slm_curr = (temp_num/4095)*60;
                ss >> comma;
                ss >> temp_num;
                ss >> comma;

                // print to text boxes
                if(!anode_volt_active)
                    ui->anode_volt_edit->setText(QString::number(slm_volt,'f',2));
                if(!anode_curr_active)
                    ui->anode_curr_edit->setText(QString::number(slm_curr,'f',2));
            }
            else
            {
                slm_err_count++;
                if(slm_err_count > 1)
                {
                    ui->anode_curr_edit->setStyleSheet(error_edit);
                    ui->anode_volt_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR could not read from DXM");
                    if(!anode_volt_active)
                        ui->anode_volt_edit->setText(QString::fromUtf8(recv_str.c_str()));
                    if(!anode_curr_active)
                        ui->anode_curr_edit->setText(QString::fromUtf8(recv_str.c_str()));
                    slm_enabled = false;
                    ui->slm_group->setEnabled(false);
                    slm_volt = -1;
                    slm_curr = -1;
                    slm_err_count = 0;
                    action_log_write("ERROR writing to SLM, attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(slm_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "SLM read failed, retrying...\n";
                    action_log_write("SLM read failed, retrying");
                    usleep(10000);
                    goto SLM_CMD1;
                }
            }

            qApp->processEvents();
        }

        if(spc_enabled)
        {
            // read Gamma Vac pressure
SPC_CMD:
            recv_str = eth_IO("spc 0B\r\n", &::ED_SPCE);
            //cout << "original:" << recv_str << endl;
            if(recv_str.find("!ERR") == string::npos && recv_str.length() > 5 && recv_str.find("T") != string::npos) // check for errors
            {
                recv_str.erase(0,6); // erase first 6 chars
                recv_str.erase(recv_str.find("T")-1, (recv_str.length()-recv_str.find("T")-1)); // erase everything after the space before "Torr"
                press_str = recv_str;
                press_str.erase(std::remove(press_str.begin(), press_str.end(), '\x20'), press_str.end()); // remove space
                press_str.erase(std::remove(press_str.begin(), press_str.end(), '\x0A'), press_str.end()); // remove line feed
                press_str.erase(std::remove(press_str.begin(), press_str.end(), '\x0D'), press_str.end()); // remove carriage return
                try
                {
                    pressure = stod(press_str);
                }
                catch(std::exception&) // catch(std::exception& e)
                {
                    spc_err_count++;
                    if(spc_err_count > 1)
                    {
                        cout << "failed to convert to double: " << recv_str << endl;
                        spc_enabled = false;
                        ui->press_frame->setEnabled(false);
                        pressure = -1;
                        spc_err_count = 0;
                        action_log_write("ERROR converting output from SPCe, attempting reconnect");
                        QTimer::singleShot(100, this, SLOT(spc_recon())); // wait 0.1s then attempt reconnect to device
                    }
                    else
                    {
                        cout << "gamma vac stod conversion failed, retrying command...\n";
                        usleep(10000);
                        goto SPC_CMD;
                    }
                }
                spc_err_count = 0;
                ui->press_frame->setEnabled(true);
                ui->press_frame->setStyleSheet(green_press_frame);
                ui->press_label->setText(QString::number(pressure)); // change this later to display 2 decimals
                press_plot_label->setText(QString::number(pressure) + QString(" Torr"));
            }
            else
            {
                spc_err_count++;
                if(spc_err_count > 1)
                {
                    ui->press_frame->setEnabled(false);
                    ui->statusBar->showMessage("ERROR could not read from SPCe");
                    ui->press_label->setText(QString::fromUtf8(recv_str.c_str()));
                    press_plot_label->setText(QString::fromUtf8(recv_str.c_str()));
                    spc_enabled = false;
                    ui->press_frame->setEnabled(false);
                    pressure = -1;
                    spc_err_count = 0;
                    action_log_write("ERROR reading from SPCe, attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(spc_recon())); // wait 0.2s then attempt reconnect to device
                }
                else
                {
                    cout << "gamma vac read failed, retrying...\n";
                    action_log_write("Gamma Vac SPCe read failed, retrying");
                    usleep(10000);
                    goto SPC_CMD;
                }
            }

            qApp->processEvents();

            // check fatal pressure limit
            if(pressure >= fatal_press && !e_ramping)
            {
                e_ramping = true;
                ui->press_frame->setStyleSheet(red_press_frame);
                ui->statusBar->showMessage("Pressure reached fatal limit! Quickly ramping down filament.");
                cout << "hit fatal pressure limit! hard ramping down filament.\n";
                action_log_write("hit fatal pressure limit, begin relaxation state");
            }

            // check pressure bounds for filament
            if(pressure >= press_bound3 && !relaxing)
            {
                relaxing = true;
                ui->press_frame->setStyleSheet(yellow_press_frame);
                ui->statusBar->showMessage("Relaxing pressure...");
                cout << "relaxing pressure...\n";
                action_log_write("hit pressure high limit for filament, begin relaxation state");
            }
            else if(pressure <= press_bound2 && relaxing)
            {
                relaxing = false;
                ui->statusBar->clearMessage();
                ui->press_frame->setStyleSheet(green_press_frame);
                cout << "done relaxing pressure\n";
                action_log_write("hit pressure low limit for filament, end relaxation state");
            }
            else
            {
                ui->press_frame->setStyleSheet(green_press_frame);
            }

            // check pressure bounds for HV
            if(pressure >= press_bound2 && !dxm_hv_blocked && dxm_hv_on) // fatal pressure hit
            {
                // turn HV off
                out_str = "98,0,";
                out_str = '\x02' + out_str + '\x03';
                eth_write(out_str, &::ED_DXM);

                dxm_hv_blocked = true;
                dxm_hv_on = false;
                ui->dxm_hv_button->setStyleSheet(green_hv_button);
                ui->dxm_hv_button->setText("ENABLE\nHV");
                ui->dxm_hv_button->setEnabled(false);
                ui->beam_volt_edit->setReadOnly(true);
                ui->beam_volt_button->setEnabled(false);
                ui->beam_volt_button->setText("✎");
                ui->beam_volt_button->setFont(button_font);
                ui->beam_volt_button->setStyleSheet(green_edit_button);

                freq_pid_on = false;
                power_pid_on = false;
                beam_curr_pid_on = false;

                ui->freq_pid_button->setEnabled(false);
                ui->freq_pid_button->setStyleSheet(green_auto_button);
                ui->freq_pid_button->setText("FREQUENCY");
                ui->power_pid_button->setEnabled(false);
                ui->power_pid_button->setStyleSheet(green_auto_button);
                ui->power_pid_button->setText("POWER");
                ui->beam_curr_pid_button->setEnabled(false);
                ui->beam_curr_pid_button->setStyleSheet(green_auto_button);
                ui->beam_curr_pid_button->setText("CURRENT");

                ui->fil_curr_button->setEnabled(true);
                ui->fil_curr_button->setText("✎");
                ui->fil_curr_button->setFont(button_font);
                ui->fil_curr_edit->setReadOnly(true);
                ui->fil_curr_button->setStyleSheet(green_edit_button);
                ui->beam_curr_button->setEnabled(false);
                ui->beam_curr_edit->setReadOnly(true);
                ui->beam_curr_button->setStyleSheet(green_edit_button);
                beam_curr_active = false;
                ui->power_button->setEnabled(false);
                ui->power_edit->setReadOnly(true);
                ui->power_button->setStyleSheet(green_edit_button);
                power_active = false;


                // may need to save PID state here (as an integer code) to be restored later

                ui->statusBar->showMessage("Blocking HV...");
                cout << "blocking HV...\n";
                action_log_write("hit pressure high limit for HV, begin blocking HV");
            }
            else if(pressure <= press_bound1 && dxm_hv_blocked)
            {
                // turn HV on
                out_str = "98,1,";
                out_str = '\x02' + out_str + '\x03';
                eth_write(out_str, &::ED_DXM);

                dxm_hv_blocked = false;
                dxm_hv_on = true;
                ui->dxm_hv_button->setStyleSheet(orange_hv_button);
                ui->dxm_hv_button->setText("DISABLE\nHV");
                ui->dxm_hv_button->setEnabled(true);
                ui->beam_volt_button->setEnabled(true);
                ui->beam_volt_button->setText("✎");
                ui->beam_volt_button->setFont(button_font);
                ui->beam_volt_edit->setReadOnly(true);
                ui->beam_volt_button->setStyleSheet(green_edit_button);
                if(fms_enabled) ui->freq_pid_button->setEnabled(true);
                ui->power_pid_button->setEnabled(true);
                ui->beam_curr_pid_button->setEnabled(true);

                // may need to restore PID state here

                ui->statusBar->clearMessage();
                cout << "unblocking HV...\n";
                action_log_write("hit pressure high limit for HV, stop blocking HV");
            }

            qApp->processEvents();
        }

        if(labjack_enabled)
        {
LJ_CMD:
            point = daq.lj_read(); // read analog input channels 1&2
            if(point)
            {
                lj_err_count = 0;
                ain0 = point[0];
                ain1 = point[1];
                ain2 = point[2];
                /*
                if(ain0 > 5 || ain0 < 0) // if out of expected range than sample isn't connected
                {
                    ui->collector_read->setText("NC");
                }
                else // otherwise convert and update value in GUI
                {
                    ain0 = (point[0]/5)*30.864; // convert to mA
                    temp_str = QString::number(ain0,'f',2);
                    ui->collector_read->setText(temp_str);
                }*/
                ain0 = (ain0/(-5))*30.864; // convert to mA
                //temp_str = QString::number(ain0,'f',2);
                ui->collector_read->setText(QString::number(ain0,'f',2));

                /*
                if(ain1 > 5 || ain1 < 0) // if out of expected range than sample isn't connected
                {
                    ui->body_read->setText("NC");
                }
                else // otherwise convert and update value in GUI
                {
                    temp_str = QString::number(ain1,'f',2);
                    ui->body_read->setText(temp_str);
                }*/
                ain1 = ain1*(-1);
                //temp_str = QString::number(ain1,'f',2);
                ui->body_read->setText(QString::number(ain1,'f',2));

                diode_volt = ain2*(-1000); // convert to mV and swap sign to make plotting easier
                if(!power_active)
                    ui->power_edit->setText(QString::number(diode_volt,'f',6));
                power_plot_label->setText(QString::number(diode_volt,'f',2) + QString(" mV"));
            }
            else
            {
                lj_err_count++;
                if(lj_err_count > 1)
                {
                    //ui->fil_curr_edit->setText("!ERR");
                    //ui->beam_curr_edit->setText("!ERR");
                    beam_plot_label->setText("!ERR");
                    ui->statusBar->showMessage("ERROR could not read from LabJack");
                    labjack_enabled = false;
                    ui->lj_curr_group->setEnabled(false);
                    diode_volt = -1;
                    lj_err_count = 0;
                    action_log_write("ERROR reading from LabJack, attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(ljReconnect())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "labjack read failed, retrying...\n";
                    action_log_write("LabJack read failed, retrying");
                    usleep(10000);
                    goto LJ_CMD;
                }
            }

            qApp->processEvents();
        }

        if(rigol1_enabled && toggle_rigol)
        {
            // read Rigol CH1 current
            //eth_write(":INST CH1\r\n", &::ED_RIGOL);
RIGOL_CMD1:
            recv_str = eth_IO(":MEAS:CURR?\r\n", &::ED_RIGOL);
            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                rigol_err_count = 0;
                if(!shim_curr_active)
                    ui->shim_curr_edit->setStyleSheet(clear_edit);
            }
            else
            {
                rigol_err_count++;
                if(rigol_err_count > 1)
                {
                    ui->shim_curr_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR could not read from Rigol");
                    rigol1_enabled = false;
                    rigol2_enabled = false;
                    ui->rigol1_group->setEnabled(false);
                    ui->rigol2_group->setEnabled(false);
                    rigol_err_count = 0;
                    action_log_write("ERROR reading from Rigol (ch1), attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(rigol_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "rigol read failed, retrying...\n";
                    action_log_write("Rigol read failed, retrying");
                    usleep(10000);
                    goto RIGOL_CMD1;
                }
            }
            if(rigol1_enabled && recv_str.find("error") == string::npos) // only update if it didn't return "command error"
            {
                //ui->shim_curr_edit->setText(QString::fromUtf8(recv_str.c_str()));
                ss.clear();
                ss.str(recv_str);
                ss >> temp_num;
                if(!shim_curr_active)
                    ui->shim_curr_edit->setText(QString::number(temp_num,'f',3));
            }
            else
            {
                if(!shim_curr_active)
                    ui->shim_curr_edit->setText("error");
            }

            // read Rigol CH1 voltage
RIGOL_CMD2:
            recv_str = eth_IO(":MEAS:VOLT?\r\n", &::ED_RIGOL);
            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                rigol_err_count = 0;
                if(!shim_volt_active)
                    ui->shim_volt_edit->setStyleSheet(clear_edit);
            }
            else
            {
                rigol_err_count++;
                if(rigol_err_count > 1)
                {
                    if(!shim_volt_active)
                        ui->shim_volt_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR could not read from Rigol");
                    rigol1_enabled = false;
                    rigol2_enabled = false;
                    ui->rigol1_group->setEnabled(false);
                    ui->rigol2_group->setEnabled(false);
                    rigol_err_count = 0;
                    action_log_write("ERROR reading from Rigol (ch1), attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(rigol_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "rigol read failed, retrying...\n";
                    action_log_write("Rigol read failed, retrying");
                    usleep(10000);
                    goto RIGOL_CMD2;
                }
            }
            if(rigol1_enabled && recv_str.find("error") == string::npos) // only update if it didn't return "command error"
            {
                //ui->shim_volt_edit->setText(QString::fromUtf8(recv_str.c_str()));
                ss.clear();
                ss.str(recv_str);
                ss >> temp_num;
                if(!shim_volt_active)
                    ui->shim_volt_edit->setText(QString::number(temp_num,'f',3));
            }
            else
                ui->shim_volt_edit->setText("error");

            if(rigol2_enabled) toggle_rigol = false;

            qApp->processEvents();
        }
        if(rigol2_enabled && !toggle_rigol)
        {
            // read Rigol CH2 current
            //eth_write(":INST CH2\r\n", &::ED_RIGOL);
RIGOL_CMD3:
            recv_str = eth_IO(":MEAS:CURR?\r\n", &::ED_RIGOL);
            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                rigol_err_count = 0;
                if(!trim_curr_active)
                    ui->trim_curr_edit->setStyleSheet(clear_edit);
            }
            else
            {
                rigol_err_count++;
                if(rigol_err_count > 1)
                {
                    ui->trim_curr_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR could not read from Rigol");
                    rigol1_enabled = false;
                    rigol2_enabled = false;
                    ui->rigol1_group->setEnabled(false);
                    ui->rigol2_group->setEnabled(false);
                    rigol_err_count = 0;
                    action_log_write("ERROR reading from Rigol (ch2), attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(rigol_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "rigol read failed, retrying...\n";
                    action_log_write("Rigol read failed, retrying");
                    usleep(10000);
                    goto RIGOL_CMD3;
                }
            }
            if(rigol2_enabled && recv_str.find("error") == string::npos) // only update if it didn't return "command error"
            {
                //ui->trim_curr_edit->setText(QString::fromUtf8(recv_str.c_str()));
                ss.clear();
                ss.str(recv_str);
                ss >> temp_num;
                if(!trim_curr_active)
                    ui->trim_curr_edit->setText(QString::number(temp_num,'f',3));
            }
            else
            {
                if(!trim_volt_active)
                    ui->trim_curr_edit->setText("error");
            }

            // read Rigol CH2 voltage
RIGOL_CMD4:
            recv_str = eth_IO(":MEAS:VOLT?\r\n", &::ED_RIGOL);
            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                rigol_err_count = 0;
                if(!trim_volt_active)
                    ui->trim_volt_edit->setStyleSheet(clear_edit);
            }
            else
            {
                rigol_err_count++;
                if(rigol_err_count > 1)
                {
                    ui->trim_volt_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR could not read from Rigol");
                    rigol1_enabled = false;
                    rigol2_enabled = false;
                    ui->rigol1_group->setEnabled(false);
                    ui->rigol2_group->setEnabled(false);
                    rigol_err_count = 0;
                    action_log_write("ERROR reading from Rigol (ch2), attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(rigol_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "rigol read failed, retrying...\n";
                    action_log_write("Rigol read failed, retrying");
                    usleep(10000);
                    goto RIGOL_CMD4;
                }
            }
            if(rigol2_enabled && recv_str.find("error") == string::npos) // only update if it didn't return "command error"
            {
                //ui->trim_volt_edit->setText(QString::fromUtf8(recv_str.c_str()));
                ss.clear();
                ss.str(recv_str);
                ss >> temp_num;
                if(!trim_volt_active)
                    ui->trim_volt_edit->setText(QString::number(temp_num,'f',3));
            }
            else
            {
                if(!trim_volt_active)
                    ui->trim_volt_edit->setText("error");
            }

            if(rigol1_enabled) toggle_rigol = true;

            qApp->processEvents();
        }

        if(fms_enabled)
        {
FMS_CMD:
            recv_str = eth_IO("source?\r\n", &::ED_FMS);
            if(recv_str.find("!ERR") == string::npos && recv_str.length() > 2)
            {
                fms_err_count = 0;
                ss.clear();
                ss.str(recv_str);
                ss >> freq;
                if(!freq_active)
                    ui->freq_edit->setText(QString::number(freq));
            }
            else
            {
                fms_err_count++;
                if(fms_err_count > 1)
                {
                    ui->freq_edit->setStyleSheet(error_edit);
                    if(!freq_active)
                        ui->freq_edit->setText(QString::fromUtf8(recv_str.c_str()));
                    fms_enabled = false;
                    ui->fms_group->setEnabled(false);
                    ui->freq_pid_button->setEnabled(false);
                    freq = -1;
                    fms_err_count = 0;
                    action_log_write("ERROR reading from FMS, attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(fms_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "FMS read failed, retrying...\n";
                    action_log_write("FMS read failed, retrying");
                    usleep(10000);
                    goto FMS_CMD;
                }
            }

            qApp->processEvents();
        }

        // update data & replot
        if(spc_enabled && dxm_enabled && labjack_enabled)
        {
            // update plot vectors
            time_data.push_back(key);
            press_data.push_back(pressure);
            beam_data.push_back(dxm_curr);
            power_data.push_back(diode_volt);
            beam_sp_data.push_back(beam_curr_sp);
            beam_time_data.push_back(key);
            power_sp_data.push_back(power_sp);
            power_time_data.push_back(key);

            while(key - *time_data.begin() > plot_span) // if data points are present before the lower time bound
            {
                if(!beam_data.empty()) beam_data.erase(beam_data.begin());
                if(!press_data.empty()) press_data.erase(press_data.begin());
                if(!power_data.empty()) power_data.erase(power_data.begin());
                if(!time_data.empty()) time_data.erase(time_data.begin());
            }
            if(!power_time_data.empty())
            {
                while(key - *power_time_data.begin() > plot_span)
                {
                    if(!power_sp_data.empty()) power_sp_data.erase(power_sp_data.begin());
                    if(!power_time_data.empty()) power_time_data.erase(power_time_data.begin());
                }
            }
            if(!beam_time_data.empty())
            {
                while(key - *beam_time_data.begin() > plot_span)
                {
                    if(!beam_sp_data.empty()) beam_sp_data.erase(beam_sp_data.begin());
                    if(!beam_time_data.empty()) beam_time_data.erase(beam_time_data.begin());
                }
            }

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
                diode_max = *std::max_element(power_data.constBegin(), power_data.constEnd());
                diode_min = *std::min_element(power_data.constBegin(), power_data.constEnd());
                diode_max_bound = (floor(diode_max/5)+1)*5;
                diode_min_bound = (floor(diode_min/5)-1)*5;
                if(diode_min_bound < 0) diode_min_bound = 0;
                ui->power_plot->yAxis2->setRange(diode_min_bound,diode_max_bound);
                resize_tracker = 0;
            }

            if(ui->tabWidget->currentIndex() == 1)
            {
                ui->press_plot->replot(); // update plot if visible
                ui->beam_plot->replot();
                ui->power_plot->replot();
            }

            qApp->processEvents();
        }

        qApp->processEvents();

        if(dxm_enabled)
        {
            // read all faults status
            out_str = "68,";
            out_str = "\x02" + out_str + "\x03";
DXM_CMD4:
            //cout << "SENT (dxm fault): \"" << out_str << "\"\n";
            recv_str = eth_IO(out_str, &::ED_DXM);
            //recv_str = eth_read(&::ED_DXM);
            recv_str.erase(std::remove(recv_str.begin(), recv_str.end(), '\x02'), recv_str.end());
            recv_str.erase(std::remove(recv_str.begin(), recv_str.end(), '\x03'), recv_str.end());
            //cout << "RECIEVED (dxm fault): \"" << recv_str << "\"\n";
            if(recv_str.find("!ERR") == string::npos && recv_str.length() > 10) // check for errors
            {
                dxm_err_count = 0;
                // reset text boxes to normal
                if(!fil_curr_active)
                    ui->fil_curr_edit->setStyleSheet(clear_edit);
                if(!beam_volt_active)
                    ui->beam_volt_edit->setStyleSheet(clear_edit);
                if(!beam_curr_active)
                    ui->beam_curr_edit->setStyleSheet(clear_edit);
                recv_str.erase(0,3); // erase first 3 chars (command echoed back)

                // parse remaining measurement values, delimited by comma
                ss.clear();
                ss.str(recv_str);
                ss >> temp_int;
                if(temp_int == 0) dxm_arc = false; else dxm_arc = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) dxm_over_temp = false; else dxm_over_temp = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) dxm_over_volt = false; else dxm_over_volt = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) dxm_under_volt = false; else dxm_under_volt = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) dxm_over_curr = false; else dxm_over_curr = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) dxm_under_curr = false; else dxm_under_curr = true;
                ss >> comma;
            }
            else
            {
                dxm_err_count++;
                if(dxm_err_count > 1)
                {
                    ui->fil_curr_edit->setStyleSheet(error_edit);
                    ui->beam_curr_edit->setStyleSheet(error_edit);
                    ui->beam_volt_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR could not read from DXM");
                    dxm_enabled = false;
                    ui->dxm_group->setEnabled(false);
                    ui->pid_group->setEnabled(false);
                    dxm_err_count = 0;
                    action_log_write("ERROR reading from DXM, attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(dxm_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "DXM read failed, retrying...\n";
                    usleep(10000);
                    goto DXM_CMD4;
                }
            }

            if(dxm_volt < 0.2 && beam_volt_sp > 0.5)
                dxm_arc = true;

            qApp->processEvents();
        }
        if(slm_enabled)
        {
            // read all faults status
            out_str = "68,";
            out_str = "\x02" + out_str + "\x03";
SLM_CMD2:
            //cout << "SENT (slm fault): \"" << out_str << "\"\n";
            recv_str = eth_IO(out_str, &::ED_SLM);
            //recv_str = eth_read(&::ED_SLM);
            recv_str.erase(std::remove(recv_str.begin(), recv_str.end(), '\x02'), recv_str.end());
            recv_str.erase(std::remove(recv_str.begin(), recv_str.end(), '\x03'), recv_str.end());
            //cout << "RECIEVED (slm fault): \"" << recv_str << "\"\n";
            if(recv_str.find("!ERR") == string::npos && recv_str.length() > 11) // check for errors
            {
                slm_err_count = 0;
                // reset text boxes to normal
                if(!anode_volt_active)
                    ui->anode_volt_edit->setStyleSheet(clear_edit);
                if(!anode_curr_active)
                    ui->anode_curr_edit->setStyleSheet(clear_edit);
                recv_str.erase(0,3); // erase first 3 chars (command echoed back)

                // parse remaining measurement values, delimited by comma
                ss.clear();
                ss.str(recv_str);
                ss >> temp_int;
                if(temp_int == 0) slm_arc = false; else slm_arc = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) slm_over_temp = false; else slm_over_temp = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) slm_over_volt = false; else slm_over_volt = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) slm_under_volt = false; else slm_under_volt = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) slm_over_curr = false; else slm_over_curr = true;
                ss >> comma;
                ss >> temp_int;
                if(temp_int == 0) slm_under_curr = false; else slm_under_curr = true;
                ss >> comma;
                ss >> temp_int; // regulation error? still need to figure out what that last digit is for SLM
                ss >> comma;
            }
            else
            {
                slm_err_count++;
                if(slm_err_count > 1)
                {
                    ui->anode_curr_edit->setStyleSheet(error_edit);
                    ui->anode_volt_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR could not read from SLM");
                    slm_enabled = false;
                    ui->slm_group->setEnabled(false);
                    slm_err_count = 0;
                    action_log_write("ERROR reading from SLM, attempting reconnect");
                    QTimer::singleShot(100, this, SLOT(slm_recon())); // wait 0.1s then attempt reconnect to device
                }
                else
                {
                    cout << "SLM read failed, retrying...\n";
                    usleep(10000);
                    goto SLM_CMD2;
                }
            }

            qApp->processEvents();
        }

        check_faults(); // update tree widget and respond to any device faults
        //QTimer::singleShot(100, this, SLOT(check_faults())); // wait 0.1s to read faults and prevent read errors

        last_key = key;
    }

    if(key-last_log_key > log_rate)
    {
        // update data log
        if(spc_enabled)
            out_str = press_str;
        else
            out_str = "NaN";

        if(logging) // if logging enabled, record everything
        {
            if(dxm_enabled)
                out_str += "," + to_string(dxm_curr) + "," + to_string(dxm_volt) + "," + to_string(fil_curr);
            else
                out_str += ",NaN,NaN,NaN";

            if(labjack_enabled)
                out_str += "," + to_string(diode_volt) + "," + to_string(ain0) + "," + to_string(ain1);
            else
                out_str += ",NaN,NaN,NaN";

            if(slm_enabled)
                out_str += "," + to_string(slm_volt) + "," + to_string(slm_curr);
            else
                out_str += ",NaN,NaN";

            if(fms_enabled)
                out_str += "," + to_string(freq);
            else
                out_str += ",NaN";
        }
        data_log_write(out_str);
    }
}

void MainWindow::dxm_recon()
{
    ethReconnect(&::ED_DXM);
}

void MainWindow::slm_recon()
{
    ethReconnect(&::ED_SLM);
}

void MainWindow::rigol_recon()
{
    ethReconnect(&::ED_RIGOL);
}

void MainWindow::spc_recon()
{
    ethReconnect(&::ED_SPCE);
}

void MainWindow::fms_recon()
{
    ethReconnect(&::ED_FMS);
}

void MainWindow::ethReconnect(ethdevice *dev)
{
    bool ready_to_log = false;
    int probe_tries = 0;

    dev->recon_tick(); // increment reconnection counter for this device

    if(dev->get_recon_count() == 0) // if this is the first attempt
    {
        if(dev == &::ED_DXM) // if it's the DXM disable all PID and ramping
        {
            freq_pid_on = false;
            power_pid_on = false;
            beam_curr_pid_on = false;
            ramping_up = false;
            ramping_down = false;
            dxm_hv_on = false;

            ui->ramp_down_button->setEnabled(true);
            ui->ramp_down_button->setStyleSheet(green_ramp_button);
            ui->ramp_down_button->setText("RAMP DOWN ↓");
            ui->ramp_up_button->setEnabled(true);
            ui->ramp_up_button->setStyleSheet(green_ramp_button);
            ui->ramp_up_button->setText("↑ RAMP UP");
            ui->freq_pid_button->setEnabled(true);
            ui->freq_pid_button->setStyleSheet(green_auto_button);
            ui->freq_pid_button->setText("FREQUENCY");
            ui->power_pid_button->setEnabled(true);
            ui->power_pid_button->setStyleSheet(green_auto_button);
            ui->power_pid_button->setText("POWER");
            ui->beam_curr_pid_button->setEnabled(true);
            ui->beam_curr_pid_button->setStyleSheet(green_auto_button);
            ui->beam_curr_pid_button->setText("CURRENT");
            ui->dxm_hv_button->setStyleSheet(green_hv_button);
            ui->dxm_hv_button->setText("ENABLE\nHV");

            ui->fil_curr_button->setEnabled(true);
            ui->fil_curr_button->setText("✎");
            ui->fil_curr_button->setFont(button_font);
            ui->fil_curr_edit->setReadOnly(true);
            ui->fil_curr_button->setStyleSheet(green_edit_button);
            ui->beam_volt_button->setEnabled(true);
            ui->beam_volt_button->setText("✎");
            ui->beam_volt_button->setFont(button_font);
            ui->beam_volt_edit->setReadOnly(true);
            ui->beam_volt_button->setStyleSheet(green_edit_button);
            ui->beam_curr_button->setEnabled(false);
            ui->beam_curr_edit->setReadOnly(true);
            beam_curr_active = false;
            ui->beam_curr_button->setStyleSheet(green_edit_button);
            power_active = false;
            ui->power_button->setEnabled(false);
            ui->power_edit->setReadOnly(true);
            ui->power_button->setStyleSheet(green_edit_button);
        }
        else if(dev == &ED_FMS && freq_pid_on) // if it's the FMS disable frequency PID
        {
            freq_pid_on = false;
            ui->ramp_down_button->setEnabled(true);
            ui->ramp_up_button->setEnabled(true);
            ui->power_pid_button->setEnabled(true);
            ui->beam_curr_pid_button->setEnabled(true);
            ui->freq_pid_button->setStyleSheet(green_auto_button);
            ui->freq_pid_button->setText("FREQUENCY");
            ui->fil_curr_button->setEnabled(true);
            ui->fil_curr_button->setText("✎");
            ui->fil_curr_button->setFont(button_font);
            ui->fil_curr_edit->setReadOnly(true);
            ui->fil_curr_button->setStyleSheet(green_edit_button);
            ui->beam_volt_button->setEnabled(true);
            ui->beam_volt_button->setText("✎");
            ui->beam_volt_button->setFont(button_font);
            ui->beam_volt_edit->setReadOnly(true);
            ui->beam_volt_button->setStyleSheet(green_edit_button);
        }

        eth_disconnect(dev);
        // send device-appropriate signal to restart function in 0.2 sec
        if(dev == &::ED_DXM)
            QTimer::singleShot(200, this, SLOT(dxm_recon()));
        else if(dev == &::ED_SLM)
            QTimer::singleShot(200, this, SLOT(slm_recon()));
        else if(dev == &::ED_RIGOL)
            QTimer::singleShot(200, this, SLOT(rigol_recon()));
        else if(dev == &::ED_SPCE)
            QTimer::singleShot(200, this, SLOT(spc_recon()));
        else if(dev == &::ED_FMS)
            QTimer::singleShot(200, this, SLOT(fms_recon()));
    }
    else
    {
        status = eth_connect(dev);
        if(status < 0)
        {
            cout << "ERROR failed reconnect attempt " << dev->get_recon_count() << " to " << dev->getname() << endl;
            out_str = "Failed " + dev->getname() + " reconnect attempt " + to_string(dev->get_recon_count());
            action_log_write(out_str);

            if(dev == &::ED_DXM)
            {
                if(dev->get_recon_count() < err_limit2)
                    QTimer::singleShot(1000, this, SLOT(dxm_recon()));
                else
                    ready_to_log = true;
            }
            else if(dev == &::ED_SLM)
            {
                if(dev->get_recon_count() < err_limit1)
                    QTimer::singleShot(1000, this, SLOT(slm_recon()));
                else
                {
                    // DXM HV off
                    out_str = "98,0,";
                    out_str = '\x02' + out_str + '\x03';
                    eth_write(out_str, &::ED_DXM);

                    ready_to_log = true;
                }
            }
            else if(dev == &::ED_RIGOL)
            {
                if(dev->get_recon_count() < err_limit1)
                    QTimer::singleShot(1000, this, SLOT(rigol_recon()));
                else
                {
                    // DXM HV off
                    out_str = "98,0,";
                    out_str = '\x02' + out_str + '\x03';
                    eth_write(out_str, &::ED_DXM);

                    ready_to_log = true;
                }
            }
            else if(dev == &::ED_SPCE)
            {
                if(dev->get_recon_count() < err_limit2)
                {
                    if(dev->get_recon_count() >= err_limit1)
                    {
                        // disable all PID and ramping
                        freq_pid_on = false;
                        power_pid_on = false;
                        beam_curr_pid_on = false;
                        ramping_up = false;
                        ramping_down = false;
                        dxm_hv_on = false;

                        ui->ramp_down_button->setEnabled(true);
                        ui->ramp_down_button->setStyleSheet(green_ramp_button);
                        ui->ramp_down_button->setText("RAMP DOWN ↓");
                        ui->ramp_up_button->setEnabled(true);
                        ui->ramp_up_button->setStyleSheet(green_ramp_button);
                        ui->ramp_up_button->setText("↑ RAMP UP");
                        ui->freq_pid_button->setEnabled(true);
                        ui->freq_pid_button->setStyleSheet(green_auto_button);
                        ui->freq_pid_button->setText("FREQUENCY");
                        ui->power_pid_button->setEnabled(true);
                        ui->power_pid_button->setStyleSheet(green_auto_button);
                        ui->power_pid_button->setText("POWER");
                        ui->beam_curr_pid_button->setEnabled(true);
                        ui->beam_curr_pid_button->setStyleSheet(green_auto_button);
                        ui->beam_curr_pid_button->setText("CURRENT");
                        ui->dxm_hv_button->setStyleSheet(green_hv_button);
                        ui->dxm_hv_button->setText("ENABLE\nHV");

                        ui->fil_curr_button->setEnabled(true);
                        ui->fil_curr_button->setText("✎");
                        ui->fil_curr_button->setFont(button_font);
                        ui->fil_curr_edit->setReadOnly(true);
                        ui->fil_curr_button->setStyleSheet(green_edit_button);
                        ui->beam_volt_button->setEnabled(true);
                        ui->beam_volt_button->setText("✎");
                        ui->beam_volt_button->setFont(button_font);
                        ui->beam_volt_edit->setReadOnly(true);
                        ui->beam_volt_button->setStyleSheet(green_edit_button);
                        ui->beam_curr_button->setEnabled(false);
                        ui->beam_curr_edit->setReadOnly(true);
                        ui->beam_curr_button->setStyleSheet(green_edit_button);
                        beam_curr_active = false;
                        ui->power_button->setEnabled(false);
                        ui->power_edit->setReadOnly(true);
                        ui->power_button->setStyleSheet(green_edit_button);
                        power_active = false;

                        // DXM HV off
                        out_str = "98,0,";
                        out_str = '\x02' + out_str + '\x03';
                        eth_write(out_str, &::ED_DXM);
                    }
                    QTimer::singleShot(1000, this, SLOT(spc_recon()));
                }
                else
                {
                    e_ramping = true;
                    fil_curr_sp = fil_curr;
                    e_ramp_rate = (fil_curr/5)*refresh_rate; // set emergency ramp rate to ramp down in 5 seconds

                    ui->ramp_down_button->setEnabled(false);
                    ui->ramp_up_button->setEnabled(false);
                    ui->beam_curr_pid_button->setEnabled(false);
                    ui->power_pid_button->setEnabled(false);
                    ui->freq_pid_button->setEnabled(false);
                    ui->dxm_hv_button->setEnabled(false);

                    ui->fil_curr_button->setEnabled(false);
                    ui->fil_curr_button->setText("set: ");
                    ui->fil_curr_button->setFont(small_button_font);
                    ui->fil_curr_edit->setReadOnly(true);
                    ui->fil_curr_button->setStyleSheet(orange_pid_button);
                    ui->beam_volt_button->setEnabled(false);
                    ui->beam_volt_button->setText("✎");
                    ui->beam_volt_button->setFont(button_font);
                    ui->beam_volt_edit->setReadOnly(true);
                    ui->beam_volt_button->setStyleSheet(green_edit_button);

                    cout << "beginning emergency ramp down\n";
                    ready_to_log = true;
                }
            }
            else if(dev == &::ED_FMS)
            {
                if(dev->get_recon_count() < err_limit1)
                    QTimer::singleShot(1000, this, SLOT(fms_recon()));
                else
                    ready_to_log = true;
            }

            if(ready_to_log)
            {
                out_str = "Failed " + to_string(err_limit2) + " attempts to reconnect to " + dev->getname();
                action_log_write(out_str);
                if(dev == &::ED_SPCE)
                {
                    action_log_write("Beginning emergency ramp down of filament");
                }
            }
        }
        else
        {
            cout << "successfully reconnected to " << dev->getname() << endl;

            if(dev == &::ED_DXM)
            {
                while(status != 1 && probe_tries < 2)
                {
                    out_str = "99,1,";
                    out_str = "\x02" + out_str + "\x03";
                    status = ethprobe(&::ED_DXM,out_str,"$");
                    probe_tries++;
                }
                if(status == 1)
                {
                    dxm_enabled = true;
                    ui->dxm_group->setEnabled(true);
                    ui->pid_group->setEnabled(true);
                }
                else
                {
                    eth_disconnect(dev);
                    QTimer::singleShot(200, this, SLOT(dxm_recon()));
                }
            }
            else if(dev == &::ED_SLM)
            {
                while(status != 1 && probe_tries < 2)
                {
                    out_str = "99,1,";
                    out_str = "\x02" + out_str + "\x03";
                    status = ethprobe(&::ED_SLM,out_str,"$");
                    probe_tries++;
                }
                if(status == 1)
                {
                    slm_enabled = true;
                    ui->slm_group->setEnabled(true);
                }
                else
                {
                    eth_disconnect(dev);
                    QTimer::singleShot(200, this, SLOT(slm_recon()));
                }
            }
            else if(dev == &::ED_RIGOL)
            {
                while(status != 1 && probe_tries < 2)
                {
                    status = ethprobe(&::ED_RIGOL,"*IDN?\r\n","DP821A");
                    probe_tries++;
                }
                if(status == 1)
                {
                    rigol1_enabled = rigol1_enabled_init; // restore to initial value
                    rigol2_enabled = rigol2_enabled_init;
                    if(rigol1_enabled) ui->rigol1_group->setEnabled(true);
                    if(rigol2_enabled) ui->rigol2_group->setEnabled(true);
                }
                else
                {
                    eth_disconnect(dev);
                    QTimer::singleShot(200, this, SLOT(rigol_recon()));
                }
            }
            else if(dev == &::ED_SPCE)
            {
                while(status != 1 && probe_tries < 2)
                {
                    status = ethprobe(&::ED_SPCE,"spc 01\r\n","00");
                    probe_tries++;
                }
                if(status == 1)
                {
                    spc_enabled = true;
                    ui->press_frame->setEnabled(true);
                }
                else
                {
                    eth_disconnect(dev);
                    QTimer::singleShot(200, this, SLOT(spc_recon()));
                }
            }
            else if(dev == &::ED_FMS)
            {
                status = 1; // swap this with a proper re-probe operation later
                fms_enabled = true;
                ui->fms_group->setEnabled(true);
                ui->freq_pid_button->setEnabled(true);
            }

            if(status != 1)
            {
                cout << dev->getname() << " re-probe failed on reconnect attempt " << dev->get_recon_count() << "\n";
                out_str = "Reconnected but failed reprobe of " + dev->getname() + " on attempt " + to_string(dev->get_recon_count());
                action_log_write(out_str);
            }
            else
            {
                dev->reset_recon_count();
                cout << "successfully reprobed " << dev->getname() << endl;
                out_str = "Successfully reconnected to and reprobed " + dev->getname();
                action_log_write(out_str);
            }
        }
    }
}

void MainWindow::ljReconnect()
{
    lj_recon_count++;

    if(lj_recon_count == 0)
    {
        daq.lj_close();
        QTimer::singleShot(200, this, SLOT(ljReconnect())); // restart function in 0.2 sec
    }
    else
    {
        if(daq.lj_init() < 0)
        {
            cout << "ERROR failed reconnect attempt " << lj_recon_count << " to LabJack\n";
            if(lj_recon_count < err_limit1)
            {
                QTimer::singleShot(1000, this, SLOT(ljReconnect())); // try again in 1 sec
            }
            else
            {
                out_str = "Failed " + to_string(err_limit1) + " attempts to reconnect to LabJack";
                action_log_write(out_str);
            }
        }
        else
        {
            cout << "successfully reconnected to LabJack\n";
            lj_recon_count = -1;
            labjack_enabled = true;
            ui->lj_curr_group->setEnabled(true);
        }
    }
}

void MainWindow::parse_config()
{
    ifstream file("config"); // load config file
    string line;
    istringstream ss;

    while (getline(file, line)) // read line by line and store values as they are found
    {
        if (line.find("Current PID Proportional") != std::string::npos)
        {
            kp = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Current PID Integral") != std::string::npos)
        {
            ki = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Current PID Derivative") != std::string::npos)
        {
            kd = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Frequency PID Proportional") != std::string::npos)
        {
            fkp = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Frequency PID Integral") != std::string::npos)
        {
            fki = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Frequency PID Derivative") != std::string::npos)
        {
            fkd = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Filament Ramp Set Point") != std::string::npos)
        {
            ramp_sp = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Normal Ramp Time") != std::string::npos)
        {
            ramp_time = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Emergency Ramp Time") != std::string::npos)
        {
            e_ramp_time = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Fatal Pressure Limit") != std::string::npos)
        {
            ss.clear();
            ss.str(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
            ss >> fatal_press;
        }
        else if (line.find("Filament Pressure High Limit") != std::string::npos) // pressure max/min use string streams b/c variable will be in scietific notation
        {
            ss.clear();
            ss.str(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
            ss >> press_bound3;
        }
        else if (line.find("Filament Pressure Low Limit") != std::string::npos)
        {
            ss.clear();
            ss.str(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
            ss >> press_bound2;
        }
        else if (line.find("HV Pressure Low Limit") != std::string::npos)
        {
            ss.clear();
            ss.str(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
            ss >> press_bound1;
        }
        else if (line.find("Beam Current Limit") != std::string::npos)
        {
            dxm_curr_limit = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Output Power Limit") != std::string::npos)
        {
            power_limit = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Power Meter Calibration") != std::string::npos)
        {
            power_calibrate = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Error Count First Limit") != std::string::npos)
        {
            err_limit1 = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
        else if (line.find("Error Count Second Limit") != std::string::npos)
        {
            err_limit2 = stod(line.substr(line.find("[")+1,(line.find("]")-line.find("[")-1)));
        }
    }

    ramp_rate = (ramp_sp/(ramp_time*60))*refresh_rate; // calculate ramp rate from time and sp
    e_ramp_rate = (3/e_ramp_time)*refresh_rate;

    if(kp < 0 || ki < 0 || kd < 0 || ramp_rate < 0 || e_ramp_rate < 0 || fkp < 0 || fki < 0 || fkd < 0)
        cout << "ERROR reading from pid config file\n"; // need to throw exception here eventually

    cout << "Kp=" << kp << ", Ki=" << ki << ", Kd=" << kd << endl;
    cout << "fKp=" << fkp << ", fKi=" << fki << ", fKd=" << fkd << endl;
    cout << "Ramp SP=" << ramp_sp << ", Ramp Time=" << ramp_time << ", Ramp Rate=" << ramp_rate << endl;
    cout << "Fatal Pressure=" << fatal_press << ", Fil Limit High=" << press_bound3 << endl;
    cout << "Fil Limit Low/HV High=" << press_bound2 << ", HV Limit Low=" << press_bound1 << endl;
    cout << "Beam Current Limit=" << dxm_curr_limit << ", Power Limit=" << power_limit << endl;
    cout << "Power Meter Calibration=" << power_calibrate << endl;
    cout << "First Error Limit=" << err_limit1 << ", Second Error Limit=" << err_limit2 << endl;

    ui->beam_kp_write->setText(QString::number(kp));
    ui->beam_ki_write->setText(QString::number(ki));
    ui->beam_kd_write->setText(QString::number(kd));
    ui->ramp_sp_write->setText(QString::number(ramp_sp));
    ui->ramp_time_write->setText(QString::number(ramp_time));
    ui->fkp_write->setText(QString::number(fkp));
    ui->fki_write->setText(QString::number(fki));
    ui->fkd_write->setText(QString::number(fkd));

    file.close();
    pid = new PID(kp, ki, kd); // initialize PID controller with these variables
    fpid = new PID(fkp, fki, fkd); // initialize frequency PID controller
}

void MainWindow::check_faults()
{
    bool first_fault;

    // update each child item
    if(dxm_arc)
        ui->treeWidget->topLevelItem(0)->child(0)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(0)->child(0)->setIcon(0, QIcon("images/circle_green.png"));

    if(dxm_over_temp)
        ui->treeWidget->topLevelItem(0)->child(1)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(0)->child(1)->setIcon(0, QIcon("images/circle_green.png"));

    if(dxm_over_volt)
        ui->treeWidget->topLevelItem(0)->child(2)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(0)->child(2)->setIcon(0, QIcon("images/circle_green.png"));

    if(dxm_under_volt)
        ui->treeWidget->topLevelItem(0)->child(3)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(0)->child(3)->setIcon(0, QIcon("images/circle_green.png"));

    if(dxm_over_curr)
        ui->treeWidget->topLevelItem(0)->child(4)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(0)->child(4)->setIcon(0, QIcon("images/circle_green.png"));

    if(dxm_under_curr)
        ui->treeWidget->topLevelItem(0)->child(5)->setIcon(0, QIcon("images/circle_orange.png"));
    else
        ui->treeWidget->topLevelItem(0)->child(5)->setIcon(0, QIcon("images/circle_green.png"));

    // update top level item
    if(dxm_arc || dxm_over_temp || dxm_over_volt || dxm_under_volt || dxm_over_curr)
        ui->treeWidget->topLevelItem(0)->setIcon(0, QIcon("images/circle_red.png"));
    else if(dxm_under_curr)
        ui->treeWidget->topLevelItem(0)->setIcon(0, QIcon("images/circle_orange.png"));
    else
        ui->treeWidget->topLevelItem(0)->setIcon(0, QIcon("images/circle_green.png"));

    // update each child item
    if(slm_arc)
        ui->treeWidget->topLevelItem(1)->child(0)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(1)->child(0)->setIcon(0, QIcon("images/circle_green.png"));

    if(slm_over_temp)
        ui->treeWidget->topLevelItem(1)->child(1)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(1)->child(1)->setIcon(0, QIcon("images/circle_green.png"));

    if(slm_over_volt)
        ui->treeWidget->topLevelItem(1)->child(2)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(1)->child(2)->setIcon(0, QIcon("images/circle_green.png"));

    if(slm_under_volt)
        ui->treeWidget->topLevelItem(1)->child(3)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(1)->child(3)->setIcon(0, QIcon("images/circle_green.png"));

    if(slm_over_curr)
        ui->treeWidget->topLevelItem(1)->child(4)->setIcon(0, QIcon("images/circle_red.png"));
    else
        ui->treeWidget->topLevelItem(1)->child(4)->setIcon(0, QIcon("images/circle_green.png"));

    if(slm_under_curr)
        ui->treeWidget->topLevelItem(1)->child(5)->setIcon(0, QIcon("images/circle_orange.png"));
    else
        ui->treeWidget->topLevelItem(1)->child(5)->setIcon(0, QIcon("images/circle_green.png"));

    // update top level item
    if(slm_arc || slm_over_temp || slm_over_volt || slm_under_volt || slm_over_curr)
        ui->treeWidget->topLevelItem(1)->setIcon(0, QIcon("images/circle_red.png"));
    else if(slm_under_curr)
        ui->treeWidget->topLevelItem(1)->setIcon(0, QIcon("images/circle_orange.png"));
    else
        ui->treeWidget->topLevelItem(1)->setIcon(0, QIcon("images/circle_green.png"));

    // dxm fatal fault handling
    if(!dxm_fault_triggered && (dxm_arc || dxm_over_temp || dxm_over_volt || dxm_under_volt || dxm_over_curr))
    {
        dxm_fault_triggered = true;
        first_fault = true;

        soft_kill();

        out_str = "ERROR fatal fault from DXM: ";
        if(dxm_arc)
        {
            out_str += "arc";
            first_fault = false;
        }
        if(dxm_over_temp)
        {
            if(!first_fault)
                out_str += ", ";
            out_str += "over temp";
            first_fault = false;
        }
        if(dxm_over_volt)
        {
            if(!first_fault)
                out_str += ", ";
            out_str += "over voltage";
            first_fault = false;
        }
        if(dxm_under_volt)
        {
            if(!first_fault)
                out_str += ", ";
            out_str += "under voltage";
            first_fault = false;
        }
        if(dxm_over_curr)
        {
            if(!first_fault)
                out_str += ", ";
            out_str += "over current";
            first_fault = false;
        }

        cout << out_str << endl;
        ui->statusBar->showMessage(QString::fromStdString(out_str));
        action_log_write(out_str);
    }

    // slm fatal fault handling
    if(!slm_fault_triggered && (slm_arc || slm_over_temp || slm_over_volt || slm_under_volt || slm_over_curr))
    {
        slm_fault_triggered = true;
        first_fault = true;

        soft_kill();

        // turn off HV on SLM
        out_str = "98,0,";
        out_str = '\x02' + out_str + '\x03';
        eth_write(out_str, &::ED_SLM);

        out_str = "ERROR fatal fault from SLM: ";
        if(slm_arc)
        {
            out_str += "arc";
            first_fault = false;
        }
        if(slm_over_temp)
        {
            if(!first_fault)
                out_str += ", ";
            out_str += "over temp";
            first_fault = false;
        }
        if(slm_over_volt)
        {
            if(!first_fault)
                out_str += ", ";
            out_str += "over voltage";
            first_fault = false;
        }
        if(slm_under_volt)
        {
            if(!first_fault)
                out_str += ", ";
            out_str += "under voltage";
            first_fault = false;
        }
        if(slm_over_curr)
        {
            if(!first_fault)
                out_str += ", ";
            out_str += "over current";
            first_fault = false;
        }

        cout << out_str << endl;
        ui->statusBar->showMessage(QString::fromStdString(out_str));
        action_log_write(out_str);
    }

    // this will change the text color of the faults tab if a fatal fault occurs
    if(dxm_arc || dxm_over_temp || dxm_over_volt || dxm_under_volt || dxm_over_curr || slm_arc || slm_over_temp || slm_over_volt || slm_under_volt || slm_over_curr)
    {
        ui->tabWidget->tabBar()->setTabTextColor(3, QColor(209,96,85,255));
    }
    else
    {
        ui->tabWidget->tabBar()->setTabTextColor(3, QColor(0,0,0,255));
    }
}

void MainWindow::soft_kill()
{
    out_str = "98,0,";
    out_str = '\x02' + out_str + '\x03';
    eth_write(out_str, &::ED_DXM);

    freq_pid_on = false;
    power_pid_on = false;
    beam_curr_pid_on = false;
    ramping_up = false;
    ramping_down = false;
    dxm_hv_on = false;

    ui->ramp_down_button->setEnabled(true);
    ui->ramp_down_button->setStyleSheet(green_ramp_button);
    ui->ramp_down_button->setText("RAMP DOWN ↓");
    ui->ramp_up_button->setEnabled(true);
    ui->ramp_up_button->setStyleSheet(green_ramp_button);
    ui->ramp_up_button->setText("↑ RAMP UP");
    ui->freq_pid_button->setEnabled(true);
    ui->freq_pid_button->setStyleSheet(green_auto_button);
    ui->freq_pid_button->setText("FREQUENCY");
    ui->power_pid_button->setEnabled(true);
    ui->power_pid_button->setStyleSheet(green_auto_button);
    ui->power_pid_button->setText("POWER");
    ui->beam_curr_pid_button->setEnabled(true);
    ui->beam_curr_pid_button->setStyleSheet(green_auto_button);
    ui->beam_curr_pid_button->setText("CURRENT");
    ui->dxm_hv_button->setStyleSheet(green_hv_button);
    ui->dxm_hv_button->setText("ENABLE\nHV");

    ui->fil_curr_button->setEnabled(true);
    ui->fil_curr_button->setText("✎");
    ui->fil_curr_button->setFont(button_font);
    ui->fil_curr_edit->setReadOnly(true);
    ui->fil_curr_button->setStyleSheet(green_edit_button);
    ui->beam_volt_button->setEnabled(true);
    ui->beam_volt_button->setText("✎");
    ui->beam_volt_button->setFont(button_font);
    ui->beam_volt_edit->setReadOnly(true);
    ui->beam_volt_button->setStyleSheet(green_edit_button);
    ui->beam_curr_button->setEnabled(false);
    ui->beam_curr_edit->setReadOnly(true);
    ui->beam_curr_button->setStyleSheet(green_edit_button);
    beam_curr_active = false;
    ui->power_button->setEnabled(false);
    ui->power_edit->setReadOnly(true);
    ui->power_button->setStyleSheet(green_edit_button);
    power_active = false;
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Gyrotron Controls"),
                                                                tr("Are you sure you want to quit?\n\nAll devices will be disconnected in their current state.\n"),
                                                                QMessageBox::Cancel | QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        shutdown();
        event->accept();
    }
}

void MainWindow::rigol1_set_curr_true()
{
    // this is a slot that only exists to allow for the delay of the enclosed function
    rigol1_set_curr(true);
}

void MainWindow::rigol1_set_volt_true()
{
    // this is a slot that only exists to allow for the delay of the enclosed function
    rigol1_set_volt(true);
}

void MainWindow::rigol2_set_curr_true()
{
    // this is a slot that only exists to allow for the delay of the enclosed function
    rigol2_set_curr(true);
}

void MainWindow::rigol2_set_volt_true()
{
    // this is a slot that only exists to allow for the delay of the enclosed function
    rigol2_set_volt(true);
}

void MainWindow::rigol1_set_curr(bool toggle_back)
{
    out_str = ":CURR " + ui->shim_curr_edit->text().toStdString() + "\r\n";
    recv_str = eth_write(out_str, &::ED_RIGOL);
    if(recv_str.find("!ERR") == string::npos) // check for errors
    {
        shim_curr_sp = ui->shim_curr_edit->text().toDouble();
        shim_curr_active = false;
        ui->shim_curr_edit->setReadOnly(true);
        ui->shim_curr_edit->setStyleSheet(clear_edit);
        ui->shim_curr_edit->setText("");
        ui->shim_curr_button->setStyleSheet(green_edit_button);
        ui->shim_curr_button->setText("✎");
    }
    else
    {
        ui->shim_curr_edit->setStyleSheet(error_edit);
        ui->statusBar->showMessage("ERROR could not write to Rigol");
    }
    if(toggle_back) rigol1_enabled = true;
}

void MainWindow::rigol1_set_volt(bool toggle_back)
{
    out_str = ":VOLT " + ui->shim_volt_edit->text().toStdString() + "\r\n";
    recv_str = eth_write(out_str, &::ED_RIGOL);
    if(recv_str.find("!ERR") == string::npos) // check for errors
    {
        shim_volt_sp = ui->shim_volt_edit->text().toDouble();
        shim_volt_active = false;
        ui->shim_volt_edit->setReadOnly(true);
        ui->shim_volt_edit->setStyleSheet(clear_edit);
        ui->shim_volt_edit->setText("");
        ui->shim_volt_button->setStyleSheet(green_edit_button);
        ui->shim_volt_button->setText("✎");
    }
    else
    {
        ui->shim_volt_edit->setStyleSheet(error_edit);
        ui->statusBar->showMessage("ERROR could not write to Rigol");
    }
    if(toggle_back) rigol1_enabled = true;
}

void MainWindow::rigol2_set_volt(bool toggle_back)
{
    out_str = ":VOLT " + ui->trim_volt_edit->text().toStdString() + "\r\n";
    recv_str = eth_write(out_str, &::ED_RIGOL);
    if(recv_str.find("!ERR") == string::npos) // check for errors
    {
        trim_volt_sp = ui->trim_volt_edit->text().toDouble();
        trim_volt_active = false;
        ui->trim_volt_edit->setReadOnly(true);
        ui->trim_volt_edit->setStyleSheet(clear_edit);
        ui->trim_volt_edit->setText("");
        ui->trim_volt_button->setStyleSheet(green_edit_button);
        ui->trim_volt_button->setText("✎");
    }
    else
    {
        ui->trim_volt_edit->setStyleSheet(error_edit);
        ui->statusBar->showMessage("ERROR could not write to Rigol");
    }
    if(toggle_back) rigol1_enabled = true;
}

void MainWindow::rigol2_set_curr(bool toggle_back)
{
    out_str = ":CURR " + ui->trim_curr_edit->text().toStdString() + "\r\n";
    recv_str = eth_write(out_str, &::ED_RIGOL);
    if(recv_str.find("!ERR") == string::npos) // check for errors
    {
        trim_curr_sp = ui->trim_curr_edit->text().toDouble();
        trim_curr_active = false;
        ui->trim_curr_edit->setReadOnly(true);
        ui->trim_curr_edit->setStyleSheet(clear_edit);
        ui->trim_curr_edit->setText("");
        ui->trim_curr_button->setStyleSheet(green_edit_button);
        ui->trim_curr_button->setText("✎");
    }
    else
    {
        ui->trim_curr_edit->setStyleSheet(error_edit);
        ui->statusBar->showMessage("ERROR could not write to Rigol");
    }
    if(toggle_back) rigol2_enabled = true;
}

void MainWindow::on_dxm_hv_button_clicked()
{
    if(dxm_enabled)
    {
        if(dxm_hv_on)
        {
            // turn off HV
            out_str = "98,0,";
            out_str = '\x02' + out_str + '\x03';
            recv_str = eth_write(out_str, &::ED_DXM);

            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                dxm_hv_on = false;
                ui->dxm_hv_button->setStyleSheet(green_hv_button);
                ui->dxm_hv_button->setText("ENABLE\nHV");

                // turn off all PID
                freq_pid_on = false;
                power_pid_on = false;
                beam_curr_pid_on = false;
                ui->freq_pid_button->setStyleSheet(green_auto_button);
                ui->freq_pid_button->setText("FREQUENCY");
                ui->power_pid_button->setStyleSheet(green_auto_button);
                ui->power_pid_button->setText("POWER");
                ui->beam_curr_pid_button->setStyleSheet(green_auto_button);
                ui->beam_curr_pid_button->setText("CURRENT");
                ui->freq_pid_button->setEnabled(false);
                ui->beam_curr_pid_button->setEnabled(false);
                ui->power_pid_button->setEnabled(false);
                ui->fil_curr_button->setEnabled(true);
                ui->fil_curr_button->setText("✎");
                ui->fil_curr_button->setFont(button_font);
                ui->fil_curr_edit->setReadOnly(true);
                ui->fil_curr_button->setStyleSheet(green_edit_button);
                ui->beam_volt_button->setEnabled(true);
                ui->beam_volt_button->setText("✎");
                ui->beam_volt_button->setFont(button_font);
                ui->beam_volt_edit->setReadOnly(true);
                ui->beam_volt_button->setStyleSheet(green_edit_button);
                ui->beam_curr_button->setEnabled(false);
                ui->beam_curr_edit->setReadOnly(true);
                ui->beam_curr_button->setStyleSheet(green_edit_button);
                beam_curr_active = false;
                ui->power_button->setEnabled(false);
                ui->power_edit->setReadOnly(true);
                ui->power_button->setStyleSheet(green_edit_button);
                power_active = false;
                ui->dxm_group->setTitle("Cathode");
            }
            else
                ui->statusBar->showMessage("ERROR could not write to DXM");
        }
        else
        {
            // turn on HV
            out_str = "98,1,";
            out_str = '\x02' + out_str + '\x03';
            recv_str = eth_write(out_str, &::ED_DXM);

            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                dxm_hv_on = true;
                ui->dxm_hv_button->setStyleSheet(orange_hv_button);
                ui->dxm_hv_button->setText("DISABLE\nHV");

                // enable PID buttons
                if(fms_enabled) ui->freq_pid_button->setEnabled(true);
                ui->beam_curr_pid_button->setEnabled(true);
                ui->power_pid_button->setEnabled(true);
                ui->dxm_group->setTitle("Cathode ⚡");
            }
            else
                ui->statusBar->showMessage("ERROR could not write to DXM");
        }
    }
}

void MainWindow::on_dxm_reset_button_clicked()
{
    if(dxm_enabled)
    {
        out_str = "31,";
        out_str = '\x02' + out_str + '\x03';
        recv_str = eth_write(out_str, &::ED_DXM);

        if(recv_str.find("!ERR") == string::npos) // check for errors
        {
            ui->statusBar->showMessage("Successfully reset faults on DXM supply");
            dxm_fault_triggered = false;
        }
        else
            ui->statusBar->showMessage("ERROR could not write to DXM");
    }
}

void MainWindow::on_slm_hv_button_clicked()
{
    if(slm_enabled)
    {
        if(slm_hv_on)
        {
            out_str = "98,0,";
            out_str = '\x02' + out_str + '\x03';
            recv_str = eth_write(out_str, &::ED_SLM);

            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                slm_hv_on = false;
                ui->slm_hv_button->setStyleSheet(green_hv_button);
                ui->slm_hv_button->setText("ENABLE\nHV");
                ui->slm_group->setTitle("Mod Anode");
            }
            else
                ui->statusBar->showMessage("ERROR could not write to SLM");
        }
        else
        {
            out_str = "98,1,";
            out_str = '\x02' + out_str + '\x03';
            recv_str = eth_write(out_str, &::ED_SLM);

            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                slm_hv_on = true;
                ui->slm_hv_button->setStyleSheet(orange_hv_button);
                ui->slm_hv_button->setText("DISABLE\nHV");
                ui->slm_group->setTitle("Mod Anode ⚡");
            }
            else
                ui->statusBar->showMessage("ERROR could not write to SLM");
        }
    }
}

void MainWindow::on_slm_reset_button_clicked()
{
    if(slm_enabled)
    {
        out_str = "31,";
        out_str = '\x02' + out_str + '\x03';
        recv_str = eth_write(out_str, &::ED_SLM);

        if(recv_str.find("!ERR") == string::npos) // check for errors
        {
            ui->statusBar->showMessage("Successfully reset faults on SLM supply");
            slm_fault_triggered = false;
        }
        else
            ui->statusBar->showMessage("ERROR could not write to SLM");
    }
}

void MainWindow::on_rigol1_hv_button_clicked()
{
    if(rigol1_enabled)
    {
        if(rigol1_hv_on)
        {
            recv_str = eth_write(":OUTP CH1,OFF\r\n", &::ED_RIGOL);

            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                rigol1_hv_on = false;
                ui->rigol1_hv_button->setStyleSheet(green_hv_button);
                ui->rigol1_hv_button->setText("ENABLE\nSHIM");
                ui->rigol1_group->setTitle("Shim Coil");
            }
            else
                ui->statusBar->showMessage("ERROR could not write to Rigol");
        }
        else
        {
            recv_str = eth_write(":OUTP CH1,ON\r\n", &::ED_RIGOL);

            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                rigol1_hv_on = true;
                ui->rigol1_hv_button->setStyleSheet(orange_hv_button);
                ui->rigol1_hv_button->setText("DISABLE\nSHIM");
                ui->rigol1_group->setTitle("Shim Coil ⚡");
            }
            else
                ui->statusBar->showMessage("ERROR could not write to Rigol");
        }
    }
}

void MainWindow::on_rigol2_hv_button_clicked()
{
    if(rigol2_enabled)
    {
        if(rigol2_hv_on)
        {
            recv_str = eth_write(":OUTP CH2,OFF\r\n", &::ED_RIGOL);

            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                rigol2_hv_on = false;
                ui->rigol2_hv_button->setStyleSheet(green_hv_button);
                ui->rigol2_hv_button->setText("ENABLE\nSHIM");
                ui->rigol2_group->setTitle("Cavity Trim Coil");
            }
            else
                ui->statusBar->showMessage("ERROR could not write to Rigol");
        }
        else
        {
            recv_str = eth_write(":OUTP CH2,ON\r\n", &::ED_RIGOL);

            if(recv_str.find("!ERR") == string::npos) // check for errors
            {
                rigol2_hv_on = true;
                ui->rigol2_hv_button->setStyleSheet(orange_hv_button);
                ui->rigol2_hv_button->setText("DISABLE\nCTC");
                ui->rigol2_group->setTitle("Cavity Trim Coil ⚡");
            }
            else
                ui->statusBar->showMessage("ERROR could not write to Rigol");
        }
    }
}

void MainWindow::on_ramp_up_button_clicked()
{
    if(ramping_up)
    {
        cout << "ramp up stopped\n";
        ui->statusBar->showMessage("Ramp up stopped");
        ramping_up = false;

        // reenable PID buttons if HV is on
        if(dxm_hv_on)
        {
            ui->beam_curr_pid_button->setEnabled(true);
            ui->power_pid_button->setEnabled(true);
            if(fms_enabled) ui->freq_pid_button->setEnabled(true);
        }

        ui->ramp_down_button->setEnabled(true);
        ui->fil_curr_button->setEnabled(true);
        ui->fil_curr_button->setText("✎");
        ui->fil_curr_button->setFont(button_font);
        ui->fil_curr_edit->setReadOnly(true);
        ui->fil_curr_button->setStyleSheet(green_edit_button);
        ui->ramp_up_button->setStyleSheet(green_ramp_button);
        ui->ramp_up_button->setText("↑ RAMP UP");
        action_log_write("filament ramp up stopped");
    }
    else
    {
        cout << "ramp up started\n";
        ui->statusBar->showMessage("Ramp up in progress");
        beam_curr_pid_on = false;
        ramping_up = true;

        // turn off all PID and disable buttons
        freq_pid_on = false;
        power_pid_on = false;
        beam_curr_pid_on = false;
        ui->freq_pid_button->setStyleSheet(green_auto_button);
        ui->freq_pid_button->setText("FREQUENCY");
        ui->power_pid_button->setStyleSheet(green_auto_button);
        ui->power_pid_button->setText("POWER");
        ui->beam_curr_pid_button->setStyleSheet(green_auto_button);
        ui->beam_curr_pid_button->setText("CURRENT");
        ui->beam_volt_button->setEnabled(true);
        ui->beam_volt_button->setText("✎");
        ui->beam_volt_button->setFont(button_font);
        ui->beam_volt_edit->setReadOnly(true);
        ui->beam_volt_button->setStyleSheet(green_edit_button);
        ui->beam_curr_button->setEnabled(false);
        ui->beam_curr_edit->setReadOnly(true);
        ui->beam_curr_button->setStyleSheet(green_edit_button);
        beam_curr_active = false;
        ui->power_button->setEnabled(false);
        ui->power_edit->setReadOnly(true);
        ui->power_button->setStyleSheet(green_edit_button);
        power_active = false;
        ui->beam_curr_pid_button->setEnabled(false);
        ui->power_pid_button->setEnabled(false);
        ui->freq_pid_button->setEnabled(false);

        fil_curr_sp = fil_curr;
        ui->ramp_down_button->setEnabled(false);
        ui->fil_curr_button->setEnabled(false);
        ui->fil_curr_button->setText("set: ");
        ui->fil_curr_button->setFont(small_button_font);
        ui->fil_curr_edit->setReadOnly(true);
        ui->fil_curr_button->setStyleSheet(orange_pid_button);
        ui->ramp_up_button->setStyleSheet(orange_ramp_button);
        ui->ramp_up_button->setText("STOP RAMP");
        action_log_write("filament ramp up started");
    }
}

void MainWindow::on_ramp_down_button_clicked()
{
    if(ramping_down)
    {
        cout << "ramp down stopped\n";
        ui->statusBar->showMessage("Ramp down stopped");
        ramping_down = false;

        // reenable PID buttons
        if(dxm_hv_on)
        {
            ui->beam_curr_pid_button->setEnabled(true);
            ui->power_pid_button->setEnabled(true);
            if(fms_enabled) ui->freq_pid_button->setEnabled(true);
        }

        ui->ramp_up_button->setEnabled(true);
        ui->fil_curr_button->setEnabled(true);
        ui->fil_curr_button->setText("✎");
        ui->fil_curr_button->setFont(button_font);
        ui->fil_curr_edit->setReadOnly(true);
        ui->fil_curr_button->setStyleSheet(green_edit_button);
        ui->ramp_down_button->setStyleSheet(green_ramp_button);
        ui->ramp_down_button->setText("RAMP DOWN ↓");
        action_log_write("filament ramp down stopped");
    }
    else
    {
        cout << "ramp down started\n";
        ui->statusBar->showMessage("Ramp down started");
        ramping_down = true;

        // turn off all PID and disable buttons
        freq_pid_on = false;
        power_pid_on = false;
        beam_curr_pid_on = false;
        ui->freq_pid_button->setStyleSheet(green_auto_button);
        ui->freq_pid_button->setText("FREQUENCY");
        ui->power_pid_button->setStyleSheet(green_auto_button);
        ui->power_pid_button->setText("POWER");
        ui->beam_curr_pid_button->setStyleSheet(green_auto_button);
        ui->beam_curr_pid_button->setText("CURRENT");
        ui->beam_volt_button->setEnabled(true);
        ui->beam_volt_button->setText("✎");
        ui->beam_volt_button->setFont(button_font);
        ui->beam_volt_edit->setReadOnly(true);
        ui->beam_volt_button->setStyleSheet(green_edit_button);
        ui->beam_curr_button->setEnabled(false);
        ui->beam_curr_edit->setReadOnly(true);
        ui->beam_curr_button->setStyleSheet(green_edit_button);
        beam_curr_active = false;
        ui->power_button->setEnabled(false);
        ui->power_edit->setReadOnly(true);
        ui->power_button->setStyleSheet(green_edit_button);
        power_active = false;
        ui->beam_curr_pid_button->setEnabled(false);
        ui->power_pid_button->setEnabled(false);
        ui->freq_pid_button->setEnabled(false);


        fil_curr_sp = fil_curr;
        ui->ramp_up_button->setEnabled(false);
        ui->fil_curr_button->setEnabled(false);
        ui->fil_curr_button->setText("set: ");
        ui->fil_curr_button->setFont(small_button_font);
        ui->fil_curr_edit->setReadOnly(true);
        ui->fil_curr_button->setStyleSheet(orange_pid_button);
        ui->ramp_down_button->setStyleSheet(orange_ramp_button);
        ui->ramp_down_button->setText("STOP RAMP");
        action_log_write("filament ramp down started");
    }
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if(index == 1)
    {
        ui->press_plot->replot();
        ui->beam_plot->replot();
    }
}

void MainWindow::on_beam_curr_pid_button_clicked()
{
    if(dxm_enabled)
    {
        if(beam_curr_pid_on)
        {
            beam_curr_pid_on = false;
            ui->beam_curr_button->setEnabled(false);
            ui->beam_curr_edit->setReadOnly(true);
            ui->beam_curr_button->setStyleSheet(green_edit_button);
            ui->beam_curr_pid_button->setStyleSheet(green_auto_button);
            ui->beam_curr_pid_button->setText("CURRENT");
            ui->fil_curr_button->setEnabled(true);
            ui->fil_curr_button->setText("✎");
            ui->fil_curr_button->setFont(button_font);
            ui->fil_curr_edit->setReadOnly(true);
            ui->fil_curr_button->setStyleSheet(green_edit_button);
            ui->ramp_down_button->setEnabled(true);
            ui->ramp_up_button->setEnabled(true);
            ui->power_pid_button->setEnabled(true);
            if(fms_enabled) ui->freq_pid_button->setEnabled(true);
            action_log_write("beam current PID control turned off");
        }
        else
        {
            beam_curr_pid_on = true;
            ui->beam_curr_button->setEnabled(true);
            ui->beam_curr_edit->setReadOnly(true);
            ui->beam_curr_button->setStyleSheet(green_edit_button);
            ui->beam_curr_pid_button->setStyleSheet(orange_auto_button);
            ui->beam_curr_pid_button->setText("CURRENT");
            ui->fil_curr_button->setEnabled(false);
            ui->fil_curr_button->setText("set: ");
            ui->fil_curr_button->setFont(small_button_font);
            ui->fil_curr_edit->setReadOnly(true);
            ui->fil_curr_button->setStyleSheet(orange_pid_button);
            ui->ramp_down_button->setEnabled(false);
            ui->ramp_up_button->setEnabled(false);
            ui->power_pid_button->setEnabled(false);
            ui->freq_pid_button->setEnabled(false);

            beam_curr_sp = dxm_curr;
            beam_sp_data.push_back(beam_curr_sp);
            beam_time_data.push_back(key);
            ui->beam_curr_edit->setText(QString::number(beam_curr_sp,'f',2));
            pid = new PID(kp, ki, kd); // re-initialize PID controller

            action_log_write("beam current PID control turned on");

            reset_pid_time = true; // if timer is already active then trigger a reset of the dt tracking
        }
    }
}

void MainWindow::on_power_pid_button_clicked()
{
    if(dxm_enabled)
    {
        if(power_pid_on)
        {
            power_pid_on = false;
            ui->power_button->setEnabled(false);
            ui->power_edit->setReadOnly(true);
            ui->power_button->setStyleSheet(green_edit_button);
            power_active = false;
            ui->power_pid_button->setStyleSheet(orange_auto_button);
            ui->power_pid_button->setText("POWER");
            ui->fil_curr_button->setEnabled(true);
            ui->fil_curr_button->setText("✎");
            ui->fil_curr_button->setFont(button_font);
            ui->fil_curr_edit->setReadOnly(true);
            ui->fil_curr_button->setStyleSheet(green_edit_button);
            ui->ramp_down_button->setEnabled(true);
            ui->ramp_up_button->setEnabled(true);
            ui->beam_curr_pid_button->setEnabled(true);
            if(fms_enabled) ui->freq_pid_button->setEnabled(true);
            action_log_write("power PID control turned off");
        }
        else
        {
            power_pid_on = true;
            ui->power_button->setEnabled(true);
            ui->power_edit->setReadOnly(true);
            ui->power_pid_button->setStyleSheet(orange_auto_button);
            ui->power_pid_button->setText("POWER");
            ui->fil_curr_button->setEnabled(false);
            ui->fil_curr_button->setText("✎");
            ui->fil_curr_button->setFont(button_font);
            ui->fil_curr_edit->setReadOnly(true);
            ui->fil_curr_button->setStyleSheet(green_edit_button);
            fil_curr_active = false;
            ui->ramp_down_button->setEnabled(false);
            ui->ramp_up_button->setEnabled(false);
            ui->beam_curr_pid_button->setEnabled(false);
            ui->freq_pid_button->setEnabled(false);

            if(power >= 0) power_sp = power;
            else if(power > 5) power_sp = 5;
            else power_sp = 0;
            //ui->power_edit->setText(QString::number(power_sp));
            pid = new PID(kp, ki, kd); // re-initialize PID controller
            power_sp_data.push_back(power_sp);
            power_time_data.push_back(key);

            action_log_write("power PID control turned on");

            reset_pid_time = true; // if timer is already active then trigger a reset of the dt tracking
        }
    }
}

void MainWindow::new_data_log()
{
    string log_temp_name;

    t = time(nullptr);   // get time now
    now = localtime( & t );
    strftime(timestamp_full, sizeof(timestamp_full), "%Y-%m-%d %R", now);
    dat_file_name = timestamp_full;
    dat_day_tracker = stoi(dat_file_name.substr(8,2)); // store the day in which this log was created to determine when a log file has run over multiple days
    replace( dat_file_name.begin(), dat_file_name.end(), ' ', '@'); // replace the space with an @ to prevent system command errors
    replace( dat_file_name.begin(), dat_file_name.end(), ':', '_'); // replace the replace colon with underscore to make files/folders readable on Windows

    if(num_data_logs < 1)
    {
        // create data file
        data_log_file_name = "logs/" + folder_name + "/" + dat_file_name + ".dat";
        data_log_stream.open(data_log_file_name.c_str(), ios::out | ios::app);
        out_str = "# STARTED DATA LOG at " + string(timestamp_full) + "\n";
    }
    else
    {
        logging = false;
        // close old data file and create new one
        data_log_stream.close();
        log_temp_name = data_log_file_name;
        data_log_file_name = "logs/" + folder_name + "/" + dat_file_name + " (" + to_string(num_data_logs) + ").dat";
        data_log_stream.open(data_log_file_name.c_str(), ios::out | ios::app);
        out_str = "# CONTINUE DATA LOG at " + string(timestamp_full) + ", previous data file: \"" + log_temp_name + "\"\n";
    }
    num_data_logs++;
    out_str += "# Kp=" + to_string(kp) + ", Ki=" + to_string(ki) + ", Kd=" + to_string(kd) + "\n";
    out_str += "# Ramp SP=" + to_string(ramp_sp) + ", Ramp Time=" + to_string(ramp_time) + ", Ramp Rate=" + to_string(ramp_rate) + "\n";
    out_str += "# Max Pressure=" + to_string(press_max) + ", Min Pressure=" + to_string(press_min) + "\n";
    out_str += "# Beam Current Limit=" + to_string(dxm_curr_limit) + ", Power Limit=" + to_string(power_limit) + "\n";
    out_str += "# Power Meter Calibration=" + to_string(power_calibrate) + "\n";
    out_str += "# FORMAT: absolute time (h:m:s), pressure (Torr), beam current (mA), beam voltage (V), filament current (A), power (W), diode power (mV), collector current (mA), body current (mA), mod anode voltage (kV), mod anode current (mA), frequency (GHz)";
    data_log_write(out_str, false);
    logging = true;
}

void MainWindow::new_action_log()
{
    string log_temp_name;

    t = time(nullptr);   // get time now
    now = localtime( & t );
    strftime(timestamp_full, sizeof(timestamp_full), "%Y-%m-%d %R", now);
    log_file_name = timestamp_full;
    log_day_tracker = stoi(log_file_name.substr(8,2)); // store the day in which this log was created to determine when a log file has run over multiple days
    replace( log_file_name.begin(), log_file_name.end(), ' ', '@'); // replace the space with an @ to prevent system command errors
    replace( log_file_name.begin(), log_file_name.end(), ':', '_'); // replace the replace colon with underscore to make files/folders readable on Windows

    string timestamp_str = timestamp_full;
    //int day_current = stoi(timestamp_str.substr(8,2)); // use this later to add support for extra long action log

    if(num_action_logs < 1)
    {
        // create data file
        action_log_file_name = "logs/" + folder_name + "/" + log_file_name + ".log";
        action_log_stream.open(action_log_file_name.c_str(), ios::out | ios::app);
        out_str = "# STARTED ACTION LOG at " + string(timestamp_full) + "\n";
    }
    else
    {
        logging = false;
        // close old data file and create new one
        action_log_stream.close();
        log_temp_name = action_log_file_name;
        action_log_file_name = "logs/" + folder_name + "/" + log_file_name + " (" + to_string(num_action_logs) + ").log";
        action_log_stream.open(action_log_file_name.c_str(), ios::out | ios::app);
        out_str = "# CONTINUE ACTION LOG at " + string(timestamp_full) + ", previous data file: \"" + log_temp_name + "\"\n";
    }
    num_action_logs++;
    out_str += "# CURRENT PID CONSTANTS: Kp=" + to_string(kp) + ", Ki=" + to_string(ki) + ", Kd=" + to_string(kd);
    action_log_write(out_str, false);
    logging = true;
}

void MainWindow::data_log_write(string dlog_input, bool prepend_time)
{
    int log_max_size = 1073741824;

    t = time(nullptr);   // get time now
    now = localtime( & t );
    strftime(timestamp, sizeof(timestamp), "%T", now);
    strftime(timestamp_full, sizeof(timestamp_full), "%Y-%m-%d %R", now);
    string timestamp_str = timestamp_full;
    int day_current = stoi(timestamp_str.substr(8,2));

    if(prepend_time)
        data_log_stream << timestamp << ",";

    data_log_stream << dlog_input << "\n";
    data_log_stream.flush();
    if(get_file_size(data_log_file_name) > log_max_size || day_current != dat_day_tracker) // check file size and break if too large
        new_data_log();
}

void MainWindow::action_log_write(string alog_input, bool prepend_time)
{
    int log_max_size = 1073741824;

    t = time(nullptr);   // get time now
    now = localtime( & t );
    strftime(timestamp, sizeof(timestamp), "%T", now);
    strftime(timestamp_full, sizeof(timestamp_full), "%Y-%m-%d %R", now);
    string timestamp_str = timestamp_full;
    int day_current = stoi(timestamp_str.substr(8,2));

    if(prepend_time)
        alog_input = string(timestamp) + " - " + alog_input;

    ui->log_box->append(QString::fromStdString(alog_input));
    action_log_stream << alog_input << "\n";
    action_log_stream.flush();
    if(get_file_size(action_log_file_name) > log_max_size || day_current != log_day_tracker) // check file size and break if too large
        new_action_log();
}

long MainWindow::get_file_size(string filename) // query the size of a given file
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void MainWindow::on_beam_kp_write_returnPressed()
{
    bool ok;
    double temp;
    temp = ui->beam_kp_write->text().toDouble(&ok);
    if(ok && temp >= 0)
    {
        kp = temp;
        ui->beam_kp_write->setStyleSheet(clear_edit);
        pid = new PID(kp, ki, kd);
        out_str = "new PID constants: Kp=" + to_string(kp) + ", Ki=" + to_string(ki) + ", Kd=" + to_string(kd);
        action_log_write(out_str);
    }
    else
    {
        ui->beam_kp_write->setStyleSheet(error_edit);
        ui->statusBar->showMessage("Cannot have PID constant below zero.");
    }
}

void MainWindow::on_beam_ki_write_returnPressed()
{
    bool ok;
    double temp;
    temp = ui->beam_ki_write->text().toDouble(&ok);
    if(ok && temp >= 0)
    {
        ki = temp;
        ui->beam_ki_write->setStyleSheet(clear_edit);
        pid = new PID(kp, ki, kd);
        out_str = "new PID constants: Kp=" + to_string(kp) + ", Ki=" + to_string(ki) + ", Kd=" + to_string(kd);
        action_log_write(out_str);
    }
    else
    {
        ui->beam_ki_write->setStyleSheet(error_edit);
        ui->statusBar->showMessage("Cannot have PID constant below zero.");
    }
}

void MainWindow::on_beam_kd_write_returnPressed()
{
    bool ok;
    double temp;
    temp = ui->beam_kd_write->text().toDouble(&ok);
    if(ok && temp >= 0)
    {
        kd = temp;
        ui->beam_kd_write->setStyleSheet(clear_edit);
        pid = new PID(kp, ki, kd);
        out_str = "new PID constants: Kp=" + to_string(kp) + ", Ki=" + to_string(ki) + ", Kd=" + to_string(kd);
        action_log_write(out_str);
    }
    else
    {
        ui->beam_kd_write->setStyleSheet(error_edit);
        ui->statusBar->showMessage("Cannot have PID constant below zero.");
    }
}

void MainWindow::on_ramp_sp_write_returnPressed()
{
    bool ok;
    double temp;
    temp = ui->ramp_sp_write->text().toDouble(&ok);
    if(ok && temp >= 0)
    {
        ramp_sp = temp;
        ramp_rate = (ramp_sp/(ramp_time*60))*refresh_rate; // calculate ramp rate from time and sp
        ui->ramp_sp_write->setStyleSheet(clear_edit);
    }
    else
    {
        ui->ramp_sp_write->setStyleSheet(error_edit);
        ui->statusBar->showMessage("Cannot have ramp set point below zero.");
    }
}

void MainWindow::on_ramp_time_write_returnPressed()
{
    bool ok;
    double temp;
    temp = ui->ramp_time_write->text().toDouble(&ok);
    if(ok && temp > 0)
    {
        ramp_time = temp;
        ramp_rate = (ramp_sp/(ramp_time*60))*refresh_rate; // calculate ramp rate from time and sp
        ui->ramp_time_write->setStyleSheet(clear_edit);
    }
    else
    {
        ui->ramp_time_write->setStyleSheet(error_edit);
        ui->statusBar->showMessage("Cannot have ramp set point below zero.");
    }
}

void MainWindow::on_freq_pid_button_clicked()
{
    if(freq_pid_on)
    {
        freq_pid_on = false;
        ui->beam_volt_button->setEnabled(true);
        ui->beam_volt_button->setText("✎");
        ui->beam_volt_button->setFont(button_font);
        ui->beam_volt_edit->setReadOnly(true);
        ui->beam_volt_button->setStyleSheet(green_edit_button);
        ui->fil_curr_button->setEnabled(true);
        ui->fil_curr_button->setText("✎");
        ui->fil_curr_button->setFont(button_font);
        ui->fil_curr_edit->setReadOnly(true);
        ui->freq_button->setEnabled(false);
        ui->freq_button->setStyleSheet(green_edit_button);
        freq_active = false;
        ui->freq_pid_button->setStyleSheet(green_auto_button);
        ui->freq_pid_button->setText("FREQUENCY");
        ui->ramp_down_button->setEnabled(true);
        ui->ramp_up_button->setEnabled(true);
        ui->beam_curr_pid_button->setEnabled(true);
        ui->power_pid_button->setEnabled(true);
        action_log_write("frequency PID control turned off");
    }
    else
    {
        freq_pid_on = true;
        ui->beam_volt_button->setEnabled(false);
        ui->beam_volt_button->setText("set: ");
        ui->beam_volt_button->setFont(small_button_font);
        ui->beam_volt_edit->setReadOnly(true);
        ui->beam_volt_button->setStyleSheet(orange_pid_button);
        ui->fil_curr_button->setEnabled(false);
        ui->fil_curr_button->setText("✎");
        ui->fil_curr_button->setFont(button_font);
        ui->fil_curr_edit->setReadOnly(true);
        ui->freq_button->setEnabled(true);
        ui->freq_pid_button->setStyleSheet(orange_auto_button);
        ui->freq_pid_button->setText("FREQUENCY");
        ui->ramp_down_button->setEnabled(false);
        ui->ramp_up_button->setEnabled(false);
        ui->beam_curr_pid_button->setEnabled(false);
        ui->power_pid_button->setEnabled(false);
        action_log_write("frequency PID control turned on");

        freq_sp = freq;
        dxm_volt_ref = dxm_volt;
        //ui->freq_edit->setText(QString::number(freq_sp));
        fpid = new PID(fkp, fki, fkd); // re-initialize PID controller

        reset_pid_time = true;
    }
}

void MainWindow::on_fkp_write_returnPressed()
{
    bool ok;
    double temp;
    temp = ui->fkp_write->text().toDouble(&ok);
    if(ok && temp >= 0)
    {
        fkp = temp;
        ui->fkp_write->setStyleSheet(clear_edit);
        fpid = new PID(fkp, fki, fkd);
        out_str = "new frequency PID constants: Kp=" + to_string(kp) + ", Ki=" + to_string(ki) + ", Kd=" + to_string(kd);
        action_log_write(out_str);
    }
    else
    {
        ui->fkp_write->setStyleSheet(error_edit);
        ui->statusBar->showMessage("Cannot have PID constant below zero.");
    }
}

void MainWindow::on_fki_write_returnPressed()
{
    bool ok;
    double temp;
    temp = ui->fki_write->text().toDouble(&ok);
    if(ok && temp >= 0)
    {
        fki = temp;
        ui->fki_write->setStyleSheet(clear_edit);
        fpid = new PID(fkp, fki, fkd);
        out_str = "new frequency PID constants: Kp=" + to_string(kp) + ", Ki=" + to_string(ki) + ", Kd=" + to_string(kd);
        action_log_write(out_str);
    }
    else
    {
        ui->fki_write->setStyleSheet(error_edit);
        ui->statusBar->showMessage("Cannot have PID constant below zero.");
    }
}

void MainWindow::on_fkd_write_returnPressed()
{
    bool ok;
    double temp;
    temp = ui->fkd_write->text().toDouble(&ok);
    if(ok && temp >= 0)
    {
        fkd = temp;
        ui->fkd_write->setStyleSheet(clear_edit);
        fpid = new PID(fkp, fki, fkd);
        out_str = "new frequency PID constants: Kp=" + to_string(kp) + ", Ki=" + to_string(ki) + ", Kd=" + to_string(kd);
        action_log_write(out_str);
    }
    else
    {
        ui->fkd_write->setStyleSheet(error_edit);
        ui->statusBar->showMessage("Cannot have PID constant below zero.");
    }
}

void MainWindow::on_clear_status_button_clicked()
{
    ui->statusBar->clearMessage();
}

void MainWindow::on_dxm_recon_button_clicked()
{
    ethReconnect(&::ED_DXM);
}

void MainWindow::on_slm_recon_button_clicked()
{
    ethReconnect(&::ED_SLM);
}

void MainWindow::on_rigol_recon_button_clicked()
{
    ethReconnect(&::ED_RIGOL);
}

void MainWindow::on_spc_recon_button_clicked()
{
    ethReconnect(&::ED_SPCE);
}

void MainWindow::on_fms_recon_button_clicked()
{
    ethReconnect(&::ED_FMS);
}

void MainWindow::on_lj_recon_button_clicked()
{
    ljReconnect();
}

void MainWindow::on_reconfig_button_clicked()
{
    parse_config();
}

void MainWindow::on_log_rate_write_returnPressed()
{
    bool ok;
    double temp;

    if(ui->log_rate_write->text().isEmpty())
    {
        log_rate = refresh_rate;
        ui->log_rate_write->setStyleSheet(clear_edit);

        out_str = "log rate set to default, now logging everything";
        action_log_write(out_str);
    }
    else
    {
        temp = ui->log_rate_write->text().toDouble(&ok);
        if(ok && temp >= 0)
        {
            log_rate = temp;
            ui->log_rate_write->setStyleSheet(clear_edit);

            out_str = "log rate set to every " + to_string(log_rate) + " seconds";
            action_log_write(out_str);
        }
        else
        {
            ui->log_rate_write->setStyleSheet(error_edit);
            ui->statusBar->showMessage("Invalid log rate entry.");
        }
    }
}

void MainWindow::on_fil_curr_button_clicked()
{
    if(dxm_enabled && !beam_curr_pid_on)
    {
        if(fil_curr_active) // apply setpoint
        {
            if(ui->fil_curr_edit->text() == fil_curr_st) // nothing changed
            {
                fil_curr_active = false;
                ui->fil_curr_edit->setReadOnly(true);
                ui->fil_curr_edit->setStyleSheet(clear_edit);
                ui->fil_curr_edit->setText("");
                ui->fil_curr_button->setStyleSheet(green_edit_button);
                ui->fil_curr_button->setText("✎");
            }
            else
            {
                bool ok;
                double entered = ui->fil_curr_edit->text().toDouble(&ok);

                if(entered < 0 || entered > 3.3 || !ok)
                {
                    ui->fil_curr_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR invalid entry. must be 0-3.3");
                }
                else
                {
                    fil_curr_sp = entered;
                    double convert = (entered/3.3)*4095;
                    //cout << "writing filament current value " << ((int)convert) << endl;
                    out_str = "13," + to_string(int(convert)) + ",";
                    out_str = "\x02" + out_str + "\x03";
                    recv_str = eth_write(out_str, &::ED_DXM);
                    if(recv_str.find("!ERR") == string::npos) // check for errors
                    {
                        fil_curr_active = false;
                        ui->fil_curr_edit->setReadOnly(true);
                        ui->fil_curr_edit->setStyleSheet(clear_edit);
                        ui->fil_curr_edit->setText("");
                        ui->fil_curr_button->setStyleSheet(green_edit_button);
                        ui->fil_curr_button->setText("✎");
                    }
                    else
                    {
                        ui->fil_curr_edit->setStyleSheet(error_edit);
                        ui->statusBar->showMessage("ERROR could not write to DXM");
                    }
                }
            }
        }
        else // change setpoint
        {
            fil_curr_active = true;
            ui->fil_curr_edit->setReadOnly(false);
            ui->fil_curr_edit->setStyleSheet(active_edit);
            ui->fil_curr_edit->setText(QString::number(fil_curr_sp));
            fil_curr_st = ui->fil_curr_edit->text();
            ui->fil_curr_button->setStyleSheet(orange_edit_button);
            ui->fil_curr_button->setText("➤");
            QTimer::singleShot(0, ui->fil_curr_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_beam_volt_button_clicked()
{
    if(dxm_enabled && !dxm_hv_blocked)
    {
        if(beam_volt_active)
        {
            if(ui->beam_volt_edit->text() == beam_volt_st) // do nothing
            {
                beam_volt_active = false;
                ui->beam_volt_edit->setReadOnly(true);
                ui->beam_volt_edit->setStyleSheet(clear_edit);
                ui->beam_volt_edit->setText("");
                ui->beam_volt_button->setStyleSheet(green_edit_button);
                ui->beam_volt_button->setText("✎");
            }
            else
            {
                bool ok;
                double entered = ui->beam_volt_edit->text().toDouble(&ok);
                if(entered < 0 || entered > 20 || !ok)
                {
                    ui->beam_volt_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR invalid entry. must be 0-20");
                }
                else
                {
                    beam_volt_sp = entered;
                    double convert = (entered/20)*4095;
                    out_str = "10," + to_string(int(convert)) + ",";
                    out_str = "\x02" + out_str + "\x03";
                    recv_str = eth_write(out_str, &::ED_DXM);
                    if(recv_str.find("!ERR") == string::npos) // check for errors
                    {
                        beam_volt_active = false;
                        ui->beam_volt_edit->setReadOnly(true);
                        ui->beam_volt_edit->setStyleSheet(clear_edit);
                        ui->beam_volt_edit->setText("");
                        ui->beam_volt_button->setStyleSheet(green_edit_button);
                        ui->beam_volt_button->setText("✎");
                        beam_volt_sp = entered;
                    }
                    else
                    {
                        ui->beam_volt_edit->setStyleSheet(error_edit);
                        ui->statusBar->showMessage("ERROR could not write to DXM");
                    }
                }
            }
        }
        else
        {
            beam_volt_active = true;
            ui->beam_volt_edit->setReadOnly(false);
            ui->beam_volt_edit->setStyleSheet(active_edit);
            ui->beam_volt_edit->setText(QString::number(beam_volt_sp));
            beam_volt_st = ui->beam_volt_edit->text();
            ui->beam_volt_button->setStyleSheet(orange_edit_button);
            ui->beam_volt_button->setText("➤");
            QTimer::singleShot(0, ui->beam_volt_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_beam_curr_button_clicked()
{
    if(dxm_enabled && beam_curr_pid_on)
    {
        if(beam_curr_active)
        {
            if(ui->beam_curr_edit->text() == beam_curr_st) // do nothing
            {
                beam_curr_active = false;
                ui->beam_curr_edit->setReadOnly(true);
                ui->beam_curr_edit->setStyleSheet(clear_edit);
                ui->beam_curr_edit->setText("");
                ui->beam_curr_button->setStyleSheet(green_edit_button);
                ui->beam_curr_button->setText("✎");
            }
            else
            {
                bool ok;
                double entered = ui->beam_curr_edit->text().toDouble(&ok);
                if(entered < 0 || entered > dxm_curr_limit || !ok)
                {
                    ui->beam_curr_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR invalid entry. must be 0-30");
                }
                else
                {
                    beam_curr_active = false;
                    ui->beam_curr_edit->setReadOnly(true);
                    ui->beam_curr_edit->setStyleSheet(clear_edit);
                    ui->beam_curr_edit->setText("");
                    ui->beam_curr_button->setStyleSheet(green_edit_button);
                    ui->beam_curr_button->setText("✎");

                    if(beam_curr_sp < 0 && running)
                        reset_pid_time = true;

                    beam_curr_sp = entered;
                    beam_sp_data.push_back(beam_curr_sp);
                    beam_time_data.push_back(key);
                    cout << "beam current setpoint changed: " << beam_curr_sp << endl;
                    pid = new PID(kp, ki, kd); // re-initialize PID controller
                }
            }
        }
        else
        {
            beam_volt_active = true;
            ui->beam_curr_edit->setReadOnly(false);
            ui->beam_curr_edit->setStyleSheet(active_edit);
            if(beam_curr_sp < 0)
                ui->beam_curr_edit->setText("0");
            else
                ui->beam_curr_edit->setText(QString::number(beam_curr_sp));
            beam_curr_st = ui->beam_curr_edit->text();
            ui->beam_curr_button->setStyleSheet(orange_edit_button);
            ui->beam_curr_button->setText("➤");
            QTimer::singleShot(0, ui->beam_curr_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_anode_curr_button_clicked()
{
    if(slm_enabled)
    {
        if(anode_curr_active)
        {
            if(ui->anode_curr_edit->text() == anode_curr_st) // do nothing
            {
                anode_curr_active = false;
                ui->anode_curr_edit->setReadOnly(true);
                ui->anode_curr_edit->setStyleSheet(clear_edit);
                ui->anode_curr_edit->setText("");
                ui->anode_curr_button->setStyleSheet(green_edit_button);
                ui->anode_curr_button->setText("✎");
            }
            else
            {
                bool ok;
                double entered = ui->anode_curr_edit->text().toDouble(&ok);
                if(entered < 0 || entered > 60 || !ok)
                {
                    ui->anode_curr_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR invalid entry. must be 0-60");
                }
                else
                {
                    anode_curr_sp = entered;
                    double convert = (entered/60)*4095;
                    out_str = "11," + to_string(int(convert)) + ",";
                    out_str = "\x02" + out_str + "\x03";
                    recv_str = eth_write(out_str, &::ED_SLM);
                    if(recv_str.find("!ERR") == string::npos) // check for errors
                    {
                        anode_curr_active = false;
                        ui->anode_curr_edit->setReadOnly(true);
                        ui->anode_curr_edit->setStyleSheet(clear_edit);
                        ui->anode_curr_edit->setText("");
                        ui->anode_curr_button->setStyleSheet(green_edit_button);
                        ui->anode_curr_button->setText("✎");
                    }
                    else
                    {
                        ui->anode_curr_edit->setStyleSheet(error_edit);
                        ui->statusBar->showMessage("ERROR could not write to DXM");
                    }
                }
            }
        }
        else
        {
            anode_curr_active = true;
            ui->anode_curr_edit->setReadOnly(false);
            ui->anode_curr_edit->setStyleSheet(active_edit);
            ui->anode_curr_edit->setText(QString::number(anode_curr_sp));
            anode_curr_st = ui->anode_curr_edit->text();
            ui->anode_curr_button->setStyleSheet(orange_edit_button);
            ui->anode_curr_button->setText("➤");
            QTimer::singleShot(0, ui->anode_curr_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_anode_volt_button_clicked()
{
    if(slm_enabled)
    {
        if(anode_volt_active)
        {
            if(ui->anode_volt_edit->text() == anode_volt_st) // do nothing
            {
                anode_volt_active = false;
                ui->anode_volt_edit->setReadOnly(true);
                ui->anode_volt_edit->setStyleSheet(clear_edit);
                ui->anode_volt_edit->setText("");
                ui->anode_volt_button->setStyleSheet(green_edit_button);
                ui->anode_volt_button->setText("✎");
            }
            else
            {
                bool ok;
                double entered = ui->anode_volt_edit->text().toDouble(&ok);
                if(entered < 0 || entered > 5 || !ok)
                {
                    ui->anode_volt_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR invalid entry. must be 0-5");
                }
                else
                {
                    anode_volt_sp = entered;
                    double convert = (entered/5)*4095;
                    out_str = "10," + to_string(int(convert)) + ",";
                    out_str = "\x02" + out_str + "\x03";
                    recv_str = eth_write(out_str, &::ED_SLM);
                    if(recv_str.find("!ERR") == string::npos) // check for errors
                    {
                        anode_volt_active = false;
                        ui->anode_volt_edit->setReadOnly(true);
                        ui->anode_volt_edit->setStyleSheet(clear_edit);
                        ui->anode_volt_edit->setText("");
                        ui->anode_volt_button->setStyleSheet(green_edit_button);
                        ui->anode_volt_button->setText("✎");
                    }
                    else
                    {
                        ui->anode_volt_edit->setStyleSheet(error_edit);
                        ui->statusBar->showMessage("ERROR could not write to DXM");
                    }
                }
            }
        }
        else
        {
            anode_volt_active = true;
            ui->anode_volt_edit->setReadOnly(false);
            ui->anode_volt_edit->setStyleSheet(active_edit);
            ui->anode_volt_edit->setText(QString::number(anode_volt_sp));
            anode_volt_st = ui->anode_volt_edit->text();
            ui->anode_volt_button->setStyleSheet(orange_edit_button);
            ui->anode_volt_button->setText("➤");
            QTimer::singleShot(0, ui->anode_volt_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_power_button_clicked()
{
    if(dxm_enabled)
    {
        if(power_active)
        {
            if(ui->power_edit->text() == power_st) // do nothing
            {
                power_active = false;
                ui->power_edit->setReadOnly(true);
                ui->power_edit->setStyleSheet(clear_edit);
                ui->power_edit->setText("");
                ui->power_button->setStyleSheet(green_edit_button);
                ui->power_button->setText("✎");
            }
            else
            {
                bool ok;
                double entered = ui->power_edit->text().toDouble(&ok);
                if(entered < 0 || entered > power_limit || !ok)
                {
                    ui->power_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR invalid entry. must be 0-5");
                }
                else
                {
                    power_active = false;
                    ui->power_edit->setReadOnly(true);
                    ui->power_edit->setStyleSheet(clear_edit);
                    ui->power_edit->setText("");
                    ui->power_button->setStyleSheet(green_edit_button);
                    ui->power_button->setText("✎");

                    if(power_sp < 0)
                        reset_pid_time = true;

                    power_sp = entered;
                    power_sp_data.push_back(power_sp);
                    power_time_data.push_back(key);
                    cout << "power setpoint changed: " << power_sp << endl;
                    pid = new PID(kp, ki, kd); // re-initialize PID controller
                }
            }
        }
        else
        {
            power_active = true;
            ui->power_edit->setReadOnly(false);
            ui->power_edit->setStyleSheet(active_edit);
            if(power_sp < 0)
                ui->power_edit->setText("0");
            else
                ui->power_edit->setText(QString::number(power_sp));
            power_st = ui->power_edit->text();
            ui->power_button->setStyleSheet(orange_edit_button);
            ui->power_button->setText("➤");
            QTimer::singleShot(0, ui->power_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_freq_button_clicked()
{
    if(fms_enabled)
    {
        if(freq_active)
        {
            if(ui->freq_edit->text() == freq_st) // do nothing
            {
                freq_active = false;
                ui->freq_edit->setReadOnly(true);
                ui->freq_edit->setStyleSheet(clear_edit);
                ui->freq_edit->setText("");
                ui->freq_button->setStyleSheet(green_edit_button);
                ui->freq_button->setText("✎");
            }
            else
            {
                bool ok;
                double entered = ui->freq_edit->text().toDouble(&ok);
                if(entered < 0 || !ok)
                {
                    ui->freq_edit->setStyleSheet(error_edit);
                    ui->statusBar->showMessage("ERROR invalid entry. must be > 0");
                }
                else
                {
                    freq_active = false;
                    ui->freq_edit->setReadOnly(true);
                    ui->freq_edit->setStyleSheet(clear_edit);
                    ui->freq_edit->setText("");
                    ui->freq_button->setStyleSheet(green_edit_button);
                    ui->freq_button->setText("✎");
                    //freq_sp = entered;
                    //cout << "frequency setpoint changed: " << freq_sp << endl;
                    //pid = new PID(kp, ki, kd); // re-initialize PID controller
                }
            }
        }
        else
        {
            freq_active = true;
            ui->freq_edit->setReadOnly(false);
            ui->freq_edit->setStyleSheet(active_edit);
            ui->freq_edit->setText(QString::number(freq_sp));
            freq_st = ui->freq_edit->text();
            ui->freq_button->setStyleSheet(orange_edit_button);
            ui->freq_button->setText("➤");
            QTimer::singleShot(0, ui->freq_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_shim_curr_button_clicked()
{
    if(rigol1_enabled)
    {
        if(shim_curr_active)
        {
            if(ui->shim_curr_edit->text() == shim_curr_st) // do nothing
            {
                shim_curr_active = false;
                ui->shim_curr_edit->setReadOnly(true);
                ui->shim_curr_edit->setStyleSheet(clear_edit);
                ui->shim_curr_edit->setText("");
                ui->shim_curr_button->setStyleSheet(green_edit_button);
                ui->shim_curr_button->setText("✎");
            }
            else
            {
                if(rigol2_enabled)
                {
                    rigol1_enabled = false;
                    rigol2_enabled = false;
                    recv_str = eth_write(":INST CH1\r\n", &::ED_RIGOL);
                    if(recv_str.find("!ERR") == string::npos) // check for errors
                    {
                        QTimer::singleShot(150, this, SLOT(rigol1_set_curr_true())); // wait 0.15s after changing channel before writing new value
                    }
                    else
                    {
                        ui->statusBar->showMessage("ERROR could not select channel 1 on Rigol");
                        ui->shim_curr_edit->setStyleSheet(error_edit);
                    }
                }
                else
                    rigol1_set_curr(false);
            }
        }
        else
        {
            shim_curr_active = true;
            ui->shim_curr_edit->setReadOnly(false);
            ui->shim_curr_edit->setStyleSheet(active_edit);
            ui->shim_curr_edit->setText(QString::number(shim_curr_sp));
            shim_curr_st = ui->shim_curr_edit->text();
            ui->shim_curr_button->setStyleSheet(orange_edit_button);
            ui->shim_curr_button->setText("➤");
            QTimer::singleShot(0, ui->shim_curr_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_shim_volt_button_clicked()
{
    if(rigol1_enabled)
    {
        if(shim_volt_active)
        {
            if(ui->shim_volt_edit->text() == shim_volt_st) // do nothing
            {
                shim_volt_active = false;
                ui->shim_volt_edit->setReadOnly(true);
                ui->shim_volt_edit->setStyleSheet(clear_edit);
                ui->shim_volt_edit->setText("");
                ui->shim_volt_button->setStyleSheet(green_edit_button);
                ui->shim_volt_button->setText("✎");
            }
            else
            {
                if(rigol2_enabled)
                {
                    rigol1_enabled = false;
                    rigol2_enabled = false;
                    recv_str = eth_write(":INST CH1\r\n", &::ED_RIGOL);
                    if(recv_str.find("!ERR") == string::npos) // check for errors
                    {
                        QTimer::singleShot(150, this, SLOT(rigol1_set_volt_true())); // wait 0.15s after changing channel before writing new value
                    }
                    else
                    {
                        ui->shim_volt_edit->setStyleSheet(error_edit);
                        ui->statusBar->showMessage("ERROR could not select channel 1 on Rigol");
                    }
                }
                else
                    rigol1_set_volt(false);
            }
        }
        else
        {
            shim_volt_active = true;
            ui->shim_volt_edit->setReadOnly(false);
            ui->shim_volt_edit->setStyleSheet(active_edit);
            ui->shim_volt_edit->setText(QString::number(shim_volt_sp));
            shim_volt_st = ui->shim_volt_edit->text();
            ui->shim_volt_button->setStyleSheet(orange_edit_button);
            ui->shim_volt_button->setText("➤");
            QTimer::singleShot(0, ui->shim_volt_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_trim_curr_button_clicked()
{
    if(rigol2_enabled)
    {
        if(trim_curr_active)
        {
            if(ui->trim_curr_edit->text() == trim_curr_st) // do nothing
            {
                trim_curr_active = false;
                ui->trim_curr_edit->setReadOnly(true);
                ui->trim_curr_edit->setStyleSheet(clear_edit);
                ui->trim_curr_edit->setText("");
                ui->trim_curr_button->setStyleSheet(green_edit_button);
                ui->trim_curr_button->setText("✎");
            }
            else
            {
                if(rigol1_enabled)
                {
                    rigol1_enabled = false;
                    rigol2_enabled = false;
                    recv_str = eth_write(":INST CH2\r\n", &::ED_RIGOL);
                    if(recv_str.find("!ERR") == string::npos)
                    {
                        QTimer::singleShot(150, this, SLOT(rigol2_set_curr_true())); // wait 0.15s after changing channel before writing new value
                    }
                    else
                    {
                        ui->trim_curr_edit->setStyleSheet(error_edit);
                        ui->statusBar->showMessage("ERROR could not select channel 2 on Rigol");
                    }
                }
                else
                    rigol2_set_curr(false);
            }
        }
        else
        {
            trim_curr_active = true;
            ui->trim_curr_edit->setReadOnly(false);
            ui->trim_curr_edit->setStyleSheet(active_edit);
            ui->trim_curr_edit->setText(QString::number(trim_curr_sp));
            trim_curr_st = ui->trim_curr_edit->text();
            ui->trim_curr_button->setStyleSheet(orange_edit_button);
            ui->trim_curr_button->setText("➤");
            QTimer::singleShot(0, ui->trim_curr_edit, SLOT(setFocus()));
        }
    }
}

void MainWindow::on_trim_volt_button_clicked()
{
    if(rigol2_enabled)
    {
        if(trim_volt_active)
        {
            if(ui->trim_volt_edit->text() == trim_volt_st) // do nothing
            {
                trim_volt_active = false;
                ui->trim_volt_edit->setReadOnly(true);
                ui->trim_volt_edit->setStyleSheet(clear_edit);
                ui->trim_volt_edit->setText("");
                ui->trim_volt_button->setStyleSheet(green_edit_button);
                ui->trim_volt_button->setText("✎");
            }
            else
            {
                if(rigol1_enabled)
                {
                    rigol2_enabled = false;
                    rigol1_enabled = false;
                    recv_str = eth_write(":INST CH2\r\n", &::ED_RIGOL);
                    if(recv_str.find("!ERR") == string::npos) // check for errors
                    {
                        QTimer::singleShot(150, this, SLOT(rigol2_set_volt_true())); // wait 0.15s after changing channel before writing new value
                    }
                    else
                    {
                        ui->trim_volt_edit->setStyleSheet(error_edit);
                        ui->statusBar->showMessage("ERROR could not select channel 2 on Rigol");
                    }
                }
                else
                    rigol2_set_volt(false);
            }
        }
        else
        {
            trim_volt_active = true;
            ui->trim_volt_edit->setReadOnly(false);
            ui->trim_volt_edit->setStyleSheet(active_edit);
            ui->trim_volt_edit->setText(QString::number(trim_volt_sp));
            trim_volt_st = ui->trim_volt_edit->text();
            ui->trim_volt_button->setStyleSheet(orange_edit_button);
            ui->trim_volt_button->setText("➤");
            QTimer::singleShot(0, ui->trim_volt_edit, SLOT(setFocus()));
        }
    }
}
