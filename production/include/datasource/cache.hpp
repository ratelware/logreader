#pragma once

#include <list>

namespace datasource {

	// cache implements the basic version of LRU algorithm
	template<typename key_t, typename value_t>
	class cache {
	public:
		typedef std::pair<key_t, value_t> entry;

		cache(std::size_t size) : max_size(size) {
		}

		void put(key_t key, value_t val) {
			auto location = find(key);
			if (location == memory.end()) {
				if(memory.size() >= max_size) {
					memory.pop_back();
				}
			}
			else {
				memory.erase(location);
			}
			memory.push_front(entry(key, val));
		}

		value_t* get(key_t key) {
			auto location = find(key);
			if (location != memory.end()) {
				memory.splice(memory.begin(), memory, location);
				return &(location->second);
			}

			return nullptr;
		}

	private:
		typedef std::list<entry> entries;

		typename entries::iterator find(key_t key) {
			return std::find_if(memory.begin(), memory.end(), [key](const entry& e) { return e.first == key; });
		}

		entries memory;
		std::size_t max_size;
	};
}