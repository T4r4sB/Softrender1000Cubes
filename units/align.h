#pragma once
#include "stdint.h"
namespace tblib 
{
	template <typename T>
	struct CheckAlign_
	{
		uint8_t c;
		T t;
		CheckAlign_();
		CheckAlign_(const CheckAlign_&);
		CheckAlign_&operator=(const CheckAlign_&);
	};

	template <typename T>
	struct AlignValue_
	{
		enum { value = sizeof(CheckAlign_<T>)-sizeof(T) };
	};

	template <int N>
	struct AlignBySize_;

	template <> 
	struct AlignBySize_<1> { uint8_t  guts; };

	template <> 
	struct AlignBySize_<2> { uint16_t guts; };

	template <> 
	struct AlignBySize_<4> { uint32_t guts; };

	template <> 
	struct AlignBySize_<8> { uint64_t guts; };

	template <typename T>
	struct Align
	{
		AlignBySize_<AlignValue_<T>::value> guts;
	};
};

