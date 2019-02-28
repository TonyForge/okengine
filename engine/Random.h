#pragma once

#include <random>

namespace ok
{
	class Random
	{
	public:
		Random();
		void Seed(unsigned int seed);
		void Seed();
		//[0..1]
		float Value();
		//[range_begin..range_end]
		float Value(float range_begin, float range_end);
		int Value(int range_begin, int range_end);
	private:
		std::mt19937 gen;
		std::uniform_real_distribution<> linear_normalized_dis;
	protected:

	};
}