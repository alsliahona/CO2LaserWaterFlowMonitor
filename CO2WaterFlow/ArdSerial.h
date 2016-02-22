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

#pragma once

#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#define ARDUINO_WAIT_TIME 2000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

class ArduinoSerial
{
private:
	//Serial comm handler
	HANDLE m_hSerial{ nullptr };
	//Connection status
	bool m_bConnected{ false };
	//Get various information about the connection
	COMSTAT m_status{ 0, 0, 0 };
	//Keep track of last error
	DWORD m_dwError{ 0 };
	std::vector<char> m_vBuffer;

	void ReadAvailable();

public:
	//Initialize Serial communication with the given COM port
	ArduinoSerial()
	{
	}

	//Close the connection
	~ArduinoSerial();

	// Open:
	bool Open(const std::wstring &wstrPortName);

	// Close:
	void Close();

	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	bool ReadNullZString(std::string &strRead);
	bool WriteByte(BYTE b);

	//Check if we are actually connected
	bool IsConnected();
};

#endif // SERIALCLASS_H_INCLUDED