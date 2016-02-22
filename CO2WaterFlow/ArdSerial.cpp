// Copyright(c) 2016 Andrew L. Sandoval
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <stdafx.h>
#include "ArdSerial.h"
#include <array>
#include <vector>

bool ArduinoSerial::Open(const std::wstring &wstrPortName)
{
	if(true == m_bConnected)
	{
		return false;		// Already connected
	}
//	std::wstring wstrPortName(L"\\\\.");
//	wstrPortName += wstrPortNameIn;

	//Try to connect to the given port throuh CreateFile
	m_hSerial = CreateFile(wstrPortName.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	//Check if the connection was successfull
	if(INVALID_HANDLE_VALUE == m_hSerial)
	{
		//If not success full display an Error
		std::array<wchar_t, 1024> wzMessage;
		swprintf_s(&wzMessage[0], wzMessage.size(), L"Unable to open port %ls", wstrPortName.c_str());
		MessageBoxW(HWND_TOP, &wzMessage[0], L"ERROR Opening Port:", MB_TOPMOST);
		return false;
	}
	//If connected we try to set the comm parameters
	DCB dcbSerialParams = { 0 };

	//Try to get the current
	if(FALSE == GetCommState(m_hSerial, &dcbSerialParams))
	{
		//If impossible, show an error
		std::array<wchar_t, 1024> wzMessage;
		swprintf_s(&wzMessage[0], wzMessage.size(), L"Unable to get serial parameters for port %ls", wstrPortName.c_str());
		MessageBoxW(HWND_TOP, &wzMessage[0], L"ERROR:", MB_TOPMOST);
		CloseHandle(m_hSerial);
		m_hSerial = nullptr;
		return false;
	}
	//Define serial connection parameters for the arduino board
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	//Setting the DTR to Control_Enable ensures that the Arduino is properly
	//reset upon establishing a connection
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

	//Set the parameters and check for their proper application
	if(!SetCommState(m_hSerial, &dcbSerialParams))
	{
		std::array<wchar_t, 1024> wzMessage;
		swprintf_s(&wzMessage[0], wzMessage.size(), L"Unable to set serial parameters for port %ls", wstrPortName.c_str());
		MessageBoxW(HWND_TOP, &wzMessage[0], L"ERROR:", MB_TOPMOST);
		CloseHandle(m_hSerial);
		m_hSerial = nullptr;
		return false;
	}
	//If everything went fine we're connected
	m_bConnected = true;
	//Flush any remaining characters in the buffers 
	PurgeComm(this->m_hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	//We wait 2s as the arduino board will be reseting
	Sleep(ARDUINO_WAIT_TIME);
	return m_bConnected;
}

void ArduinoSerial::Close()
{
	//Check if we are connected before trying to disconnect
	if(false == this->m_bConnected)
	{
		return;
	}
	//We're no longer connected
	m_bConnected = false;
	//Close the serial handler
	CloseHandle(this->m_hSerial);
}

ArduinoSerial::~ArduinoSerial()
{
	Close();
}

void ArduinoSerial::ReadAvailable()
{
	// Use the ClearCommError function to get status info on the Serial port
	ClearCommError(m_hSerial, &m_dwError, &m_status);

	//Check if there is something to read
	if(m_status.cbInQue < 1)
	{
		return;
	}

	std::vector<char> vArray(m_status.cbInQue, 0);

	//Try to read the require number of chars, and return the number of read bytes on success
	DWORD dwBytesRead = 0;
	if(FALSE == ReadFile(m_hSerial, &vArray[0], static_cast<DWORD>(vArray.size()), &dwBytesRead, nullptr))
	{
		return;
	}

	for(auto c : vArray)
	{
		m_vBuffer.push_back(c);
	}

	//If nothing has been read, or that an error was detected return 0
	return;
}

bool ArduinoSerial::ReadNullZString(std::string &strRead)
{
	if(false == m_bConnected)
	{
		return false;
	}
	ReadAvailable();
	if(m_vBuffer.empty())
	{
		return false;
	}

	// Return false if no string is available yet...
	auto itLineEnd = std::find(m_vBuffer.begin(), m_vBuffer.end(), '\n');
	if(m_vBuffer.end() == itLineEnd)
	{
		return false;
	}

	if(itLineEnd + 1 != m_vBuffer.end())
	{
		std::vector<char> vbNew(itLineEnd + 1, m_vBuffer.end());
		m_vBuffer.swap(vbNew);
	}
	for(auto &c : m_vBuffer)
	{
		if(c == '\n')
		{
			break;
		}
		strRead += c;
	}
	return true;
}

bool ArduinoSerial::WriteByte(BYTE b)
{
	if(false == m_bConnected)
	{
		return false;
	}
	DWORD dwBytesSent = 0;
	//Try to write the buffer on the Serial port
	if(FALSE == WriteFile(m_hSerial, reinterpret_cast<void *>(&b), 1, &dwBytesSent, 0))
	{
		//In case it don't work get comm error and return false
		ClearCommError(this->m_hSerial, &this->m_dwError, &this->m_status);
		return false;
	}
	return true;
}

bool ArduinoSerial::IsConnected()
{
	//Simply return the connection status
	return m_bConnected;
}
