#pragma once

namespace nam
{
	template<typename T>
	using Vector = std::vector<T>;

	template<typename T, std::size_t N>
	using Array = std::array<T, N>;

	template<typename T>
	using Deque = std::deque<T>;

	template<typename T>
	using List = std::list<T>;

	template<typename T>
	using FList = std::forward_list<T>;
}