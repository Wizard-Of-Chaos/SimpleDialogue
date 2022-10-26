#include "newtreewindow.h"

NewTreeWindow::NewTreeWindow()
{
    //Declare form based layout to make it work in a vertical line
    layout = new QFormLayout(this);
    name = new QLineEdit(this);
    minSector = new QSpinBox(this);
    minSector->setMaximum(7);
    minSector->setMinimum(0);
    addSpeaker = new QPushButton(tr("Add speaker"), this);
    speakers = new QListWidget(this);
    speakers->setLayoutMode(QListView::Batched);

    layout->addRow(tr("Name: "), name);
    layout->addRow(tr("Minimum Sector"), minSector);
    layout->addRow(tr("Speakers: "), speakers);
    layout->addRow(addSpeaker);
    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    layout->addRow(buttons);
    //connect is a QT macro to add a function to a button.
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(addSpeaker, SIGNAL(clicked()), this, SLOT(onAddSpeaker()));
}
NewTreeWindow::~NewTreeWindow()
{
    delete layout;
    delete name;
    delete minSector;
    delete speakers;
    delete buttons;
    delete addSpeaker;
}

void NewTreeWindow::onAddSpeaker()
{
    newSpeakerLine = new QLineEdit(this);
    addSpeaker->hide();
    layout->insertRow(4, tr("Name:"), newSpeakerLine);
    newSpeakerConfirm = new QPushButton(tr("OK"),this);
    layout->insertRow(5, newSpeakerConfirm);
    connect(newSpeakerConfirm, SIGNAL(clicked()), this, SLOT(onNewSpeakerConfirm()));
}

void NewTreeWindow::onNewSpeakerConfirm()
{
    if(newSpeakerLine) {
        speakers->addItem(newSpeakerLine->text());
        layout->removeRow(newSpeakerLine);
        //delete newSpeakerLine;
    }
    if(newSpeakerConfirm) {
        layout->removeRow(newSpeakerConfirm);
        //delete newSpeakerConfirm;
    }
    newSpeakerLine = nullptr;
    newSpeakerConfirm = nullptr;
    addSpeaker->show();
}
