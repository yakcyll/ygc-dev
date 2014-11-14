/*
* SGFParser.h
* ygc SGF parser header
*/

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_auto.hpp>
#include <boost/spirit/include/qi_no_skip.hpp>
#include <boost/phoenix.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <vector>
//#include "ygc.h"

#define BOOST_SPIRIT_USE_PHOENIX_V3

namespace qi	= boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

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
		std::string white;
		std::string whiteRank;

		uint16_t size[2];

		std::string ruleset;
		std::string result;
		float komi;
		uint16_t timePerPlayer;
		uint16_t handicap;
	};

	class SGFNode {
	public:
		std::map<std::string, std::vector<std::string>> properties;
	};

	class SGFTreeNode {
	public:
		SGFTreeNode* parent;
		std::vector<SGFNode*> sequence;
		std::vector<SGFTreeNode*> children;

		SGFTreeNode() : parent(nullptr) {}
	};

	template <typename Iterator>
	struct SGFParser : qi::grammar<Iterator, qi::ascii::space_type> {

		std::vector<SGFTreeNode*> games;
		SGFTreeNode* currentGameTree;
		SGFNode* currentNode;

		SGFParser() : SGFParser::base_type(Collection), currentGameTree(nullptr), currentNode(nullptr)
		{
				Collection = +GameTree;
				GameTree = OpenGameTree >> Sequence >> *GameTree >> CloseGameTree;

				OpenGameTree = qi::lit('(')
					[boost::phoenix::bind(&SGFParser::add_game, this)];
				CloseGameTree = qi::lit(')')
					[boost::phoenix::bind(&SGFParser::close_game, this)];

				Sequence = +Node;
				Node = (NodeSeparator >> *Property); //doesn't conform to the specification, 
													 //but too many files during testing had empty 
													 //nodes to just ignore it
				NodeSeparator = (qi::lit(';'))
					[boost::phoenix::bind(&SGFParser::add_node, this)];

				Property = (PropIdent >> +PropValue)
					[boost::phoenix::bind(&SGFParser::add_property, this, qi::_1, qi::_2)];
				PropIdent = +UcLetter;
				PropValue = qi::lit('[') >> ValueT >> qi::lit(']');

				ValueT = -(Text);

				UcLetter = qi::upper;
				Text = boost::spirit::qi::no_skip[*((qi::print | qi::space) - qi::lit('[') - qi::lit(']'))];
		}

		qi::rule<Iterator, qi::ascii::space_type> Collection;
		qi::rule<Iterator, std::string(), qi::ascii::space_type> GameTree, OpenGameTree, CloseGameTree,
			Sequence, Node, NodeSeparator, Property, PropIdent, PropValue, ValueT, UcLetter;
		qi::rule<Iterator, std::string(), qi::ascii::space_type> Text;

		void add_game();
		void close_game();
		void add_node();
		void add_property(std::string, std::vector<std::string>);

		bool do_parse(std::string, std::vector<SGFTreeNode*>&);
	};


}
