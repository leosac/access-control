/**
 * \file configparser.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ConfigParser class implementation
 */

#include "configparser.hpp"

#include <iostream>
#include <locale>

#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/property_tree/xml_parser.hpp>

typedef boost::gregorian::date Date;

struct Flight
{
    std::string  carrier;
    unsigned     number;
    Date         date;
    bool         cancelled;
};

typedef std::vector<Flight> Sked;

class DateTranslator
{
    typedef boost::date_time::date_facet<Date, char> tOFacet;
    typedef boost::date_time::date_input_facet<Date, char> tIFacet;
    std::locale locale_;
    static std::locale isoDateLocale() {
        std::locale loc;
        loc = std::locale( loc, new tIFacet("%Y-%m-%d") );
        loc = std::locale( loc, new tOFacet("%Y-%m-%d") );
        return loc;
    }
public:
    typedef std::string
    internal_type;
    typedef Date
    external_type;
    DateTranslator( ) : locale_( isoDateLocale() ) {}
    boost::optional<external_type> get_value( internal_type const &v )
    {
        std::istringstream stream(v);
        stream.imbue(locale_);
        external_type vAns;
        if( stream >> vAns ) {
            return vAns;
        }
        else {
            return boost::none;
        }
    }
    boost::optional<internal_type> put_value(external_type const& v)
    {
        std::ostringstream ans;
        ans.imbue(locale_);
        ans << v;
        return ans.str();
    }
};

namespace boost
{
    namespace property_tree
    {
        template<>
        struct translator_between<std::string, Date>
        {
            typedef DateTranslator type;
        };
    }
}

using boost::property_tree::ptree;
using boost::property_tree::xml_parser::read_xml;

void ConfigParser::parse(std::istream& content)
{
    ptree pt;

    read_xml(content, pt);

    Sked ans;
    BOOST_FOREACH( ptree::value_type const& v, pt.get_child("sked") ) {
        if( v.first == "flight" ) {
            Flight f;
            f.carrier = v.second.get<std::string>("carrier");
            f.number = v.second.get<unsigned>("number");
            f.date = v.second.get<Date>("date");
            f.cancelled = v.second.get("<xmlattr>.cancelled", false);
            std::cout << "Flight n" << f.number << " via " << f.carrier << " is " << f.cancelled << std::endl;
            ans.push_back(f);
        }
    }
}
