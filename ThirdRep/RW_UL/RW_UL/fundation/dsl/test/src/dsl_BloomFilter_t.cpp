#include"dsl_BloomFilter.hpp"

#include"dsl_pch_t.h"
#include<chrono>

using namespace rw::dsl;

std::string generateRandomString(size_t length) {
	const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, characters.size() - 1);

	std::string randomString;
	for (size_t i = 0; i < length; ++i) {
		randomString += characters[distribution(generator)];
	}

	return randomString;
}

namespace dsl_BLoomFilter {
	TEST(DSL, BLOOMFILTER_Class) {
		//Randomly generate maximum error rate and estimated storage quantity
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_real_distribution<> errorRateDist(0.01, 0.1); //The error rate is between 0.01 and 0.1
		std::uniform_int_distribution<> storageSizeDist(10, 1000); //Estimated storage quantity between 10 and 1000

		double maxTolerance = errorRateDist(generator);
		size_t estimatedStorageSize = storageSizeDist(generator);

		//Create BloomFilter Refactor instance
		BloomFilter bf(estimatedStorageSize, maxTolerance);

		//Randomly generate stored keys
		size_t numKeys = estimatedStorageSize;
		std::vector<std::string> keys;
		for (size_t i = 0; i < numKeys; ++i) {
			keys.push_back(generateRandomString(10)); //Generate a random string of length 10
		}

		//Insert key into BloomFilter
		for (const auto& key : keys) {
			bf.insert(key);
		}

		//Check error rate
		size_t falsePositives = 0;
		size_t numChecks = 1000; //Check 1000 random strings
		for (size_t i = 0; i < numChecks; ++i) {
			std::string randomKey = generateRandomString(10);
			if (std::find(keys.begin(), keys.end(), randomKey) == keys.end() && bf.contains(randomKey)) {
				++falsePositives;
			}
		}

		double actualErrorRate = static_cast<double>(falsePositives) / numChecks;
		double errorDifference = actualErrorRate - maxTolerance;
		std::cout << "Max Tolerance: " << maxTolerance << ", Actual Error Rate: " << actualErrorRate << ", Error Difference: " << errorDifference << std::endl;

		//Declare that the actual error rate is not greater than the maximum error rate and output relevant information in case of failure
		ASSERT_LE(actualErrorRate, maxTolerance + 0.035) << "Actual Error Rate: " << actualErrorRate << ", Max Tolerance: " << maxTolerance << ", Error Difference: " << errorDifference;
	}

	//This test case is used to iteratively calculate the error range of the false positive rate of BloomFilter Refactor (i.e. given an input false positive rate of 0.1, it may actually fluctuate around 0.1)
	//Perform 1000 iterations of testing, with 1000 tests per iteration. If all 1000 tests pass, the false positive rate is considered acceptable
	//If one out of 1000 tests fails, increase the false positive rate by 0.001 and continue testing
	//The final result should be between 0.01 and 0.035
	//To improve the testing efficiency of other test cases, the current adaptiveCount is set to 1. When testing this test case, please set the adaptiveCount. When the reference result is 0.035, the adaptiveCount value is 1000
	TEST(DSL, BLOOMFILTER_REFACTOR_ADAPTIVETEST) {
		std::locale::global(std::locale("en_US.UTF-8"));
		std::wcout.imbue(std::locale());
		int adaptiveCount{ 1 };//Number of iterations
		double ErrorRate{ 0.01 }; //Initial error rate
		double adptiveDifferenceRateStep{ 0.001 }; //Incremental error rate

		//Randomly generate maximum error rate and estimated storage quantity
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_real_distribution<> errorRateDist(0.01, 0.1); //The error rate is between 0.01 and 0.1
		std::uniform_int_distribution<> storageSizeDist(100, 10000); //Estimated storage quantity between 1000 and 10000

		bool result{ false };
		int testCount{ 0 }; //Test frequency
		while (!result) {
			auto start = std::chrono::high_resolution_clock::now(); //Record start time
			std::cout << "迭代组序号: " << testCount;
			std::cout << " 当前设定测量误差值: " << ErrorRate << " 最大迭代次数:" << adaptiveCount << std::endl;
			int passcount = 0;

			double theLastErrorRateDifference;
			for (int i = 0; i < adaptiveCount; i++) {
				double maxTolerance = errorRateDist(generator);
				size_t estimatedStorageSize = storageSizeDist(generator);

				//Create BloomFilter Refactor instance
				BloomFilter bf(estimatedStorageSize, maxTolerance);

				//Randomly generate stored keys
				size_t numKeys = estimatedStorageSize;
				std::vector<std::string> keys;
				for (size_t i = 0; i < numKeys; ++i) {
					keys.push_back(generateRandomString(10)); //Generate a random string of length 10
				}

				//Insert key into BloomFilter
				for (const auto& key : keys) {
					bf.insert(key);
				}

				//Check error rate
				size_t falsePositives = 0;
				size_t numChecks = 1000; //Check 1000 random strings
				for (size_t i = 0; i < numChecks; ++i) {
					std::string randomKey = generateRandomString(10);
					if (std::find(keys.begin(), keys.end(), randomKey) == keys.end() && bf.contains(randomKey)) {
						++falsePositives;
					}
				}

				double actualErrorRate = static_cast<double>(falsePositives) / numChecks;
				double errorDifference = actualErrorRate - maxTolerance;

				if (actualErrorRate <= maxTolerance) {
					passcount++;
				}
				else {
					if (errorDifference < ErrorRate) {
						passcount++;
					}
					else {
						auto end = std::chrono::high_resolution_clock::now(); //Record end time
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); //Calculate the duration and convert it to milliseconds

						std::cout << "迭代次数： " << i;
						std::cout << " 输入假阳性率: " << maxTolerance << " 输入预估存储量: " << estimatedStorageSize << ", 实际假阳性率: " << actualErrorRate << ", 误差: " << errorDifference;
						std::cout << " 耗时: " << duration.count() << "ms" << std::endl;
						theLastErrorRateDifference = errorDifference;
						break;
					}
				}
			}

			if (passcount == adaptiveCount) {
				result = true;
			}
			else {
				if (ErrorRate < theLastErrorRateDifference) {
					ErrorRate = theLastErrorRateDifference + adptiveDifferenceRateStep;
				}
				ErrorRate += adptiveDifferenceRateStep;
			}

			testCount++;
		}

		std::cout << "假阳性率 " << ErrorRate << std::endl;

		ASSERT_LE(ErrorRate, 0.04);
	}
}