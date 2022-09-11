#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>

class StartWindow : public QWidget
{
    Q_OBJECT

public:
    StartWindow();
    ~StartWindow();

public slots:
    void onNew();
    void onLoad();

private:
    QLabel* name;
    QPushButton* load;
    QPushButton* newdialogue;
    QPushButton* exit;
    QBoxLayout* layout;
};
#endif // STARTWINDOW_H
