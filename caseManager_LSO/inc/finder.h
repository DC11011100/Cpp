#ifndef FINDER_H
#define FINDER_H

#include <qlineedit.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QFileSystemModel>
#include <QTreeView>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>

#define HOME_CLIENT_DIR "case_Manager/qt_testDB"

class CaseFinder : public QDialog
{
    Q_OBJECT

public:
    CaseFinder(QWidget *parent);

public slots:
    void searchClient();
    void updateClient(QString client);
    void updateClientFiles(QString client);
    void updateCaseFiles(QString case_name);

    void updateClientDetails(QString client);
    void updateCaseDetails(QString case_name);
    void editClientDetails();
    void addClientDetails();
    void editCaseDetails();
    void addCaseDetails();

    void chooseFile();
    void addChosenFile();
    void openFile(QModelIndex file_index);

private:
    // Subconstructors
    void createSelector();
    void createDescriptor();
    void createFileManager();

    bool loadClientDetails(QString client);

    // Labels
    QLabel *clientPhoto;
    QLabel *clientName;
    QLabel *clientEmail;
    QLabel *clientNumber;
    QLabel *clientAddress;

    QLabel *caseStart;
    QLabel *caseEnd;
    QLabel *caseSummary;

    // Boxes
    QGroupBox *selectorBox;
    QGroupBox *descriptorBox;
    QGroupBox *fileManagerBox;

    // Line Edits
    QLineEdit *clientFilters;
    QLineEdit *addfilePath;

    // Buttons
    QPushButton *findClient;
    QPushButton *findFile;
    QPushButton *addFile;
    QPushButton *addClient;
    QPushButton *addCase;
    QPushButton *editClient;
    QPushButton *editCase;

    // Drop down menus
    QComboBox *clientSelect;
    QComboBox *caseSelect;

    // File viewers
    QFileSystemModel *fileViewer;

    // File Tree View
    QTreeView *fileTree;

    // Root directory
    QDir *rootDir;

signals:
    void clientSearched(QStringList results);
    void clientUpdated(QString client);
};

#endif // FINDER_H
