#include "Space.h"

void Zoner::Space::PassTime(float hours_passed)
{
	for (auto& visitor : visitors)
	{
		visitor->PassTime(hours_passed);
	}
}

void Zoner::Space::ApplyPassedTime()
{
	for (auto& visitor : visitors)
	{
		visitor->ApplyPassedTime();
	}
}
