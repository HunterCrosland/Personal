#ifndef MESSENGER_H
#define MESSENGER_H

#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <iomanip>
#include <mutex>


class messenger
{
private:
	// string obtained from c++ web messenger and boolean for checking if str changed
	std::string msg;
	bool msgChanged;

public:

	/**
	default constructor, sets msgChanged to false, creates msg_reader and msg_reader threads
	and detaches them
	*/
	messenger();

	
	// destructor is just default
	~messenger();

	/**
	establishes a thread-local file stream and string to set msg variable in messenger class
	also changes msgChanged via set_bool function and removes to_cpp.txt 
	*/
	void set_msg();

	// checks msgChanged via get_bool and prints msg if msgChanged == true	
	void print_msg();

	/**
	allows threads to change msgChanged with mutex locking to eliminate race conditions 
	(potential conflict with print_msg and set_msg without mutex locks here)
	@param _bool is the desired boolean to which we set msgChanged
	*/
	void set_bool(const bool _bool);

	/**
	allows threads to read msgChanged with mutex locking 
	(really this mutex locking is redundant, but I wanted consistency with get and set)
	@return boolean corresponding to value of msgChanged
	*/
	const bool get_bool() const;

	/**
	returns msg
	@return msg string in messenger class
	*/
	const std::string get_msg() const;


};

#endif
