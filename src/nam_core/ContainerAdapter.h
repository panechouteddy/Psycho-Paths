#pragma once

namespace nam
{
	template<typename T>
	using Stack = std::stack<T>;

	template<typename T>
	using Queue = std::queue<T>;

	template<typename T, typename Container = std::vector<T>, typename Compare = std::less<T>>
	using PrioQueue = std::priority_queue<T, Container, Compare>;
}