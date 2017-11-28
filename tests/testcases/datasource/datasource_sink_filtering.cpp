#include <numeric>

#include <boost/test/auto_unit_test.hpp>

#include <turtle/mock.hpp>

#include <datasource/data_sink.hpp>

namespace {
	std::function<bool(const char*)> strmatch(const char* other, int size) {
		return [other, size](const char* a) {
			return !strncmp(a, other, size);
		};
	}
}

MOCK_BASE_CLASS(mock_sink, datasource::data_sink) {
	MOCK_METHOD(should_stay, 2);
};

BOOST_AUTO_TEST_SUITE(compression_roundtrip)
BOOST_AUTO_TEST_CASE(fields_proper_number_of_rows)
{
	mock_sink sink;
	char* data = "abc\n";
	MOCK_EXPECT(sink.should_stay).once().with(strmatch(data, 4), 4).returns(true);

	compressor::uncompressed_chunk b{ 0, std::vector<char>{data, data + 4}, nullptr };
	sink.consume_raw(&b);
}

BOOST_AUTO_TEST_CASE(should_pass_all_nonfiltered_data_to_child)
{
	mock_sink sink;
	char* data = "abc\ndef\n\ghi\n";
	MOCK_EXPECT(sink.should_stay).once().with(strmatch("abc\n", 4), 4).returns(true);
	MOCK_EXPECT(sink.should_stay).once().with(strmatch("def\n", 4), 4).returns(false);
	MOCK_EXPECT(sink.should_stay).once().with(strmatch("ghi\n", 4), 4).returns(true);
	compressor::uncompressed_chunk b{ 0, std::vector<char>{data, data + 12}, nullptr };
	sink.consume_raw(&b);

	std::shared_ptr<mock_sink> child = std::make_shared<mock_sink>();

	MOCK_EXPECT(child->should_stay).once().calls([](const char* data, std::size_t size) {
		BOOST_CHECK_EQUAL(size, 4);
		BOOST_CHECK_EQUAL(0, strncmp(data, "abc\n", 4));
		return true;
	});

	MOCK_EXPECT(child->should_stay).once().calls([](const char* data, std::size_t size) {
		BOOST_CHECK_EQUAL(size, 4);
		BOOST_CHECK_EQUAL(0, strncmp(data, "ghi\n", 4));
		return false;
	});

	sink.add_child(child);
}

BOOST_AUTO_TEST_CASE(should_successfully_handle_long_strings) {
	std::string s;
	s.resize(32 * 1024);
	std::iota(s.begin(), s.end(), 0);
	s.back() = '\n';

	compressor::uncompressed_chunk b{ 0, std::vector<char>{s.begin(), s.end()}, nullptr };
	mock_sink sink;
	MOCK_EXPECT(sink.should_stay).returns(true);
	sink.consume_raw(&b);

	std::shared_ptr<mock_sink> child = std::make_shared<mock_sink>();
	MOCK_EXPECT(child->should_stay).returns(true);

	sink.add_child(child);
}


BOOST_AUTO_TEST_CASE(two_layers_of_regexes_should_go_fine) {
	std::string s("abc\ndef\n\gha\njkl\nabn\n");

	compressor::uncompressed_chunk b{ 0, std::vector<char>{s.begin(), s.end()}, nullptr };
	datasource::promiscous_sink root_sink;

	root_sink.consume_raw(&b);
	std::shared_ptr<datasource::substring_data_sink> filter1 = std::make_shared<datasource::substring_data_sink>("a");
	std::shared_ptr<datasource::substring_data_sink> filter2 = std::make_shared<datasource::substring_data_sink>("b");

	root_sink.add_child(filter1);
	filter1->add_child(filter2);

	std::shared_ptr<mock_sink> bottom_filter = std::make_shared<mock_sink>();

	MOCK_EXPECT(bottom_filter->should_stay).once().with(strmatch("abc\n", 4), 4).returns(false);
	MOCK_EXPECT(bottom_filter->should_stay).once().with(strmatch("abn\n", 4), 4).returns(true);

	filter2->add_child(bottom_filter);
}

BOOST_AUTO_TEST_SUITE_END()
