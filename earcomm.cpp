#include "earcomm.h"
#include "ui_earcomm.h"

EARComm::EARComm(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::EARComm)
{
    QTableWidgetItem *tmpTableItem;
    int i=0;
    d.loadEvents();
    d.loadFIPS();
    EARtype = WX_EAR;

    ui->setupUi(this);

    updateFrequencyList();

    std::vector<sEvent>::iterator eventIter = d.events.begin();
    ui->eventCodes->setRowCount(d.events.size());
    while(eventIter != d.events.end())
    {
        tmpTableItem = new QTableWidgetItem((*eventIter).eventCode.c_str());
        ui->eventCodes->setItem(i, 0, tmpTableItem);
        tmpTableItem = new QTableWidgetItem((*eventIter).eventText.c_str());
        ui->eventCodes->setItem(i, 1, tmpTableItem);
        i++;
        ++eventIter;
    }

    std::vector<sFIPS>::iterator fipsIter = d.fips.begin();
    std::string tmpState;
    ui->FIPS1St->addItem("XX", "XX");
    ui->FIPS2St->addItem("XX", "XX");
    ui->FIPS3St->addItem("XX", "XX");
    ui->FIPS4St->addItem("XX", "XX");
    ui->FIPS1St->addItem("ALL", "000");
    ui->FIPS2St->addItem("ALL", "000");
    ui->FIPS3St->addItem("ALL", "000");
    ui->FIPS4St->addItem("ALL", "000");
    while(fipsIter != d.fips.end())
    {
        if((*fipsIter).state != tmpState)
        {
            ui->FIPS1St->addItem((*fipsIter).state.c_str(), (*fipsIter).stateCode.c_str());
            ui->FIPS2St->addItem((*fipsIter).state.c_str(), (*fipsIter).stateCode.c_str());
            ui->FIPS3St->addItem((*fipsIter).state.c_str(), (*fipsIter).stateCode.c_str());
            ui->FIPS4St->addItem((*fipsIter).state.c_str(), (*fipsIter).stateCode.c_str());
            tmpState = (*fipsIter).state;
        }
        ++fipsIter;
    }

    ui->x10House->addItem("OFF", 0x01);
    ui->x10Unit->addItem("OFF", 0x01);
    ui->x10House->addItem("A", 0x47);
    ui->x10Unit->addItem("1", 0x47);
    ui->x10House->addItem("B", 0x48);
    ui->x10Unit->addItem("2", 0x48);
    ui->x10House->addItem("C", 0x45);
    ui->x10Unit->addItem("3", 0x45);
    ui->x10House->addItem("D", 0x46);
    ui->x10Unit->addItem("4", 0x46);
    ui->x10House->addItem("E", 0x49);
    ui->x10Unit->addItem("5", 0x49);
    ui->x10House->addItem("F", 0x4A);
    ui->x10Unit->addItem("6", 0x4A);
    ui->x10House->addItem("G", 0x4B);
    ui->x10Unit->addItem("7", 0x4B);
    ui->x10House->addItem("H", 0x4C);
    ui->x10Unit->addItem("8", 0x4C);
    ui->x10House->addItem("I", 0x4F);
    ui->x10Unit->addItem("9", 0x4F);
    ui->x10House->addItem("J", 0x50);
    ui->x10Unit->addItem("10", 0x50);
    ui->x10House->addItem("K", 0x4D);
    ui->x10Unit->addItem("11", 0x4D);
    ui->x10House->addItem("L", 0x4E);
    ui->x10Unit->addItem("12", 0x4E);
    ui->x10House->addItem("M", 0x41);
    ui->x10Unit->addItem("13", 0x41);
    ui->x10House->addItem("N", 0x42);
    ui->x10Unit->addItem("14", 0x42);
    ui->x10House->addItem("O", 0x43);
    ui->x10Unit->addItem("15", 0x43);
    ui->x10House->addItem("P", 0x44);
    ui->x10Unit->addItem("16", 0x44);
}

void EARComm::on_FIPS1St_currentIndexChanged(const QString &text)
{
    updateCountyList(*(ui->FIPS1), text);
}

void EARComm::on_FIPS2St_currentIndexChanged(const QString &text)
{
    updateCountyList(*(ui->FIPS2), text);
}

void EARComm::on_FIPS3St_currentIndexChanged(const QString &text)
{
    updateCountyList(*(ui->FIPS3), text);
}

void EARComm::on_FIPS4St_currentIndexChanged(const QString &text)
{
    updateCountyList(*(ui->FIPS4), text);
}

void EARComm::updateCountyList(QComboBox &countyBox, const QString &state)
{
    std::vector<sFIPS>::iterator fipsIter = d.fips.begin();
    countyBox.clear();
    if(state == "XX")
    {
        countyBox.addItem("No Location Set", "XXX");
    }
    else if(state == "ALL")
    {
        countyBox.addItem("All Locations", "000");
    }
    else
    {
        countyBox.addItem("Entire State", "000");
    }
    while(fipsIter != d.fips.end())
    {
        if(state == (*fipsIter).state.c_str())
        {
            countyBox.addItem((*fipsIter).county.c_str(), (*fipsIter).countyCode.c_str());
        }
        ++fipsIter;
    }
}

void EARComm::updateFrequencyList()
{
    double base=162.4;
    double step=0.025;
    int maxIndex=7;
    char tmp[10];
    switch(EARtype)
    {
        case WX_EAR:
                base=162.4;
                step=0.025;
                maxIndex=7;
            break;
        case FM_EAR:
        case F2_EAR:
                base=88.1;
                step=0.2;
                maxIndex=100;
            break;
    }
    for(int i=1;i<=maxIndex;i++)
    {
        if(EARtype == WX_EAR)
        {
            snprintf(tmp, 10, "%.3f", (base+(i-1)*step));
        }
        else
        {
            snprintf(tmp, 10, "%.1f", (base+(i-1)*step));
        }
        ui->priFreq->addItem(tmp, QString((char)i));
        ui->altFreq->addItem(tmp, QString((char)i));
    }
}

void EARComm::on_readButton_clicked()
{
    int tmpEARtype;
    tmpEARtype = d.detectEAR();
    if(tmpEARtype == -1)
    {
        // error
        std::cout << "Error reading EAR type -- EAR may not be connected" << std::endl;
        return;
    }
    else
    {
        EARtype = tmpEARtype;
    }

    updateFrequencyList();

    std::string cfg = d.readData(*(ui->programStatus));
    if(cfg.length() == 0)
    {
        // error
        std::cout << "Error reading config data -- EAR may not be connected" << std::endl;
        return;
    }
    int idx;

    idx = ui->FIPS1St->findData(QString(cfg.substr(239,2).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS1St->setCurrentIndex(idx);
    idx = ui->FIPS2St->findData(QString(cfg.substr(245,2).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS2St->setCurrentIndex(idx);
    idx = ui->FIPS3St->findData(QString(cfg.substr(251,2).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS3St->setCurrentIndex(idx);
    idx = ui->FIPS4St->findData(QString(cfg.substr(257,2).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS4St->setCurrentIndex(idx);

    idx = ui->FIPS1->findData(QString(cfg.substr(241,3).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS1->setCurrentIndex(idx);
    idx = ui->FIPS2->findData(QString(cfg.substr(247,3).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS2->setCurrentIndex(idx);
    idx = ui->FIPS3->findData(QString(cfg.substr(253,3).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS3->setCurrentIndex(idx);
    idx = ui->FIPS4->findData(QString(cfg.substr(259,3).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS4->setCurrentIndex(idx);

    idx = ui->priFreq->findData(QString(cfg[236]));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->priFreq->setCurrentIndex(idx);

    idx = ui->altFreq->findData(QString(cfg[237]));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->altFreq->setCurrentIndex(idx);

    ui->eventCodes->clearSelection();

    for(int i=0; i<80; i++)
    {
        QList<QTableWidgetItem*> res = ui->eventCodes->findItems(QString(cfg.substr(271+3*i,3).c_str()), Qt::MatchExactly);
        if(!res.isEmpty())
        {
            ui->eventCodes->selectRow(res[0]->row());
        }
    }
    ui->eventCodes->setCurrentCell(0, 0, QItemSelectionModel::NoUpdate);

    idx = ui->x10House->findData(cfg[262]);
    idx = ( idx == -1 ) ? 0 : idx;
    ui->x10House->setCurrentIndex(idx);
    idx = ui->x10Unit->findData(cfg[263]);
    idx = ( idx == -1 ) ? 0 : idx;
    ui->x10Unit->setCurrentIndex(idx);

    if(cfg[269] == '1')
    {
        ui->eomSwitch->setCheckState(Qt::Checked);
    }
    else
    {
        ui->eomSwitch->setCheckState(Qt::Unchecked);
    }

    if(cfg[270] == '1')
    {
        ui->backupSwitch->setCheckState(Qt::Checked);
    }
    else
    {
        ui->backupSwitch->setCheckState(Qt::Unchecked);
    }
}

void EARComm::on_programButton_clicked()
{
    QString buf;
    for(int i=0;i<236;i++)
    {
        buf.append("<");
    }
    buf.append(ui->priFreq->itemData(ui->priFreq->currentIndex()).toString());
    buf.append(ui->altFreq->itemData(ui->altFreq->currentIndex()).toString());
    // Portion, not used.
    buf.append("0");
    buf.append(ui->FIPS1St->itemData(ui->FIPS1St->currentIndex()).toString());
    buf.append(ui->FIPS1->itemData(ui->FIPS1->currentIndex()).toString());
    buf.append("0");
    buf.append(ui->FIPS2St->itemData(ui->FIPS2St->currentIndex()).toString());
    buf.append(ui->FIPS2->itemData(ui->FIPS2->currentIndex()).toString());
    buf.append("0");
    buf.append(ui->FIPS3St->itemData(ui->FIPS3St->currentIndex()).toString());
    buf.append(ui->FIPS3->itemData(ui->FIPS3->currentIndex()).toString());
    buf.append("0");
    buf.append(ui->FIPS4St->itemData(ui->FIPS4St->currentIndex()).toString());
    buf.append(ui->FIPS4->itemData(ui->FIPS4->currentIndex()).toString());
    buf.append(ui->x10House->itemData(ui->x10House->currentIndex()).toInt());
    buf.append(ui->x10Unit->itemData(ui->x10Unit->currentIndex()).toInt());
    // Spare byte, not used.
    buf.append("<");
    // FM Synth Setup Bytes ??
    buf.append("<<<<");
    if(ui->eomSwitch->checkState() == Qt::Checked)
    {
        buf.append("1");
    }
    else
    {
        buf.append("0");
    }
    if(ui->backupSwitch->checkState() == Qt::Checked)
    {
        buf.append("1");
    }
    else
    {
        buf.append("0");
    }

    QList<QTableWidgetItem*> res = ui->eventCodes->selectedItems();
    if(res.length() > 80)
    {
        // maximum 80 items can be selected...
        std::cout << "Error: too many items selected.  Max 80" << std::endl;
    }
    for(int i=0; i<res.length(); i++)
    {
        if(res[i]->column() == 0)
        {
            buf.append(res[i]->text());
        }
    }
    while(buf.length() < 512)
    {
        buf.append(">");
    }
    std::cout << buf.toStdString() << std::endl;
    d.programData(buf.toStdString(), *(ui->programStatus));
}

void EARComm::on_testButton_clicked()
{
    d.sendTest();
}

EARComm::~EARComm()
{
    delete ui;
}
