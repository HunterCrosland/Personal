/*******************************************************
 * Copyright (C) Hunter Crosland <hunter@croslands.com>
 *
 * This file is part of my work at UCLA.
 *
 * compare.h and compare.cpp can not be copied and/or 
 * distributed without the express permission of Hunter 
 * Crosland
 *******************************************************/

 /**
	The following, along with compare.cpp,
	is an implementation of a plagiarism detector
*/



#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <fstream>
#include <utility>

#ifndef COMPARE_H
#define COMPARE_H

/**
	@class compare
	@brief The compare class is a set of functions for comparing files at certain directories

	The bst class privately contains a pointer to a root_dir, passed in through the constructor, 
	and a vector of paths to files for comparison. Publicly, it contains functions that make 
	those comparisons.
*/
class compare
{
private:
	std::filesystem::path root_dir;
	std::vector<std::filesystem::path> paths_for_comparison;

public:
	//constructor, takes in path
	compare(std::filesystem::path);

	/**
	Initializes paths_for_comparison with root_dir
	*/
	void init_dir_vector();

	/**
	Generates text vectors (strings, essentially) from a given path
	*/
	std::vector<char> generate_text_vector(const std::filesystem::path);

	/**
	Generates matrix of booleans corresponding to character matches
	*/
	std::vector<std::vector<bool>> generate_bool_matrix(const std::vector<char>*, const std::vector<char>*);

	/**
	Generates matrix of integers corresponding to len of longest common subsequence
	*/
	std::vector<std::vector<int>> generate_match_matrix(const std::vector<char>*, const std::vector<char>*);

	/**
	Compares vectors and returns longer vector
	*/
	const std::vector<char>* longer_vector(const std::vector<char>*, const std::vector<char>*);

	/**
	Compares vectors and returns shorter vector
	*/
	const std::vector<char>* shorter_vector(const std::vector<char>*, const std::vector<char>*);

	/**
	Walks match matrix to find indices of characters corresponding to common subsequence
	*/
	void make_comparison(std::vector<std::vector<int>>*, const std::vector<char>*);

	/**
	Helper function to call functions
	*/
	void call_funcs();

};

#endif