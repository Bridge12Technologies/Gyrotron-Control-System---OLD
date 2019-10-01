#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QDialog>
#include <QSlider>
#include <QStackedWidget>
#include "gyrotron.h"
#include "lib/face/face.h"

namespace Ui {
class settings_window;
}

class settings_window : public QDialog
{
    Q_OBJECT

public:
    explicit settings_window(Gyrotron* main_gyro, bool* am, QPushButton* at, QStackedWidget* sw, QWidget *parent = nullptr);
    ~settings_window();

private slots:
    void on_log_mode_switch_valueChanged(int value);
    void on_admin_mode_switch_valueChanged(int value);
    void on_done_button_clicked();

private:
    void apply_drop_shadow();
    void update();
    void set_switch_state(bool set_on, QSlider *slider, QLabel *label);
    void init_event_filter();

    Ui::settings_window *ui;
    Gyrotron *gyro;
    QGraphicsDropShadowEffect *shadow;
    DarkenEffect *darken;
    bool *admin_mode;
    QPushButton *admin_tab;
    QStackedWidget *stack_widget;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // SETTINGS_WINDOW_H
