// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2008-2010 - TortoiseGit

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

#ifndef SHARED_BASE_H
#define SHARED_BASE_H

#include "SharedPtr.h"

namespace internal {

template <typename T, typename HandleType>
class shared_base {
private:
	typedef void (shared_base::*bool_type)() const;
	void this_type_does_not_support_comparisons() const {}

public:
	typedef typename HandleType handle_type;
	typedef typename remove_pointer<handle_type>::type ptr_type;

	handle_type get() const
	{
		return m_Ptr.get();
	}

	handle_type get()
	{
		return m_Ptr.get();
	}

	void reset(handle_type h = T::default_value())
	{
		m_Ptr.reset(h, T::close);
	}

	operator bool_type() const {
		return static_cast<const T*>(this)->valid(m_Ptr.get()) ?
			&shared_base::this_type_does_not_support_comparisons : 0;
	}

protected:

	explicit shared_base(handle_type h)
		: m_Ptr(h, T::close)
	{}

	~shared_base()
	{}

private:
	shared_ptr<ptr_type> m_Ptr;
};

} // namespace internal

#endif
