#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <iostream>
#include <qdebug.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    //UI references     Note: when calling findChild, search for name of widget, set in .ui (Designer)
    Ui::MainWindow* ui = w.getUI();
    QWidget *canvas = ui->Central;

    //Widget References     Casting each SubWidget as its corresponding class for access to class members
    //ClientWidget *clientWidget = canvas->findChild<ClientWidget*>("clientWidget");
    //assert(clientWidget != 0);

    //Create instance of EventHandler
    //EventHandler *eventHandler = new EventHandler(clientWidget);

    //Event Loop

    return a.exec();
}
