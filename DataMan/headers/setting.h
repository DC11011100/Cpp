#ifndef SETTING_H
#define SETTING_H

#include <qwidget.h>
#include <qboxlayout.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>

class Setting : public QWidget
{
public:
    //Layouts
    QHBoxLayout *setting_layout;

    //Constructor
    Setting(QWidget *parentWidg);

    //General Prototypes
    QStringList dirChooser();
    QPushButton* get_chooseBtn();
    QLineEdit* get_dirEdit();

private:
    //Buttons
    QPushButton *setting_chooseBtn;

    //LineEdits
    QLineEdit *setting_dirEdit;
};

#endif // SETTING_H
