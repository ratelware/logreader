#pragma once

#include <deque>

#include <re2/re2.h>
#include <re2/stringpiece.h>

#include <datasource/content.hpp>

#include <compressor/compressor.hpp>

namespace datasource {
	class data_sink {
	public:
		void consume(compressor::chunk*);
		void consume_raw(compressor::uncompressed_chunk*);
		void consume(content&);
		void add_child(const std::shared_ptr<data_sink>& child);

		virtual ~data_sink();
	protected:
		virtual bool should_stay(const char*, std::size_t) = 0;
		std::deque<compressor::chunk> chunks;
	private:
		compressor::chunk* get_active_chunk(std::size_t bytes_to_compress);
		std::size_t get_previous_line_number();

		std::vector<std::weak_ptr<data_sink> > children;
		compressor::compressor compressor;
	};

	class promiscous_sink : public data_sink {
		virtual bool should_stay(const char*, std::size_t);
	};

	class substring_data_sink : public data_sink {
	public:
		substring_data_sink(const std::string& match);

		virtual bool should_stay(const char*, std::size_t);

	private:
		std::string match;
	};

	class grepping_data_sink : public data_sink {
	public:
		grepping_data_sink(std::unique_ptr<re2::RE2>&& regex);

		virtual bool should_stay(const char*, std::size_t);

	private:
		std::unique_ptr<re2::RE2> regex;
	};

}