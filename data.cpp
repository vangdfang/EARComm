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

#include <libserialport.h>
#include "data.h"
#include <stdexcept>
#include <unistd.h>

using namespace std;

static const int TIMEOUT = 50;

Data::Data() : config(NULL), port(NULL)
{
}

Data::~Data()
{
    closePort();
}

std::vector<std::string> Data::listPorts()
{
    struct sp_port** portList;
    std::vector<std::string> result;
    if (SP_OK != sp_list_ports(&portList))
    {
        cout << "Unable to enumerate ports" << endl;
        return result;
    }
    int i = 0;
    sp_port* currentPort = portList[i];
    while (currentPort != NULL)
    {
        char* portName = sp_get_port_name(currentPort);
        result.push_back(std::string(portName));
        i++;
        currentPort = portList[i];
    }
    sp_free_port_list(portList);
    return result;
}

string Data::readData(std::string device, QProgressBar &bar)
{
    string buf;

    if (SP_OK != openPort(device)) {
        cout << "Unable to open port " << device << endl;
        return buf;
    }
    char c=0;
    bool readFailed=true;
    sp_return res;

    for (int i=0;i<10;i++)
    {
        if (SP_OK != sp_flush(port, SP_BUF_BOTH))
        {
            cout << "Warning: Could not flush buffers!" << endl;
        }
        if ((res = sp_blocking_write(port, "d", 1, TIMEOUT)) < SP_OK)
        {
            cout << "Unable to write to port: " << res << endl;
            return buf;
        }

        for(int j=0;j<512;j++)
        {
            if ((res = sp_blocking_read(port, &c, 1, TIMEOUT)) < SP_OK)
            {
                cout << "Unable to read data from port: " << res << endl;
            }
            if (c == 0 && readFailed == true)
            {
                j=512;
                usleep(500);
            }
            else
            {
                readFailed=false;
                buf.append(1, c);
                bar.setValue((j+1)*100/512);
                QApplication::processEvents();
            }
        }
        if (!readFailed)
        {
            i=10;
        }
    }
    cout << buf << endl;
    if (SP_OK != sp_flush(port, SP_BUF_BOTH))
    {
        cout << "Warning: Could not flush buffers!" << endl;
    }

    return buf;
}

int Data::detectEAR(std::string device)
{
    string buf;

    if (SP_OK != openPort(device)) {
        cout << "Unable to open port " << device << endl;
        return -1;
    }
    char c=0;
    bool readFailed=true;
    sp_return res;

    for(int i=0;i<10;i++)
    {
        if (SP_OK != sp_flush(port, SP_BUF_BOTH))
        {
            cout << "Warning: Could not flush buffers!" << endl;
        }
        if ((res = sp_blocking_write(port, "?", 1, TIMEOUT)) < SP_OK)
        {
            cout << "Unable to write to port: " << res << endl;
            return -1;
        }

        for(int j=0;j<2;j++)
        {
            if ((res = sp_blocking_read(port, &c, 1, TIMEOUT)) < SP_OK)
            {
                cout << "Unable to read data from port: " << res << endl;
            }
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

    // Wait for the additional CR/LF to come in.
    usleep(500);
    char discard[2];
    sp_blocking_read(port, discard, 2, TIMEOUT);
    if (SP_OK != sp_flush(port, SP_BUF_BOTH))
    {
        cout << "Warning: Could not flush buffers!" << endl;
    }

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
    if (SP_OK != openPort(device)) {
        cout << "Unable to open port " << device << endl;
        return;
    }
    sp_return res;
    if (SP_OK != sp_flush(port, SP_BUF_BOTH))
    {
        cout << "Warning: Could not flush buffers!" << endl;
    }
    if ((res = sp_blocking_write(port, "t", 1, TIMEOUT)) < SP_OK)
    {
        cout << "Unable to write to port: " << res << endl;
        return;
    }
    if (SP_OK != sp_flush(port, SP_BUF_BOTH))
    {
        cout << "Warning: Could not flush buffers!" << endl;
    }
}

void Data::programData(std::string device, std::string data, QProgressBar &bar)
{
    if (SP_OK != openPort(device)) {
        cout << "Unable to open port " << device << endl;
        return;
    }
    char c=0;
    bool readFailed=true;
    sp_return res;

    for(int i=0;i<10;i++)
    {
        if (SP_OK != sp_flush(port, SP_BUF_BOTH))
        {
            cout << "Warning: Could not flush buffers!" << endl;
        }
        if ((res = sp_blocking_write(port, "p", 1, TIMEOUT)) < SP_OK)
        {
            cout << "Unable to write to port: " << res << endl;
            return;
        }
        if ((res = sp_blocking_read(port, &c, 1, TIMEOUT)) < SP_OK)
        {
            cout << "Unable to read data from port: " << res << endl;
        }
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
        if (SP_OK != sp_flush(port, SP_BUF_BOTH))
        {
            cout << "Warning: Could not flush buffers!" << endl;
        }
        return;
    }
    for(int i=0;i<512;i++)
    {
        if ((res = sp_blocking_write(port, &(data[i]), 1, TIMEOUT)) < SP_OK)
        {
            cout << "Unable to write to port: " << res << endl;
            return;
        }
        bar.setValue((i+1)*100/512);
        QApplication::processEvents();
        usleep(10000);
    }

    readFailed=true;
    for(int i=0;i<10;i++)
    {
        if ((res = sp_blocking_read(port, &c, 1, TIMEOUT)) < SP_OK)
        {
            cout << "Unable to read data from port: " << res << endl;
        }
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
    if (SP_OK != sp_flush(port, SP_BUF_BOTH))
    {
        cout << "Warning: Could not flush buffers!" << endl;
    }
}

int Data::openPort(std::string device)
{
    sp_return res = SP_OK;
    if (!port)
    {
        res = sp_get_port_by_name(device.c_str(), &port);
        if (SP_OK != res)
        {
            cout << "Failed to find port " << device << endl;
            return res;
        }
        res = sp_open(port, (sp_mode)(SP_MODE_READ | SP_MODE_WRITE));
        if (SP_OK != res)
        {
            cout << "Failed to open port " << device << endl;
            return res;
        }
        res = sp_new_config(&config);
        if (SP_OK != res)
        {
            throw std::runtime_error("Unable to allocate serial port config");
        }
        res = sp_get_config(port, config);
        if (SP_OK != res)
        {
            cout << "Failed to get port config" << endl;
            return res;
        }
        res = sp_set_config_baudrate(config, 2400);
        if (SP_OK != res)
        {
            throw std::runtime_error("Unable to set config baud rate");
        }
        res = sp_set_config_bits(config, 8);
        if (SP_OK != res)
        {
            throw std::runtime_error("Unable to set config bits");
        }
        res = sp_set_config_parity(config, SP_PARITY_NONE);
        if (SP_OK != res)
        {
            throw std::runtime_error("Unable to set config parity");
        }
        res = sp_set_config_stopbits(config, 1);
        if (SP_OK != res)
        {
            throw std::runtime_error("Unable to set config stop bits");
        }
        res = sp_set_config_flowcontrol(config, SP_FLOWCONTROL_NONE);
        if (SP_OK != res)
        {
            throw std::runtime_error("Unable to set config flow control");
        }
        res = sp_set_config(port, config);
        if (SP_OK != res)
        {
            cout << "Failed to set port config" << endl;
            return res;
        }
        sp_free_config(config);
        config = NULL;
    }
    return res;
}

int Data::closePort()
{
    sp_return res = SP_OK;
    if (port)
    {
        res = sp_flush(port, SP_BUF_BOTH);
        if (SP_OK != res)
        {
            cout << "Warning: Could not flush port" << endl;
        }
        res = sp_close(port);
        if (SP_OK != res)
        {
            cout << "Warning: Could not close port" << endl;
        }
        sp_free_port(port);
        port = NULL;
    }
    return res;
}
