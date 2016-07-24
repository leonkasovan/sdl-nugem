#ifndef MUGENUTILS_H
#define MUGENUTILS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <regex>

namespace mugen {

// std::getline adjusted for the ; comment character
std::istream& _getline(std::istream& __is, std::__cxx11::string& __str);

class MugenTextKeyValue {
public:
	static MugenTextKeyValue read(const std::string & stringToRead);
	MugenTextKeyValue();
	MugenTextKeyValue(std::string key, std::string value);
	MugenTextKeyValue(const MugenTextKeyValue & kvpair);
	MugenTextKeyValue(MugenTextKeyValue && kvpair);
	const std::string & name() const;
	const std::string & value() const;
	operator bool() const;
	MugenTextKeyValue & operator=(MugenTextKeyValue && kvpair);
	MugenTextKeyValue & operator=(const MugenTextKeyValue & kvpair);
private:
	std::string m_key;
	std::string m_value;
	bool m_empty = false;
};

class MugenTextFile {
public:
	MugenTextFile(const std::string & path);
	~MugenTextFile();
	const MugenTextKeyValue nextValue();
	const std::string nextLine();
	const std::string & section() const;
	const bool newSection() const;
	operator bool() const;
	static const std::regex regexSectionHeader;
	static const std::regex regexKeyValue;
	static const std::regex regexKeyQuotedValue;
private:
	bool m_newSection = false;
	std::string m_section;
	const std::string m_path;
	std::ifstream m_inputstream;
};

}

#endif // MUGENUTILS_H