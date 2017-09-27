#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "finder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Ui::MainWindow* getUI();

    //Prototypes
    //void dataMan_eventHandler(Setting *setting_Widget, Option *option_Widget);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
