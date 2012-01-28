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

#ifndef IP_COMMAND_SENDER_H
#define IP_COMMAND_SENDER_H

#include "IPCommandSenderBase.h"

#include "SharedPtr.h"
#include "TypeTraits.h"

template<typename R>
class IPCommandSender0 : public IPCommandSenderBase {
public:
	typedef R result_type;

	result_type Call()
	{
		DoCall();
		return ReadResponse();
	}

protected:

	explicit IPCommandSender0(const shared_ptr<InterprocessIo>& io)
		: IPCommandSenderBase(io)
	{}

	virtual ~IPCommandSender0()
	{}

private:

	virtual result_type ReadResponse()
	{
		return Read<result_type>();
	}
};

template<>
class IPCommandSender0<void> : public IPCommandSenderBase {
public:
	typedef void result_type;

	result_type Call()
	{
		DoCall();
	}

protected:

	explicit IPCommandSender0(const shared_ptr<InterprocessIo>& io)
		: IPCommandSenderBase(io)
	{}

	virtual ~IPCommandSender0()
	{}

};

//////////////////////////////////////////////////////////////////////////////////////

template<typename R, typename T1>
class IPCommandSender1 : public IPCommandSenderBase {
public:
	typedef R result_type;
	typedef T1 arg1_type;
	typedef typename remove_cv<typename remove_reference<T1>::type>::type raw_arg1_type;

	result_type Call(arg1_type arg1)
	{
		SetArguments(arg1);

		DoCall();
		return ReadResponse();
	}

protected:

	explicit IPCommandSender1(const shared_ptr<InterprocessIo>& io)
		: IPCommandSenderBase(io), m_arg1()
	{}

	virtual ~IPCommandSender1()
	{}

private:

	void SetArguments(arg1_type arg1)
	{
		m_arg1 = arg1;
	}

	virtual void SendRequest()
	{
		WriteArguments(m_arg1);
	}

	virtual void WriteArguments(arg1_type arg1)
	{
		Write(arg1);
	}

	virtual result_type ReadResponse()
	{
		return Read<result_type>();
	}

	raw_arg1_type m_arg1;
};

template<typename T1>
class IPCommandSender1<void, T1> : public IPCommandSenderBase {
public:
	typedef void result_type;
	typedef T1 arg1_type;
	typedef typename remove_cv<typename remove_reference<T1>::type>::type raw_arg1_type;

	result_type Call(arg1_type arg1)
	{
		SetArguments(arg1);

		DoCall();
	}

protected:

	explicit IPCommandSender1(const shared_ptr<InterprocessIo>& io)
		: IPCommandSenderBase(io), m_arg1()
	{}

	virtual ~IPCommandSender1()
	{}

private:

	void SetArguments(arg1_type arg1)
	{
		m_arg1 = arg1;
	}

	virtual void SendRequest()
	{
		WriteArguments(m_arg1);
	}

	virtual void WriteArguments(arg1_type arg1)
	{
		Write(arg1);
	}

	raw_arg1_type m_arg1;
};

///////////////////////////////////////////////////////////////////////////

#define DEFINE_CTOR_DETOR(base) \
	explicit IPCommandSender(const shared_ptr<InterprocessIo>& io) : base(io) {} \
	virtual ~IPCommandSender() {}


template <typename Sig>
class IPCommandSender;

template <typename R>
class IPCommandSender<R (void)> : public IPCommandSender0<R>
{
public:
	DEFINE_CTOR_DETOR(IPCommandSender0);
};

template <typename R, typename T1>
class IPCommandSender<R (T1)> : public IPCommandSender1<R, T1>
{
public:
	DEFINE_CTOR_DETOR(IPCommandSender1);
};

#undef DEFINE_CTOR_DETOR


#endif
