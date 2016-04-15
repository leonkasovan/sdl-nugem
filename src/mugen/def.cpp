#include "def.h"

#include "mugenutils.h"

mugen::DefinitionFile::DefinitionFile()
{

}

mugen::DefinitionFile::DefinitionFile(mugen::DefinitionFile && definitionFile)
{
	std::swap(m_sections, definitionFile.m_sections);
}

mugen::DefinitionFile::DefinitionFile(const std::string & filepath)
{
	readFile(filepath);
}

mugen::DefinitionFile & mugen::DefinitionFile::operator=(mugen::DefinitionFile && definitionFile)
{
	std::swap(m_sections, definitionFile.m_sections);
	return *this;
}

mugen::DefinitionFile & mugen::DefinitionFile::operator=(const mugen::DefinitionFile & definitionFile)
{
	m_sections = definitionFile.sections();
	return *this;
}

mugen::DefinitionFile & mugen::DefinitionFile::readFile(const std::string & filepath)
{
	MugenTextFile def(filepath);
	MugenTextKeyValue kv;
	while (kv = def.nextValue()) {
		std::string identifier = kv.name();
		std::transform(identifier.begin(), identifier.end(), identifier.begin(), ::tolower);
		std::string section = def.section();
		std::transform(section.begin(), section.end(), section.begin(), ::tolower);
		m_sections[section][identifier] = kv.value();
	}
	return *this;
}

const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> & mugen::DefinitionFile::sections() const
{
	return m_sections;
}

std::unordered_map< std::string, std::string > & mugen::DefinitionFile::operator[] (const std::string & section)
{
	return m_sections[section];
}
