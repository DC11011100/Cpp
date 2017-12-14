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

    w.setWindowTitle("Legal File Manager");

    return a.exec();
}
