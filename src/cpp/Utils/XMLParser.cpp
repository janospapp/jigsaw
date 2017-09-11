#include <iostream>
#include <stdlib.h>
#include "Utils/XMLParser.hpp"


//XMLNod's member functions:
XMLNode::XMLNode(XMLNode* parent):
    m_pParent(parent),
    m_sTagName(""),
    m_sValue("")
{
}


XMLNode::~XMLNode()
{
}


std::string XMLNode::GetName() const
{
	return m_sTagName;
}
	
	
std::string XMLNode::GetValue() const
{
	return m_sValue;
}


bool XMLNode::GetAttribute( const std::string& key, std::string& result ) const
{
	std::map<std::string,std::string>::const_iterator res=m_mapAttributes.find( key );
    if(res == m_mapAttributes.end())
    {
        return false;
    }
    else
    {
		//The operation was successful, the result variable contains the string, that paired to the key.
		result = res->second;
		return true;
	}
}


std::vector<XMLNode*> XMLNode::GetChildren() const
{
    std::vector<XMLNode*> ret;
    for(auto &ptr: m_vecChildren)
    {
        ret.push_back(ptr.get());
    }
    return ret;
}


XMLNode* XMLNode::GetChild(const std::string& tag, int count) const
{
    for(auto &child: m_vecChildren)
    {
        if( child->GetName().compare( tag ) == 0 )
        {
            if( count == 1 )
            {
                return child.get();
            }
            else
            {
				--count;
			}
		}
	}
	
	return NULL;
}


//Helper function. WS stands for whitespace.
bool IsCharWS(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}


XMLParser::XMLParser(const char *filename):
    m_eState(nothing),
    m_pRoot(new XMLNode(NULL))
{
	m_pBuffer = new char[256];
	m_file.open( filename );
    if(!(m_bValid = m_file.is_open() ))
    {
		std::cout << "ERROR: I couldn't open: " << filename << std::endl;
	}
    if(m_bValid)
    {
		m_file >> std::noskipws;
        m_pRoot->m_sTagName = std::string("Root");
	}
}


XMLParser::~XMLParser()
{
	delete[] m_pBuffer;
    if(m_bValid)
    {
		m_file.close();
	}
}


void XMLParser::ClearBuffer()
{
	delete[] m_pBuffer;
	m_pBuffer = new char[256];
}


bool XMLParser::IsNextCharOpen()
{
    if(m_bValid)
    {
        char c;
        m_file >> std::skipws;
        m_file >> c;
        m_file >> std::noskipws;

        if(c == '<' || m_file.eof())
        {   //I'll also return true if I reached the end of the file.
            return true;
        }
        else
        {
            return false;
        }
    }
}


bool XMLParser::IsNextCharClose()
{
    if(m_bValid)
    {
        char c;
        m_file >> std::skipws;
        m_file >> c;
        m_file >> std::noskipws;

        if(c == '>')
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

//This function does the hard work.
bool XMLParser::ParseFile()
{
    if(m_bValid){
        m_file.seekg(0);
		ClearBuffer();
		int i=0;	//This will be the buffer's index.
		
        std::string svalue(""), attrKey, attrValue;
		char c;
        m_pCurrentNode = m_pRoot.get();

        while(!m_file.eof())
        {
            switch(m_eState)
            {
            case nothing:
                if(IsNextCharOpen())
                {
                    m_eState = openBracket;
                }
                else
                {
                    return false;
                }
                break;
            case openBracket:
                if(c == '/')
                {		//It's a '</', the beginning of a close tag.
                    m_eState = closeTag;
                }
                else if(!IsCharWS(c))
                {					//Else it's the first character of the open tag name.
                    m_eState = openTag;

                    std::unique_ptr<XMLNode> newNode(new XMLNode(m_pCurrentNode));
                    m_pCurrentNode->m_vecChildren.push_back(std::move(newNode));
                    m_pCurrentNode = m_pCurrentNode->m_vecChildren.back().get();

                    m_pBuffer[i++]=c;
                }
                else
                {
                    return false;   //I don't allow whitespaces between open tag and open bracket (<)
                }
                break;
            case openTag:
                if(IsCharWS(c))
                {
                    m_eState = waitAttrKey;
                    m_pBuffer[i] = 0;
                    m_TagStack.push(std::string(m_pBuffer));
                    m_pCurrentNode->m_sTagName = std::string(m_pBuffer);
                    i=0;
                    ClearBuffer();
                }
                else if(c == '/')
                {
                    m_file >> c;
                    if(c == '>')
                    {   //It's a dangling tag that doesn't have any attribute and value.(e.g. <Static/> in Objects.xml)
                        m_eState = nothing;
                        m_pBuffer[i] = 0;
                        m_pCurrentNode->m_sTagName = std::string(m_pBuffer);
                        m_pCurrentNode = m_pCurrentNode->m_pParent;
                        i=0;
                        ClearBuffer();
                    }
                    else
                    {   //The xml file is corrupted.
                        return false;
                    }
                }
                else if(c == '>')
                {		//The open tag ended.
                    m_pBuffer[i] = 0;
                    //sopenTag = m_pBuffer;
                    m_pCurrentNode->m_sTagName = std::string(m_pBuffer);
                    m_TagStack.push(std::string(m_pBuffer));
                    i=0;
                    ClearBuffer();
                    m_file >> std::skipws;
                    m_file >> c;
                    m_file >> std::noskipws;
                    if(m_file.eof())
                    {
                        return false;	//I reached the EOF after an open tag, so the file is corrupted.
                    }
                    else if(c == '<')
                    {		//A new tag begins, it might be a childnode, so I load all the children.
                        m_eState = openBracket;
                    }
                    else
                    {
                        m_eState = value;
                        m_pBuffer[i++] = c;
                    }
                }
                else
                {
                    m_pBuffer[i++]=c;
                }
                break;
            case closeTag:
                if(c == '>')
                {
                    m_eState = nothing;
                    m_pBuffer[i] = 0;
                    if(m_TagStack.top().compare(std::string(m_pBuffer)) == 0)
                    {   //The close tag mathces to the last open tag so it's ok.
                        m_pCurrentNode->m_sValue = svalue;
                        m_pCurrentNode = m_pCurrentNode->m_pParent; //I step back in the XML tree.
                        m_TagStack.pop();
                        i=0;
                        ClearBuffer();
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    m_pBuffer[i++]=c;
                }
                break;
            case waitAttrKey:	//This state only appear in an open tag.
                if(c == '/')
                {	//It's an empty open tag (it hasn't any value).
                    if(IsNextCharClose())
                    {   //I've already set the node's tagname.
                        m_eState = nothing;

                        //I need to delete the last open tagname, because there isn't any closeTag for this.
                        m_TagStack.pop();
                        m_pCurrentNode = m_pCurrentNode->m_pParent;
                    }
                    else
                    {
                        return false;
                    }
                }
                else if(c == '>')
                {	//The open tag closed.
                    m_file >> std::skipws;
                    m_file >> c;
                    m_file >> std::noskipws;
                    if(m_file.eof())
                    {
                        return false;	//I reached the EOF after an open tag, so the file is corrupted.
                    }
                    else if(c == '<')
                    {		//A new tag begins, it might be a childnode, so I load all the children.
                        m_eState = openBracket;
                    }
                    else
                    {
                        m_eState = value;
                        m_pBuffer[i++] = c;
                    }
                }
                else if(!IsCharWS(c))
                {
                    m_eState = AttrKey;
                    m_pBuffer[i++] = c;
                }
                break;
            case AttrKey:
                if( c == '>' || c == '/' )
                {	//The file is corrupted, the key hasn't any value.
                    return false;
                }
                else if(c == '=')
                {
                    m_file >> c;
                    if(c == '\'' || c == '\"')
                    {
                        m_eState = AttrValue;
                    }
                    else
                    {	//The character isn't a quote, so the file is corrupted.
                        return false;
                    }
                    m_pBuffer[i] = 0;
                    attrKey = m_pBuffer;
                    i=0;
                    ClearBuffer();
                }
                else if(IsCharWS(c))
                {   //An attribute key needs to be followed by an '=' without any whitespaces!
                    return false;
                } else {
                    m_pBuffer[i++] = c;
                }
                break;
            case AttrValue:
                if(c == '\'' || c == '\"')
                {
                    m_eState = waitAttrKey;
                    m_pBuffer[i] = 0;
                    attrValue = m_pBuffer;
                    i=0;
                    ClearBuffer();
                    m_pCurrentNode->m_mapAttributes.insert(std::pair<std::string,std::string>(attrKey, attrValue));
                }
                else
                {	//I allow every character inside an attribute value (in the filepath I need the '/' character)
                    m_pBuffer[i++] = c;
                }
                break;
            case value:
                if(c == '<')
                {
                    m_eState = openBracket;
                    m_pBuffer[i] = 0;
                    svalue = m_pBuffer;
                    i=0;
                    ClearBuffer();
                }
                else
                {
                    m_pBuffer[i++] = c;
                }
                break;
            }
            m_file >> c;
        }
        //If I'm here, then I reached the EOF without return, so the file is OK:
        return true;
    }
}


bool XMLParser::Parse()
{
    return m_bValid ? ParseFile() : false;
}


XMLNode* XMLParser::GetRoot() const
{
    return m_pRoot.get();
}
