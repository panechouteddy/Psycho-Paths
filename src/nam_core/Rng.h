#pragma once

namespace nam
{
	class Rng
	{
	private:
		static std::random_device ms_rd;
		static std::mt19937 ms_gen;
		static int ms_seed;

	public:
		static int Int(const int min, const int max); // min & max included
		static float Float(float min = 0.f, float max = 1.f);

		static void SetSeed(int seed = -1);

		static std::mt19937& GetGen();
	};
}