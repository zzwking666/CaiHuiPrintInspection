#include"cla_utility_t.hpp"

#include "cla_utility.hpp"
#include <gtest/gtest.h>

#include"cryptopp/cryptlib.h"
#include"cryptopp/filters.h"
#include"cryptopp/modes.h"
#include"cryptopp/aes.h"
#include"cryptopp/hex.h"

namespace cla_ActivationBitsConvert
{
	// Binary to Hex and back
	TEST_F(ActivationBitsConvert_T, BinaryToHexAndBack)
	{
		for (int i = 0; i < 1000; i++)
		{
			generate();
			auto num = testObj;

			testObj = rw::cla::ActivationBitsConvert::switchBinaryTOHex(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchHexTOBinary(testObj);
			ASSERT_EQ(num, testObj);
		}
	}

	// Binary to Oct and back
	TEST_F(ActivationBitsConvert_T, BinaryToOctAndBack)
	{
		for (int i = 0; i < 1000; i++)
		{
			generate();
			auto num = testObj;

			testObj = rw::cla::ActivationBitsConvert::switchBinaryTOOct(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchOctTOBinary(testObj, num.size());
			ASSERT_EQ(num, testObj);
		}
	}

	// Binary to Hex to Oct to Binary
	TEST_F(ActivationBitsConvert_T, BinaryToHexToOctToBinary)
	{
		for (int i = 0; i < 1000; i++)
		{
			generate();
			auto num = testObj;

			testObj = rw::cla::ActivationBitsConvert::switchBinaryTOHex(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchHexTOOct(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchOctTOBinary(testObj, num.size());
			ASSERT_EQ(num, testObj);
		}
	}

	// Binary to Oct to Hex to Binary
	TEST_F(ActivationBitsConvert_T, BinaryToOctToHexToBinary)
	{
		for (int i = 0; i < 1000; i++)
		{
			generate();
			auto num = testObj;

			testObj = rw::cla::ActivationBitsConvert::switchBinaryTOOct(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchOctTOHex(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchHexTOBinary(testObj, num.size());
			ASSERT_EQ(num, testObj);
		}
	}

	// Binary to Hex to Oct to Hex to Oct to Binary
	TEST_F(ActivationBitsConvert_T, BinaryToHexToOctToHexToOctToBinary)
	{
		for (int i = 0; i < 1000; i++)
		{
			generate();
			auto num = testObj;

			testObj = rw::cla::ActivationBitsConvert::switchBinaryTOHex(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchHexTOOct(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchOctTOHex(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchHexTOOct(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchOctTOBinary(testObj, num.size());
			ASSERT_EQ(num, testObj);
		}
	}

	// Binary to Oct to Binary to Hex to Oct to Hex to Binary
	TEST_F(ActivationBitsConvert_T, BinaryToOctToBinaryToHexToOctToHexToBinary)
	{
		for (int i = 0; i < 1000; i++)
		{
			generate();
			auto num = testObj;

			testObj = rw::cla::ActivationBitsConvert::switchBinaryTOOct(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchOctTOBinary(testObj, num.size());
			testObj = rw::cla::ActivationBitsConvert::switchBinaryTOHex(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchHexTOOct(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchOctTOHex(testObj);
			testObj = rw::cla::ActivationBitsConvert::switchHexTOBinary(testObj, num.size());
			ASSERT_EQ(num, testObj);
		}
	}
}