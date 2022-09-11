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
    idEdit = new QLineEdit(this);
    speakerEdit = new QComboBox(this);
    nodeTextEdit = new QTextEdit(this);
    nextNodeEdit = new QComboBox(this);
    effectEdit = new QComboBox(this);
    addNode = new QPushButton(tr("Add Node"), this);
    choices = new QListWidget(this);
    choiceTextEdit = new QTextEdit(this);
    addChoice = new QPushButton(tr("Add Choice"), this);
    addEffect = new QPushButton(tr("Add Effect"), this);
    saveXML = new QPushButton(tr("Save to XML"), this);

    layout->setColumnMinimumWidth(1, 120);
    layout->setColumnMinimumWidth(2, 120);
    layout->setRowMinimumHeight(3, 150);
    //layout->setColumnMinimumWidth(3, 120);
    layout->setColumnMinimumWidth(4, 200);
    layout->addWidget(nodes, 0, 0, 4, 1);
    layout->addWidget(idEdit, 0, 1, 1, 1);
    layout->addWidget(speakerEdit, 0, 2, 1, 1);
    layout->addWidget(nodeTextEdit, 1, 1, 2, 2);

    layout->addWidget(addNode, 4, 0, 1, 1);
    layout->addWidget(saveXML, 4, 1, 1, 1);

    layout->addWidget(addChoice, 0, 3, 1, 1);
    layout->addWidget(choices, 1, 3, 2, 1);
    layout->addWidget(choiceTextEdit, 2, 4, 1, 1);
    layout->setColumnMinimumWidth(4, 120);
    layout->addWidget(nextNodeEdit, 3, 3, 1, 1);
    layout->addWidget(effectEdit, 3, 4, 1, 1);
    layout->addWidget(addEffect, 4, 4, 1, 1);

    connect(addNode, SIGNAL(clicked()), this, SLOT(onAddNode()));
    connect(addChoice, SIGNAL(clicked()), this, SLOT(onAddChoice()));
    connect(addEffect, SIGNAL(clicked()), this, SLOT(onAddEffect()));
    connect(saveXML, SIGNAL(clicked()), this, SLOT(onSaveXML()));
    connect(nodes, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onNodeSelect(QListWidgetItem*)));
    connect(choices, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onChoiceSelect(QListWidgetItem*)));
    this->setLayout(layout);
    this->resize(600, 400);
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
    idEdit->setText(item->text());
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
        auto choiceItem = new ChoiceItem(tr("Choice ") + QString::number(count), choices);
        choiceItem->dat = choice;
        ++count;
    }
}

void TreeView::m_renameNode(QString old, QString str)
{
    for(int i = 0; i < nodes->count(); ++i) {
        if(nodes->item(i)->text() == old) {
            nodes->item(i)->setText(str); //rename the node itself
        }
        NodeItem* node = (NodeItem*)nodes->item(i);
        for(auto choice : node->choices) {
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
void TreeView::m_saveCurrentNode()
{
    if(currentItem) {
        m_renameNode(currentItem->text(), idEdit->text());
        currentItem->setText(idEdit->text()); //save id
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
    m_saveCurrentNode();
    choiceTextEdit->setText(tr(""));
    currentItem = node;
    idEdit->setText(node->text()); //set ID
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
    for(auto dat : node->choices) { //set up all choices
        m_addChoice(dat);
    }
    currentChoice=nullptr;
}

void TreeView::m_saveCurrentChoice()
{
    if(currentChoice) {
        currentChoice->dat.choiceText = choiceTextEdit->toPlainText(); //save text
        currentChoice->dat.effect = effectEdit->currentText(); //save effect
        currentChoice->dat.nextNode = nextNodeEdit->currentText(); //save next node
    }
}

void TreeView::onChoiceSelect(QListWidgetItem* item)
{
    ChoiceItem* choice = (ChoiceItem*)item;
    if(currentChoice) {
        m_saveCurrentChoice();
    }
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

}

ChoiceItem* TreeView::m_addChoice(ChoiceData dat)
{
    int num = choices->count();
    auto item = new ChoiceItem(tr("Choice ") + QString::number(choices->count()), choices);
    item->dat = dat;
    item->dat.id = num;
    return item;
}

void TreeView::onAddChoice()
{
    auto n = m_addChoice(ChoiceData());
    currentItem->choices.push_back(n->dat);
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

void TreeView::onSaveXML()
{
    QString defaultName = id + tr(".xml");
    m_saveCurrentChoice();
    m_saveCurrentNode();
    QString filename=QFileDialog::getSaveFileName(this, "Save XML", QCoreApplication::applicationDirPath() + "/" + defaultName, "XML File(*.xml)");
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
                w.writeEndElement();
            }
            w.writeEndElement();
        }
        w.writeEndElement();
        w.writeEndDocument();
        buf.close();

        std::cout << buf.size() << std::endl;

        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(buf.buffer());
        file.close();
    }
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
                previous = "node";
                curNode = new NodeItem(attr.value("id").toString(), nodes);
            }
            if(name.toString() == "speaker" && previous == "node") {
                curNode->speaker = attr.value("name").toString();
            }
            if(name.toString() == "text" && previous == "node") {
                curNode->dialogueText = attr.value("text").toString();
            }
            if(name.toString() == "choice" && previous == "node") {
                ChoiceData dat;
                dat.choiceText = attr.value("text").toString();
                dat.nextNode = attr.value("next").toString();
                dat.effect = attr.value("effect").toString();
                effects[dat.effect]=true;
                curNode->choices.push_back(dat);
            }
        }
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

TreeView::~TreeView()
{
    delete layout;
    delete nodes;
    delete idEdit;
    delete speakerEdit;
    delete nodeTextEdit;
    delete nextNodeEdit;
    delete effectEdit;
    delete addNode;
    delete choices;
    delete choiceTextEdit;
    delete addChoice;
    delete addEffect;
    delete saveXML;
}
