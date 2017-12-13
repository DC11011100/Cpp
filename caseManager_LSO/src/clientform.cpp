#include "clientform.h"

ClientFormDialog::ClientFormDialog(QWidget *parent, bool edit, QDir *file_path, QString client_name)
{
    rootDir = file_path;
    original_name = client_name;
    editable = edit;
    reloadClient = false;

    clientPhoto   = new QLabel(this);
    QPixmap clientImage(rootDir->absoluteFilePath(".no-profile.jpg"));
    clientPhoto->setPixmap(clientImage.scaled(clientPhoto->width(), clientPhoto->height(), Qt::KeepAspectRatio));

    qDebug() << "Setting up client form components";
    promptName    = new QLabel("Name : ", this);
    promptNumber  = new QLabel("Number : ", this);
    promptEmail   = new QLabel("Email : ", this);
    promptAddress = new QLabel("Address : ", this);

    clientPhotoPath = new QLineEdit(this);
    clientPhotoPath->setPlaceholderText(" Choose profile picture..");
    clientPhotoSelect = new QPushButton("Select", this);

    clientName = new QLineEdit(this);
    clientName->setPlaceholderText(" First Middle Last");

    clientNumber = new QLineEdit(this);
    clientNumber->setPlaceholderText(" (818) 123-4567");


    clientEmail = new QLineEdit(this);
    clientEmail->setPlaceholderText(" user@domain.com");

    clientAddress = new QTextEdit(this);
    clientAddress->setPlaceholderText(" Apt#3\n 1234 fifth St.\n Los Angeles, CA 56789");
    clientAddress->setMinimumHeight(40);
    clientAddress->setMaximumHeight(80);

    cancelButton = new QPushButton("Cancel", this);
    saveButton   = new QPushButton("Save", this);

    qDebug() << "Setting up client form layout";
    // Set up layout
    QGridLayout *layout_clientForm = new QGridLayout(this);
    layout_clientForm->addWidget(new QLabel("<h4>Client Info<\h4>", this), 0, 0, 1, 4, Qt::AlignTop | Qt::AlignHCenter);
    layout_clientForm->addWidget(clientPhoto, 1, 0, 4, 4, Qt::AlignCenter);
    layout_clientForm->addWidget(clientPhotoPath, 5, 0, 1, 3);
    layout_clientForm->addWidget(clientPhotoSelect, 5, 3, 1, 1);
    layout_clientForm->addWidget(promptName, 6, 0, 1, 1, Qt::AlignTop | Qt::AlignRight);
    layout_clientForm->addWidget(promptEmail, 7, 0, 1, 1, Qt::AlignTop | Qt::AlignRight);
    layout_clientForm->addWidget(promptNumber, 8, 0, 1, 1, Qt::AlignTop | Qt::AlignRight);
    layout_clientForm->addWidget(promptAddress, 9, 0, 1, 1, Qt::AlignTop | Qt::AlignRight);
    layout_clientForm->addWidget(cancelButton, 10, 1, 1, 1);
    layout_clientForm->addWidget(saveButton, 10, 2, 1, 1);

    layout_clientForm->addWidget(clientName, 6, 1, 1, 3, Qt::AlignTop);
    layout_clientForm->addWidget(clientEmail, 7, 1, 1, 3, Qt::AlignTop);
    layout_clientForm->addWidget(clientNumber, 8, 1, 1, 3, Qt::AlignTop);
    layout_clientForm->addWidget(clientAddress, 9, 1, 1, 3, Qt::AlignTop | Qt::AlignLeft);

    qDebug() << "Setting up layout";
    setLayout(layout_clientForm);

    // Set up connections
    qDebug() << "Setting up connections";
    connect(clientPhotoSelect, SIGNAL(clicked()), this, SLOT(choosePhoto()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelForm()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveForm()));
}

void ClientFormDialog::loadForm(QString name, QString email, QString number, QString address)
{
    clientName->setText(name);
    clientEmail->setText(email);
    clientNumber->setText(number);
    clientAddress->setText(address);
}

QString ClientFormDialog::getClientEntry()
{
    return clientName->text();
}

void ClientFormDialog::choosePhoto()
{
    QString chosen = QFileDialog::getOpenFileName(this, "Choose a client profile picture", "/", "");
    clientPhotoPath->setText(chosen);
    return;
}

void ClientFormDialog::cancelForm()
{
    close();
}

void ClientFormDialog::saveForm()
{
    // Get input client name and make directory appropriate
    QString client_name = clientName->text();
    for(int i=0; i<client_name.length(); i++)
    {
        if (client_name[i] == ' ') client_name[i] = '_';
    }

    // Don't add clients that already exist
    if (!editable)
    {
        if (rootDir->exists(rootDir->absoluteFilePath(client_name)))
        {
            QMessageBox notify_msg;
            notify_msg.setText("This users already exists!\n Please use a different name.");
            notify_msg.exec();

            return;
        }

        // New client
        original_name = client_name;
        rootDir->mkdir(rootDir->absolutePath() + "/" + client_name);

    } else
    {
        if (original_name != client_name)
        {
            rootDir->rename(rootDir->absoluteFilePath(original_name)
                            , rootDir->absolutePath() + "/" + client_name);
            original_name = client_name;
        }
    }

    // In both addition/editions delete the old file because it is being overwritten.
    QFile::remove(rootDir->absoluteFilePath(original_name + "/.info"));

    // Save new contents to case info file
    QFile fp_details(rootDir->absoluteFilePath(client_name + "/.info"));
    if(fp_details.open(QIODevice::ReadWrite))
    {
        QTextStream details( &fp_details );
        details << clientName->text() << endl;
        details << clientEmail->text() << endl;
        details << clientNumber->text() << endl;
        details << clientAddress->toPlainText();

        fp_details.close();
        qDebug() << "Successfully edited case file!";
    }

    reloadClient = true;

    close();
}

bool ClientFormDialog::isReloadRequired()
{
    return reloadClient;
}

