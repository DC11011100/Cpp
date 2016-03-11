#include "setting.h"

Setting::Setting(QWidget *parentWidg) : QWidget(parentWidg)
{
    //LineEdits
    setting_dirEdit = new QLineEdit;
    setting_dirEdit->setFixedWidth(500);

    //Buttons
    setting_chooseBtn = new QPushButton("Choose", this);

    //Layouts
    setting_layout = new QHBoxLayout;
    setting_layout->addWidget(setting_dirEdit);
    setting_layout->addWidget(setting_chooseBtn);
    setLayout(setting_layout);

}

QStringList Setting::dirChooser() {
    QFileDialog dialog(0, Qt::Dialog);  //Set parent = 0 so it opens on seperate window
    dialog.setFileMode(QFileDialog::Directory); //Sets valid choices exclusively to directories
    dialog.setViewMode(QFileDialog::Detail);    //Sets detailed file descriptions within

    QStringList dirNames;
    if (dialog.exec()) dirNames = dialog.selectedFiles(); //If user hits "ok", then store list to "dirNames"
    return dirNames;
}

//Getter Functions for private class element
QPushButton* Setting::get_chooseBtn() {
    return setting_chooseBtn;
}

QLineEdit* Setting::get_dirEdit() {
    return setting_dirEdit;
}
