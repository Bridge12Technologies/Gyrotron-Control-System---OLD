#include "settings_window.h"
#include "ui_settings_window.h"

settings_window::settings_window(Gyrotron* main_gyro, bool* am, QPushButton* at, QStackedWidget* sw, QWidget *parent) :
    QDialog(parent),ui(new Ui::settings_window),gyro(main_gyro),admin_mode(am),admin_tab(at),stack_widget(sw)
{
    parent->setGraphicsEffect(new DarkenEffect);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    ui->setupUi(this);

    init_event_filter();
    apply_drop_shadow();
    update();

    gyro->log_event("settings window opened");
}

settings_window::~settings_window() { delete ui; delete shadow; }

void settings_window::init_event_filter()
{
    // install event filter to prevent wheel events on sliders
    QSlider *sliders[] = {ui->log_mode_switch,ui->admin_mode_switch};
    for(auto obj : sliders) { obj->installEventFilter(this); }
}

void settings_window::apply_drop_shadow()
{
    shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setColor(QColor(30,30,30,50));
    ui->settings_frame->setGraphicsEffect(shadow);
}

void settings_window::update()
{
    QSlider *sliders[] = {ui->log_mode_switch,ui->admin_mode_switch};
    QLabel *labels[] = {ui->log_mode_label, ui->admin_mode_label};
    bool states[] = {gyro->log_is_bin(),*admin_mode}; // true if data log enabled

    for(int i = 0; i < 2; i++) { set_switch_state(states[i], sliders[i], labels[i]); }
}

void settings_window::set_switch_state(bool set_on, QSlider *slider, QLabel *label)
{
    if(set_on)
    {
        slider->setStyleSheet(toggle_switch_on);
        if(slider->value() != 1) slider->setValue(1);
        label->setText("ON");
    }
    else
    {
        slider->setStyleSheet(toggle_switch_off);
        if(slider->value() != 0) slider->setValue(0);
        label->setText("OFF");
    }
}

bool settings_window::eventFilter(QObject *obj, QEvent *event) { return wheel_filter(this,obj,event); }

void settings_window::on_log_mode_switch_valueChanged(int value)
{
    if(value == 1)
    {
        gyro->set_log_in_bin(true);
        set_switch_state(true,ui->log_mode_switch,ui->log_mode_label);
    }
    else
    {
        gyro->set_log_in_bin(false);
        set_switch_state(false,ui->log_mode_switch,ui->log_mode_label);
    }
}

void settings_window::on_admin_mode_switch_valueChanged(int value)
{
    if(value == 1)
    {
        *admin_mode = true;
        admin_tab->setVisible(true);
        if(stack_widget->currentIndex() == 3) stack_widget->setCurrentIndex(0);
        set_switch_state(true,ui->admin_mode_switch,ui->admin_mode_label);
    }
    else
    {
        *admin_mode = false;
        admin_tab->setVisible(false);
        set_switch_state(false,ui->admin_mode_switch,ui->admin_mode_label);
    }
}

void settings_window::on_done_button_clicked() { accept(); parentWidget()->setGraphicsEffect(nullptr); }
