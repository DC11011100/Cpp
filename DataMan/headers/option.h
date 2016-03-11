#ifndef OPTION_H
#define OPTION_H

#include <qwidget.h>
#include <qtabwidget.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <QVBoxLayout>
#include <QGridLayout>

class Option : public QTabWidget
{
public:
    //Widget Children
    QWidget *findWidg, *orgWidg, *orgWidg_find, *orgWidg_form;

    //Layouts
    QFormLayout *findWidg_layout, *orgWidg_formlayout;
    QVBoxLayout *orgWidg_layout;
    QGridLayout *orgWidg_findlayout;

    //Constructor
    Option(QWidget *parentWidg);

    //General Prototypes
    QStringList fileChooser();

    QLineEdit* get_pathEdit();
    QLineEdit* get_findnameEdit();
    QLineEdit* get_finddoaEdit();
    QLineEdit* get_orgnameEdit();
    QLineEdit* get_orgdoaEdit();

    QPushButton* get_chooseBtn();
    QPushButton* get_orgBtn();
    QPushButton* get_findBtn();

private:
    //Line Edits
    QLineEdit *findWidg_nameEdit, *findWidg_doaEdit, *orgWidg_nameEdit,
        *orgWidg_doaEdit, *orgWidg_pathEdit;

    //Buttons
    QPushButton *findWidg_findBtn, *orgWidg_chooseBtn, *orgWidg_orgBtn;


};

#endif // OPTION_H
