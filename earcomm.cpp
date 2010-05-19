/*
 * Project:     EARComm - Emergency Alert Radio Tool
 * Name:        earcomm.cpp
 * Description: Main application class
 * Copyright:   (c) 2010 Doug Kelly
 *
 *  EARComm is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EARComm is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with EARComm.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    ui->FIPS1Reg->addItem("ALL", "0");
    ui->FIPS2Reg->addItem("ALL", "0");
    ui->FIPS3Reg->addItem("ALL", "0");
    ui->FIPS4Reg->addItem("ALL", "0");
    ui->FIPS1Reg->addItem("NW", "1");
    ui->FIPS2Reg->addItem("NW", "1");
    ui->FIPS3Reg->addItem("NW", "1");
    ui->FIPS4Reg->addItem("NW", "1");
    ui->FIPS1Reg->addItem("N", "2");
    ui->FIPS2Reg->addItem("N", "2");
    ui->FIPS3Reg->addItem("N", "2");
    ui->FIPS4Reg->addItem("N", "2");
    ui->FIPS1Reg->addItem("NE", "3");
    ui->FIPS2Reg->addItem("NE", "3");
    ui->FIPS3Reg->addItem("NE", "3");
    ui->FIPS4Reg->addItem("NE", "3");
    ui->FIPS1Reg->addItem("W", "4");
    ui->FIPS2Reg->addItem("W", "4");
    ui->FIPS3Reg->addItem("W", "4");
    ui->FIPS4Reg->addItem("W", "4");
    ui->FIPS1Reg->addItem("C", "5");
    ui->FIPS2Reg->addItem("C", "5");
    ui->FIPS3Reg->addItem("C", "5");
    ui->FIPS4Reg->addItem("C", "5");
    ui->FIPS1Reg->addItem("E", "6");
    ui->FIPS2Reg->addItem("E", "6");
    ui->FIPS3Reg->addItem("E", "6");
    ui->FIPS4Reg->addItem("E", "6");
    ui->FIPS1Reg->addItem("SW", "7");
    ui->FIPS2Reg->addItem("SW", "7");
    ui->FIPS3Reg->addItem("SW", "7");
    ui->FIPS4Reg->addItem("SW", "7");
    ui->FIPS1Reg->addItem("S", "8");
    ui->FIPS2Reg->addItem("S", "8");
    ui->FIPS3Reg->addItem("S", "8");
    ui->FIPS4Reg->addItem("S", "8");
    ui->FIPS1Reg->addItem("SE", "9");
    ui->FIPS2Reg->addItem("SE", "9");
    ui->FIPS3Reg->addItem("SE", "9");
    ui->FIPS4Reg->addItem("SE", "9");
    ui->FIPS1St->addItem("XX", "\x01\x01");
    ui->FIPS2St->addItem("XX", "\x01\x01");
    ui->FIPS3St->addItem("XX", "\x01\x01");
    ui->FIPS4St->addItem("XX", "\x01\x01");
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
    updateRegionList(*(ui->FIPS1Reg), text);
}

void EARComm::on_FIPS2St_currentIndexChanged(const QString &text)
{
    updateCountyList(*(ui->FIPS2), text);
    updateRegionList(*(ui->FIPS2Reg), text);
}

void EARComm::on_FIPS3St_currentIndexChanged(const QString &text)
{
    updateCountyList(*(ui->FIPS3), text);
    updateRegionList(*(ui->FIPS3Reg), text);
}

void EARComm::on_FIPS4St_currentIndexChanged(const QString &text)
{
    updateCountyList(*(ui->FIPS4), text);
    updateRegionList(*(ui->FIPS4Reg), text);
}

void EARComm::updateCountyList(QComboBox &countyBox, const QString &state)
{
    std::vector<sFIPS>::iterator fipsIter = d.fips.begin();
    countyBox.clear();
    if(state == "XX")
    {
        countyBox.addItem("No Location Set", "\x01\x01\x01");
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

void EARComm::updateRegionList(QComboBox &regionBox, const QString &state)
{
    if(state == "XX") {
        regionBox.setEnabled(false);
        regionBox.addItem("X", "\x01");
        regionBox.setCurrentIndex(regionBox.findText("X"));
    }
    else {
        regionBox.removeItem(regionBox.findText("X"));
        regionBox.setEnabled(true);
        regionBox.setCurrentIndex(regionBox.findText("ALL"));
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
    tmpEARtype = d.detectEAR(ui->device->text().toStdString());
    if(tmpEARtype == -1)
    {
        // error
        QMessageBox err(QMessageBox::Warning, "Error", "Could not detect EAR type.  Check that EAR is connected, the device is correct, and try again.", QMessageBox::Ok );
        std::cout << "Error reading EAR type -- EAR may not be connected" << std::endl;
        err.exec();
        return;
    }
    else
    {
        EARtype = tmpEARtype;
    }

    updateFrequencyList();

    std::string cfg = d.readData(ui->device->text().toStdString(), *(ui->programStatus));
    if(cfg.length() == 0)
    {
        // error
        QMessageBox err(QMessageBox::Warning, "Error", "Could not read EAR data.  Check that EAR is connected, the device is correct, and try again.", QMessageBox::Ok );
        std::cout << "Error reading config data -- EAR may not be connected" << std::endl;
        err.exec();
        return;
    }
    int idx;

    idx = ui->FIPS1Reg->findData(QString(cfg.substr(238,1).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS1Reg->setCurrentIndex(idx);
    idx = ui->FIPS2Reg->findData(QString(cfg.substr(244,1).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS2Reg->setCurrentIndex(idx);
    idx = ui->FIPS3Reg->findData(QString(cfg.substr(250,1).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS3Reg->setCurrentIndex(idx);
    idx = ui->FIPS4Reg->findData(QString(cfg.substr(256,1).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS4Reg->setCurrentIndex(idx);

    idx = ui->FIPS1St->findData(QString(cfg.substr(239,2).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS1St->setCurrentIndex(idx);
    updateRegionList(*(ui->FIPS1Reg), ui->FIPS1St->currentText());
    idx = ui->FIPS2St->findData(QString(cfg.substr(245,2).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS2St->setCurrentIndex(idx);
    updateRegionList(*(ui->FIPS2Reg), ui->FIPS2St->currentText());
    idx = ui->FIPS3St->findData(QString(cfg.substr(251,2).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    updateRegionList(*(ui->FIPS3Reg), ui->FIPS3St->currentText());
    ui->FIPS3St->setCurrentIndex(idx);
    idx = ui->FIPS4St->findData(QString(cfg.substr(257,2).c_str()));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS4St->setCurrentIndex(idx);
    updateRegionList(*(ui->FIPS4Reg), ui->FIPS4St->currentText());

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
        buf.append(0x01);
    }
    buf.append(ui->priFreq->itemData(ui->priFreq->currentIndex()).toString());
    buf.append(ui->altFreq->itemData(ui->altFreq->currentIndex()).toString());
    buf.append(ui->FIPS1Reg->itemData(ui->FIPS1Reg->currentIndex()).toString());
    buf.append(ui->FIPS1St->itemData(ui->FIPS1St->currentIndex()).toString());
    buf.append(ui->FIPS1->itemData(ui->FIPS1->currentIndex()).toString());
    buf.append(ui->FIPS2Reg->itemData(ui->FIPS2Reg->currentIndex()).toString());
    buf.append(ui->FIPS2St->itemData(ui->FIPS2St->currentIndex()).toString());
    buf.append(ui->FIPS2->itemData(ui->FIPS2->currentIndex()).toString());
    buf.append(ui->FIPS3Reg->itemData(ui->FIPS3Reg->currentIndex()).toString());
    buf.append(ui->FIPS3St->itemData(ui->FIPS3St->currentIndex()).toString());
    buf.append(ui->FIPS3->itemData(ui->FIPS3->currentIndex()).toString());
    buf.append(ui->FIPS4Reg->itemData(ui->FIPS4Reg->currentIndex()).toString());
    buf.append(ui->FIPS4St->itemData(ui->FIPS4St->currentIndex()).toString());
    buf.append(ui->FIPS4->itemData(ui->FIPS4->currentIndex()).toString());
    buf.append(ui->x10House->itemData(ui->x10House->currentIndex()).toInt());
    buf.append(ui->x10Unit->itemData(ui->x10Unit->currentIndex()).toInt());
    // Spare byte, not used.
    buf.append("\x01");
    // FM Synth Setup Bytes ??
    buf.append("\x01\x01\x01\x01\x01");
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
        QMessageBox err(QMessageBox::Critical, "Error", "Too many items selected.  A maximum of 80 events may be selected.", QMessageBox::Ok );
        std::cout << "Error: too many items selected.  Max 80" << std::endl;
        err.exec();
        return;
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
        buf.append(0x01);
    }
    std::cout << buf.toStdString() << std::endl;
    d.programData(ui->device->text().toStdString(), buf.toStdString(), *(ui->programStatus));
}

void EARComm::on_testButton_clicked()
{
    d.sendTest(ui->device->text().toStdString());
}

EARComm::~EARComm()
{
    delete ui;
}
