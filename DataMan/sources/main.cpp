#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <iostream>
#include <qdebug.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    //UI references     Note: when calling findChild, search for name of widget, set in .ui (Designer)
    Ui::MainWindow* ui_ref = w.getUI();
    QWidget *central_ref = ui_ref->centralWidget;

    //Widget References     Casting each SubWidget as its corresponding class for access to class members
    Setting *settingWidg_ref = (Setting*)central_ref->findChild<QWidget*>("setting_widget");
    Option *optionWidg_ref = (Option*)central_ref->findChild<QWidget*>("option_widget");
    //QWidget* *progressWidg_ref = central_ref->findChild<QWidget*>("progress_widget");
    Result *resultWidg_ref = central_ref->findChild<Result*>("result_widget");


    //Create instance of EventHandler
    EventHandler *eventHandler = new EventHandler(settingWidg_ref, optionWidg_ref, resultWidg_ref);

    //Event Loop

    return a.exec();
}
