#include "Random.h"

ok::Random::Random() : linear_normalized_dis(0.0, std::nextafter(1.0, DBL_MAX))
{
}

void ok::Random::Seed(unsigned int seed)
{
	gen.seed(seed);
}

void ok::Random::Seed()
{
	std::random_device rd;
	gen.seed(rd());
}

float ok::Random::Value()
{
	return static_cast<float>(linear_normalized_dis(gen));
}

float ok::Random::Value(float range_begin, float range_end)
{
	return range_begin + Value()*(range_end - range_begin);
}

int ok::Random::Value(int range_begin, int range_end)
{
	return range_begin + static_cast<int>(glm::round(Value()*static_cast<float>(range_end - range_begin)));
}
