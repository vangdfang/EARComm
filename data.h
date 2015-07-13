/*
 * Project:     EARComm - Emergency Alert Radio Tool
 * Name:        data.h
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

#ifndef DATA_H
#define DATA_H
#include <iostream>
#include <fstream>
#include <string>
#include <string>
#include <vector>
#include <QProgressBar>
#include <QApplication>

enum {
    WX_EAR = 0,
    FM_EAR = 1,
    F2_EAR = 2
};

struct sp_port;
struct sp_port_config;

struct sEvent
{
	std::string eventCode;
	std::string eventText;
};

struct sFIPS
{
	std::string state;
	std::string county;
	std::string stateCode;
	std::string countyCode;
};

class Data
{
    public:
        Data();
        ~Data();
        std::vector<std::string> listPorts();
        std::string readData(std::string device, QProgressBar &bar);
        int detectEAR(std::string device);
        bool loadFIPS();
        bool loadEvents();
        void sendTest(std::string device);
        void programData(std::string device, std::string cfg, QProgressBar &bar);
        std::vector<sFIPS> fips;
        std::vector<sEvent> events;
    private:
        int openPort(std::string device);
        int closePort();
        sp_port_config* config;
        sp_port* port;
};

#endif
