#include "bst.h"

#include<iostream>
#include<string>
#include<algorithm>

/**
function compares reversed strings!
@param s1 the first string
@param s2 the second string
@return if s1 reversed < s2 reversed
*/
bool rev_str(std::string s1, std::string s2) {
	std::reverse(s1.begin(), s1.end());
	std::reverse(s2.begin(), s2.end());
	return s1 < s2;
}

int main() {
	// standard case
	pic10c::bst<int> b1;
	b1.insert(3);
	b1.insert(4);
	b1.insert(0);

	// print the values: requires proper iterators for range-for
 	std::cout << "b1:\n";
	for (const auto& i : b1) {
 		std::cout << i << '\n';
	}

	pic10c::bst<std::string, bool (*)(std::string, std::string)> b2(rev_str);

	b2.emplace(3, 'a');
	b2.insert("aaa"); // duplicate!
	b2.insert("zzzzzzzza");
	b2.insert("aaaaaaaaz");
	b2.emplace(6, 'm');

	auto b3 = b2;

	std::cout << "b3:\n";
	for (const auto& s : b3) { // print b3, copy of b2
		std::cout << s << '\n';
	}

	auto b4 = std::move(b3); // move construct b4 from b3...
	std::cout << "size b3 " << b3.size() << '\n';
	std::cout << "size b4 " << b4.size() << '\n';

	std::cout << "iterator stuff:\n";
	pic10c::bst<std::string, decltype(&rev_str)>::iterator it =
		b4.begin();
	std::cout << *it << '\n';
	std::cout << it->size() << '\n';
	b4.erase(it);

	std::cout << "b4:\n";
	for (const auto& s : b4) { // no more "aaa"
		std::cout << s << '\n';
	}

	return 0;
}