#ifndef CASEFORMDIALOG_H
#define CASEFORMDIALOG_H

#include <QDialog>
#include <QDir>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDir>
#include <QDebug>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>


class CaseFormDialog : public QDialog
{
    Q_OBJECT

public:
    CaseFormDialog(QWidget *parent, bool edit, QDir *app_root, QString clientName, QString caseName);

public slots:
    void cancelForm();
    void saveForm();
    void loadForm(QString start, QString end, QString summary);
    QString getCaseEntry();
    bool isReloadRequired();

private:
    QDir *rootDir;
    QString client_name;
    QString original_case;
    bool editable;
    bool reloadCase;

    // Labels
    QLabel *promptStart;
    QLabel *promptEnd;
    QLabel *promptSummary;

    // Line Edits
    QLineEdit *caseStart;
    QLineEdit *caseEnd;
    QTextEdit *caseSummary;

    // Buttons
    QPushButton *cancelButton;
    QPushButton *saveButton;
};
#endif // CASEFORM_H
