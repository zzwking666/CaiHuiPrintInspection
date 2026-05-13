#include "dsl_BloomFilter.hpp"

#include <functional>
#include <cmath>
namespace rw
{
	namespace dsl {
		BloomFilter::BloomFilter
		(size_t estimatedStorageSize, double maxTolerance)
			:_estimatedStorageSize(estimatedStorageSize), _maxTolerance(maxTolerance)
		{
			//Calculate the number of buffer bits for the Bloom filter and initialize the buffer
			auto size = getBloomFilterBitNum(estimatedStorageSize, _maxTolerance);
			size *= 1.3;//Increase the buffer size by 30% to reduce the false positive rate
			_bloomFilterBuffer.resize(size, false);

			//Calculate the number of hash functions and initialize the hash functions
			auto maxHashFunctions = static_cast<double>(size) / static_cast<double>(estimatedStorageSize) * (cln2);
			_maxHashFunctions = static_cast<size_t>(maxHashFunctions);
			iniHashFunctions(_maxHashFunctions, size);
		}

		void BloomFilter::iniHashFunctions(size_t numHashFunctions, size_t numBits)
		{
			//Initialize the hash function set using the formula
			// hi (key)=murmurmHash (key)+i * fnv1Hash (key)+i ^ 2% numBits
			for (size_t i = 0; i < numHashFunctions; ++i) {
				_hashFunctions.push_back([numBits, i, this](const std::string& str) {
					const uint32_t hash1 = _murmurHash(str.c_str());
					const uint32_t hash2 = _fnv1Hash(str);

					return (hash1 + i * hash2 + i * i) % numBits;
					});
			}
		}

		void BloomFilter::insert(const std::string& key)
		{
			for (auto& hashFunction : _hashFunctions) {
				const auto index = hashFunction(key);
				_bloomFilterBuffer[index] = true;
			}
		}

		bool BloomFilter::contains(const std::string& key)
		{
			for (auto& hashFunction : _hashFunctions) {
				auto index = hashFunction(key);
				if (!_bloomFilterBuffer[index]) {
					return false;
				}
			}
			return true;
		}
	}
}