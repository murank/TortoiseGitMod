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

#include "stdafx.h"
#include "CommandResultUTF8.h"

#include "CommandResultACP.h"

namespace {

bool ConverToACP(const char* src, size_t len, int encoding, std::vector<char>& dest)
{
	size_t required_size = MultiByteToWideChar(encoding, 0, src, len, NULL, 0);
	if(required_size == 0) {
		return false;
	}

	std::vector<wchar_t> wvec(required_size);
	if(!MultiByteToWideChar(encoding, 0, src, len, &(wvec[0]), wvec.size())) {
		return false;
	}

	required_size = WideCharToMultiByte(CP_ACP, 0, &(wvec[0]), wvec.size(), NULL, 0, NULL, NULL);
	if(required_size==0) {
		return false;
	}

	size_t old_size = dest.size();
	dest.resize(dest.size() + required_size);
	if(!WideCharToMultiByte(CP_ACP, 0, &(wvec[0]), wvec.size(), &(dest[old_size]), dest.size()-old_size, NULL, NULL)){
		return false;
	}

	return true;
}

void MoveCachedData(std::vector<char>& src, size_t len, char* dest)
{
	std::copy(src.begin(), src.begin()+len, dest);
	src.erase(src.begin(), src.begin()+len);
}

} // namespace

CommandResultUTF8::CommandResultUTF8(PROCESS_INFORMATION pi, const shared_handle& hRead)
	: ENCODING(CP_UTF8), m_resultACP(new CommandResultACP(pi, hRead)), m_rawCache(), m_convCache()
{
}

CommandResultUTF8::CommandResultUTF8(const shared_ptr<CommandResult>& result)
	: ENCODING(CP_UTF8), m_resultACP(result), m_rawCache(), m_convCache()
{
}

CommandResultUTF8::~CommandResultUTF8()
{
}

size_t CommandResultUTF8::AppendReadCache(std::vector<char>& buf, size_t size_cached)
{
	const size_t EXPANSION_THRESHOLD = 10;
	if(size_cached+EXPANSION_THRESHOLD > buf.size()) {
		buf.resize(2*buf.size());
	}

	size_t size_readable = buf.size()-size_cached;

	size_t size_read = m_resultACP->Read(&(buf[size_cached]), size_readable);
	size_cached += size_read;

	return size_cached;
}

size_t CommandResultUTF8::GetValidCharSequence(int encoding, std::vector<char>& data, size_t& size_cached)
{
	assert(!data.empty());

	size_t size_convert = size_cached;
	while(MultiByteToWideChar(encoding, MB_ERR_INVALID_CHARS, &(data[0]), size_convert, NULL, 0) == 0) {

		--size_convert;
		if(size_convert == 0) {

			size_t size_cached_new = AppendReadCache(data, size_cached);
			if(size_cached == size_cached_new) {
				// we cannot get new data any more
				return 0;
			}
			size_cached = size_cached_new;
			size_convert = size_cached_new;
		}
	}

	return size_convert;
}

size_t CommandResultUTF8::Read(char* buf, size_t len)
{
	assert(len>0 && "buffer size must be more than 0");

	if(len <= m_convCache.size()) {
		MoveCachedData(m_convCache, len, buf);
		return len;
	}

	std::vector<char> buffer(max(m_rawCache.size(), len));
	std::copy(m_rawCache.begin(), m_rawCache.end(), buffer.begin());

	size_t size_cached = AppendReadCache(buffer, m_rawCache.size());
	if(size_cached == 0) {
		return 0;
	}

	size_t size_convertable = GetValidCharSequence(ENCODING, buffer, size_cached);
	m_rawCache.assign(buffer.begin()+size_convertable, buffer.begin()+size_cached);

	std::vector<char> acp_data(m_convCache);
	if(!ConverToACP(&(buffer[0]), size_convertable, ENCODING, acp_data)) {
		return 0;
	}

	size_t size_write = min(len, acp_data.size());
	MoveCachedData(acp_data, size_write, buf);
	m_convCache = acp_data;

	return size_write;
}

bool CommandResultUTF8::Join()
{
	return m_resultACP->Join();
}

int CommandResultUTF8::GetExitCode() const
{
	return m_resultACP->GetExitCode();
}
