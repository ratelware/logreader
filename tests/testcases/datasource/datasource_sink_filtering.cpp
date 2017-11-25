#include <boost/test/auto_unit_test.hpp>

#include <turtle/mock.hpp>

#include <datasource/data_source.hpp>

MOCK_BASE_CLASS(mock_sink, datasource::data_sink) {
	MOCK_METHOD(should_stay, 2);
};

BOOST_AUTO_TEST_SUITE(compression_roundtrip)
BOOST_AUTO_TEST_CASE(fields_proper_number_of_rows)
{
	mock_sink sink;
	const char* data = "abc\n";
	MOCK_EXPECT(sink.should_stay).once().with(data, 4).returns(true);
	sink.consume_raw(data, 4);
	sink.end_stream();
}
/*
BOOST_AUTO_TEST_CASE(should_pass_all_nonfiltered_data_to_child)
{
	mock_sink sink;
	const char* data = "abc\ndef\n\ghi\n";
	MOCK_EXPECT(sink.should_stay).once().with(data, 4).returns(true);
	MOCK_EXPECT(sink.should_stay).once().with(data + 4, 4).returns(false);
	MOCK_EXPECT(sink.should_stay).once().with(data + 8, 4).returns(true);
	sink.consume_raw(data, 12);
	sink.end_stream();

	std::shared_ptr<mock_sink> child = std::make_shared<mock_sink>();

	MOCK_EXPECT(child->should_stay).once().calls([](const char* data, std::size_t size) {
		BOOST_CHECK_EQUAL(size, 4);
		BOOST_CHECK_EQUAL(0, strncmp(data, "abc\n", 4));
		return true;
	});
	
	MOCK_EXPECT(child->should_stay).once().calls([](const char* data, std::size_t size) {
		BOOST_CHECK_EQUAL(size, 4);
		BOOST_CHECK_EQUAL(0, strncmp(data, "abc\n", 4));
		return false;
	});

	sink.add_child(child);
}
*/
BOOST_AUTO_TEST_SUITE_END()
