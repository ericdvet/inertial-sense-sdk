/*
MIT LICENSE

Copyright (c) 2014-2023 Inertial Sense, Inc. - http://inertialsense.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "ISSerialPort.h"
#include "ISLogger.h"
#include "ISFileManager.h"

using namespace std;


cISSerialPort::cISSerialPort(serial_port_t* serial) : cISStream()
{
	if (serial != NULLPTR)
	{
		m_serial = *serial;
	}
	else
	{
		serialPortPlatformInit(&m_serial);
	}
	Close();
}

cISSerialPort::~cISSerialPort()
{
	Close();
}

bool cISSerialPort::Open(const std::string& portName, int baudRate, int timeout, bool blocking)
{
	m_timeout = timeout;
	m_blocking = blocking;
    return (serialPortOpen(&m_serial, portName.c_str(), baudRate, (int)m_blocking) != 0);
}

int cISSerialPort::Close()
{
	return serialPortClose(&m_serial);
}

int cISSerialPort::Read(void* data, int dataLength)
{
	return serialPortReadTimeout(&m_serial, (unsigned char*)data, dataLength, m_timeout);
}

int cISSerialPort::Write(const void* data, int dataLength)
{
	return serialPortWrite(&m_serial, (const unsigned char*)data, dataLength);
}

void cISSerialPort::GetComPorts(vector<string>& ports)
{
	ports.clear();

#if PLATFORM_IS_WINDOWS

	char comPort[64];
	char targetPath[256];

    for (int i = 0; i < 256; i++) // checking ports from COM0 to COM255
	{
		snprintf(comPort, sizeof(comPort), "COM%d", i);
		if (QueryDosDeviceA(comPort, targetPath, 256))
		{
			ports.push_back(comPort);
		}
	}

#else

    // FIXME: This is actually a non-ideal approach, as it doesn't account for custom udev rules, etc -- better to look in /sys/class/tty and find devices that are actually enumerated as serial devices
    // there are a couple a approaches that we could/consider, many are outlined here: https://stackoverflow.com/questions/2530096/how-to-find-all-serial-devices-ttys-ttyusb-on-linux-without-opening-them
    ISFileManager::GetAllFilesInDirectory("/dev", false, "^/dev/ttyUSB", ports);
    ISFileManager::GetAllFilesInDirectory("/dev", false, "^/dev/ttyACM", ports);
    // ISFileManager::GetAllFilesInDirectory("/dev", false, "^/dev/ttyS", ports);
    // ISFileManager::GetAllFilesInDirectory("/dev", false, "^/dev/ttyAMA", ports);
    // ISFileManager::GetAllFilesInDirectory("/dev", false, "^/dev/serial", ports);

#endif

}

std::string cISSerialPort::ConnectionInfo()
{
	return string(m_serial.port);
}
