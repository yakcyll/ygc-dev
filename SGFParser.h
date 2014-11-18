/*
* SGFParser.h
* ygc SGF parser header
*/

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_auto.hpp>
#include <boost/spirit/include/qi_no_skip.hpp>
#include <boost/phoenix.hpp>

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <utility>
//#include "ygc.h"

#define BOOST_SPIRIT_USE_PHOENIX_V3

namespace qi	= boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

using std::shared_ptr;

namespace ygc {

	class FileHeader {
	public:
		std::string sourceApplication;
		std::string fileEncoding;
		uint8_t fileFormat;
		uint8_t gameMode;
	};

	class GameMetaData {
	public:
		std::string name;
		std::string extrainfo;
		std::string location;
		struct {
			uint16_t year;
			uint16_t month;
			uint16_t day;
		} date;

		std::string atevent;
		std::string round;

		std::string source;
	};

	class MatchInfo {
	public:
		std::string black;
		std::string blackRank;
        std::string blackTeam;
		std::string white;
		std::string whiteRank;
        std::string whiteTeam;

		uint16_t size[2];

		std::string ruleset;
		std::string result;
		float komi;
		float timePerPlayer;
		uint16_t handicap;

        uint16_t startingPlayer;

        MatchInfo() : komi(0), handicap(0), timePerPlayer(0), startingPlayer(0) { size[0] = size[1] = 0; }
	};

    class GameTreeDesc {
    public:
        FileHeader fileHeader;
        GameMetaData gameMetaData;
        MatchInfo matchInfo;

        std::vector<std::pair<uint16_t, std::pair<uint16_t, uint16_t>>> premadeStones, moves;
    };

	class SGFNode {
	public:
		std::map<std::string, std::vector<std::string>> properties;
	};

	class SGFTreeNode {
	public:
		shared_ptr<SGFTreeNode> parent;
		std::vector<shared_ptr<SGFNode>> sequence;
		std::vector<shared_ptr<SGFTreeNode>> children;

		SGFTreeNode() : parent(nullptr) {}
	};

	typedef std::shared_ptr<SGFTreeNode> spTreeNode;
	typedef std::shared_ptr<SGFNode> spNode;


	//Parsers
	
	template <typename Iterator>
	struct SGFFileParser : qi::grammar<Iterator, qi::ascii::space_type> {

		shared_ptr<std::vector<spTreeNode>> games;
		spTreeNode currentGameTree;
		spNode currentNode;

		SGFFileParser(shared_ptr<std::vector<spTreeNode>> g) : SGFFileParser::base_type(Collection), currentGameTree(nullptr), currentNode(nullptr), games(g)
		{
				Collection = +GameTree;
				GameTree = OpenGameTree >> Sequence >> *GameTree >> CloseGameTree;

				OpenGameTree = qi::lit('(')
					[boost::phoenix::bind(&SGFFileParser::add_game, this)];
				CloseGameTree = qi::lit(')')
					[boost::phoenix::bind(&SGFFileParser::close_game, this)];

				Sequence = +Node;
				Node = (NodeSeparator >> *Property); //doesn't conform to the specification, 
													 //but too many files during testing had empty 
													 //nodes to just ignore it
				NodeSeparator = (qi::lit(';'))
					[boost::phoenix::bind(&SGFFileParser::add_node, this)];

				Property = (PropIdent >> +PropValue)
					[boost::phoenix::bind(&SGFFileParser::add_property, this, qi::_1, qi::_2)];
				PropIdent = +UcLetter;
				PropValue = qi::lit('[') >> ValueT >> qi::lit(']');

				ValueT = -(Text);

				UcLetter = qi::upper;
				Text = boost::spirit::qi::no_skip[*((qi::print | qi::space | qi::lit('\\') >> qi::char_) - qi::lit('[') - qi::lit(']'))];
		}

		qi::rule<Iterator, qi::ascii::space_type> Collection;
		qi::rule<Iterator, std::string(), qi::ascii::space_type> GameTree, OpenGameTree, CloseGameTree,
			Sequence, Node, NodeSeparator, Property, PropIdent, PropValue, ValueT, UcLetter, Text;

		void add_game();
		void close_game();
		void add_node();
		void add_property(std::string, std::vector<std::string>);

		bool do_parse(std::string);
	};


	class SGFParser {
	public:
		std::string fileBuffer;
		shared_ptr<SGFFileParser<std::string::const_iterator>> fileParser;
		shared_ptr<std::vector<spTreeNode>> games;
        std::map<std::string, std::function<bool(std::string, std::vector<std::string>&)>> nodeParsers;

        GameTreeDesc gameTreeDescription;
		
		SGFParser() { InitParser(); }
		SGFParser(std::string fb) : fileBuffer(fb) { InitParser(); }
        void InitParser();
		bool ParseBuffer(std::string = std::string());
        bool ParseTree();
        bool TraverseTree(spTreeNode);
        bool ParseNode(spNode);
        std::string ConvertToSimpleText(std::string);
	};

}
