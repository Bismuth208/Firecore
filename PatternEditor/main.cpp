#include <QApplication>

#define BUILD " v0.1a " // __DATE__" " __TIME__

#include "patterneditorwidget.h"
#include "common.h"

PatternEditorWidget *mainWindow = NULL;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    mainWindow = new PatternEditorWidget();
    mainWindow->setMinimumWidth(mainWindow->tableEditor->width());
    mainWindow->setWindowTitle("PatternEditor" BUILD);
    mainWindow->show();

    int res = app.exec();

    delete mainWindow;

    return res;
}
