#pragma once

#include <memory>
#include <deque>
#include <boost/filesystem.hpp>
#include <re2/re2.h>

#include <compressor/chunk.hpp>
#include <compressor/compressor.hpp>

namespace datasource {

	class data_sink;

	class content {
	public:
		content();

		void add_data(const char* buffer, std::size_t char_count);
		data_sink* get_sink();
		void end_stream();
	private:
		std::unique_ptr<data_sink> sink;
		std::basic_string<char> carry;
	};

	class data_sink {
	public:
		void consume(compressor::chunk*);
		void consume_raw(const char*, std::size_t);
		void consume(content&);
		void end_stream();
		void add_child(const std::shared_ptr<data_sink>& child);
		void preserve_dictionaries();

		virtual ~data_sink() {};
	protected:
		virtual bool should_stay(const char*, std::size_t) = 0;
		std::deque<compressor::chunk> chunks;
	private:
		std::vector<std::weak_ptr<data_sink> > children;
		compressor::compressor compressor;
	};

	class promiscous_sink : public data_sink {
		virtual bool should_stay(const char*, std::size_t);
	};

	class grepping_data_sink : public data_sink {
	public:
		grepping_data_sink(std::unique_ptr<re2::RE2>&& regex);

		virtual bool should_stay(const char*, std::size_t);

	private:
		std::unique_ptr<re2::RE2> regex;
	};

	class data_source {
	public:
		virtual std::shared_ptr<content> readfile(const boost::filesystem::path& p);
	};

}
