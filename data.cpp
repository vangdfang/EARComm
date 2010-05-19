/*
 * Project:     EARComm - Emergency Alert Radio Tool
 * Name:        data.cpp
 * Description: Serial communications and data parsing
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

#include "data.h"

using namespace std;
using LibSerial::SerialStream;
using LibSerial::SerialStreamBuf;

string Data::readData(std::string device, QProgressBar &bar)
{
    string buf;

    SerialStream serial(device, std::ios::in|std::ios::out);
    serial.SetBaudRate(SerialStreamBuf::BAUD_2400);
    serial.SetNumOfStopBits(1);
    serial.SetParity(SerialStreamBuf::PARITY_NONE);
    serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
    serial.SetVTime(10);
    serial.SetVMin(0);
    char c=0;
    bool readFailed=true;

    for(int i=0;i<10;i++)
    {
        serial << "d";

        for(int j=0;j<512;j++)
        {
            serial.get(c);
            if(c == 0 && readFailed == true)
            {
                j=512;
                usleep(50);
            }
            else
            {
                readFailed=false;
                buf.append(1, c);
                bar.setValue((j+1)*100/512);
                QApplication::processEvents();
            }
        }
        if(!readFailed)
        {
            i=10;
        }
    }
    cout << buf << endl;
    serial.Close();

    return buf;
}

int Data::detectEAR(std::string device)
{
    string buf;

    SerialStream serial(device, std::ios::in|std::ios::out);
    serial.SetBaudRate(SerialStreamBuf::BAUD_2400);
    serial.SetNumOfStopBits(1);
    serial.SetParity(SerialStreamBuf::PARITY_NONE);
    serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
    serial.SetVTime(50);
    serial.SetVMin(0);
    char c=0;
    bool readFailed=true;

    for(int i=0;i<10;i++)
    {
        serial << "?";

        for(int j=0;j<2;j++)
        {
            serial.get(c);
            if(c == 0 && readFailed==true)
            {
                j=2;
                usleep(50);
            }
            else
            {
                readFailed=false;
                buf.append(1, c);
            }
        }
        if(!readFailed)
        {
            i=10;
        }
    }
    cout << buf << endl;
    serial.Close();

    if(buf == "WX")
    {
        cout << "Got Weather EAR" << endl;
        return WX_EAR;
    }
    else if(buf == "FM")
    {
        cout << "Got FM EAR" << endl;
        return FM_EAR;
    }
    else if(buf == "F2")
    {
        cout << "Got FM EAR rev 2" << endl;
        return F2_EAR;
    }

    return -1;
}

bool Data::loadFIPS()
{
    ifstream data;
    string line;
    string::size_type pos, pos2;
    sFIPS tmp;

    data.open("fips.dat");
    if(!data)
    {
        cout << "Error opening FIPS file" << endl;
        return false;
    }
    while(getline(data, line))
    {
        pos = line.find(",", 0);
        tmp.state = line.substr(0, pos);
        pos2 = line.find(",", pos+1);
        tmp.stateCode = line.substr(pos+1, 2);
        pos = line.find(",", pos2+1);
        tmp.countyCode = line.substr(pos2+1, pos-pos2-1);
        tmp.county = line.substr(pos+1);
        fips.push_back(tmp);
    }

    return true;
}

bool Data::loadEvents()
{
    ifstream data;
    string line;
    string::size_type pos;
    sEvent tmp;

    data.open("events.dat");
    if(!data)
    {
        cout << "Error opening events file" << endl;
        return false;
    }
    while(getline(data, line))
    {
        pos = line.find(",", 0);
        tmp.eventCode = line.substr(0, pos);
        tmp.eventText = line.substr(pos+1);
        events.push_back(tmp);
    }

    return true;
}

void Data::sendTest(std::string device)
{
    SerialStream serial(device, std::ios::in|std::ios::out);
    serial.SetBaudRate(SerialStreamBuf::BAUD_2400);
    serial.SetNumOfStopBits(1);
    serial.SetParity(SerialStreamBuf::PARITY_NONE);
    serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
    serial.SetVTime(50);
    serial.SetVMin(0);
    serial << "t";
    serial.Close();
}

void Data::programData(std::string device, std::string data, QProgressBar &bar)
{
    SerialStream serial(device, std::ios::in|std::ios::out);
    serial.SetBaudRate(SerialStreamBuf::BAUD_2400);
    serial.SetNumOfStopBits(1);
    serial.SetParity(SerialStreamBuf::PARITY_NONE);
    serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
    serial.SetVTime(50);
    serial.SetVMin(0);
    char c=0;
    bool readFailed=true;

    for(int i=0;i<10;i++)
    {
        serial << "p";
        serial.get(c);
        if(c == 0 && readFailed==true)
        {
            usleep(50);
        }
        else if(c == '!')
        {
            readFailed=false;
            i=10;
        }
    }
    if(readFailed)
    {
        cout << "Failed writing to device" << endl;
        serial.Close();
        return;
    }
    for(int i=0;i<512;i++)
    {
        serial << data[i];
        bar.setValue((i+1)*100/512);
        QApplication::processEvents();
        usleep(10000);
    }

    readFailed=true;
    for(int i=0;i<10;i++)
    {
        serial.get(c);
        if(c == 0 && readFailed==true)
        {
            usleep(50);
        }
        else if(c == '*')
        {
            readFailed=false;
            i=10;
        }
    }
    if(readFailed) {
        cout << "Failed finishing programming: " << c << endl;
    }
    serial.Close();
}
