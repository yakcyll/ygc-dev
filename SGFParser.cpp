/*
* SGFParser.cpp
* ygc SGF parser implementation
*/

#include "SGFParser.h"

using namespace ygc;
using std::shared_ptr;

template <typename Iterator>
void SGFFileParser<Iterator>::add_game() 
{ 
    spTreeNode newGameTree = spTreeNode(new SGFTreeNode()); 

    if (!currentGameTree || !currentGameTree->parent) { 
        newGameTree->parent = nullptr;
        games.push_back(newGameTree); 
    } 
    else { 
        newGameTree->parent = currentGameTree;
        currentGameTree->children.push_back(newGameTree);
    } 

    currentGameTree = newGameTree;
}

template <typename Iterator>
void SGFFileParser<Iterator>::close_game()
{
    if (currentGameTree->parent)
        currentGameTree = currentGameTree->parent;
    else
        currentGameTree = nullptr;

    currentNode = nullptr;
}

template <typename Iterator>
void SGFFileParser<Iterator>::add_node() 
{ 
    currentNode = spNode(new SGFNode()); 
    currentGameTree->sequence.push_back(currentNode); 
}

template <typename Iterator>
void SGFFileParser<Iterator>::add_property(std::string propident, std::vector<std::string> propvalues) 
{
    currentNode->properties[propident] = propvalues;
}

template <typename Iterator>
bool SGFFileParser<Iterator>::do_parse(std::string buffer, std::vector<SGFTreeNode*>& root)
{
    std::string::const_iterator iter = buffer.begin(), end = buffer.end();
    return qi::phrase_parse(iter, end, *this, ascii::space) && (iter == end);
}

template void SGFFileParser<std::string::const_iterator>::add_game();
template void SGFFileParser<std::string::const_iterator>::close_game();
template void SGFFileParser<std::string::const_iterator>::add_node();
template void SGFFileParser<std::string::const_iterator>::add_property(std::string, std::vector<std::string>);
template bool SGFFileParser<std::string::const_iterator>::do_parse(std::string, std::vector<SGFTreeNode*>&);


bool SGFParser::parseBuffer(std::string fb)
{
    fileBuffer = fb;

}
