#include <boost/test/auto_unit_test.hpp>

#include <datasource/cache.hpp>

BOOST_AUTO_TEST_SUITE(cache_test)

BOOST_AUTO_TEST_CASE(cache_has_elements_that_were_put) {
	datasource::cache<int, int> c(2);

	BOOST_CHECK_EQUAL(c.get(1), nullptr);
	c.put(1, 3);

	BOOST_CHECK_EQUAL(*(c.get(1)), 3);
}

BOOST_AUTO_TEST_CASE(cache_removes_elements_older_than_max) {
	datasource::cache<int, std::string> c(3);
	c.put(1, "a");
	c.put(2, "ab");
	c.put(4, "abcd");

	BOOST_CHECK_EQUAL(*(c.get(1)), "a");
	BOOST_CHECK_EQUAL(*(c.get(2)), "ab");
	BOOST_CHECK_EQUAL(*(c.get(4)), "abcd");

	c.put(5, "abcde");
	BOOST_CHECK_EQUAL(c.get(1), nullptr);
	BOOST_CHECK_EQUAL(*(c.get(2)), "ab");
	BOOST_CHECK_EQUAL(*(c.get(4)), "abcd");
	BOOST_CHECK_EQUAL(*(c.get(5)), "abcde");
}

BOOST_AUTO_TEST_CASE(cache_get_moves_entry_up) {
	datasource::cache<int, std::string> c(3);
	c.put(1, "a");
	c.put(2, "ab");
	c.put(4, "abcd");

	BOOST_CHECK_EQUAL(*(c.get(2)), "ab");
	BOOST_CHECK_EQUAL(*(c.get(4)), "abcd");
	BOOST_CHECK_EQUAL(*(c.get(1)), "a");

	c.put(5, "abcde");
	BOOST_CHECK_EQUAL(*(c.get(1)), "a");
	BOOST_CHECK_EQUAL(c.get(2), nullptr);
	BOOST_CHECK_EQUAL(*(c.get(4)), "abcd");
	BOOST_CHECK_EQUAL(*(c.get(5)), "abcde");
}

BOOST_AUTO_TEST_CASE(cache_put_overrides_previous_data_and_moves_to_front) {
	datasource::cache<int, std::string> c(2);
	c.put(1, "a");
	c.put(2, "ab");

	BOOST_CHECK_EQUAL(*(c.get(1)), "a");
	BOOST_CHECK_EQUAL(*(c.get(2)), "ab");

	c.put(1, "abc");
	c.put(4, "abba");
	BOOST_CHECK_EQUAL(*(c.get(1)), "abc");
	BOOST_CHECK_EQUAL(c.get(2), nullptr);
	BOOST_CHECK_EQUAL(*(c.get(4)), "abba");

}


BOOST_AUTO_TEST_SUITE_END()