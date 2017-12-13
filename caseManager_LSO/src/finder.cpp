#include "finder.h"

CaseFinder::CaseFinder(QWidget *parent)
{
    setWindowTitle("");
    // [DEV] Later on declaration on client Dir will be set by wrapper class
    rootDir = new QDir::QDir(QDir::home());
    rootDir->cd(HOME_CLIENT_DIR);
    qDebug() << rootDir->path();

    createSelector();
    createDescriptor();
    createFileManager();

    // Set main layout
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(selectorBox);
    mainLayout->addWidget(descriptorBox);
    mainLayout->addWidget(fileManagerBox);
    setLayout(mainLayout);

    // Connect events
    connect(findClient, SIGNAL(released()), this, SLOT(searchClient()));
    connect(clientFilters, SIGNAL(returnPressed()), this, SLOT(searchClient()));

    connect(clientSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateClient(QString)));
    connect(clientSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateClientDetails(QString)));
    connect(clientSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateClientFiles(QString)));

    connect(caseSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateCaseDetails(QString)));
    connect(caseSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateCaseFiles(QString)));

    connect(findFile, SIGNAL(released()), this, SLOT(chooseFile()));
    connect(addFile, SIGNAL(released()), this, SLOT(addChosenFile()));
    connect(fileTree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openFile(QModelIndex)));

    connect(editClient, SIGNAL(released()), this, SLOT(editClientDetails()));
    connect(addClient, SIGNAL(released()), this, SLOT(addClientDetails()));
    connect(editCase, SIGNAL(released()), this, SLOT(editCaseDetails()));
    connect(addCase, SIGNAL(released()), this, SLOT(addCaseDetails()));

    connect(this, SIGNAL(clientUpdated(QString)), this, SLOT(updateClient(QString)));
}

void CaseFinder::createSelector()
{
    selectorBox = new QGroupBox(tr("Search Case"));
    selectorBox->setMaximumHeight(100);

    // Selector line edits
    clientFilters = new QLineEdit(this);
    clientFilters->setPlaceholderText("Client Name");
    clientFilters->setMaximumWidth(250);

    // Selector buttons
    findClient = new QPushButton("Search", this);
    findClient->setMaximumWidth(100);

    // Selector drop menus
    clientSelect = new QComboBox(this);
    clientSelect->addItem("Choose Client...");

    caseSelect   = new QComboBox(this);
    caseSelect->addItem("Choose Case...");

    // Layout
    QHBoxLayout *layout_selectorBox = new QHBoxLayout;

    QWidget *searchBox = new QWidget();
    QHBoxLayout *layout_searchBox = new QHBoxLayout;
    layout_searchBox->addWidget(clientFilters);
    layout_searchBox->addWidget(findClient);
    searchBox->setLayout(layout_searchBox);

    layout_selectorBox->addWidget(searchBox);
    layout_selectorBox->setSpacing(20);
    layout_selectorBox->addWidget(clientSelect);
    layout_selectorBox->addWidget(caseSelect);

    selectorBox->setLayout(layout_selectorBox);
}

void CaseFinder::createDescriptor()
{
    descriptorBox = new QGroupBox(tr("Details"));
    descriptorBox->setMinimumHeight(100);
    descriptorBox->setMaximumHeight(250);

    clientPhoto = new QLabel(this);
    QPixmap clientImage(rootDir->absoluteFilePath(".no-profile.jpg"));
    clientPhoto->setPixmap(clientImage.scaled(clientPhoto->width(), clientPhoto->height(), Qt::KeepAspectRatio));

    clientName = new QLabel("--", this);
    clientEmail = new QLabel("--", this);
    clientNumber = new QLabel("--", this);
    clientAddress = new QLabel("--", this);

    caseStart = new QLabel("--", this);
    caseEnd = new QLabel("--", this);

    caseSummary = new QLabel("--", this);
    caseSummary->setWordWrap(true);

    addClient = new QPushButton("Add", this);
    editClient = new QPushButton("Edit", this);

    addCase = new QPushButton("Add", this);
    editCase = new QPushButton("Edit", this);

    // Layout
    QGridLayout *layout_descriptorBox = new QGridLayout;
    layout_descriptorBox->setSpacing(2);

    layout_descriptorBox->addWidget(clientName, 0, 0, 1, 2, Qt::AlignCenter | Qt::AlignTop);

    layout_descriptorBox->addWidget(clientPhoto, 1, 0, 5, 2, Qt::AlignCenter);


    layout_descriptorBox->addWidget(new QLabel("<h4>Client Info<\h4>", this), 0, 2, 1, 3, Qt::AlignTop | Qt::AlignHCenter);
    layout_descriptorBox->addWidget(new QLabel("<b>Email : <\b>", this), 1, 2, 1, 1, Qt::AlignRight);
    layout_descriptorBox->addWidget(clientEmail, 1, 3, 1, 2, Qt::AlignLeft);

    layout_descriptorBox->addWidget(new QLabel("<b>Phone : <\b>", this), 2, 2, 1, 1, Qt::AlignRight | Qt::AlignTop);
    layout_descriptorBox->addWidget(clientNumber, 2, 3, 1, 2, Qt::AlignLeft | Qt::AlignTop);

    layout_descriptorBox->addWidget(new QLabel("<b>Address : <\b>", this), 3, 2, 1, 1, Qt::AlignRight | Qt::AlignTop);
    layout_descriptorBox->addWidget(clientAddress, 3, 3, 3, 2, Qt::AlignLeft | Qt::AlignTop);

    layout_descriptorBox->addWidget(addClient, 6, 3, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    layout_descriptorBox->addWidget(editClient, 6, 4, 1, 1, Qt::AlignLeft | Qt::AlignTop);

    layout_descriptorBox->addWidget(new QLabel("<h4>Case Info<\h4>", this), 0, 7, 1, 4, Qt::AlignTop | Qt::AlignHCenter);
    layout_descriptorBox->addWidget(new QLabel("<b>Start : <\b>", this), 1, 6, 1, 1, Qt::AlignRight);
    layout_descriptorBox->addWidget(caseStart, 1, 7, 1, 1, Qt::AlignLeft);

    layout_descriptorBox->addWidget(new QLabel("<b>End : <\b>", this), 1, 9, 1, 1, Qt::AlignRight);
    layout_descriptorBox->addWidget(caseEnd, 1, 10, 1, 1, Qt::AlignLeft);

    layout_descriptorBox->addWidget(new QLabel("<b>Summary : <\b>", this), 2, 6, 1, 1, Qt::AlignRight | Qt::AlignTop);
    layout_descriptorBox->addWidget(caseSummary, 2, 7, 4, 4, Qt::AlignLeft | Qt::AlignTop);

    layout_descriptorBox->addWidget(addCase, 6, 8, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    layout_descriptorBox->addWidget(editCase, 6, 9, 1, 1, Qt::AlignLeft | Qt::AlignTop);

    descriptorBox->setLayout(layout_descriptorBox);
}

void CaseFinder::createFileManager()
{
    fileManagerBox = new QGroupBox(tr("Case Files"));

    // New File Selector
    addfilePath = new QLineEdit(this);
    addfilePath->setPlaceholderText("Choose File...");

    findFile = new QPushButton("Select", this);
    addFile  = new QPushButton("Add", this);

    // File Viewer
    fileViewer = new QFileSystemModel;
    fileViewer->setRootPath(rootDir->absolutePath());

    fileTree = new QTreeView(this);
    fileTree->setModel(fileViewer);
    fileTree->setRootIndex(fileViewer->index(rootDir->absolutePath()));
    fileTree->resizeColumnToContents(0);
    fileTree->resizeColumnToContents(1);

    // Layout
    QVBoxLayout *layout_fileManagerBox = new QVBoxLayout;

    QWidget *fileAdder = new QWidget();
    QHBoxLayout *layout_fileAdder = new QHBoxLayout;
    layout_fileAdder->addWidget(addfilePath);
    layout_fileAdder->addWidget(findFile);
    layout_fileAdder->addWidget(addFile);
    fileAdder->setLayout(layout_fileAdder);

    layout_fileManagerBox->addWidget(fileAdder);
    layout_fileManagerBox->addWidget(fileTree);

    fileManagerBox->setLayout(layout_fileManagerBox);
}

