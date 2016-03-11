#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "setting.h"
#include "option.h"
#include "result.h"
#include <qobject.h>

class EventHandler : public QObject
{
    Q_OBJECT

public:
    Setting *settingWidget_ptr;
    Option *optionWidget_ptr;
    Result *resultWidget_ptr;

    EventHandler(Setting *settingWidget, Option *optionWidget, Result *resultWidget);

private slots:
    void setting_chooseClicked();

    void option_chooseClicked();
    void option_findClicked();
    void option_orgClicked();

private:
    QPushButton *settingWidget_chooseBtn, *optionWidget_chooseBtn, *optionWidget_findBtn,
                *optionWidget_orgBtn;

    QLineEdit *settingWidget_dirEdit, *optionWidget_pathEdit, *optionWidget_findnameEdit,
                *optionWidget_finddoaEdit, *optionWidget_orgnameEdit, *optionWidget_orgdoaEdit;
};

#endif // EVENTHANDLER_H
