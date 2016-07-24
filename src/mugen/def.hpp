#ifndef DEF_H
#define DEF_H

#include <string>
#include <unordered_map>

namespace mugen {

class DefinitionFile {
private:
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_sections;
public:
	DefinitionFile();
	DefinitionFile(const std::string & filepath);
	DefinitionFile(DefinitionFile && definitionFile);
	DefinitionFile & operator=(DefinitionFile && definitionFile);
	DefinitionFile & operator=(const DefinitionFile & definitionFile);
	DefinitionFile & readFile(const std::string & filepath);
	std::unordered_map<std::string, std::string> & operator[](const std::string& section);
	const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> & sections() const;
};

}

#endif // DEF_H
