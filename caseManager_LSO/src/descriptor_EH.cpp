// -----------------------------------------------------------------------------------------
// Name       : David Chalco
// Description: It's the block labeled "Details", this is the event handler for the
//              case Descriptor. It describes the currently selected client.
// -----------------------------------------------------------------------------------------

#include "finder.h"

typedef struct ClientDetails
{
    QString legal_name;
    QString contact_number;
    QString address;
} ClientDetails;

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

    // Open Client descriptor
    QFile fp_details(rootDir->absoluteFilePath(client + "/.info"));

    qDebug() << rootDir->absoluteFilePath(client + "/.info");
    if (fp_details.open(QFile::ReadOnly | QFile::Text))
    {
        // Open up data input stream and parse file
        QTextStream details(&fp_details);
        QStringList detail_lines = details.readAll().split('\n');

        // It is assumed that the client/case creator
        // always puts a new line in place of an empty entry,
        // gaurenteeing there will be at least as many valid entries in parsed list.
        qDebug() << detail_lines;
        // Update display
        clientName->setText("<b>" + detail_lines[0] + "<\b>");
        clientEmail->setText(detail_lines[1]);
        clientNumber->setText(detail_lines[2]);

        QString accumulater;
        for (int i=3; i<detail_lines.size(); i++)
        {
            accumulater += detail_lines[i] + "\n";
        }
        clientAddress->setText(accumulater);
    } else
    {
        // Clear display and notify User
        clientName->setText("File Error!");
        clientEmail->setText("--");
        clientNumber->setText("--");
        clientAddress->setText("--");
    }
}

void CaseFinder::updateCaseDetails(QString case_name)
{
    QString client = clientSelect->currentText();
    if (!rootDir->exists(client + "/" + case_name))
    {
        // Clear display
        caseStart->setText("--");
        caseEnd->setText("--");
        caseSummary->setText("Case Note Found");

        return;
    }

    // Open Client descriptor
    QFile fp_details(rootDir->absoluteFilePath(client + "/" + case_name + "/.info"));

    // Get details from
    qDebug() << rootDir->absoluteFilePath(client + "/" + case_name + "/.info");
    if (fp_details.open(QFile::ReadOnly | QFile::Text))
    {
        // Open up data input stream and parse file
        QTextStream details(&fp_details);

        QStringList detail_lines = details.readAll().split('\n', QString::SkipEmptyParts);
        qDebug() << detail_lines;

        QString status = case_name;
        caseStart->setText(status);

        QString date = ("0" == detail_lines[1])? ("OPEN") : (detail_lines[1]);
        caseEnd->setText(date);

        QString accumulater;
        for (int i=2; i<detail_lines.size(); i++)
        {
            accumulater += detail_lines[i] + "\n";
        }
        caseSummary->setText(accumulater);
    } else
    {
        // Clear display
        caseStart->setText("--");
        caseEnd->setText("--");
        caseSummary->setText("File Error!");
    }
}
