#include "max_heap.h"

#include <string>
#include <iostream>

int main() {
	// plain vanilla max heap, does things by <
	pic10c::max_heap<int> m_less;
	m_less.insert(3);
	m_less.insert(4);
	m_less.insert(5);
	m_less.insert(0);

	std::cout << "top of m_less: " << m_less.top() << '\n';
	m_less.pop();
	std::cout << "top of m_less: " << m_less.top() << '\n';

	// more fancy, replaces < with > so sorting is reversed
	pic10c::max_heap<int, std::greater<int>> m_great;
	m_great.insert(3);
	m_great.insert(4);
	m_great.insert(5);
	m_great.insert(0);

	std::cout << "top of m_great: " << m_great.top() << '\n';
	m_great.pop();
	std::cout << "top of m_great: " << m_great.top() << '\n';
	
	// lambda that will compare two strings by size
	// you'll need at least C++14 settings for this to compile!
	auto by_length = [](const auto& s1, const auto& s2) {
		return s1.size() < s2.size();
	};
	
	// this max heap will sort things by the length of the strings it stores
	pic10c::max_heap<std::string, decltype(by_length)> m_length{ by_length };
	m_length.insert("hello");
	m_length.emplace(10, 'c'); // the emplace function, creates std::string(10,c)
	m_length.emplace(); // the emplace function, creates std::string()
	
	std::cout << "top of m_length: " << m_length.top() << '\n';
	std::cout << "size of m_length: " << m_length.size() << '\n';
	
	pic10c::max_heap<int> uh_oh;
	try { // go ahead, try to do a pop
		uh_oh.pop();
	}
	catch (const std::logic_error & E) { // and we'll catch the exception if it comes up
		std::cout << E.what() << '\n';
	} 

	return 0; 
}