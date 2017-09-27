// -----------------------------------------------------------------------------------------
// Name       : David Chalco
// Description: It's the block labeled "Search Case", this is the event handler for the
//              case Selector. It handles the block's dynamic features.
// -----------------------------------------------------------------------------------------


#include "finder.h"
// Description : Tokenizes each name part and filters the directory
//                to yield client folders that match all name parts.
// Signals In  : clicked "Search" button or pressed "Return/Enter"
void CaseFinder::searchClient()
{
    QStringList nameFilters = clientFilters->text().split(" ");
    QStringList results = rootDir->entryList();
    for (QString filter : nameFilters)
    {
        results = results.filter(filter, Qt::CaseInsensitive);
    }

    // Update Client Selection Menu
    clientSelect->clear();
    clientSelect->addItem("Select Client...");
    clientSelect->addItems(results);
}

// Description : Updates selection list in Case Select Menu
// To Do       : Put some clues in output for NULL results to avoid confusion
// Signals In  : Client selected from Client Selection Menu
// Signals Out : updateClient();
void CaseFinder::updateClient(QString client)
{
    if (!rootDir->exists(client))
    {
        caseSelect->clear();
        caseSelect->addItem("Select Case...");

        return;
    }

    QDir clientDir(rootDir->absoluteFilePath(client));

    // Filter then show case listings
    clientDir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    caseSelect->clear();
    caseSelect->addItem("Select Case...");
    caseSelect->addItems(clientDir.entryList());
}
