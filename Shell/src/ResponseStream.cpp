//
// ResponseStream.cpp
//
// $Id: //poco/1.4/Shell/src/ResponseStream.cpp#1 $
//
// Library: Shell
// Package: Shell
// Module:  ResponseStream
//
// Copyright (c) 2009-2013, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Shell/ResponseStream.h"
#include "Poco/NumberFormatter.h"


using Poco::NumberFormatter;


namespace Poco {
namespace Shell {


//
// ResponseStreamBuf
//


ResponseStreamBuf::ResponseStreamBuf(std::ostream& ostr, int status):
	_ostr(ostr),
	_status(status),
	_lineComplete(false)
{
}


ResponseStreamBuf::~ResponseStreamBuf()
{
	try
	{
		sync();
	}
	catch (...)
	{
	}
}

	
int ResponseStreamBuf::sync()
{
	flushLine(false);
	_ostr.flush();
	return 0;
}

	
void ResponseStreamBuf::setStatus(int status)
{
	if (status != _status)
	{
		flushLine(false);
		_status = status;
	}
}


int ResponseStreamBuf::writeToDevice(char c)
{
	if (_lineComplete)
	{
		flushLine(true);
	}
	if (c == '\n')
	{
		_lineComplete = true;
	}
	else _line += c;
	return charToInt(c);
}


void ResponseStreamBuf::flushLine(bool more)
{
	_lineComplete = _lineComplete || !_line.empty();
	if (_lineComplete)
	{
		_ostr << NumberFormatter::format0(_status, 3) << (more ? '-' : ' ') << _line << "\r\n";
	}
	_line.clear();
	_lineComplete = false;
}


//
// ResponseIOS
//


ResponseIOS::ResponseIOS(std::ostream& ostr, int status):
	_buf(ostr, status)
{
	poco_ios_init(&_buf);
}

	
ResponseIOS::~ResponseIOS()
{
}

	
ResponseStreamBuf* ResponseIOS::rdbuf()
{
	return &_buf;
}


//
// ResponseStream
//


ResponseStream::ResponseStream(std::ostream& ostr, int status):
	ResponseIOS(ostr, status),
	std::ostream(&_buf)
{
}

	
ResponseStream::~ResponseStream()
{
}


} } // namespace Poco::Shell