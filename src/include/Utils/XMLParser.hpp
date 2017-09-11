#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include "Utils/Helper.hpp"

//Class for the xml-trees:
class XMLNode
{
public:
    XMLNode(XMLNode *parent);
	~XMLNode();
	
	std::string GetName() const;
	std::string GetValue() const;
    bool GetAttribute(const std::string& key, std::string& result) const;
    std::vector<XMLNode*> GetChildren() const;

    //Find and returns a specific child of the Node. I ignore the first count-1 node, which has 'tag' TagName.
    //This is useful, when a node has several children with the same tag name.
    XMLNode* GetChild(const std::string& tag, int count = 1) const;

	friend class XMLParser;
private:
    XMLNode* m_pParent;
	std::string m_sTagName;
	std::string m_sValue;
	std::map<std::string,std::string> m_mapAttributes;
    std::vector<std::unique_ptr<XMLNode>> m_vecChildren;
};



class XMLParser
{
public:
    XMLParser(const char *filename);
	~XMLParser();
	
	bool Parse();	//Return value: true - success | false - fail
    XMLNode* GetRoot() const; //std::weak_ptr
	
private:
    //FUNCTIONS
    bool ParseFile();
	void ClearBuffer();
    bool IsNextCharOpen();  //Is it <
    bool IsNextCharClose(); //Is it >
	
	//VARIABLES
	//The root of the xml-tree.
    std::unique_ptr<XMLNode> m_pRoot;
    XMLNode *m_pCurrentNode;

    std::stack<std::string> m_TagStack;
	
	//The machine's states. For description and transition graph, see my textbook.
	//In the main switch i will check this state (and not the actual character). I also need a helpfunction to decide wether is a char a whitespace
	//or not.
    enum state{ nothing, openBracket, openTag, closeTag, waitAttrKey, AttrKey, AttrValue, value } m_eState;
	
	std::ifstream m_file;
	char *m_pBuffer;
	bool m_bValid;	//<-This indicate wether the file is open or not. The successful of the parsing is the Parse function's return value.
};

#endif //XMLPARSER_HPP
