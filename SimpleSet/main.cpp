#include <iostream>
#include <string_view>
#include "SimpleSet.h"

int main() {

	constexpr std::string_view line("---------------\n");
	pic10c::simple_set<float> floats;
	floats.insert(2.2f);
	floats.insert(4.4f);
	floats.insert(4.4f);
	floats.insert(-0.98333f);
	std::cout << std::boolalpha;
	std::cout << "floats:\n";
	std::cout << floats.find(4.4f) << '\n';
	std::cout << floats.find(19.f) << '\n';
	std::cout << floats.size() << '\n';
	std::cout << line;
	pic10c::simple_set<int*> ints;
	int i1 = 1, i2 = 2, i3 = 3;
	int j1 = 1, j2 = 2;
	ints.insert(&i1);
	ints.insert(&j1);
	ints.insert(&i2);
	ints.insert(&j2);
	ints.insert(&i3);
	ints.erase(1);
	std::cout << "ints:\n";
	std::cout << ints.find(1) << '\n';
	std::cout << ints.find(3) << '\n';
	std::cout << ints.size() << '\n';
	std::cout << line;
	pic10c::simple_set<bool> bools;
	bools.insert(true);
	bools.insert(true);
	bools.insert(false);
	bools.erase(true);
	std::cout << "bools:\n";
	std::cout << bools.find(true) << '\n';
	std::cout << bools.find(false) << '\n';
	std::cout << bools.size() << '\n';
	std::cout << line;
	return 0;
}
