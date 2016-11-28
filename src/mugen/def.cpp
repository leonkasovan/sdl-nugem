#include "def.hpp"

#include "mugenutils.hpp"

namespace Nugem {
namespace Mugen {

DefinitionFile::DefinitionFile()
{

}

DefinitionFile::DefinitionFile(DefinitionFile && definitionFile)
{
	std::swap(m_sections, definitionFile.m_sections);
}

DefinitionFile::DefinitionFile(const std::string & filepath)
{
	readFile(filepath);
}

DefinitionFile & DefinitionFile::operator=(DefinitionFile && definitionFile)
{
	std::swap(m_sections, definitionFile.m_sections);
	return *this;
}

DefinitionFile & DefinitionFile::operator=(const DefinitionFile & definitionFile)
{
	m_sections = definitionFile.sections();
	return *this;
}

DefinitionFile & DefinitionFile::readFile(const std::string & filepath)
{
	MugenTextFile def(filepath);
	MugenTextKeyValue kv;
	while ((kv = def.nextValue())) {
		std::string identifier = kv.name();
		std::transform(identifier.begin(), identifier.end(), identifier.begin(), ::tolower);
		std::string section = def.section();
		std::transform(section.begin(), section.end(), section.begin(), ::tolower);
		m_sections[section][identifier] = kv.value();
	}
	return *this;
}

const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> & DefinitionFile::sections() const
{
	return m_sections;
}

std::unordered_map< std::string, std::string > & DefinitionFile::operator[] (const std::string & section)
{
	return m_sections[section];
}

}
}
