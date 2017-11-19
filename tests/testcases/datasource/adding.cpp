#include <boost/test/auto_unit_test.hpp>

BOOST_AUTO_TEST_SUITE(reading_file)
BOOST_AUTO_TEST_CASE(fields_proper_number_of_rows)
{
    BOOST_CHECK_EQUAL(4, 2+2); // this is the "usual" assert
    BOOST_REQUIRE_EQUAL(6, 4+2); // this one throws if is unhappy
    BOOST_WARN_EQUAL(8, 4+4); // this one only prints, so nobody cares
}

BOOST_AUTO_TEST_SUITE_END()
