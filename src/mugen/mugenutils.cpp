#include "mugenutils.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include "sprites.hpp"

using namespace std;

namespace Nugem {
namespace Mugen {
	

istream & _getline(std::istream & __is, std::string & __str)
{
    std::istream & _is = getline(__is, __str);
	// Cut the line at the comments: find the ; character
	bool ignored = false;
	for (unsigned int index = 0; index < __str.size(); index++) {
		if (ignored) {
			if (__str[index] == '\"')
				ignored = false;
			continue;
		}
		// else
		switch (__str[index]) {
		case '"':
			ignored = true;
			break;
		case ';': // do not process comments
			__str = __str.substr(0, index);
			index = __str.size();
			break;
		}
	}
	return _is;
}

const regex MugenTextFile::regexSectionHeader("^[ \t]*\\[[ \t]*([^\\]]+?)[ \t]*\\][ \t\r]*$");
const regex MugenTextFile::regexKeyValue("^[ \t]*([^=]+?)[ \t]*=[ \t]*([^\r]+?)[ \t\r]*$");
const regex MugenTextFile::regexKeyQuotedValue("^[ \t]*([^=]+?)[ \t]*=[ \t]*\"([^\r\"]+?)\"[ \t\r]*$");

MugenTextFile::MugenTextFile(const std::string & path): m_path(path), m_inputstream(path)
{
	m_section = "";
}

MugenTextFile::~MugenTextFile()
{
	if (m_inputstream.is_open())
		m_inputstream.close();
}

const std::string & MugenTextFile::section() const
{
	return m_section;
}

const bool MugenTextFile::newSection() const
{
	return m_newSection;
}

const std::string MugenTextFile::nextLine()
{
	m_newSection = false;
	std::string s;
	_getline(m_inputstream, s);
	smatch sm;
	if (regex_match(s, sm, regexSectionHeader)) {
		m_section = sm[1];
		m_newSection = true;
	}
	return s;
}

MugenTextFile::operator bool() const
{
	return (bool) m_inputstream;
}

const MugenTextKeyValue MugenTextFile::nextValue()
{
	std::string line;
	m_newSection = false;
	while (_getline(m_inputstream, line)) {
		smatch sm;
		if (regex_match(line, sm, regexSectionHeader)) {
			m_section = sm[1];
			m_newSection = true;
		}
		else if (regex_match(line, sm, regexKeyQuotedValue)) {
			return MugenTextKeyValue(sm[1], sm[2]);
		}
		else if (regex_match(line, sm, regexKeyValue)) {
			return MugenTextKeyValue(sm[1], sm[2]);
		}
	}
	return MugenTextKeyValue();
}

MugenTextKeyValue::MugenTextKeyValue(): m_empty(true)
{
}

MugenTextKeyValue MugenTextKeyValue::read(const std::string & stringToRead)
{
	smatch sm;
	if (regex_match(stringToRead, sm, MugenTextFile::regexKeyQuotedValue)) {
		return MugenTextKeyValue(sm[1], sm[2]);
	}
	else if (regex_match(stringToRead, sm, MugenTextFile::regexKeyValue)) {
		return MugenTextKeyValue(sm[1], sm[2]);
	}
	return MugenTextKeyValue();
}

MugenTextKeyValue::MugenTextKeyValue(std::string key, std::string value): m_key(key), m_value(value)
{
}

MugenTextKeyValue::MugenTextKeyValue(const MugenTextKeyValue & kvpair): m_key(kvpair.name()), m_value(kvpair.value())
{
}

MugenTextKeyValue::MugenTextKeyValue(MugenTextKeyValue && kvpair)
{
	swap(m_empty, kvpair.m_empty);
	if (!m_empty) {
		swap(m_key, kvpair.m_key);
		swap(m_value, kvpair.m_value);
	}
}

MugenTextKeyValue & MugenTextKeyValue::operator=(MugenTextKeyValue && kvpair)
{
	swap(m_empty, kvpair.m_empty);
	if (!m_empty) {
		swap(m_key, kvpair.m_key);
		swap(m_value, kvpair.m_value);
	}
	return *this;
}

MugenTextKeyValue & MugenTextKeyValue::operator=(const MugenTextKeyValue & kvpair)
{
	m_empty = ! (bool) kvpair;
	if (!m_empty) {
		m_key = kvpair.name();
		m_value = kvpair.value();
	}
	return *this;
}

MugenTextKeyValue::operator bool() const
{
	return !m_empty;
}

const std::string & MugenTextKeyValue::name() const
{
	return m_key;
}

const std::string & MugenTextKeyValue::value() const
{
	return m_value;
}

}
}
