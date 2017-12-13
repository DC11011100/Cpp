#ifndef CLIENTFORMDIALOG_H
#define CLIENTFORMDIALOG_H

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


class ClientFormDialog : public QDialog
{
    Q_OBJECT

public:
    ClientFormDialog(QWidget *parent, bool edit, QDir *file_path, QString client_name);
    void loadForm(QString name, QString email, QString number, QString address);
    QString getClientEntry();
    bool isReloadRequired();

public slots:
    void choosePhoto();
    void cancelForm();
    void saveForm();

private:
    QDir *rootDir;
    QString original_name;
    bool editable;
    bool reloadClient;

    // Labels
    QLabel *clientPhoto;
    QLabel *promptName;
    QLabel *promptNumber;
    QLabel *promptEmail;
    QLabel *promptAddress;

    // Line Edits
    QLineEdit *clientPhotoPath;
    QLineEdit *clientName;
    QLineEdit *clientNumber;
    QLineEdit *clientEmail;
    QTextEdit *clientAddress;

    // Buttons
    QPushButton *clientPhotoSelect;
    QPushButton *cancelButton;
    QPushButton *saveButton;
};
#endif // CLIENTFORM_H
