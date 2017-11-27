#pragma once

#include <deque>

#include <re2/re2.h>
#include <re2/stringpiece.h>

#include <compressor/compressor.hpp>

namespace datasource {
	class content;

	class data_sink {
	public:
		void consume(compressor::chunk*);
		void consume_raw(const char*, std::size_t);
		void consume(content&);
		void add_child(const std::shared_ptr<data_sink>& child);

		virtual ~data_sink() {};
	protected:
		virtual bool should_stay(const char*, std::size_t) = 0;
		std::deque<compressor::chunk> chunks;
	private:
		compressor::chunk* get_active_chunk(std::size_t bytes_to_compress);

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

}