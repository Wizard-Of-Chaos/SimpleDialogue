#include "treeview.h"
#include <iostream>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFileDialog>
#include <QCoreApplication>
#include <QBuffer>
#include <QFile>

TreeView::TreeView()
{
    layout = new QGridLayout(this);
    nodes = new QListWidget(this);
    nodeId = new QLabel(this);
    nodeId->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    renameNode = new QPushButton(tr("Rename"), this);
    speakerEdit = new QComboBox(this);
    nodeTextEdit = new QTextEdit(this);
    nextNodeEdit = new QComboBox(this);
    effectEdit = new QComboBox(this);
    addNode = new QPushButton(tr("Add Node"), this);
    choices = new QListWidget(this);
    choiceTextEdit = new QTextEdit(this);
    addChoice = new QPushButton(tr("Add Choice"), this);
    addEffect = new QPushButton(tr("Add Immediate Effect"), this);
    saveXML = new QPushButton(tr("Save to XML"), this);
    removeChoice = new QPushButton(tr("Remove Choice"), this);
    nextNodeLabel = new QLabel(tr("Next node:"), this);

    addRequiredFlag = new QPushButton(tr("Add Required Flag"), this);
    removeRequiredFlag = new QPushButton(tr("Remove Required Flag"), this);
    addRequiredTree = new QPushButton(tr("Add Required Tree"), this);
    removeRequiredTree = new QPushButton(tr("Remove Required Tree"), this);
    requiredFlags = new QListWidget(this);
    requiredTrees = new QListWidget(this);

    addSetFlag = new QPushButton(tr("Add Flag To Set"), this);
    removeSetFlag = new QPushButton(tr("Remove Flag To Set"), this);
    setsFlags = new QListWidget(this);

    layout->setColumnMinimumWidth(0,150);
    layout->setColumnMinimumWidth(1, 120);
    layout->setColumnMinimumWidth(2, 120);
    layout->setRowMinimumHeight(1, 150);
    layout->setRowMinimumHeight(3, 80);
    layout->setRowMinimumHeight(4, 80);
    layout->addWidget(nodes, 1, 0, 2, 1);
    layout->addWidget(nodeId, 3, 1, 1, 1);
    layout->addWidget(renameNode, 3, 2, 1, 1);
    layout->addWidget(speakerEdit, 0, 1, 1, 2);
    layout->addWidget(nodeTextEdit, 1, 1, 2, 2);

    layout->addWidget(addNode, 0, 0, 1, 1);
    layout->addWidget(saveXML, 2, 0, 1, 1);

    layout->addWidget(addChoice, 0, 3, 1, 1);
    layout->addWidget(removeChoice, 0, 4, 1, 1);
    layout->addWidget(nextNodeLabel, 3, 3, 1, 1);
    layout->addWidget(nextNodeEdit, 3, 4, 1, 1);
    layout->addWidget(choices, 1, 3, 2, 2);
    layout->addWidget(choiceTextEdit, 1, 5, 1, 2);
    layout->addWidget(effectEdit, 4, 3, 1, 1);
    layout->addWidget(addEffect, 4, 4, 1, 1);

    layout->addWidget(addRequiredFlag, 2, 5, 1, 1);
    layout->addWidget(removeRequiredFlag, 2, 6, 1, 1);
    layout->addWidget(requiredFlags, 3, 5, 2, 2);
    layout->addWidget(addRequiredTree, 2, 7, 1, 1);
    layout->addWidget(removeRequiredTree, 2, 8, 1, 1);
    layout->addWidget(requiredTrees, 3, 7, 2, 2);

    layout->addWidget(addSetFlag, 0, 7, 1, 1);
    layout->addWidget(removeSetFlag, 0, 8, 1, 1);
    layout->addWidget(setsFlags, 1, 7, 1, 2);

    connect(removeChoice, SIGNAL(clicked()), this, SLOT(onRemoveChoice()));

    connect(addNode, SIGNAL(clicked()), this, SLOT(onAddNode()));
    connect(addChoice, SIGNAL(clicked()), this, SLOT(onAddChoice()));
    connect(addEffect, SIGNAL(clicked()), this, SLOT(onAddEffect()));
    connect(saveXML, SIGNAL(clicked()), this, SLOT(onSaveXML()));
    connect(nodes, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onNodeSelect(QListWidgetItem*)));
    connect(choices, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onChoiceSelect(QListWidgetItem*)));
    connect(addRequiredFlag, SIGNAL(clicked()), this, SLOT(onAddRequiredFlag()));
    connect(addRequiredTree, SIGNAL(clicked()), this, SLOT(onAddRequiredTree()));
    connect(addSetFlag, SIGNAL(clicked()), this, SLOT(onAddSetFlag()));
    connect(removeRequiredFlag, SIGNAL(clicked()), this, SLOT(onRemoveRequiredFlag()));
    connect(removeRequiredTree, SIGNAL(clicked()), this, SLOT(onRemoveRequiredTree()));
    connect(removeSetFlag, SIGNAL(clicked()), this, SLOT(onRemoveSetFlag()));
    connect(effectEdit, SIGNAL(activated(int)), this, SLOT(onEffectActivate(int)));
    connect(nextNodeEdit, SIGNAL(activated(int)), this, SLOT(onNodeEditActivate(int)));
    connect(renameNode, SIGNAL(clicked()), this, SLOT(onRenameNode()));

    nextNodeEdit->setEnabled(false);
    effectEdit->setEnabled(false);

    this->setLayout(layout);
    //this->resize(600, 400);
    this->show();
}

void TreeView::newTree(std::vector<QString>& speakerList)
{
    NodeItem* node = new NodeItem(tr("root"), nodes);
    effectEdit->addItem(tr("none"));
    effectEdit->addItem(tr("exit"));
    for(auto speaker : speakerList) {
        node->speaker = speaker;
        speakers.push_back(speaker);
        speakerEdit->addItem(speaker);
    }
    nextNodeEdit->addItem("none");
    nextNodeEdit->addItem(node->text());
    m_displayNode(node);
    currentItem = node;
}

void TreeView::m_displayNode(NodeItem* item)
{
    nodeId->setText(item->text());
    QString txt = item->dialogueText;
    nodeTextEdit->setText(txt);
    bool found=false;
    for(int i = 0; i < speakerEdit->count(); ++i) {
        if(speakerEdit->itemText(i) == item->speaker) {
            speakerEdit->setCurrentIndex(i);
            found=true;
            break;
        }
    }
    if(!found) {
        speakers.push_back(item->speaker);
    }
    choices->clear();
    int count = 0;
    for(auto choice : item->choices) {
        std::string choiceName = "Choice " + std::to_string(count);
        if(choice.nextNode == "none") choiceName += " [EXIT]";
        if (choice.effect == "exit") choiceName += " [NONE]";
        auto choiceItem = new ChoiceItem(tr(choiceName.c_str()), choices);
        choiceItem->dat = choice;
        ++count;
    }
    currentItem=item;
    currentChoice=nullptr;
}

void TreeView::m_renameNode(QString old, QString str)
{
    for(int i = 0; i < nodes->count(); ++i) {
        if(nodes->item(i)->text() == old) {
            nodes->item(i)->setText(str); //rename the node itself
        }
        NodeItem* node = (NodeItem*)nodes->item(i);
        for(auto& choice : node->choices) {
            if(choice.nextNode == old) {
                choice.nextNode = str; //rename all choices leading to this node
            }
        }
    }
    for(int i = 0; i < nextNodeEdit->count(); ++i) {
        if(nextNodeEdit->itemText(i) == old) {
            nextNodeEdit->setItemText(i, str); //rename the combo box thingy for the node
        }
    }
}

void TreeView::onRenameNode()
{
    if(!currentItem) return;
    QDialog log(this);
    QLineEdit name(&log);
    QDialogButtonBox box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &log);
    QFormLayout lay(&log);
    lay.addRow(tr("New ID:"), &name);
    lay.addRow(&box);
    connect(&box, SIGNAL(accepted()), &log, SLOT(accept()));
    connect(&box, SIGNAL(rejected()), &log, SLOT(reject()));
    if(log.exec() != QDialog::Accepted) return;

    m_renameNode(nodeId->text(), name.text());
    nodeId->setText(name.text());
    m_autosave();
}

void TreeView::m_autosave()
{
    m_saveCurrentNode();
    m_saveCurrentChoice();
    m_saveXML(tr("autosave-") + xmlName());
}

void TreeView::m_saveCurrentNode()
{
    if(currentItem) {
        //m_renameNode(currentItem->text(), nodeId->text());
        currentItem->setText(nodeId->text()); //save id
        currentItem->dialogueText=nodeTextEdit->toPlainText(); //save text
        currentItem->speaker=speakerEdit->currentText(); //save speaker
        currentItem->choices.clear();
        if(currentChoice) { //save current choice stuff
            m_saveCurrentChoice();
            //choiceTextEdit->setText(tr(""));
        }
        for(int i = 0; i < choices->count(); ++i) { //save all choice data
            ChoiceItem* c = (ChoiceItem*)choices->item(i);
            currentItem->choices.push_back(c->dat);
        }
    }
}

void TreeView::onNodeSelect(QListWidgetItem* item)
{
    NodeItem* node = (NodeItem*)item;
    m_autosave();
    choiceTextEdit->setText(tr(""));
    currentItem = node;
    nodeId->setText(node->text()); //set ID
    nodeTextEdit->setText(node->dialogueText); //set text
    bool found = false;
    for(int i = 0; i < speakerEdit->count(); ++i) {
        if(speakerEdit->itemText(i) == node->speaker) {
            speakerEdit->setCurrentIndex(i);
            found = true;
            break;
        }
    }
    if(!found) {
        speakers.push_back(node->speaker); //set speaker
    }
    choices->clear();
    int count = 0;
    for(auto dat : node->choices) { //set up all choices
        std::string choiceName = "Choice " + std::to_string(count);
        if (dat.effect == "exit") choiceName += " [EXIT]";
        if(dat.nextNode == "none") choiceName += " [NONE]";
        m_addChoice(dat, choiceName);
        ++count;
    }
    requiredFlags->clear();
    requiredTrees->clear();
    setsFlags->clear();

    nextNodeEdit->setEnabled(false);
    effectEdit->setEnabled(false);

    currentChoice=nullptr;
}

void TreeView::m_saveCurrentChoice()
{
    if(currentChoice) {
        currentChoice->dat.choiceText = choiceTextEdit->toPlainText(); //save text
        currentChoice->dat.effect = effectEdit->currentText(); //save effect
        currentChoice->dat.nextNode = nextNodeEdit->currentText(); //save next node
        currentChoice->dat.requiredFlags.clear();
        currentChoice->dat.requiredTrees.clear();
        currentChoice->dat.setFlags.clear();
        for(int i = 0; i < requiredFlags->count(); ++i) {
            currentChoice->dat.requiredFlags.push_back(requiredFlags->item(i)->text());
        }
        for(int i = 0; i < requiredTrees->count(); ++i) {
            currentChoice->dat.requiredTrees.push_back(requiredTrees->item(i)->text());
        }
        for(int i = 0; i < setsFlags->count(); ++i) {
            currentChoice->dat.setFlags.push_back(setsFlags->item(i)->text());
        }
    }
}

void TreeView::onChoiceSelect(QListWidgetItem* item)
{
    ChoiceItem* choice = (ChoiceItem*)item;
    m_autosave();
    currentChoice = choice;
    choiceTextEdit->setText(currentChoice->dat.choiceText); //set text
    bool found = false;
    for(int i = 0; i < effectEdit->count(); ++i) {
        if(choice->dat.effect == effectEdit->itemText(i)) { //set effect
            effectEdit->setCurrentIndex(i);
            found = true;
            break;
        }
    }
    if(!found) effectEdit->insertItem(0, choice->dat.effect);
    found=false;
    for(int i = 0; i < nextNodeEdit->count(); ++i) {
        if(nextNodeEdit->itemText(i) == choice->dat.nextNode) { //set next node
            nextNodeEdit->setCurrentIndex(i);
            break;
        }
    }
    if(choice->dat.nextNode == "none") {
        nextNodeEdit->setCurrentIndex(0);
    }
    requiredTrees->clear();
    requiredFlags->clear();
    setsFlags->clear();
    for(int i = 0; i <choice->dat.requiredFlags.size(); ++i) {
        requiredFlags->addItem(choice->dat.requiredFlags[i]);
    }
    for(int i = 0; i <choice->dat.requiredTrees.size(); ++i) {
        requiredTrees->addItem(choice->dat.requiredTrees[i]);
    }
    for(int i = 0; i <choice->dat.setFlags.size(); ++i) {
        setsFlags->addItem(choice->dat.setFlags[i]);
    }
    nextNodeEdit->setEnabled(true);
    effectEdit->setEnabled(true);
}

void TreeView::onEffectActivate(int index)
{
    if(!currentChoice) return;
    m_autosave();
    onChoiceSelect(currentChoice);
    int num = 0;
    for(int i = 0; i < choices->count(); ++i) {
        ChoiceItem* c = (ChoiceItem*)choices->item(i);
        if(c==currentChoice) num = i;
        break;
    }
    std::string name = "Choice " + std::to_string(num);
    if(currentChoice->dat.effect == "exit") name += " [EXIT]";
    if(currentChoice->dat.nextNode == "none") name += " [NONE]";
    currentChoice->setText(tr(name.c_str()));
}
void TreeView::onNodeEditActivate(int index)
{
    if(!currentChoice) return;
    m_autosave();
    onChoiceSelect(currentChoice);
    int num = 0;
    for(int i = 0; i < choices->count(); ++i) {
        ChoiceItem* c = (ChoiceItem*)choices->item(i);
        if(c==currentChoice) num = i;
        break;
    }
    std::string name = "Choice " + std::to_string(num);
    if(currentChoice->dat.effect == "exit") name += " [EXIT]";
    if(currentChoice->dat.nextNode == "none") name += " [NONE]";
    currentChoice->setText(tr(name.c_str()));
}

void TreeView::onAddNode()
{
    QDialog log(this);
    QLineEdit name(&log);
    QDialogButtonBox box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &log);
    QFormLayout lay(&log);
    lay.addRow(tr("Node ID:"), &name);
    lay.addRow(&box);
    connect(&box, SIGNAL(accepted()), &log, SLOT(accept()));
    connect(&box, SIGNAL(rejected()), &log, SLOT(reject()));
    if(log.exec() != QDialog::Accepted) return;

    NodeItem* newNode = new NodeItem(name.text(), nodes);
    nextNodeEdit->addItem(newNode->text()); //added to node editor just in case
    m_autosave();
}

ChoiceItem* TreeView::m_addChoice(ChoiceData dat, std::string name)
{
    int num = choices->count();
    std::string cName = name;
    if(cName == "") cName = "Choice " + std::to_string(choices->count()) + " [NONE]"; //fresh choice!
    auto item = new ChoiceItem(tr(cName.c_str()), choices);
    item->dat = dat;
    item->dat.id = num;
    return item;
    m_autosave();
}

void TreeView::onAddChoice()
{
    auto n = m_addChoice(ChoiceData());
    currentItem->choices.push_back(n->dat);
}

void TreeView::onRemoveChoice()
{
    if(!currentItem) return;

    if(currentChoice) {
        for(int i=0; i < choices->count(); ++i) {
            ChoiceItem* item = (ChoiceItem*)choices->item(i);
            if(item->dat.choiceText == currentChoice->dat.choiceText) {
                choices->takeItem(i);
                break;
            }
        }
        for(int i=0; i < choices->count(); ++i) {
            ChoiceItem* item = (ChoiceItem*)choices->item(i);
            item->dat.id=i;
        }
        currentItem->choices.clear();
        for(int i =0; i < choices->count(); ++i) {
            ChoiceItem* item = (ChoiceItem*)choices->item(i);
            currentItem->choices.push_back(item->dat);
        }
        m_autosave();
        onNodeSelect(currentItem);
    }
}

void TreeView::onAddEffect()
{
    QDialog log(this);
    QLineEdit name(&log);
    QDialogButtonBox box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &log);
    QFormLayout lay(&log);
    lay.addRow(tr("Effect:"), &name);
    lay.addRow(&box);
    connect(&box, SIGNAL(accepted()), &log, SLOT(accept()));
    connect(&box, SIGNAL(rejected()), &log, SLOT(reject()));
    if(log.exec() != QDialog::Accepted) return;
    effectEdit->addItem(name.text());
}

void TreeView::onAddRequiredFlag()
{
    QDialog log(this);
    QLineEdit name(&log);
    QDialogButtonBox box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &log);
    QFormLayout lay(&log);
    lay.addRow(tr("New required flag:"), &name);
    lay.addRow(&box);
    connect(&box, SIGNAL(accepted()), &log, SLOT(accept()));
    connect(&box, SIGNAL(rejected()), &log, SLOT(reject()));
    if(log.exec() != QDialog::Accepted) return;
    requiredFlags->addItem(name.text());

    m_autosave();
}

void TreeView::onRemoveRequiredFlag()
{
    if(!currentChoice) return;
    for(QListWidgetItem* item : requiredFlags->selectedItems()) {
        delete requiredFlags->takeItem(requiredFlags->row(item));
    }
    m_autosave();
}

void TreeView::onAddRequiredTree()
{
    QDialog log(this);
    QLineEdit name(&log);
    QDialogButtonBox box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &log);
    QFormLayout lay(&log);
    lay.addRow(tr("New required tree ID:"), &name);
    lay.addRow(&box);
    connect(&box, SIGNAL(accepted()), &log, SLOT(accept()));
    connect(&box, SIGNAL(rejected()), &log, SLOT(reject()));
    if(log.exec() != QDialog::Accepted) return;
    requiredTrees->addItem(name.text());
}

void TreeView::onRemoveRequiredTree()
{
    if(!currentChoice) return;
    for(QListWidgetItem* item : requiredTrees->selectedItems()) {
        delete requiredTrees->takeItem(requiredTrees->row(item));
    }
    m_autosave();
}

void TreeView::onAddSetFlag()
{
    QDialog log(this);
    QLineEdit name(&log);
    QDialogButtonBox box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &log);
    QFormLayout lay(&log);
    lay.addRow(tr("New flag to set:"), &name);
    lay.addRow(&box);
    connect(&box, SIGNAL(accepted()), &log, SLOT(accept()));
    connect(&box, SIGNAL(rejected()), &log, SLOT(reject()));
    if(log.exec() != QDialog::Accepted) return;
    setsFlags->addItem(name.text());
}

void TreeView::onRemoveSetFlag()
{
    if(!currentChoice) return;
    for(QListWidgetItem* item : setsFlags->selectedItems()) {
        delete setsFlags->takeItem(setsFlags->row(item));
    }
    m_autosave();
}

QString TreeView::xmlName()
{
    return id + tr(".xml");
}

void TreeView::onSaveXML()
{
    m_autosave();
    QString filename=QFileDialog::getSaveFileName(this, "Save XML", QCoreApplication::applicationDirPath() + "/" + xmlName(), "XML File(*.xml)");
    m_saveXML(filename);
}

void TreeView::loadXML(QString fname)
{
    QXmlStreamReader xml;
    QFile file(fname);
    file.open(QIODevice::ReadOnly);

    QByteArray bytes = file.readAll();
    xml.addData(bytes);

    std::unordered_map<QString, bool> effects;

    QString previous="";
    NodeItem* curNode=nullptr;

    ChoiceData dat = ChoiceData();

    while(!xml.atEnd()) {
        auto token = xml.readNext();
        auto name = xml.name();
        auto attr = xml.attributes();

        if(token == QXmlStreamReader::StartElement) {
            if(name.toString() == "tree") {
                id = attr.value("id").toString();
                minSector = attr.value("minsector").toInt();
            }
            if(name.toString() == "speakers") {
                previous = "speakers";
            }
            if(name.toString() == "speaker" && previous == "speakers") {
                speakers.push_back(attr.value("name").toString());
            }
            if(name.toString() == "node") {
                if(previous == "choice") {
                    curNode->choices.push_back(dat);
                    dat = ChoiceData();
                }
                previous = "node";
                curNode = new NodeItem(attr.value("id").toString(), nodes);
            }
            if(name.toString() == "speaker" && previous == "node") {
                curNode->speaker = attr.value("name").toString();
            }
            if(name.toString() == "text" && previous == "node") {
                curNode->dialogueText = attr.value("text").toString();
            }
            if(name.toString() == "choice" && previous == "choice") {
                curNode->choices.push_back(dat);
                dat = ChoiceData();
                dat.choiceText = attr.value("text").toString();
                dat.nextNode = attr.value("next").toString();
                dat.effect = attr.value("effect").toString();
                effects[dat.effect]=true;
            }
            if(name.toString() == "choice" && previous == "node") {
                dat = ChoiceData();
                previous = "choice";
                dat.choiceText = attr.value("text").toString();
                dat.nextNode = attr.value("next").toString();
                dat.effect = attr.value("effect").toString();
                effects[dat.effect]=true;
            }
            if(name.toString() == "required_flag" && previous == "choice") {
                dat.requiredFlags.push_back(attr.value("id").toString());
            }
            if(name.toString() == "required_tree" && previous == "choice") {
                dat.requiredTrees.push_back(attr.value("id").toString());
            }
            if(name.toString() == "sets_flag" && previous == "choice") {
                dat.setFlags.push_back(attr.value("id").toString());
            }
        }
    }
    if(dat.choiceText != "") {
        curNode->choices.push_back(dat);
    }
    for(int i = 0; i < speakers.size(); ++i) {
        speakerEdit->addItem(speakers[i]);
    }
    nextNodeEdit->addItem("none");
    for(int i = 0; i < nodes->count(); ++i) {
        NodeItem* item = (NodeItem*)nodes->item(i);
        if(item->text() == "root") {
            m_displayNode(item);
        }
        nextNodeEdit->addItem(item->text());
    }
    for(auto [effect, is] : effects) {
        effectEdit->addItem(effect);
    }
    file.close();
}

void TreeView::m_saveXML(QString filename)
{
    if(!filename.isNull()) {
        QBuffer buf;
        buf.open(QBuffer::ReadWrite);
        QXmlStreamWriter w(&buf);
        w.setAutoFormatting(true);
        w.writeStartDocument();
        w.writeStartElement("tree");
        w.writeAttribute("id", id);
        w.writeAttribute("minsector", QString::number(minSector));

        w.writeStartElement("speakers");
        for(int i = 0; i < speakerEdit->count(); ++i) {
            w.writeStartElement("speaker");
            w.writeAttribute("name", speakerEdit->itemText(i));
            w.writeEndElement();
        }
        w.writeEndElement();
        for(int i = 0; i < nodes->count(); ++i) {
            NodeItem* node = (NodeItem*)nodes->item(i);
            if(!node) continue;

            w.writeStartElement("node");
            w.writeAttribute("id", node->text());
            w.writeStartElement("speaker");
            w.writeAttribute("name", node->speaker);
            w.writeEndElement();
            w.writeStartElement("text");
            w.writeAttribute("text", node->dialogueText);
            w.writeEndElement();
            for(auto choice : node->choices) {
                w.writeStartElement("choice");
                w.writeAttribute("text", choice.choiceText);
                w.writeAttribute("next", choice.nextNode);
                w.writeAttribute("effect", choice.effect);
                for(auto flag : choice.requiredFlags) {
                    w.writeStartElement("required_flag");
                    w.writeAttribute("id", flag);
                    w.writeEndElement();
                }
                for(auto tree : choice.requiredTrees) {
                    w.writeStartElement("required_tree");
                    w.writeAttribute("id", tree);
                    w.writeEndElement();
                }
                for(auto flag : choice.setFlags) {
                    w.writeStartElement("sets_flag");
                    w.writeAttribute("id", flag);
                    w.writeEndElement();
                }
                w.writeEndElement();
            }
            w.writeEndElement();
        }
        w.writeEndElement();
        w.writeEndDocument();
        buf.close();

        //std::cout << buf.size() << std::endl;

        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(buf.buffer());
        file.close();
    }
}

TreeView::~TreeView()
{
    delete layout;
    delete nodes;
    delete nodeId;
    delete speakerEdit;
    delete nodeTextEdit;
    delete nextNodeEdit;
    delete effectEdit;
    delete addNode;
    delete choices;
    delete choiceTextEdit;
    delete addChoice;
    delete addEffect;
    delete nextNodeLabel;
    delete saveXML;
    delete addRequiredFlag;
    delete removeRequiredFlag;
    delete addRequiredTree;
    delete removeRequiredTree;
    delete requiredFlags;
    delete requiredTrees;

    delete addSetFlag;
    delete removeSetFlag;
    delete setsFlags;
}
