#ifndef FINAL_TUPLE_H
#define FINAL_TUPLE_H

#include<iostream>

//base struct for inheritance
template<typename ... Types>
struct tuple;

//inherits base struct and specializes for variadic parameters
template<typename T, typename ... Types>
struct tuple<T, Types...> : tuple<Types...> {
    using base = tuple<Types...>; //for forwarding

    constexpr tuple() : value() {} //default constructor for empty Types...
    tuple(const tuple&) = default; //const tuple copy constructor
    tuple(tuple&&) = default; //tuple move constructor

    //parameter pack constructor; assigns value to first
    //and forwards rest into base constructor, recursively
    //continues until it reaches default
    constexpr tuple(T&& first, Types&&... rest) : base(std::forward<Types>(rest)...), value(first) {}

    //value storage
    T value;
};

//empty tuple 
template<>
struct tuple<> {
    tuple() = default;
};

//non-const tuple helper struct for get when i!=0
template<size_t i, typename T, typename ... Types>
struct tuple_helper {
    /**
    * Recursive tuple unpacker, recurses until desired i is reached
    * Static so that initialization of tuple_helper is unnecessary
    * @param t is a templated tuple 
    * @return the Types... value at index i forwarded from the stack trace
    */
    constexpr static auto& get(tuple<T, Types...>& t) {
        //will continue to recurse until i==0
        return tuple_helper<i - 1, Types...>::get(
            static_cast<tuple<Types...>&>(t));
    }
};

//non-const tuple helper struct for get when desired 
//index has been reached
template<typename T, typename ... Types>
struct tuple_helper<0, T, Types...> {
    /**
    * Returns T value from desired index
    * Static so that initialization of tuple_helper is unnecessary
    * @param t is a templated tuple
    * @return is a T corresponding to the tuple at index i or 0,
    * based on whether get was recursively called or not
    */
    constexpr static T& get(tuple<T, Types...>& t) {
        return t.value;
    }
};

//const tuple helper struct for when i!= 0
template<size_t i, typename T, typename ... Types>
struct const_tuple_helper {
    /**
    * Recursive tuple unpacker, recurses until desired i is reached
    * Static so that initialization of const_tuple_helper is unnecessary
    * @param t is a templated tuple
    * @return the const Types... value at index i forwarded from the stack trace
    */
    constexpr static auto& get(const tuple<T, Types...>& t) {
        //will continue to recurse until i = 0
        return const_tuple_helper<i - 1, Types...>::get(
            static_cast<const tuple<Types...>&>(t));
    }
};

// totally legit, partial specialization is awesome
template<typename T, typename ... Types>
struct const_tuple_helper<0, T, Types...> {
    /**
    * Returns T value from desired index
    * Static so that initialization of const_tuple_helper is unnecessary
    * @param t is a templated tuple
    * @return is a const T corresponding to the tuple at index i or 0,
    * based on whether get was recursively called or not
    */
    constexpr static const T& get(const tuple<T, Types...>& t) {
        return t.value;
    }
};

/**
* Standard non-member get function for tuples which forwards to the
* recursive tuple_helper member function get()

* @param t is a templated tuple
* @return is a T corresponding to the tuple value at index i
*/
template<size_t i, typename T, typename ... Types>
constexpr auto& get(tuple<T, Types...>& t) {
    return tuple_helper<i, T, Types...>::get(t);
}

/**
* Standard non-member get function for const tuples which forwards
*  to the recursive const_tuple_helper member function get()
* 
* @param t is a templated tuple
* @return is a const T corresponding to the tuple value at index i
*/
template<size_t i, typename T, typename ... Types>
constexpr auto& get(const tuple<T, Types...>& t) {
    return const_tuple_helper<i, T, Types...>::get(t);
}


#endif // !FINAL_TUPLE_H
