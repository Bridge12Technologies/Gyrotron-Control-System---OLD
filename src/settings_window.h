#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QDialog>
#include <QSlider>
#include <QStackedWidget>
#include <functional>
#include "gyrotron.h"
#include "lib/face/face.h"

typedef std::function<void(bool)> F_bool;

namespace Ui {
class settings_window;
}

class settings_window : public QDialog
{
    Q_OBJECT

public:
    explicit settings_window(bool* am, QPushButton* at, QStackedWidget* sw, bool* be, F_bool sbe, bool* tm, QWidget *parent = nullptr);
    ~settings_window();

private slots:
    void on_done_button_clicked();
    void on_admin_mode_switch_valueChanged(int value);
    void on_blink_switch_valueChanged(int value);
    void on_time_mode_switch_valueChanged(int value);

private:
    void apply_drop_shadow();
    void update();
    void set_switch_state(bool set_on, QSlider *slider, QLabel *label);
    void init_event_filter();

    Ui::settings_window *ui;
    QGraphicsDropShadowEffect *shadow;
    DarkenEffect *darken;
    bool *admin_mode, *blink_enabled, *time_mode;
    QPushButton *admin_tab;
    QStackedWidget *stack_widget;
    F_bool set_blink_enabled;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // SETTINGS_WINDOW_H
