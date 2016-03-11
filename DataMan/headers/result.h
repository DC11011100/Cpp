#ifndef RESULT_H
#define RESULT_H

#include <qwidget.h>
#include <qlineedit.h>
#include <QVBoxLayout>
#include <qlabel.h>
#include <QListWidget>
#include <QListWidgetItem>

class Result : public QWidget
{
public:
    //Layouts
    QVBoxLayout *layout;

    //List widgets
    QListWidget *resList;
    int resList_rowCount;

    //Constructor
    Result(QWidget *parentWidg);

    //General Prototypes
    void insertListItem(QString title);

private:

};

#endif // RESULT_H
