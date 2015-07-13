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
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <sstream>

EARComm::EARComm(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::EARComm)
{
    QTableWidgetItem *tmpTableItem;
    int i=0;
    d.loadEvents();
    d.loadFIPS();
    EARtype = WX_EAR;

    ui->setupUi(this);

    EARtype = d.findEAR();
    if(EARtype == -1)
    {
        // error
        QMessageBox err(QMessageBox::Warning, "Error", "Could not detect EAR type.  Check that EAR is connected, the device is correct, and try again.", QMessageBox::Ok );
        std::cout << "Error reading EAR type -- EAR may not be connected" << std::endl;
        err.exec();
    }

    switch (EARtype)
    {
        case WX_EAR:
            ui->device->setText("Weather EAR");
            break;
        case FM_EAR:
            ui->device->setText("FM EAR");
            break;
        case F2_EAR:
            ui->device->setText("FM EAR (rev 2)");
            break;
        default:
            ui->device->setText("No EAR detected!");
    }

    updateFrequencyList();

    ui->events->setPlainText("Listening for incoming events...\n");
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateEvents()));
    timer->start(500);

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
    populateRegionList(*(ui->FIPS1Reg));
    populateRegionList(*(ui->FIPS2Reg));
    populateRegionList(*(ui->FIPS3Reg));
    populateRegionList(*(ui->FIPS4Reg));
    populateStateList("XX", "\x01\x01");
    populateStateList("ALL", "000");
    while(fipsIter != d.fips.end())
    {
        if((*fipsIter).state != tmpState)
        {
            populateStateList((*fipsIter).state.c_str(), (*fipsIter).stateCode.c_str());
            tmpState = (*fipsIter).state;
        }
        ++fipsIter;
    }

    ui->x10House->addItem("OFF", "\x01");
    ui->x10Unit->addItem("OFF", "\x01");
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

void EARComm::populateRegionList(QComboBox &regionBox)
{
    regionBox.addItem("ALL", "0");
    regionBox.addItem("NW", "1");
    regionBox.addItem("N", "2");
    regionBox.addItem("NE", "3");
    regionBox.addItem("W", "4");
    regionBox.addItem("C", "5");
    regionBox.addItem("E", "6");
    regionBox.addItem("SW", "7");
    regionBox.addItem("S", "8");
    regionBox.addItem("SE", "9");
}

void EARComm::populateStateList(std::string state, std::string code)
{
    ui->FIPS1St->addItem(QString::fromStdString(state), QString::fromStdString(code));
    ui->FIPS2St->addItem(QString::fromStdString(state), QString::fromStdString(code));
    ui->FIPS3St->addItem(QString::fromStdString(state), QString::fromStdString(code));
    ui->FIPS4St->addItem(QString::fromStdString(state), QString::fromStdString(code));
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
    std::string cfg = d.readData(*(ui->programStatus));
    if(cfg.length() == 0)
    {
        // error
        QMessageBox err(QMessageBox::Warning, "Error", "Could not read EAR data.  Check that EAR is connected, the device is correct, and try again.", QMessageBox::Ok );
        std::cout << "Error reading config data -- EAR may not be connected" << std::endl;
        err.exec();
        return;
    }
    int idx;

    idx = ui->FIPS1Reg->findData(QString::fromStdString(cfg.substr(238,1)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS1Reg->setCurrentIndex(idx);
    idx = ui->FIPS2Reg->findData(QString::fromStdString(cfg.substr(244,1)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS2Reg->setCurrentIndex(idx);
    idx = ui->FIPS3Reg->findData(QString::fromStdString(cfg.substr(250,1)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS3Reg->setCurrentIndex(idx);
    idx = ui->FIPS4Reg->findData(QString::fromStdString(cfg.substr(256,1)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS4Reg->setCurrentIndex(idx);

    idx = ui->FIPS1St->findData(QString::fromStdString(cfg.substr(239,2)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS1St->setCurrentIndex(idx);
    updateRegionList(*(ui->FIPS1Reg), ui->FIPS1St->currentText());
    idx = ui->FIPS2St->findData(QString::fromStdString(cfg.substr(245,2)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS2St->setCurrentIndex(idx);
    updateRegionList(*(ui->FIPS2Reg), ui->FIPS2St->currentText());
    idx = ui->FIPS3St->findData(QString::fromStdString(cfg.substr(251,2)));
    idx = ( idx == -1 ) ? 0 : idx;
    updateRegionList(*(ui->FIPS3Reg), ui->FIPS3St->currentText());
    ui->FIPS3St->setCurrentIndex(idx);
    idx = ui->FIPS4St->findData(QString::fromStdString(cfg.substr(257,2)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS4St->setCurrentIndex(idx);
    updateRegionList(*(ui->FIPS4Reg), ui->FIPS4St->currentText());

    idx = ui->FIPS1->findData(QString::fromStdString(cfg.substr(241,3)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS1->setCurrentIndex(idx);
    idx = ui->FIPS2->findData(QString::fromStdString(cfg.substr(247,3)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS2->setCurrentIndex(idx);
    idx = ui->FIPS3->findData(QString::fromStdString(cfg.substr(253,3)));
    idx = ( idx == -1 ) ? 0 : idx;
    ui->FIPS3->setCurrentIndex(idx);
    idx = ui->FIPS4->findData(QString::fromStdString(cfg.substr(259,3)));
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
        QList<QTableWidgetItem*> res = ui->eventCodes->findItems(QString::fromStdString(cfg.substr(271+3*i,3)), Qt::MatchExactly);
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
        buf.append("\x01");
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
    // FM Synth Setup Bytes
    if(EARtype == WX_EAR) {
        buf.append("\x01\x01\x01\x01");
    }
    else {
        buf.append(QString::fromStdString(getSynthSetup(ui->priFreq->itemData(ui->priFreq->currentIndex()).toInt())));
        buf.append(QString::fromStdString(getSynthSetup(ui->priFreq->itemData(ui->altFreq->currentIndex()).toInt())));
    }
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
        buf.append("\x01");
    }
    std::cout << buf.toStdString() << std::endl;
    d.programData(buf.toStdString(), *(ui->programStatus));
}

// This function handles determining the Synth Setup bytes
// It is a port of Skip White's original VB function, with
// some minor alterations to make things a little more accurate.
std::string EARComm::getSynthSetup(int channel)
{
    char fmd[3];
    int i;
    int j=2;
    long intermediate;
    std::string ret;

    // frequency in kHz
    int freq = 87900 + (channel * 200);
    if(EARtype == FM_EAR) {
        fmd[2] = 35;
        intermediate = (2 * freq) + 21400;
    }
    else if(EARtype == F2_EAR) {
        fmd[2] = 160;
        // add IF
        freq += 10700;
        // divide by reference frequency
        freq /= 25;
        // left shift one bit, store in intermediate
        intermediate = freq * 2; 
    }
    else {
        // not a FM EAR--these are unused
        return std::string("\x01\x01");
    }

    fmd[0] = 1;
    fmd[1] = 0;

    // This is the magic--end result is the two bytes used by the EAR
    for(i=17; i>=0; i--) {
        if(intermediate & (2 ^ i)) {
            fmd[j] = fmd[j] + (2^(i%8));
        }
        if(i == 16) {
            j=1;
        }
        else if(i == 8) {
            j=0;
        }
    }

    ret.append(1, fmd[0]);
    ret.append(1, fmd[1]);
    return ret;
}

void EARComm::on_testButton_clicked()
{
    d.sendTest();
}

void EARComm::updateEvents()
{
    // Read event from serial port
    std::string buffer = d.readData();
    if (buffer.length() > 0)
    {
        size_t start, end;
        if(std::string::npos != (start = buffer.find("ZCZC", 0))) {
            std::string event;
            time_t now, waitUntil;
            time(&now);
            // Wait no more than 120 seconds for full message.
            waitUntil = now + 120;
            while(std::string::npos == (end = buffer.find("NNNN", 0)) && now <= waitUntil) {
                buffer.append(d.readData());
                time(&now);
            }
            // Good, full event (or as much as we may have received..
            event = buffer.substr(start, end - start);
            parseSAME(event);
        }
    }
}

void EARComm::parseSAME(const std::string &data)
{
    // SAME data is sent three times with no error
    // correction: we'll try parsing the message for
    // each instance of the data we get, and if we get
    // a valid message (as best as we can tell), we
    // will update the user.
    size_t start = 0;
    std::stringstream parsed;

    std::cout << "Parsing: " << data << std::endl;
    while (std::string::npos != (start = data.find("ZCZC", start)))
    {
        parsed.str("");
        size_t token = start + 5;
        start++;
        std::string buffer = data.substr(token, 3);
        token += 4;
        if (buffer == "PEP")
        {
            parsed << "The PRIMARY ENTRY POINT has issued a ";
        }
        else if (buffer == "CIV")
        {
            parsed << "CIVIL AUTHORITIES have issued a ";
        }
        if (buffer == "WXR")
        {
            parsed << "The NATIONAL WEATHER SERVICE has issued a ";
        }
        if (buffer == "EAS")
        {
            parsed << "A BROADCAST STATION has issued a ";
        }
        if (buffer == "EAN")
        {
            parsed << "The EMERGENCY ACTION NOTIFICATION NETWORK has issued a ";
        }
        buffer = data.substr(token, 3);
        token += 4;
        std::vector<sEvent>::iterator eventIter = d.events.begin();
        bool foundEvent = false;
        while (eventIter != d.events.end())
        {
            if ((*eventIter).eventCode == buffer)
            {
                parsed << (*eventIter).eventText;
                parsed << " for ";
                foundEvent = true;
            }
            ++eventIter;
        }

        size_t lastLocation = data.find("+", token);
        bool validLocations = true;
        while (token < lastLocation)
        {
            char region = data.at(token);
            std::string state = data.substr(token+1, 2);
            std::string county = data.substr(token+3, 3);
            token += 7;
            bool regionValid = true;
            switch (region)
            {
                case '0':
                    // Not sub-region
                    break;
                case '1':
                    parsed << "NORTHWEST ";
                    break;
                case '2':
                    parsed << "NORTH ";
                    break;
                case '3':
                    parsed << "NORTHEAST ";
                    break;
                case '4':
                    parsed << "WEST ";
                    break;
                case '5':
                    parsed << "CENTRAL ";
                    break;
                case '6':
                    parsed << "EAST ";
                    break;
                case '7':
                    parsed << "SOUTHWEST ";
                    break;
                case '8':
                    parsed << "SOUTH ";
                    break;
                case '9':
                    parsed << "SOUTHEAST ";
                    break;
                default:
                    regionValid = false;
            }
            if (state == "00" && county == "000")
            {
                parsed << "ALL REGIONS";
            }
            else
            {
                std::vector<sFIPS>::iterator fipsIter = d.fips.begin();
                while (fipsIter != d.fips.end())
                {
                    if ((*fipsIter).stateCode == state && (*fipsIter).countyCode == county)
                    {
                        parsed << (*fipsIter).county;
                        parsed << " ";
                        parsed << (*fipsIter).state;
                        break;
                    }
                    if ((*fipsIter).stateCode == state && county == "000")
                    {
                        parsed << "ALL COUNTIES IN ";
                        parsed << (*fipsIter).state;
                        break;
                    }
                    ++fipsIter;
                }
                if (fipsIter == d.fips.end() || !regionValid)
                {
                    validLocations = false;
                }
            }
            if (token < lastLocation)
            {
                parsed << ", ";
            }
        }
        if (foundEvent && validLocations && std::string::npos != lastLocation)
        {
            token = lastLocation + 1;
            std::string purge = data.substr(token, 4);
            token += 5;
            std::string issue = data.substr(token, 7);
            token += 8;
            size_t lastToken = data.find("-", token);
            std::string station = data.substr(token, lastToken - token);
            parsed << ". Message sent by ";
            parsed << station;
            parsed << " at ";
            struct tm issueTime, tmpTime;
            struct tm *currentTime;
            time_t now;
            char timeBuffer[255];
            memset(&issueTime, 0, sizeof(issueTime));
            memset(&tmpTime, 0, sizeof(tmpTime));
            time(&now);
            currentTime = localtime(&now);
            strftime(timeBuffer, 255, "%a, %d %b %Y %T %z", currentTime);
            std::string currentTimeString(timeBuffer);
            memcpy(&issueTime, currentTime, sizeof(issueTime));
            issueTime.tm_sec = 0;
            std::stringstream splitTime;
            splitTime << issue.substr(0,3);
            splitTime << "-";
            splitTime << issue.substr(3,2);
            splitTime << "-";
            splitTime << issue.substr(5,2);
            splitTime << "-";
            splitTime << currentTime->tm_year;
            if (NULL == strptime(splitTime.str().c_str(), "%j-%H-%M-%Y", &tmpTime))
            {
                std::cout << "failed to parse: " << splitTime.str() << std::endl;
            }
            issueTime.tm_mday = tmpTime.tm_mday;
            issueTime.tm_mon = tmpTime.tm_mon;
            issueTime.tm_wday = tmpTime.tm_wday;
            issueTime.tm_yday = tmpTime.tm_yday;
            issueTime.tm_hour = tmpTime.tm_hour;
            issueTime.tm_min = tmpTime.tm_min;

            strftime(timeBuffer, 255, "%a, %d %b %Y %T %z", &issueTime);
            parsed << timeBuffer;
            parsed << "; message no longer valid at ";
            splitTime.str("");
            splitTime << purge.substr(0, 2);
            splitTime << ":";
            splitTime << purge.substr(2, 2);
            memset(&tmpTime, 0, sizeof(tmpTime));
            if (NULL == strptime(splitTime.str().c_str(), "%H:%M", &tmpTime))
            {
                std::cout << "failed to parse: " << splitTime.str() << std::endl;
            }
            struct tm *expireTime;
            time_t expireAt = mktime(&issueTime) + tmpTime.tm_min * 60 + tmpTime.tm_sec;
            expireTime = localtime(&expireAt);
            strftime(timeBuffer, 255, "%a, %d %b %Y %T %z", expireTime);
            parsed << timeBuffer;
            parsed << ". Current time: ";
            parsed << currentTimeString;
            ui->events->setPlainText(ui->events->toPlainText() + "\n\n" + QString::fromStdString(parsed.str()));
            ui->events->moveCursor(QTextCursor::End);
            break;
        }

    }
}

EARComm::~EARComm()
{
    timer->stop();
    delete timer;
    delete ui;
}
