#include "rbt.h"
#include "Timer.h"
#include<iostream>
#include<vector>
#include<string>

auto get_rbt() {
	pic10c::rbt<double, std::greater<double>> vals;
	vals.insert(3.3);
	vals.insert(1.1);
	vals.insert(4.4);
	vals.insert(5.3);
	vals.emplace(1.1); // duplicate
	vals.emplace(); // adds 0
	return vals;
}

int main() {
	pic10c::rbt<int> vals;
	for (int i = 0; i < 25; ++i) {
		vals.insert(i);
		vals.print();
	}
	auto p = vals.find(11);
	auto j = vals.find(52);
	auto k = vals.find(22);
	vals.erase(p);
	vals.print();
	vals.erase(j);
	vals.print();
	vals.erase(k);
	vals.print();




	// basic inserting, handling duplicates, etc.
	pic10c::rbt<std::string> colours;
	colours.insert("red");
	colours.insert("orange");
	colours.insert("yellow");
	colours.insert("green");
	colours.insert("blue");
	colours.insert("indigo");
	colours.insert("green"); // dupicate
	colours.insert("violet");
	std::cout << "colours size: " << colours.size() << '\n';
	// print the structure...
	std::cout << "colours current structure:\n";
	colours.print();
	// check find
	std::vector< pic10c::rbt<std::string>::iterator > places{ colours.find("red"),
	colours.find("cherry"), colours.find("green") };
	// green will be there... and it has
	std::cout << "green has " << places.back()->size() << " characters.\n";
	// try to erase all these
	for (const auto& p : places) {
		if (p != colours.end()) {
			colours.erase(p);
		}
	}
	std::cout << "colours new structure:\n";
	colours.print();
	// call a function to get rbt
	const auto doubles = get_rbt();
	// print the doubles
	std::cout << "printing the doubles:\n";
	for (const auto& d : doubles) {
		std::cout << d << '\n';
	}
	std::cout << "now we do some time trails...\n";
	pic10c::rbt<int> ints;
	simple_timer::timer<'u'> t;
	std::cout << "time each of 1000 inserations:\n";
	for (int i = 0; i < 1000; ++i) {
		t.tick();
		ints.insert(i);
		std::cout << t.tock() << ' ';
	}
	std::cout << '\n'; 
	std::cout << "time each of 1000 removals:\n";
	for (int i = 0; i < 1000; ++i) {
		t.tick();
 		auto p = ints.find(i);
		ints.erase(p);
		std::cout << t.tock() << ' ';
	}
	std::cout << '\n';
	return 0;
}

