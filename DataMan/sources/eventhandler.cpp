#include "eventhandler.h"
#include "qdebug.h"

EventHandler::EventHandler(Setting *settingWidget, Option *optionWidget, Result *resultWidget):QObject()
{
    settingWidget_ptr = settingWidget;
    optionWidget_ptr = optionWidget;
    resultWidget_ptr = resultWidget;

    //Setting
        //LineEdits
    settingWidget_dirEdit = settingWidget->get_dirEdit();

    optionWidget_findnameEdit = optionWidget_ptr->get_findnameEdit();
    optionWidget_finddoaEdit = optionWidget_ptr->get_finddoaEdit();
    optionWidget_orgnameEdit = optionWidget_ptr->get_orgdoaEdit();
    optionWidget_orgnameEdit = optionWidget_ptr->get_orgnameEdit();

        //Buttons
    settingWidget_chooseBtn = settingWidget->get_chooseBtn();

    optionWidget_chooseBtn = optionWidget_ptr->get_chooseBtn();
    optionWidget_findBtn = optionWidget_ptr->get_findBtn();
    optionWidget_orgBtn = optionWidget_ptr->get_orgBtn();

        //Connections
    QObject::connect(settingWidget_chooseBtn, SIGNAL(clicked()), this, SLOT(setting_chooseClicked()));

    //Option
        //LineEdits
    optionWidget_pathEdit = optionWidget->get_pathEdit();

        //Buttons
    optionWidget_chooseBtn = optionWidget->get_chooseBtn();
    qDebug() << optionWidget_chooseBtn;

        //Connections
    QObject::connect(optionWidget_chooseBtn, SIGNAL(clicked()), this, SLOT(option_chooseClicked()));
    QObject::connect(optionWidget_findBtn, SIGNAL(clicked()), this, SLOT(option_findClicked()));
    QObject::connect(optionWidget_orgBtn, SIGNAL(clicked()), this, SLOT(option_orgClicked()));
}

//Sub Event Handlers
void EventHandler::setting_chooseClicked() {
    QStringList dirList = settingWidget_ptr->dirChooser();
    if (!dirList.isEmpty()) settingWidget_dirEdit->setText(dirList.takeFirst());
}

void EventHandler::option_chooseClicked() {
     QStringList fileList = optionWidget_ptr->fileChooser();
     if (!fileList.isEmpty()) optionWidget_pathEdit->setText(fileList.takeFirst());
 }

void EventHandler::option_findClicked() {
    qDebug() << "Find Name Edit text : " << optionWidget_ptr->get_findnameEdit()->text();
    qDebug() << "Find D.O.A. text: " << optionWidget_ptr->get_finddoaEdit()->text();
}

void EventHandler::option_orgClicked() {
    qDebug() << "Org Edit text : " << optionWidget_ptr->get_orgnameEdit()->text();
    qDebug() << "Org D.O.A. text: " << optionWidget_ptr->get_orgdoaEdit()->text();
}
