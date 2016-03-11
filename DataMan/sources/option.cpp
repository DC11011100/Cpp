#include "option.h"

Option::Option(QWidget *parentWidg) : QTabWidget(parentWidg)
{
    //Widgets
    findWidg = new QWidget;
    orgWidg = new QWidget;
    orgWidg_find = new QWidget;
    orgWidg_form = new QWidget;

    //LineEdits
    findWidg_nameEdit = new QLineEdit;
    findWidg_doaEdit = new QLineEdit;

    orgWidg_nameEdit = new QLineEdit;
    orgWidg_doaEdit = new QLineEdit;
    orgWidg_pathEdit = new QLineEdit;
    orgWidg_pathEdit->setFixedWidth(150);

    //CheckBoxes
    QCheckBox *findWidg_openChk = new QCheckBox("Open", findWidg);

    QCheckBox *orgWidg_openChk = new QCheckBox("Open", orgWidg);

    //Buttons
    findWidg_findBtn = new QPushButton("Find", findWidg);

    orgWidg_orgBtn = new QPushButton("Organize", orgWidg);
    orgWidg_chooseBtn = new QPushButton("Choose", orgWidg);

    //Layouts
    findWidg_layout = new QFormLayout;
    findWidg_layout->addRow(tr("&Name: "), findWidg_nameEdit);
    findWidg_layout->addRow(tr("D.O.A: "), findWidg_doaEdit);
    findWidg_layout->addRow(findWidg_openChk, findWidg_findBtn);
    findWidg->setLayout(findWidg_layout);

    orgWidg_findlayout = new QGridLayout;
    orgWidg_findlayout->addWidget(orgWidg_pathEdit);
    orgWidg_findlayout->addWidget(orgWidg_chooseBtn, 0, 1, Qt::AlignRight);
    orgWidg_findlayout->setContentsMargins(0, 0, 0, 0);
    orgWidg_find->setLayout(orgWidg_findlayout);

    orgWidg_formlayout = new QFormLayout;
    orgWidg_formlayout->addRow(tr("&Name: "), orgWidg_nameEdit);
    orgWidg_formlayout->addRow(tr("D.O.A: "), orgWidg_doaEdit);
    orgWidg_formlayout->addRow(orgWidg_openChk, orgWidg_orgBtn);
    orgWidg_form->setLayout(orgWidg_formlayout);

    orgWidg_layout = new QVBoxLayout;
    orgWidg_layout->addWidget(orgWidg_find);
    orgWidg_layout->addWidget(orgWidg_form);
    orgWidg_layout->setContentsMargins(11,11, 0, 11);
    orgWidg->setLayout(orgWidg_layout);

    //Option tabs
    this->addTab(findWidg, "Find");
    this->addTab(orgWidg, "Organize");
}

QStringList Option::fileChooser() {
    QFileDialog dialog(0, Qt::Dialog);
    dialog.setFileMode(QFileDialog::ExistingFile); //Sets valid choices exclusively to existing files
    dialog.setViewMode(QFileDialog::Detail);    //Sets detailed file descriptions within

    QStringList fileNames;
    if (dialog.exec()) fileNames = dialog.selectedFiles(); //If user hits "ok", then store list to "dirNames"
    return fileNames;
}

//Line Edits
QLineEdit* Option::get_pathEdit() {
    return orgWidg_pathEdit;
}

QLineEdit* Option::get_findnameEdit() {
    return findWidg_nameEdit;
}

QLineEdit* Option::get_finddoaEdit() {
    return findWidg_doaEdit;
}

QLineEdit* Option::get_orgnameEdit() {
    return orgWidg_nameEdit;
}

QLineEdit* Option::get_orgdoaEdit() {
    return orgWidg_doaEdit;
}

//Buttons
QPushButton* Option::get_chooseBtn() {
    return orgWidg_chooseBtn;
}

QPushButton* Option::get_orgBtn() {
    return orgWidg_orgBtn;
}

QPushButton* Option::get_findBtn() {
    return findWidg_findBtn;
}

