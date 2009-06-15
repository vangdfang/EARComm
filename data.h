#ifndef DATA_H
#define DATA_H
#include <iostream>
#include <fstream>
#include <string>
#include <SerialStream.h>
#include <string>
#include <vector>
#include <QtGui/QProgressBar>

#define MAX_LINE 100
#define MAX_EVENTS 100
#define MAX_FIPS 4000

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
        int main(int argc, char* argv[]);
        std::string readData(QProgressBar &bar);
        void parseData(std::string cfg);
        bool loadFIPS();
        bool loadEvents();
        std::vector<sFIPS> fips;
        std::vector<sEvent> events;
};

#endif
