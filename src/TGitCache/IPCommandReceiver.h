// TortoiseSVN - a Windows shell extension for easy version control

// External Cache Copyright (C) 2005 - 2006 - Will Dean, Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef IP_COMMAND_RECEIVER_H
#define IP_COMMAND_RECEIVER_H

#include "IPCommandReceiverBase.h"

#include "SharedPtr.h"
#include "TypeTraits.h"

template<typename R>
class IPCommandReceiver0 : public IPCommandReceiverBase {
public:
	typedef R result_type;

protected:

	explicit IPCommandReceiver0(const shared_ptr<InterprocessIo>& io)
		: IPCommandReceiverBase(io), m_result()
	{}

	virtual ~IPCommandReceiver0()
	{}

private:

	virtual result_type DoCommand() = 0;

	virtual void CallCommand()
	{
		m_result = DoCommand();
	}

	virtual void SendResponse()
	{
		WriteResult(m_result);
	}

	virtual void WriteResult(result_type result)
	{
		Write(result);
	}

	result_type m_result;
};

template<>
class IPCommandReceiver0<void> : public IPCommandReceiverBase {
public:
	typedef void result_type;

protected:

	explicit IPCommandReceiver0(const shared_ptr<InterprocessIo>& io)
		: IPCommandReceiverBase(io)
	{}

	virtual ~IPCommandReceiver0()
	{}

private:

	virtual result_type DoCommand() = 0;

	virtual void CallCommand()
	{
		DoCommand();
	}
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename R, typename T1>
class IPCommandReceiver1 : public IPCommandReceiverBase {
public:
	typedef R result_type;
	typedef T1 arg1_type;
	typedef typename remove_cv<typename remove_reference<T1>::type>::type raw_arg1_type;

protected:

	explicit IPCommandReceiver1(const shared_ptr<InterprocessIo>& io)
		: IPCommandReceiverBase(io), m_result(), m_arg1()
	{}

	virtual ~IPCommandReceiver1()
	{}

private:

	virtual result_type DoCommand(arg1_type arg1) = 0;

	virtual void ReceiveRequest()
	{
		ReadArguments(m_arg1);
	}

	virtual void ReadArguments(raw_arg1_type& arg1)
	{
		arg1 = Read<raw_arg1_type>();
	}

	virtual void CallCommand()
	{
		m_result = DoCommand(m_arg1);
	}

	virtual void SendResponse()
	{
		WriteResult(m_result);
	}

	virtual void WriteResult(result_type result)
	{
		Write(result);
	}

	result_type m_result;
	raw_arg1_type m_arg1;
};

template<typename T1>
class IPCommandReceiver1<void, T1> : public IPCommandReceiverBase {
public:
	typedef void result_type;
	typedef T1 arg1_type;
	typedef typename remove_cv<typename remove_reference<T1>::type>::type raw_arg1_type;

protected:

	explicit IPCommandReceiver1(const shared_ptr<InterprocessIo>& io)
		: IPCommandReceiverBase(io), m_arg1()
	{}

	virtual ~IPCommandReceiver1()
	{}

private:

	virtual void DoCommand(arg1_type arg1) = 0;

	virtual void ReceiveRequest()
	{
		ReadArguments(m_arg1);
	}

	virtual void ReadArguments(raw_arg1_type& arg1)
	{
		arg1 = Read<raw_arg1_type>();
	}

	virtual void CallCommand()
	{
		DoCommand(m_arg1);
	}

	raw_arg1_type m_arg1;
};


///////////////////////////////////////////////////////////////////////////

#define DEFINE_CTOR_DETOR(base) \
	explicit IPCommandReceiver(const shared_ptr<InterprocessIo>& io) : base(io) {} \
	virtual ~IPCommandReceiver() {}


template <typename Sig>
class IPCommandReceiver;

template <typename R>
class IPCommandReceiver<R (void)> : public IPCommandReceiver0<R>
{
public:
	DEFINE_CTOR_DETOR(IPCommandReceiver0);
};

template <typename R, typename T1>
class IPCommandReceiver<R (T1)> : public IPCommandReceiver1<R, T1>
{
public:
	DEFINE_CTOR_DETOR(IPCommandReceiver1);
};


#undef DEFINE_CTOR_DETOR

#endif
