#ifndef DSL_BLOOMFILTERREFACTOR_H_
#define DSL_BLOOMFILTERREFACTOR_H_

#include"dsl_core.hpp"

#include<vector>
#include <cmath>
#include<functional>
#include<string>

namespace rw {
	namespace dsl {
		inline uint32_t MurmurHash3(const void* key, int len, uint32_t seed);
		inline uint32_t Fnv1Hash(const std::string& str);

		//BloomFilter: Bloom filter
		//          The false positive rate of the input will have an error of 0-0.035 compared to the actual value
		class BloomFilter {
			static constexpr double cln2 = 0.6931471805599453;

			static constexpr double cln2Square = 0.4804530139182014;

		public:
			//Parameters:
			//      estimatedStorageSize: The expected number of elements to be stored
			//      maxTolerance: Maximum tolerable false positive rate (misjudgment rate)
			//
			//BloomFilter（） ：Initialize Bloom filter
			explicit BloomFilter(size_t estimatedStorageSize, double maxTolerance = 0.1);

			~BloomFilter() = default;

		public:
			//Parameters:
			//      key: Elements to be inserted
			//
			//insert（） ：Insert element
			void insert(const std::string& key);

			//Parameters:
			//      key: Elements to be queried
			//
			//Returns: bool
			//      When the element exists in the Bloom filter, return true; otherwise,  return false (resulting in false positives)
			//
			//contains（） ：Query elements
			bool contains(const std::string& key);

		private:
			//Parameters:
			//      estimatedStorageSize: Expected quantity of elements to be stored
			//      maxTolerance: Maximum tolerable false positive rate (misjudgment rate)
			//
			//Returns: size_t
			//      Return the number of buffer bits for the Bloom filter
			//
			//getBloomFilterBitNum（） ：Calculate the number of buffer bits for the Bloom filter
			//
			//Supplement
			//      The formula is m=- n * ln (p)/(ln (2) ^ 2), where m is the number of buffer bits in the Bloom filter, n is the expected number of inserted elements, and p is the false positive rate
			inline size_t getBloomFilterBitNum(size_t estimatedStorageSize, double maxTolerance) {
				const auto bloomFilterBitNum = -(estimatedStorageSize * log(maxTolerance) / (cln2Square));
				const size_t size = static_cast<size_t>(std::ceil(bloomFilterBitNum));
				return size;
			}

		private:
			std::function<uint32_t(const std::string&)> _murmurHash = [](const std::string& str) {
				return MurmurHash3(str.c_str(), str.size(), 0);
				};

			std::function<uint32_t(const std::string&)> _fnv1Hash = [this](const std::string& str) {
				return Fnv1Hash(str);
				};

			//Parameters:
			//      numHashFunctions: Number of hash functions
			//      numBits: Number of buffer bits in the Bloom filter
			//
			//iniHashFunctions: Initialize member variable hash function set_ hash functions
			//
			//Supplement
			//      The formula is hi(key) = murmurHash(key) + i * fnv1Hash(key) + i ^ 2 % numBits
			void iniHashFunctions(size_t numHashFunctions, size_t numBits);

		private:
			//Buffer bits of Bloom filter
			Vector<bool> _bloomFilterBuffer;

			//Expected number of elements to be stored
			size_t _estimatedStorageSize;

			//Maximum tolerable false positive rate (misjudgment rate)
			double _maxTolerance;

			//Number of hash functions
			size_t _maxHashFunctions;

			//Hash function set
			Vector<std::function<size_t(const std::string&)>> _hashFunctions;
		};

		//Parameters:
		//      key: Elements to be hashed
		//      len: Length of the element
		//      seed: Seed
		//
		// returns: uint32_t
		//      the hash value of the element
		//
		//MurmurHash3: MurmurHash3 hash function
		inline uint32_t MurmurHash3(const void* key, int len, uint32_t seed)
		{
			const auto data = static_cast<const uint8_t*>(key);
			const int nBlocks = len / 4;
			uint32_t h1 = seed;

			constexpr uint32_t c1 = 0xcc9e2d51;
			constexpr uint32_t c2 = 0x1b873593;

			// Body
			const auto blocks = reinterpret_cast<const uint32_t*>(data + nBlocks * 4);
			for (int i = -nBlocks; i; i++) {
				uint32_t k1 = blocks[i];

				k1 *= c1;
				k1 = (k1 << 15) | (k1 >> (32 - 15));
				k1 *= c2;

				h1 ^= k1;
				h1 = (h1 << 13) | (h1 >> (32 - 13));
				h1 = h1 * 5 + 0xe6546b64;
			}

			// Tail
			const auto tail = (const uint8_t*)(data + nBlocks * 4);
			uint32_t k1 = 0;

			switch (len & 3) {
			case 3: k1 ^= tail[2] << 16;
			case 2: k1 ^= tail[1] << 8;
			case 1: k1 ^= tail[0];
				k1 *= c1; k1 = (k1 << 15) | (k1 >> (32 - 15)); k1 *= c2; h1 ^= k1;
			};

			// Finalization
			h1 ^= len;
			h1 ^= h1 >> 16;
			h1 *= 0x85ebca6b;
			h1 ^= h1 >> 13;
			h1 *= 0xc2b2ae35;
			h1 ^= h1 >> 16;

			return h1;
		}

		//Parameters:
		//      str: String to be hashed
		//
		//returns: uint32_t
		//      the hash value of the string
		//
		//Fnv1Hash: FNV-1 hash function
		inline uint32_t Fnv1Hash(const std::string& str) {
			uint32_t hash = 2166136261;

			for (const char c : str) {
				constexpr uint32_t fnv_prime = 16777619;
				hash *= fnv_prime;
				hash ^= c;
			}

			return hash;
		}
	}
}

#endif // !DSL_BLOOMFILTERREFACTOR_H_