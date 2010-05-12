#include "data.h"

using namespace std;
using LibSerial::SerialStream;
using LibSerial::SerialStreamBuf;

#if 0
int main(int argc, char* argv[])
{
	string buf;
	loadEvents();
	loadFIPS();

	for(int j=0;j<MAX_FIPS;j++)
	{
		if(fips[j].state.size() != 0)
		{
		//	cout << "State: " << fips[j].state << " County: " << fips[j].county << " FIPS: " << fips[j].stateCode << fips[j].countyCode << endl;
		}
	}

	for(int j=0;j<MAX_EVENTS;j++)
	{
		if(events[j].eventCode.size() != 0)
		{
		//	cout << "Event: " << events[j].eventCode << " Description: " << events[j].eventText << endl;
		}
	}

	SerialStream serial("/dev/ttyUSB0");
	serial.SetBaudRate(SerialStreamBuf::BAUD_2400);
	serial.SetNumOfStopBits(1);
	serial.SetParity(SerialStreamBuf::PARITY_NONE);
	serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
	serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
	serial << "dddd";
	//serial.read(buf, 512);
	char c;
	for(int i=0;i<512;i++)
	{
		serial.get(c);
		buf.append(1, c);
		cout << c;
	}
	cout << endl;
	serial.Close();

	parseData(buf);

	delete[] events;
	delete[] fips;

	return 0;
}
#endif

string Data::readData(QProgressBar &bar)
{
    string buf;

    SerialStream serial(string("/dev/ttyUSB0"), std::ios::in|std::ios::out);
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

int Data::detectEAR()
{
    string buf;

    SerialStream serial(string("/dev/ttyUSB0"), std::ios::in|std::ios::out);
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

#if 0
void Data::parseData(string cfg)
{
	cout << "Primary: " << (162.400 + ((cfg.at(236)-1)*.025)) << endl;
	cout << "Secondary: " << (162.400 + ((cfg.at(237)-1)*.025)) << endl;
	for(int i=0; i<MAX_FIPS; i++)
	{
		if(fips[i].stateCode == cfg.substr(239,2) && fips[i].countyCode == cfg.substr(241,3))
		{
			cout << "Location: " << fips[i].county << ", " << fips[i].state << endl;
		}
	} 
	for(int i=0; i<MAX_FIPS; i++)
	{
		if(fips[i].stateCode == cfg.substr(245,2) && fips[i].countyCode == cfg.substr(247,3))
		{
			cout << "Location: " << fips[i].county << ", " << fips[i].state << endl;
		}
	} 
	for(int i=0; i<MAX_FIPS; i++)
	{
		if(fips[i].stateCode == cfg.substr(251,2) && fips[i].countyCode == cfg.substr(253,3))
		{
			cout << "Location: " << fips[i].county << ", " << fips[i].state << endl;
		}
	} 
	for(int i=0; i<MAX_FIPS; i++)
	{
		if(fips[i].stateCode == cfg.substr(257,2) && fips[i].countyCode == cfg.substr(259,3))
		{
			cout << "Location: " << fips[i].county << ", " << fips[i].state << endl;
		}
	}
	for(int i=0; i<80; i++)
	{
		for(int j=0; j<MAX_EVENTS; j++)
		{
			if(events[j].eventCode == cfg.substr((271+3*i),3))
			{
				cout << "Event: " << events[j].eventText << endl;
			}
		}
	}
}
#endif
 
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

void Data::sendTest()
{
    SerialStream serial(string("/dev/ttyUSB0"), std::ios::in|std::ios::out);
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

void Data::programData(string data, QProgressBar &bar)
{
    SerialStream serial(string("/dev/ttyUSB0"), std::ios::in|std::ios::out);
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
