// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2012 - TortoiseGit

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

#ifndef SHARED_RESOURCE_H
#define SHARED_RESOURCE_H

#include "SharedPtr.h"

namespace internal {

template <typename T>
inline bool IsValidHandle(T handle)
{
	return ((handle != NULL) && (handle != INVALID_HANDLE_VALUE));
}

template <typename T, typename C>
class SafeCloser : public std::unary_function<T, void> {
public:
	SafeCloser(C c) : close(c) {}
	~SafeCloser() {}

	void operator()(T handle) {
		if(IsValidHandle(handle)) {
			close(handle);
		}
	}

private:
	C close;
};

} // namespace internal

template <typename T>
class shared_resource {
private:
	typedef void (shared_resource::*bool_type)() const;
	void this_type_does_not_support_comparisons() const {}

public:

	template <typename U, typename D>
	shared_resource(U handle, D d)
		: m_ptr(handle, ::internal::SafeCloser<U, D>(d))
	{}

	~shared_resource()
	{}

	const T get() const
	{
		return m_ptr.get();
	}

	T get()
	{
		return m_ptr.get();
	}

	operator bool_type() const {
		return ::internal::IsValidHandle(m_ptr.get()) ?
			&shared_resource::this_type_does_not_support_comparisons : 0;
	}

private:

	typedef shared_ptr<typename remove_pointer<T>::type> ptr_type;
	ptr_type m_ptr;
};

template <typename T1, typename T2>
bool operator==(const shared_resource<T1>& lhs, const shared_resource<T2>& rhs)
{
	return (lhs.get() == rhs.get());
}

template <typename T1, typename T2>
bool operator!=(const shared_resource<T1>& lhs, const shared_resource<T2>& rhs)
{
	return !(lhs == rhs);
}

#endif
