// -----------------------------------------------------------------------------------------
// Name       : David Chalco
// Description: It's the block labeled "Details", this is the event handler for the
//              case Descriptor. It describes the currently selected client.
// To Do      :
//               1.) Filter client input in forms before passing off to program.
//               2.) Crash when user enters insufficient lines in form --> details[3] = NULL
//               3.) Outline selector with grid
//               4.) Make sure Case summary isnt reading/saving extraneous newlines
//               5.) Finish implementing client photos
//               6.) Fix Crash when consecutively adding clients. Check for same issue in cases
//               7.) Finish adding/editing cases. ALMOST DONE!!!
// -----------------------------------------------------------------------------------------

#include "finder.h"
#include "clientform.h"
#include "caseform.h"




bool CaseFinder::loadClientDetails(QString client)
{
    // Open Client descriptor
    QFile fp_details(rootDir->absoluteFilePath(client + "/.info"));

    if (fp_details.open(QFile::ReadOnly | QFile::Text))
    {
        // Open up data input stream and parse file
        QTextStream details(&fp_details);
        QStringList detail_lines = details.readAll().split('\n');

        // It is assumed that the client/case creator
        // always puts a new line in place of an empty entry,
        // gaurenteeing there will be at least as many valid entries in parsed list.
        // Update display
        clientName->setText("<b>" + detail_lines[0] + "<\b>");
        clientEmail->setText(detail_lines[1]);
        clientNumber->setText(detail_lines[2]);

        QString accumulater;
        for (int i=3; i<detail_lines.size() - 1; i++)
        {
            accumulater += detail_lines[i] + "\n";
        }
        accumulater += detail_lines[detail_lines.size() - 1]; // No extraneous newlines

        clientAddress->clear();
        clientAddress->setText(accumulater);

        fp_details.close();

        return true;
    } else
    {
        // Clear display and notify User
        clientName->setText("File Error!");
        clientEmail->setText("--");
        clientNumber->setText("--");
        clientAddress->setText("--");

        return false;
    }
}

// Description :
// Signals In  : case selected from case Selection Menu
void CaseFinder::updateClientDetails(QString client)
{
    if (!rootDir->exists(client))
    {
        // Clear display and notify User
        clientName->setText("User Not Found!");
        clientEmail->setText("--");
        clientNumber->setText("--");
        clientAddress->setText("--");

        return;
    }

    loadClientDetails(client);

}

void CaseFinder::updateCaseDetails(QString case_name)
{
    QString client = clientSelect->currentText();
    if (!rootDir->exists(client + "/" + case_name))
    {
        // Clear display
        caseStart->setText("--");
        caseEnd->setText("--");
        caseSummary->clear();
        caseSummary->setText("Case Note Found");

        return;
    }

    // Open Client descriptor
    if (!rootDir->exists(client + "/" + case_name + "/.info")) qDebug() << "Attempting to open case file without .info file!";
    QFile fp_details(rootDir->absoluteFilePath(client + "/" + case_name + "/.info"));

    // Get details from
    qDebug() << rootDir->absoluteFilePath(client + "/" + case_name + "/.info");
    if (fp_details.open(QFile::ReadOnly | QFile::Text))
    {
        // Open up data input stream and parse file
        QTextStream details(&fp_details);

        QStringList detail_lines = details.readAll().split('\n', QString::SkipEmptyParts);

        //QString status = case_name;
        caseStart->setText(case_name);

        QString date = ("0" == detail_lines[1])? ("OPEN") : (detail_lines[1]);
        caseEnd->setText(date);

        QString accumulater;
        for (int i=2; i<detail_lines.size(); i++)
        {
            accumulater += detail_lines[i] + "\n";
        }
        caseSummary->setText(accumulater);

        fp_details.close();
    } else
    {
        // Clear display
        caseStart->setText("--");
        caseEnd->setText("--");
        caseSummary->setText("File Error!");
    }
}

void CaseFinder::editClientDetails()
{
    // Make sure a client a valid client is chose first
    if(!QFile::exists(rootDir->absoluteFilePath(clientSelect->currentText())))
    {
        QMessageBox notify_msg;
        notify_msg.setText("Please select a client before editing a client!");
        notify_msg.exec();
        return;
    }

    // Load contents into form, and save
    ClientFormDialog form(this, true, rootDir, QString(clientSelect->currentText()));

    QString name = clientSelect->currentText();
    for (int i=0; i<name.length(); i++)
    {
        if (name[i] == '_') name[i] = ' ';
    }

    form.setWindowTitle("Editing " + clientSelect->currentText());
    form.loadForm(name, clientEmail->text(), clientNumber->text(), clientAddress->text());
    form.setModal( true );
    form.exec();   

    // Check for necessary reload
    if (form.isReloadRequired())
    {
        QString clientDirName = form.getClientEntry();
        for (int i=0; i<clientDirName.length(); i++)
        {
            if (clientDirName[i] == ' ') clientDirName[i] = '_';
        }

        // Reload selector
        clientSelect->setItemText(clientSelect->currentIndex(), clientDirName);
        emit clientUpdated(clientDirName);

        // Reload descriptor
        loadClientDetails(clientDirName);
    }
}

// TODO: Refresh client display with newly added client's
void CaseFinder::addClientDetails()
{   
    // Load NEW form and save where necessary
    ClientFormDialog form(this, false, rootDir, clientSelect->currentText());

    // Upload client listing and refresh
    form.setWindowTitle("Adding A New Client");
    form.setModal( true );
    form.exec();

    // Refresh client info display
    if (form.isReloadRequired())
    {
        QString clientDirName = form.getClientEntry();
        for (int i=0; i<clientDirName.length(); i++)
        {
            if (clientDirName[i] == ' ') clientDirName[i] = '_';
        }

        // Append selection menu with new entry
        clientSelect->addItem(clientDirName);
        clientSelect->setCurrentIndex(clientSelect->count() - 1);
        emit clientUpdated(clientDirName);

        // Load new client details to client info display
        loadClientDetails(clientDirName);

    }
}

void CaseFinder::editCaseDetails()
{
    // Make sure a client a valid client is chosen first
    if(!QFile::exists(rootDir->absoluteFilePath(clientSelect->currentText()))
            || !QFile::exists(rootDir->absoluteFilePath(clientSelect->currentText() + "/" + caseSelect->currentText())))
    {
        QMessageBox notify_msg;
        notify_msg.setText("Please select a valid client and case, before editing a case!");
        notify_msg.exec();
        return;
    }

    // Load contents into form
    CaseFormDialog form(this, true, rootDir, clientSelect->currentText(), caseSelect->currentText());

    // Closes on Save or Cancel
    form.setWindowTitle("Editing " + clientSelect->currentText() + " : " + caseSelect->currentText());
    form.loadForm(caseStart->text(), caseEnd->text(), caseSummary->text());
    form.setModal( true );
    form.exec();

    // Reload case info display on save
    if (form.isReloadRequired())
    {
        // Reload case descriptors with new info
        updateCaseDetails(caseSelect->currentText());
    }
}


// TODO: Refresh client display with newly added cases
void CaseFinder::addCaseDetails()
{
    // Make sure a client a valid client is chose first
    if(!QFile::exists(rootDir->absoluteFilePath(clientSelect->currentText())))
    {
        QMessageBox notify_msg;
        notify_msg.setText("Please select a valid client before adding a case!");
        notify_msg.exec();
        return;
    }

    qDebug() << "Adding case";
    // Load NEW form
    CaseFormDialog form(this, false, rootDir, clientSelect->currentText(), "new form");


    // Closes on save or cancel
    form.setWindowTitle("Adding A New Case");
    form.setModal( true );
    form.exec();

    // Reload client info details when necessary
    if (form.isReloadRequired())
    {
        // Make new case the current selection
        QString CaseDirName = form.getCaseEntry();
        caseSelect->addItem(CaseDirName);
        caseSelect->setCurrentIndex(caseSelect->count() - 1);

        // Reload Case descriptor to show new case details
        updateCaseDetails(CaseDirName);
    }
}

