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