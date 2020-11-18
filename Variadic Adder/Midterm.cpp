#include <iostream>

/*
returns value if passed only one argument
@tparam first is an arbitrary type value for return
@return is a value of type T of the value first
*/
template<typename T>
auto var_sum(const T first) {
	return first;
}

/*
sums values, with safety typename F if first two args are different types

@tparam first is an arbitrary type value 
@tparam second is a value of a (potentially) different type from first 
@tparam ...args is a parameter pack
@return is the sum of first, second, and all subsequent args
*/
template<typename T, typename F, typename... args>
auto var_sum(const T first, const F second, args&&... arg1) {
	return first + var_sum(second, arg1...);
}


int main() {
	std::cout << var_sum(1) << '\n';
	std::cout << var_sum(1, 2.5, 3u, 4, 4, 3.2f) << '\n';
	int x = 1;
	double y = 2.5111;
	float z = 3;
	std::string hello = "hello";
	std::string hi = "hi";
	char c = 'c';
	std::cout << var_sum(x,1,2,3,4,5,5u,y,z) << '\n';
	std::cout << var_sum(hello, c, hi) << '\n';
	return 0;
}
