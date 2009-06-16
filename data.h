#ifndef DATA_H
#define DATA_H
#include <iostream>
#include <fstream>
#include <string>
#include <SerialStream.h>
#include <string>
#include <vector>
#include <QtGui/QProgressBar>
#include <QApplication>

#define WX_EAR 0
#define FM_EAR 1
#define F2_EAR 2

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
        std::string readData(QProgressBar &bar);
        int detectEAR();
        bool loadFIPS();
        bool loadEvents();
        void sendTest();
        void programData(std::string cfg, QProgressBar &bar);
        std::vector<sFIPS> fips;
        std::vector<sEvent> events;
};

#endif
