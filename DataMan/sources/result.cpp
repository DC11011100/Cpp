#include "result.h"

Result::Result(QWidget *parentWidg):QWidget(parentWidg)
{
    //Labels
    QLabel *desc = new QLabel(tr("Results:"), this, Qt::Widget);
    desc->setFixedWidth(175);

    //Line Edits
    QLineEdit *res1Edit = new QLineEdit("Test1");
    QLineEdit *res2Edit = new QLineEdit("Test2");
    QLineEdit *res3Edit = new QLineEdit("Test3");

    //List Widget
    resList = new QListWidget(this);

    //Layouts
    layout = new QVBoxLayout;
    layout->addWidget(desc);
    layout->addWidget(resList);

    setLayout(layout);
}

void Result::insertListItem(QString title) {
    new QListWidgetItem::QListWidgetItem(title, resList);
}
