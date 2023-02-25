#ifndef HARZ_CCKHASH_MAP
#define HARZ_CCKHASH_MAP

// Custom implementation of hash map based on cuckoo hashing
// c++14 and newer versions(possibly c++11 also, but not guaranteed)
// repository: https://github.com/IHarzI/Cuckoo_hashing-containers
// made by IHarzI

#include <vector>
#include <functional>

// custom params, experiment with different values for better perfomance
#define HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD (2.5f) 
#define HARZ_CCKHASH_MAP_RESIZE_MOD (1.5f)

namespace harz
{

	template<typename K, typename V>
	class cuckooHashMap
	{
	public:

		cuckooHashMap(const uint32_t capacity = 64, const uint32_t tablesCount = 2)
			: _capacity(capacity), _tablesCount(tablesCount), _maxIters((uint32_t)(tablesCount* HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD) + 1)
		{

			_data.resize(tablesCount);
			for (uint32_t iter = 0; iter < tablesCount; iter++)
			{
				_data[iter].resize(capacity);
			}
		}

		struct TableSlot
		{

			K key;
			V value;
			bool occupied = false;
		};

		struct K_V_pair
		{
			K key;
			V value;
		};

		// ("rehash") container,  be careful if give custom parameter, possible loss of data (if newCapacity < current capacity)
		bool resize(uint32_t newCapacity = 0)
		{
			if (newCapacity <= 0)
			{
				newCapacity = (uint32_t)(_capacity * HARZ_CCKHASH_MAP_RESIZE_MOD) + 1;
			}

			std::vector<std::vector<TableSlot>> oldData = _data;

			_capacity = newCapacity;

			clear();

			for (std::vector<TableSlot> table : oldData)
			{
				for (auto& slot : table)
				{
					if (slot.occupied)
						insert(slot.key, slot.value);
				};
			};

			return true;
		};

		// Change tables count, be careful if give custom parameter, possible loss of data (if newTablesCount < current tables count)
		bool restrain(const uint32_t newTablesCount)
		{
			if (newTablesCount <= 2)
				return false;

			_tablesCount = newTablesCount;
			resize(_capacity);

			_maxIters = _tablesCount * HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD;
			return true;
		};


	private:
		uint32_t _capacity = 64;
		uint32_t _tablesCount = 2;
		uint32_t _maxIters = _tablesCount * HARZ_CCKHASH_MAP_MAX_ITERATIONS_MOD + 1;

		std::vector<std::vector<TableSlot>> _data;

		std::function <const uint32_t(K, uint32_t, uint32_t, uint32_t)> _g_CCKHT_l_hashFunction = [](K key, uint32_t cap, uint32_t tablecnt, uint32_t i)-> const uint32_t
		{
			return ((std::hash< uint32_t>()(std::hash<K>()(key) + std::hash< uint32_t>()(i % (tablecnt + cap))))) % cap;
		};

		const bool _CCKHT_insertData(const K& key, const V& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (contains(key))
					{
						return false;
					}
					if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = key;
						_data[currentTable][hashedKey].value = value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].key = key;
						_data[currentTable][hashedKey].value = value;
						_data[currentTable][hashedKey].occupied = true;
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(const K&& key, const V&& value, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

					if (contains(key))
					{
						return false;
					}
					if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = std::move(key);
						_data[currentTable][hashedKey].value = std::move(value);
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].key = std::move(key);
						_data[currentTable][hashedKey].value = std::move(value);
						_data[currentTable][hashedKey].occupied = true;
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(const K_V_pair& k_v_pair, uint32_t iterations = 0)
		{
			while (true)
			{
				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(k_v_pair.key, _capacity, _tablesCount, iterations);

					if (contains(k_v_pair.key))
					{
						return false;
					}
					if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = k_v_pair.key;
						_data[currentTable][hashedKey].value = k_v_pair.value;
						return _CCKHT_insertData(std::move(temp), iterations);
					}
					else
					{
						_data[currentTable][hashedKey].key = k_v_pair.key;
						_data[currentTable][hashedKey].value = k_v_pair.value;
						_data[currentTable][hashedKey].occupied = true;
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

		const bool _CCKHT_insertData(K_V_pair&& k_v_pair, uint32_t iterations = 0)
		{
			while (true)
			{

				while (iterations < _maxIters)
				{
					const uint32_t currentTable = iterations % _tablesCount;
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(k_v_pair.key, _capacity, _tablesCount, iterations);

					if (contains(k_v_pair.key))
					{
						return false;
					}
					if (_data[currentTable][hashedKey].occupied)
					{
						K_V_pair temp;
						temp.key = _data[currentTable][hashedKey].key;
						temp.value = _data[currentTable][hashedKey].value;
						_data[currentTable][hashedKey].key = std::move(k_v_pair.key);
						_data[currentTable][hashedKey].value = std::move(k_v_pair.value);
						k_v_pair = temp;
					}
					else
					{
						_data[currentTable][hashedKey].key = std::move(k_v_pair.key);
						_data[currentTable][hashedKey].value = std::move(k_v_pair.value);
						_data[currentTable][hashedKey].occupied = true;
						return true;
					}
					iterations++;
				}
				resize();
				iterations = 0;
			}
		}

	public:

		// Erases all elements that satisfy the predicate pred from the container
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (predicate(slot.key, slot.value))
					{
						slot.value = V();
						slot.key = K();
						slot.occupied = false;
						erasuresCount += 1;
					}
				}
			}
			return erasuresCount;
		}

		// Erases all elements that satisfy the predicate pred from the container
		// Predicate must take parameters in (K key, V value) form
		template <typename PredicateT>
		const uint32_t erase_if(const PredicateT&& predicate)
		{
			uint32_t erasuresCount = 0;
			for (auto& table : _data)
			{
				for (auto& slot : table)
				{
					if (predicate(slot.key, slot.value))
					{
						slot.value = V();
						slot.key = K();
						slot.occupied = false;
						erasuresCount += 1;
					}
				}
			}
			return erasuresCount;
		}

		// Extract element by key
		K_V_pair extract(K& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].key == key)
					{
						K_V_pair tmp;
						tmp.key = _data[currentTable][hashedKey].key;
						tmp.value = _data[currentTable][hashedKey].value;
						erase(key);
						return std::move(tmp);
					}
				iters++;
			}
			return K_V_pair();
		}

		// Extract element by key
		K_V_pair extract(K&& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;
				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].key == key)
					{
						K_V_pair tmp;
						tmp.key = _data[currentTable][hashedKey].key;
						tmp.value = _data[currentTable][hashedKey].value;
						erase(key);
						return std::move(tmp);
					}
				iters++;
			}
			return K_V_pair();
		}
		// Extract elements by keys from init list
		std::vector<K_V_pair> extract(std::initializer_list<K> l)
		{
			std::vector<K_V_pair> results;
			results.reserve(l.size());
			uint32_t index = 0;
			for (index < l.size(); index++;)
			{
				auto& element = l[index];
				uint32_t iters = 0;
				while (iters < _maxIters)
				{
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(element, _capacity, _tablesCount, iters);
					const int32_t currentTable = iters % _tablesCount;
					if (_data[currentTable][hashedKey].occupied)
						if (_data[currentTable][hashedKey].key == element)
						{
							K_V_pair tmp;
							tmp.key = _data[currentTable][hashedKey].key;
							tmp.value = _data[currentTable][hashedKey].value;
							results.push_back(std::move(tmp));
							erase(element);
						}
					iters++;
				}
			}
			return results;
		}

		// Erase all elements.
		void clear()
		{
			_data = std::vector<std::vector<TableSlot>>();
			_data.resize(_tablesCount);
			for (uint32_t tables = 0; tables < _tablesCount; tables++)
			{
				_data[tables].resize(_capacity);
			}
		}
		// Erase element by keys from init list.
		std::vector<bool> erase(std::initializer_list<K> l)
		{
			std::vector<bool> results(l.size(), false);
			uint32_t iters = 0;
			for (auto key : l)
			{
				for (uint32_t inTableIndex = 0; inTableIndex < _maxIters; inTableIndex++)
				{
					const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, inTableIndex);
					const uint32_t currentTable = inTableIndex % _tablesCount;

					if (_data[currentTable][hashedKey].key == key && _data[currentTable][hashedKey].occupied)
					{
						_data[currentTable][hashedKey].key = K();
						_data[currentTable][hashedKey].value = V();
						_data[currentTable][hashedKey].occupied = false;
						results[iters] = true;
					}
				}
				iters++;
			}
			return results;
		}
		// Erase element by key
		const bool erase(const K& key)
		{
			for (uint32_t iters = 0; iters < _maxIters; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].key == key && _data[currentTable][hashedKey].occupied)
				{
					_data[currentTable][hashedKey].key = K();
					_data[currentTable][hashedKey].value = V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}
		// Erase element by key
		const bool erase(const K&& key)
		{
			for (uint32_t iters = 0; iters < _maxIters; iters++)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].key == key && _data[currentTable][hashedKey].occupied)
				{
					_data[currentTable][hashedKey].key = K();
					_data[currentTable][hashedKey].value = V();
					_data[currentTable][hashedKey].occupied = false;
					return true;
				}
			}
			return false;
		}
		// Find element by key, returns a pointer to value
		V* find(const K& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].key == key)
						return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}

		// Find element by key, returns a pointer to value
		V* find(const K&& key)
		{
			uint32_t iters = 0;
			while (iters < _maxIters)
			{
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iters);
				const uint32_t currentTable = iters % _tablesCount;

				if (_data[currentTable][hashedKey].occupied)
					if (_data[currentTable][hashedKey].key == key)
						return &_data[currentTable][hashedKey].value;
				iters++;
			}
			return nullptr;
		}
		// Insert element by key and value
		const bool insert(const K& key, const V& value)
		{
			return _CCKHT_insertData(key, value);
		}
		// Insert element by {key} and {value}
		const bool insert(const K&& key, const V&& value)
		{
			return _CCKHT_insertData(key, value);
		}
		// Insert element by key, value
		const bool insert(const K_V_pair& k_v_pair)
		{
			return _CCKHT_insertData(k_v_pair);
		}
		// Insert element by {key, value}
		const bool insert(const K_V_pair&& k_v_pair)
		{
			return _CCKHT_insertData(k_v_pair);
		}
		// Insert element by {keys, values, ....}
		std::vector<bool> insert(std::initializer_list<K_V_pair> l) {
			std::vector<bool> results(l.size(), false);
			uint32_t iter = 0;
			for (auto element : l)
			{
				results[iter] = _CCKHT_insertData(element);
				iter++;
			}
			return results;
		}

		// Get internal container (std::vector<std::vector<TableSlot<K,V>>)
		const std::vector<std::vector<TableSlot>>& rawData()
		{
			return _data;
		}
		// Return tables count
		const uint32_t tablesCount()
		{
			return _tablesCount;
		}
		// Return capacity 
		const uint32_t capacity()
		{
			return _capacity;
		}
		// Return capacity * tables count
		const uint32_t totalCapacity()
		{
			return _capacity * _tablesCount;
		}

		// Find element by [key]
		V* operator [](const K& key)
		{
			return find(key);
		}

		// Find element by [key]
		V* operator [](const K&& key)
		{
			return find(key);
		}
		// Calculate load factor
		const double loadFactor()
		{
			uint32_t result = 0;
			for (auto table : _data)
			{
				for (auto slot : table)
					result += slot.occupied;
			}
			return (double)((double)result / (double)totalCapacity());
		}
		// Check if map contains value on [key]
		const bool contains(const K& key)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].key == key && _data[currentTable][hashedKey].occupied)
				{
					return true;
				}
			}
			return false;
		}
		// Check if map contains value on [key]
		const bool contains(const K&& key)
		{
			for (uint32_t iterations = 0; iterations < _tablesCount; iterations++)
			{
				const uint32_t currentTable = iterations % _tablesCount;
				const uint32_t hashedKey = _g_CCKHT_l_hashFunction(key, _capacity, _tablesCount, iterations);

				if (_data[currentTable][hashedKey].key == key && _data[currentTable][hashedKey].occupied)
				{
					return true;
				}
			}
			return false;
		}
		// Return count of values on [key]
		const int count(const K& key)
		{
			return contains(key);
		}
		// Return count of values on [key]
		const int count(const K&& key)
		{
			return contains(key);
		}
	};
}
#endif // !HARZ_CCKHASH