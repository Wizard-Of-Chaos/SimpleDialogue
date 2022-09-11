#include "startwindow.h"
#include "newtreewindow.h"
#include "treeview.h"
#include <QFileDialog>
#include <QCoreApplication>

StartWindow::StartWindow()
{
    name = new QLabel(tr("Dialogue Tree Editor / Exporter"));
    load = new QPushButton(tr("Load Tree"));
    newdialogue = new QPushButton(tr("New Tree"));
    exit = new QPushButton(tr("Exit"));

    layout = new QVBoxLayout;
    layout->addWidget(name);
    layout->addWidget(load);
    layout->addWidget(newdialogue);
    layout->addWidget(exit);

    setLayout(layout);

    connect(load, SIGNAL(clicked()), this, SLOT(onLoad()));
    connect(newdialogue, SIGNAL(clicked()), this, SLOT(onNew()));
    connect(exit, SIGNAL(clicked()), this, SLOT(close()));
}

void StartWindow::onLoad()
{
    QString filename=QFileDialog::getOpenFileName(this, "Save XML", QCoreApplication::applicationDirPath(), "XML File(*.xml)");
    if(filename.isNull()) return;

    TreeView* view = new TreeView();
    view->loadXML(filename);
    view->resize(500,500);
    view->show();
}

void StartWindow::onNew()
{
    NewTreeWindow wind;
    if(wind.exec() == QDialog::Accepted) {
        TreeView* view = new TreeView();
        std::vector<QString> speakers;
        for(int i = 0; i < wind.speakers->count(); ++i) {
            speakers.push_back(wind.speakers->item(i)->text());
        }
        view->setSector(wind.minSector->value());
        view->setTreeID(wind.name->text());
        view->newTree(speakers);
        view->resize(500,500);
        view->show();
    }
}

StartWindow::~StartWindow()
{
    delete name;
    delete load;
    delete newdialogue;
    delete exit;
    delete layout;
}

