#include "caseform.h"

CaseFormDialog::CaseFormDialog(QWidget *parent, bool edit, QDir *app_root, QString clientName, QString caseName)
{
    rootDir = app_root;
    client_name = clientName;
    original_case = caseName;
    editable = edit;
    reloadCase = false;

    promptStart    = new QLabel("Start : ", this);
    promptEnd  = new QLabel("Finish : ", this);
    promptSummary   = new QLabel("Summary : ", this);

    caseStart = new QLineEdit(this);
    caseStart->setPlaceholderText(" MM-DD-YYYY");

    caseEnd = new QLineEdit(this);
    caseEnd->setPlaceholderText(" MM-DD-YYYY");

    caseSummary = new QTextEdit(this);
    caseSummary->setPlaceholderText("Quickly describe the case here...");
    caseSummary->setMinimumHeight(60);
    caseSummary->setMaximumHeight(100);

    cancelButton = new QPushButton("Cancel", this);
    saveButton   = new QPushButton("Save", this);

    qDebug() << "Setting up client form layout";
    // Set up layout
    QGridLayout *layout_caseForm = new QGridLayout(this);
    layout_caseForm->addWidget(new QLabel("<h4>Case Info<\h4>", this), 0, 0, 1, 4, Qt::AlignTop | Qt::AlignHCenter);
    layout_caseForm->addWidget(promptStart, 1, 0, 1, 1, Qt::AlignTop | Qt::AlignRight);
    layout_caseForm->addWidget(promptEnd, 2, 0, 1, 1, Qt::AlignTop | Qt::AlignRight);
    layout_caseForm->addWidget(promptSummary, 3, 0, 1, 1, Qt::AlignTop | Qt::AlignRight);
    layout_caseForm->addWidget(cancelButton, 6, 1, 1, 1);
    layout_caseForm->addWidget(saveButton, 6, 2, 1, 1);

    layout_caseForm->addWidget(caseStart, 1, 1, 1, 3, Qt::AlignTop);
    layout_caseForm->addWidget(caseEnd, 2, 1, 1, 3, Qt::AlignTop);
    layout_caseForm->addWidget(caseSummary, 3, 1, 3, 3, Qt::AlignTop | Qt::AlignLeft);

    setLayout(layout_caseForm);

    // Set up connections
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelForm()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveForm()));
}

void CaseFormDialog::loadForm(QString start, QString end, QString summary)
{
    caseStart->setText(start);
    caseEnd->setText(end);
    caseSummary->setText(summary);
}

void CaseFormDialog::cancelForm()
{
    close();
}

static bool isCleanName(QString *s)
{
    // Enforce format
    QStringList indices = s->split("-", QString::SkipEmptyParts);

    if (indices.length() == 3)
    {
        int month = indices[0].toInt();
        int day   = indices[1].toInt();
        int year  = indices[2].toInt();

        if (0 > month || month > 12) return false;
        if (0 > day || day > 31) return false;
        if (0 > year || year > 2100) return false;

        return true;
    } else
    {
        return false;
    }
}

void CaseFormDialog::saveForm()
{
    // TODO
    // Filter/verify all input here
    QString case_name = caseStart->text();
    isCleanName(&case_name);

    // Don't add cases that already exist
    if(!editable)
    {
        if (rootDir->exists(rootDir->absoluteFilePath(client_name + "/" + case_name)))
        {
            QMessageBox notify_msg;
            notify_msg.setText("This case already exists!\n Please use a different date.");
            notify_msg.exec();

            return;
        }

        // No name collistion, create new case directory
        rootDir->mkdir(rootDir->absoluteFilePath(client_name)+ "/" + case_name);
        original_case = case_name;

    } else
    {
        // Rename the case file if that was an edit
        if (original_case != case_name)
        {
            rootDir->rename(rootDir->absoluteFilePath(client_name + "/" + original_case)
                            , rootDir->absoluteFilePath(client_name) + "/" + case_name);
            original_case = case_name;
        }
    }

    // Delete old file
    QFile::remove(rootDir->absoluteFilePath(client_name + "/" + original_case + "/.info"));

    // Save new contents to case info file
    QFile fp_details(rootDir->absoluteFilePath(client_name + "/" + original_case + "/.info"));
    if(fp_details.open(QIODevice::ReadWrite))
    {
        QTextStream details( &fp_details );
        details << caseStart->text() << endl;
        details << caseEnd->text() << endl;
        details << caseSummary->toPlainText();

        fp_details.close();
        qDebug() << "Successfully edited case file!";
    }

    // Make sure to reload case displays
    reloadCase = true;

    close();
}

QString CaseFormDialog::getCaseEntry()
{
    return caseStart->text();
}

bool CaseFormDialog::isReloadRequired()
{
    return reloadCase;
}
