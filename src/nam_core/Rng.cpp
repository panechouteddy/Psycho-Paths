#include "pch.h"
#include "Rng.h"

namespace nam
{
	std::random_device Rng::ms_rd;
	std::mt19937 Rng::ms_gen(Rng::ms_rd());

	int Rng::Int(const int min, const int max)
	{
		std::uniform_int_distribution<int> val(min, max);
		return val(ms_gen);
	}

	float Rng::Float(float min, float max)
	{
		std::uniform_real_distribution<float> val(min, max);
		return val(ms_gen);
	}

	void Rng::SetSeed(int seed)
	{
		if (seed == -1)
			return;

		ms_gen.seed(seed);
	}

	std::mt19937& Rng::GetGen()
	{
		return ms_gen;
	}
}
