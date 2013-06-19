#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ZAI");
    BOOST_CHECK(GetBoolArg("-ZAI"));
    BOOST_CHECK(GetBoolArg("-ZAI", false));
    BOOST_CHECK(GetBoolArg("-ZAI", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ZAIo"));
    BOOST_CHECK(!GetBoolArg("-ZAIo", false));
    BOOST_CHECK(GetBoolArg("-ZAIo", true));

    ResetArgs("-ZAI=0");
    BOOST_CHECK(!GetBoolArg("-ZAI"));
    BOOST_CHECK(!GetBoolArg("-ZAI", false));
    BOOST_CHECK(!GetBoolArg("-ZAI", true));

    ResetArgs("-ZAI=1");
    BOOST_CHECK(GetBoolArg("-ZAI"));
    BOOST_CHECK(GetBoolArg("-ZAI", false));
    BOOST_CHECK(GetBoolArg("-ZAI", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noZAI");
    BOOST_CHECK(!GetBoolArg("-ZAI"));
    BOOST_CHECK(!GetBoolArg("-ZAI", false));
    BOOST_CHECK(!GetBoolArg("-ZAI", true));

    ResetArgs("-noZAI=1");
    BOOST_CHECK(!GetBoolArg("-ZAI"));
    BOOST_CHECK(!GetBoolArg("-ZAI", false));
    BOOST_CHECK(!GetBoolArg("-ZAI", true));

    ResetArgs("-ZAI -noZAI");  // -ZAI should win
    BOOST_CHECK(GetBoolArg("-ZAI"));
    BOOST_CHECK(GetBoolArg("-ZAI", false));
    BOOST_CHECK(GetBoolArg("-ZAI", true));

    ResetArgs("-ZAI=1 -noZAI=1");  // -ZAI should win
    BOOST_CHECK(GetBoolArg("-ZAI"));
    BOOST_CHECK(GetBoolArg("-ZAI", false));
    BOOST_CHECK(GetBoolArg("-ZAI", true));

    ResetArgs("-ZAI=0 -noZAI=0");  // -ZAI should win
    BOOST_CHECK(!GetBoolArg("-ZAI"));
    BOOST_CHECK(!GetBoolArg("-ZAI", false));
    BOOST_CHECK(!GetBoolArg("-ZAI", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ZAI=1");
    BOOST_CHECK(GetBoolArg("-ZAI"));
    BOOST_CHECK(GetBoolArg("-ZAI", false));
    BOOST_CHECK(GetBoolArg("-ZAI", true));

    ResetArgs("--noZAI=1");
    BOOST_CHECK(!GetBoolArg("-ZAI"));
    BOOST_CHECK(!GetBoolArg("-ZAI", false));
    BOOST_CHECK(!GetBoolArg("-ZAI", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", "eleven"), "eleven");

    ResetArgs("-ZAI -bar");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", "eleven"), "");

    ResetArgs("-ZAI=");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", "eleven"), "");

    ResetArgs("-ZAI=11");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", "eleven"), "11");

    ResetArgs("-ZAI=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ZAI", 0), 0);

    ResetArgs("-ZAI -bar");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ZAI=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ZAI=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ZAI");
    BOOST_CHECK_EQUAL(GetBoolArg("-ZAI"), true);

    ResetArgs("--ZAI=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ZAI", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noZAI");
    BOOST_CHECK(!GetBoolArg("-ZAI"));
    BOOST_CHECK(!GetBoolArg("-ZAI", true));
    BOOST_CHECK(!GetBoolArg("-ZAI", false));

    ResetArgs("-noZAI=1");
    BOOST_CHECK(!GetBoolArg("-ZAI"));
    BOOST_CHECK(!GetBoolArg("-ZAI", true));
    BOOST_CHECK(!GetBoolArg("-ZAI", false));

    ResetArgs("-noZAI=0");
    BOOST_CHECK(GetBoolArg("-ZAI"));
    BOOST_CHECK(GetBoolArg("-ZAI", true));
    BOOST_CHECK(GetBoolArg("-ZAI", false));

    ResetArgs("-ZAI --noZAI");
    BOOST_CHECK(GetBoolArg("-ZAI"));

    ResetArgs("-noZAI -ZAI"); // ZAI always wins:
    BOOST_CHECK(GetBoolArg("-ZAI"));
}

BOOST_AUTO_TEST_SUITE_END()
