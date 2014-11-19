/*
 * SGFParser.cpp
 * ygc SGF parser implementation
 */

#include "pch.h"
#include <boost/algorithm/string/replace.hpp>
#include "SGFParser.h"

using namespace ygc;
using std::shared_ptr;

template <typename Iterator>
void SGFFileParser<Iterator>::add_game() 
{ 
    spTreeNode newGameTree = spTreeNode(new SGFTreeNode()); 

    if (!currentGameTree || !currentGameTree->parent) { 
        newGameTree->parent = nullptr;
        games->push_back(newGameTree); 
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
bool SGFFileParser<Iterator>::do_parse(std::string buffer)
{
    std::string::const_iterator iter = buffer.begin(), end = buffer.end();
    return qi::phrase_parse(iter, end, *this, ascii::space);
}

template void SGFFileParser<std::string::const_iterator>::add_game();
template void SGFFileParser<std::string::const_iterator>::close_game();
template void SGFFileParser<std::string::const_iterator>::add_node();
template void SGFFileParser<std::string::const_iterator>::add_property(std::string, std::vector<std::string>);
template bool SGFFileParser<std::string::const_iterator>::do_parse(std::string);



void SGFParser::InitParser()
{
    games = shared_ptr<std::vector<spTreeNode>>(new std::vector<spTreeNode>());
    fileParser = shared_ptr<SGFFileParser<std::string::const_iterator>>(new SGFFileParser<std::string::const_iterator>(games));
    
    // root properties
    nodeParsers["AP"] = [this](std::string propId, std::vector<std::string>& property) {
        if (this->gameTreeDescription.fileHeader.sourceApplication == "" && qi::parse(property[0].begin(), property[0].end(), boost::spirit::no_skip[+((qi::lit('\\') >> qi::print) | (qi::print | qi::space) - qi::lit(']') - qi::lit(':'))] >> ":" >> *boost::spirit::no_skip[((qi::lit('\\') >> qi::print) | (qi::print | qi::space) - qi::lit(']') - qi::lit(':'))]))
            this->gameTreeDescription.fileHeader.sourceApplication = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["CA"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.fileHeader.fileEncoding = property[0];
        return true;
    };

    nodeParsers["GM"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.fileHeader.gameMode = std::stoi(property[0]);
        return true;
    };

    nodeParsers["FF"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.fileHeader.fileFormat = std::stoi(property[0]);
        return true;
    };

    nodeParsers["ST"] = [this](std::string propId, std::vector<std::string>& property) {
        return true;
    };

    // game info properties
    nodeParsers["DT"] = [this](std::string propId, std::vector<std::string>& property) {
        if (this->gameTreeDescription.gameMetaData.date.year == 0 && qi::parse(property[0].begin(), property[0].end(), (qi::lit('1') | qi::lit('2') >> qi::repeat(3)[qi::digit] >> qi::lit('-') >> (qi::lit('0') >> qi::char_("1-9") | qi::lit('1') >> qi::char_("0-2")) >> qi::lit('-') >> (qi::lit('0') >> qi::char_("1-9") | qi::char_("1-2") >> qi::digit | qi::lit('3') >> qi::char_("0-1"))))) {
            this->gameTreeDescription.gameMetaData.date.year = std::stoi(std::string(property[0].begin(), property[0].begin()+4));
            this->gameTreeDescription.gameMetaData.date.month = std::stoi(std::string(property[0].begin()+5, property[0].begin()+7));
            this->gameTreeDescription.gameMetaData.date.day = std::stoi(std::string(property[0].begin()+8, property[0].end()));
        }
        return true;
    };

    nodeParsers["EV"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.gameMetaData.atevent = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["GN"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.gameMetaData.name = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["GC"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.gameMetaData.extrainfo = property[0];
        return true;
    };

    nodeParsers["PC"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.gameMetaData.location = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["RO"] = [this](std::string propId, std::vector<std::string>& property) {
        if (this->gameTreeDescription.gameMetaData.round == "" && qi::parse(property[0].begin(), property[0].end(), (qi::uint_ >> qi::lit('(') >> (qi::lit("final") | qi::lit("playoff") | qi::lit("league") | qi::lit("...")) >> qi::lit(')'))))
            this->gameTreeDescription.gameMetaData.round = property[0];
        return true;
    };

    nodeParsers["SO"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.gameMetaData.source = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["PB"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.matchInfo.black = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["BR"] = [this](std::string propId, std::vector<std::string>& property) {
        if (qi::parse(property[0].begin(), property[0].end(), (qi::uint_ >> (qi::lit("kyu") | qi::lit("k") | qi::lit("dan") | qi::lit("d")) >> -(qi::lit('?') | qi::lit('*')))))
            this->gameTreeDescription.matchInfo.blackRank = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["BT"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.matchInfo.blackTeam = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["PW"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.matchInfo.white = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["WR"] = [this](std::string propId, std::vector<std::string>& property) {
        if (qi::parse(property[0].begin(), property[0].end(), (qi::uint_ >> (qi::lit("kyu") | qi::lit("k") | qi::lit("dan") | qi::lit("d")) >> -(qi::lit('?') | qi::lit('*')))))
            this->gameTreeDescription.matchInfo.whiteRank = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["WT"] = [this](std::string propId, std::vector<std::string>& property) {
        this->gameTreeDescription.matchInfo.whiteTeam = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["SZ"] = [this](std::string propId, std::vector<std::string>& property) {
        if (qi::parse(property[0].begin(), property[0].end(), (qi::uint_ >> ":" >> qi::uint_))) {
            this->gameTreeDescription.matchInfo.size[0] = std::stoi(std::string(0, property[0].find(':')));
            this->gameTreeDescription.matchInfo.size[1] = std::stoi(std::string(property[0].find(':')+1, property[0].length())); 
        } else if (qi::parse(property[0].begin(), property[0].end(), qi::uint_, this->gameTreeDescription.matchInfo.size[1])) {
            this->gameTreeDescription.matchInfo.size[0] = this->gameTreeDescription.matchInfo.size[1];
        }
        return true;
    };

    nodeParsers["RU"] = [this](std::string propId, std::vector<std::string>& property) {
        if (qi::parse(property[0].begin(), property[0].end(), (qi::lit("AGA") | qi::lit("GOE") | qi::lit("Japanese") | qi::lit("NZ"))))
            this->gameTreeDescription.matchInfo.ruleset = property[0];
        return true;
    };

    nodeParsers["RE"] = [this](std::string propId, std::vector<std::string>& property) {
        if (qi::parse(property[0].begin(), property[0].end(), (qi::lit('0') | qi::lit("Draw") | (qi::lit("B+") | qi::lit("W+")) >> -(qi::uint_ | qi::lit("R") | qi::lit("Resign") | qi::lit("T") | qi::lit("Time") | qi::lit("F") | qi::lit("Forfeit")) | qi::lit("Void") | qi::lit("?"))))
            this->gameTreeDescription.matchInfo.result = ConvertToSimpleText(property[0]);
        return true;
    };

    nodeParsers["KM"] = [this](std::string propId, std::vector<std::string>& property) {
        if (qi::parse(property[0].begin(), property[0].end(), (~qi::lit('-') >> qi::float_)))
            this->gameTreeDescription.matchInfo.komi = std::stof(property[0]);
        return true;
    };

    nodeParsers["TM"] = [this](std::string propId, std::vector<std::string>& property) {
        qi::parse(property[0].begin(), property[0].end(), (qi::float_), this->gameTreeDescription.matchInfo.timePerPlayer);
        return true;
    };

    nodeParsers["HA"] = [this](std::string propId, std::vector<std::string>& property) {
        qi::parse(property[0].begin(), property[0].end(), (qi::uint_), this->gameTreeDescription.matchInfo.handicap);
        return true;
    };

    // move/setup properties
    nodeParsers["B"] = nodeParsers["W"] = nodeParsers["AB"] = nodeParsers["AW"] = [this](std::string propId, std::vector<std::string>& property) {
        for (auto c : property) {
            if (qi::parse(c.begin(), c.end(), qi::char_("a-zA-Z") >> qi::char_("a-zA-Z"))) {
                std::pair<uint16_t, uint16_t> coord;
                coord.first = c[0] >= 97 ? c[0] - 97 : c[0] - 65;
                coord.second = c[1] >= 97 ? c[1] - 97 : c[1] - 65;

                if (coord.first >= this->gameTreeDescription.matchInfo.size[0] || coord.second >= this->gameTreeDescription.matchInfo.size[1])
                    if (propId == "B" || propId == "W")
                        this->gameTreeDescription.moves.push_back(std::pair<uint16_t, std::pair<uint16_t, uint16_t>>(propId == "W", coord));
                    else
                        this->gameTreeDescription.premadeStones.push_back(std::pair<uint16_t, std::pair<uint16_t, uint16_t>>(propId == "AW", coord));
            }
        }
        return true;
    };

    nodeParsers["PL"] = [this](std::string propId, std::vector<std::string>& property) {
        if (qi::parse(property[0].begin(), property[0].end(), (qi::lit('B') | qi::lit('W'))))
            this->gameTreeDescription.matchInfo.startingPlayer = (property[0] == "W");
        return true;
    };

    // node annotation properties
    nodeParsers["C"] = [this](std::string propId, std::vector<std::string>& property) {
        return true;
    };

    nodeParsers["N"] = [this](std::string propId, std::vector<std::string>& property) {
        return true;
    };
}
 
bool SGFParser::ParseBuffer(std::string fb)
{
    if (fb != std::string())
        fileBuffer = fb;
    return fileParser->do_parse(fileBuffer);
}

bool SGFParser::ParseTree()
{
    //traverse the game trees

    /* for (auto i : *games) {
     *     TraverseTree(i);
     * }
     */

    TraverseTree(games->at(0));

    //validate data
    if (gameTreeDescription.fileHeader.fileFormat != 4 || gameTreeDescription.fileHeader.gameMode != 1)
        return false;

    return true;
}

bool SGFParser::TraverseTree(spTreeNode gameTree)
{
    for (auto n : gameTree->sequence)
        ParseNode(n);

    /* for (auto c : gameTree->children)
     *  TraverseTree(c);
     */

    if (gameTree->children.size() > 0)
        TraverseTree(gameTree->children[0]);

    return true;
}

bool SGFParser::ParseNode(spNode node)
{
    for (auto p : node->properties)
        nodeParsers[p.first](p.first, p.second);

    return true;
}

std::string SGFParser::ConvertToSimpleText(std::string text)
{
    boost::algorithm::replace_all(text, "\n", " ");
    boost::algorithm::replace_all(text, "\t", " ");
    boost::algorithm::replace_all(text, "\v", " ");
    boost::algorithm::replace_all(text, "\\ ", " ");

    return text;
}
