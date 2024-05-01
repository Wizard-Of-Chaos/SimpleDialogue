#ifndef NEWTREEWINDOW_H
#define NEWTREEWINDOW_H
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpinBox>

class NewTreeWindow : public QDialog
{
    Q_OBJECT
public:
    NewTreeWindow();
    ~NewTreeWindow();
    QFormLayout* layout;
    QLineEdit* name;
    QSpinBox* minSector;
    QListWidget* speakers;
    QPushButton* addSpeaker;
    QDialogButtonBox* buttons;

    QLineEdit* newSpeakerLine;
    QPushButton* newSpeakerConfirm;
public slots:
    void onAddSpeaker();
    void onNewSpeakerConfirm();
    void onAccept();
};


#endif // NEWTREEWINDOW_H
