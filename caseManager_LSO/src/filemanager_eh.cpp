// -----------------------------------------------------------------------------------------
// Name       : David Chalco
// Description: It's the block labeled "Case Files", this is the event handler for the
//              case file manager. It allows fast User traversal/injection of files, which
//              the applicaton automatically bookkeeps.
// -----------------------------------------------------------------------------------------
#include "finder.h"

void CaseFinder::updateClientFiles(QString client)
{
    if (!rootDir->exists(client))
    {
        fileTree->setRootIndex(fileViewer->index(rootDir->absolutePath()));
        fileTree->resizeColumnToContents(0);
        fileTree->resizeColumnToContents(1);

        return;
    }

    // Display the new existing case directory
    fileTree->setRootIndex(fileViewer->index(rootDir->absoluteFilePath(client)));
    fileTree->setColumnWidth(0,200);
}

void CaseFinder::updateCaseFiles(QString case_name)
{
    QString client = clientSelect->currentText();
    if (!rootDir->exists(client + "/" + case_name))
    {
        if (!rootDir->exists(client))
        {
            fileTree->setRootIndex(fileViewer->index(rootDir->absolutePath()));
            fileTree->resizeColumnToContents(0);
            fileTree->resizeColumnToContents(1);
        } else
        {
            fileTree->setRootIndex(fileViewer->index(rootDir->absoluteFilePath(client)));
            fileTree->resizeColumnToContents(0);
            fileTree->resizeColumnToContents(1);
        }

        return;
    }

    // Display the new existing client directory
    fileTree->setRootIndex(fileViewer->index(rootDir->absoluteFilePath(client + "/" + case_name)));
    fileTree->resizeColumnToContents(0);
    fileTree->resizeColumnToContents(1);

}

void CaseFinder::chooseFile()
{
    QString chosen = QFileDialog::getOpenFileName(this, "Choose a file to add to the folder", "/", "");
    addfilePath->setText(chosen);
}

void CaseFinder::addChosenFile()
{
    QString chosen = addfilePath->text();

    // Users can only add files to client/case folders
    addfilePath->clear();

    // Get file name
    QFileInfo chosenInfo(chosen);
    QString chosen_name(chosenInfo.fileName());

    // TO DO xDC
    // For now the copy will can fail safely so no input checks are necessary
    // However, in the event of an error it would be best to notify users using QErrorMessage
    // but for now this is not implemented
    QString currDirPath = fileViewer->filePath(fileTree->rootIndex());
    if (false == QFile::copy(chosen, currDirPath + "/" + chosen_name))
    {
        // Error handling
    }
}

void CaseFinder::openFile(QModelIndex file_index)
{
    QString chosen = fileViewer->filePath(file_index);
    QDesktopServices::openUrl(QUrl("file://" + chosen));
}

