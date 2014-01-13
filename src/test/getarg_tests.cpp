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
    ResetArgs("-CLC");
    BOOST_CHECK(GetBoolArg("-CLC"));
    BOOST_CHECK(GetBoolArg("-CLC", false));
    BOOST_CHECK(GetBoolArg("-CLC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CLCo"));
    BOOST_CHECK(!GetBoolArg("-CLCo", false));
    BOOST_CHECK(GetBoolArg("-CLCo", true));

    ResetArgs("-CLC=0");
    BOOST_CHECK(!GetBoolArg("-CLC"));
    BOOST_CHECK(!GetBoolArg("-CLC", false));
    BOOST_CHECK(!GetBoolArg("-CLC", true));

    ResetArgs("-CLC=1");
    BOOST_CHECK(GetBoolArg("-CLC"));
    BOOST_CHECK(GetBoolArg("-CLC", false));
    BOOST_CHECK(GetBoolArg("-CLC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCLC");
    BOOST_CHECK(!GetBoolArg("-CLC"));
    BOOST_CHECK(!GetBoolArg("-CLC", false));
    BOOST_CHECK(!GetBoolArg("-CLC", true));

    ResetArgs("-noCLC=1");
    BOOST_CHECK(!GetBoolArg("-CLC"));
    BOOST_CHECK(!GetBoolArg("-CLC", false));
    BOOST_CHECK(!GetBoolArg("-CLC", true));

    ResetArgs("-CLC -noCLC");  // -CLC should win
    BOOST_CHECK(GetBoolArg("-CLC"));
    BOOST_CHECK(GetBoolArg("-CLC", false));
    BOOST_CHECK(GetBoolArg("-CLC", true));

    ResetArgs("-CLC=1 -noCLC=1");  // -CLC should win
    BOOST_CHECK(GetBoolArg("-CLC"));
    BOOST_CHECK(GetBoolArg("-CLC", false));
    BOOST_CHECK(GetBoolArg("-CLC", true));

    ResetArgs("-CLC=0 -noCLC=0");  // -CLC should win
    BOOST_CHECK(!GetBoolArg("-CLC"));
    BOOST_CHECK(!GetBoolArg("-CLC", false));
    BOOST_CHECK(!GetBoolArg("-CLC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CLC=1");
    BOOST_CHECK(GetBoolArg("-CLC"));
    BOOST_CHECK(GetBoolArg("-CLC", false));
    BOOST_CHECK(GetBoolArg("-CLC", true));

    ResetArgs("--noCLC=1");
    BOOST_CHECK(!GetBoolArg("-CLC"));
    BOOST_CHECK(!GetBoolArg("-CLC", false));
    BOOST_CHECK(!GetBoolArg("-CLC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CLC", "eleven"), "eleven");

    ResetArgs("-CLC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CLC", "eleven"), "");

    ResetArgs("-CLC=");
    BOOST_CHECK_EQUAL(GetArg("-CLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CLC", "eleven"), "");

    ResetArgs("-CLC=11");
    BOOST_CHECK_EQUAL(GetArg("-CLC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CLC", "eleven"), "11");

    ResetArgs("-CLC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CLC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CLC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CLC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CLC", 0), 0);

    ResetArgs("-CLC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CLC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CLC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CLC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CLC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CLC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CLC");
    BOOST_CHECK_EQUAL(GetBoolArg("-CLC"), true);

    ResetArgs("--CLC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CLC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCLC");
    BOOST_CHECK(!GetBoolArg("-CLC"));
    BOOST_CHECK(!GetBoolArg("-CLC", true));
    BOOST_CHECK(!GetBoolArg("-CLC", false));

    ResetArgs("-noCLC=1");
    BOOST_CHECK(!GetBoolArg("-CLC"));
    BOOST_CHECK(!GetBoolArg("-CLC", true));
    BOOST_CHECK(!GetBoolArg("-CLC", false));

    ResetArgs("-noCLC=0");
    BOOST_CHECK(GetBoolArg("-CLC"));
    BOOST_CHECK(GetBoolArg("-CLC", true));
    BOOST_CHECK(GetBoolArg("-CLC", false));

    ResetArgs("-CLC --noCLC");
    BOOST_CHECK(GetBoolArg("-CLC"));

    ResetArgs("-noCLC -CLC"); // CLC always wins:
    BOOST_CHECK(GetBoolArg("-CLC"));
}

BOOST_AUTO_TEST_SUITE_END()
