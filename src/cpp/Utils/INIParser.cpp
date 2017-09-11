#include "Utils/INIParser.hpp"
#include "Utils/Config.hpp"

INIParser::INIParser(const char *filename):
    m_eState(file)
{
	m_pBuffer = new char[512];
    m_file.open(filename, std::ifstream::in);
    if(!m_file.is_open())
    {
		std::cout << "Something went wrong with opening the file\n";
	}
}


INIParser::~INIParser()
{
	delete[] m_pBuffer;
    if(m_file.is_open())
    {
		m_file.close();
	}
}

//Assumptions: There is only one instance from every section in the file, and the keys are unique.
template<class T>
T INIParser::findValue(const char *section, const char *key)
{
	T ret;
    if(m_file.is_open())
    {
		m_file.clear();
        m_file.seekg(0, std::ios::beg);
		std::string currSection;	//if these were const char * and pointed to m_pBuffer, then the 
		std::string currKey;		//section will be lost every time, so i need to store in another variable
		char c;
		int i=0;
		
		m_file >> std::noskipws;

        while(!m_file.eof())
        {
			m_file >> c;
            switch(m_eState)
            {
			case file:
                if(c == '[')
                {
					m_eState = sectionOpen;
					i=0;
				}
				break;
			case sectionOpen:
                if(c == ']')
                {
					m_pBuffer[i] = '\0';
					currSection = m_pBuffer;
					m_eState = sectionInside;
					i=0;
                }
                else
                {
					m_pBuffer[i++] = c;
				}
				break;
			case sectionInside:
                if(c == '=')
                {
					m_pBuffer[i] = '\0';
					currKey = m_pBuffer;
                    if(section == currSection && key == currKey)
                    {
						m_file >> ret;
						return ret;		//I return so the algorithm will stop at this point.
					}
					i=0;
                }
                else if(c == '\n')
                {
					i=0;				//In new lines, we check the keys from beginning.
                }
                else if(c == '[')
                {
					m_eState = sectionOpen;
					i=0;
                }
                else
                {
					m_pBuffer[i++] = c;
				}
				break;
			}
		}
	}
	return ret; //if we didn't found it, or the file isn't opened. That would be a better solution
	//to pass the return variable's reference as parameter, end return a bool (success)
	//or an int (error code)
}

template int INIParser::findValue<int>(const char*, const char*);
template U16 INIParser::findValue<U16>(const char*, const char*);
template U32 INIParser::findValue<U32>(const char*, const char*);
template float INIParser::findValue<float>(const char*, const char*);
template std::string INIParser::findValue<std::string>(const char*, const char*);
