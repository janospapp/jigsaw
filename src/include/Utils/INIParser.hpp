#ifndef INIPARSER_HPP
#define INIPARSER_HPP

#include <fstream>
#include <iostream>
#include <string>


class INIParser
{
public:
    INIParser(const char *filename);
	~INIParser();

	template<class T>
    T findValue(const char *section, const char *key);

private:
    enum Level{file, sectionOpen, sectionInside};
	
	Level m_eState;
	std::ifstream m_file;
	char *m_pBuffer;

};

#endif //INIPARSER_HPP
