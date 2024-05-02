#ifndef TREEVIEW_H
#define TREEVIEW_H
#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>

struct ChoiceData
{
    QString choiceText="";
    QString effect="none";
    QString nextNode="none";
    std::vector<QString> requiredFlags;
    std::vector<QString> requiredTrees;
    std::vector<QString> setFlags;
    int id=0;
};

class ChoiceItem : public QListWidgetItem
{
public:
    ChoiceItem(QString str, QListWidget* view) : QListWidgetItem(str, view) {}
    ChoiceData dat;
};

class NodeItem : public QListWidgetItem
{
public:
    NodeItem(QString str, QListWidget* view) : QListWidgetItem(str, view) {}
    QString dialogueText="";
    QString speaker="";
    std::vector<ChoiceData> choices;
};

class TreeView : public QWidget
{
    Q_OBJECT
public:
    TreeView();
    ~TreeView();
    void newTree(std::vector<QString>& speakers);
    void setSector(unsigned int min) {minSector = min;}
    void setTreeID(QString nid) {id = nid;}
    void loadXML(QString fname);

public slots:
    void onNodeSelect(QListWidgetItem* item);
    void onChoiceSelect(QListWidgetItem* item);
    void onEffectActivate(int index);
    void onNodeEditActivate(int index);
    void onAddNode();
    void onAddChoice();
    void onAddEffect();
    void onRemoveChoice();

    void onAddRequiredFlag();
    void onRemoveRequiredFlag();
    void onAddRequiredTree();
    void onRemoveRequiredTree();
    void onAddSetFlag();
    void onRemoveSetFlag();

    void onSaveXML();

private:
    void m_displayNode(NodeItem* item);
    void m_renameNode(QString old, QString str);
    ChoiceItem* m_addChoice(ChoiceData dat, std::string name="");
    void m_autosave();
    void m_saveCurrentChoice();
    void m_saveCurrentNode();
    NodeItem* currentItem=nullptr;
    ChoiceItem* currentChoice=nullptr;

    QString xmlName();
    void m_saveXML(QString fname);

    QGridLayout* layout;
    QListWidget* nodes;
    QLineEdit* idEdit;
    QComboBox* speakerEdit;
    QTextEdit* nodeTextEdit;
    QLabel* nextNodeLabel;
    QComboBox* nextNodeEdit;
    QComboBox* effectEdit;
    QPushButton* addNode;

    QListWidget* choices;
    QPushButton* addChoice;
    QPushButton* removeChoice; //Removes the CURRENT choice
    QTextEdit* choiceTextEdit;
    QPushButton* addEffect;

    QPushButton* addRequiredFlag;
    QPushButton* removeRequiredFlag;
    QPushButton* addRequiredTree;
    QPushButton* removeRequiredTree;
    QListWidget* requiredFlags;
    QListWidget* requiredTrees;

    QPushButton* addSetFlag;
    QPushButton* removeSetFlag;
    QListWidget* setsFlags;

    QPushButton* saveXML;

    QString id;
    unsigned int minSector;
    std::vector<QString> speakers;
};

#endif // TREEVIEW_H
