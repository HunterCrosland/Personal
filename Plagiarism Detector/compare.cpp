#include "compare.h"

namespace fs = std::filesystem;

compare::compare(fs::path _root_dir) : root_dir(_root_dir) {};

/**
Initializes paths_for_comparison with root_dir
*/
void compare::init_dir_vector() { 
	// range-based for loop using fs:: built in recursive_dir_iterator
	for (const auto& dirEntry : fs::recursive_directory_iterator(root_dir)) {
		//uses is_regular_file to find whether path is a dir or file
		if (dirEntry.is_regular_file()) { paths_for_comparison.push_back(dirEntry); }
	}
}

/**
Generates text vectors (strings, essentially) from a given path
@param _path is the directory at which our file for conversion lies
@return is a vector of chars for comparison
*/
std::vector<char> compare::generate_text_vector(const fs::path _path) {
	std::ifstream f;
	//opens file at path from vector of paths
	f.open(_path, std::ios_base::in);
	std::vector<char> forReturn;
	char tmp;
	//reads from file into tmp and pushes to a char vector for return
	while (f >> std::noskipws >> tmp) { forReturn.push_back(tmp); }
	//closes and clears filestream
	f.close();
	f.clear();
	return forReturn;
}

/**
Generates matrix of booleans corresponding to character matches, called by
@generate_match_matrix
@param compare1 is the first vector of chars for comparison
@param compare2 is the second vector of chars for comparison
@return is a matrix of booleans for comparison in match matrix function
*/
std::vector<std::vector<bool>> compare::generate_bool_matrix(const std::vector<char>* compare1, const std::vector<char>* compare2) {
	std::vector< std::vector<bool> > forReturn;

	// checks each char in compare 1
	for (char c : *compare1) {
		std::vector<bool> tmp;
	// checks each char in compare 2
		for (size_t i = 0; i < compare2->size(); ++i) {
			tmp.push_back(c == (*compare2)[i]);
		}
	forReturn.push_back(tmp);
	}
	return forReturn;
}

/**
Generates matrix of ints corresponding to longest subsequence of chars at a given index
@param compare1 is the first vector of chars for comparison
@param compare2 is the second vector of chars for comparison
@return is a matrix of ints for reading subsequence into an index vector
*/
std::vector<std::vector<int>> compare::generate_match_matrix(const std::vector<char>* compare1, const std::vector<char>* compare2) {
	
	//calls @generate_bool_matrix to get binary matches, which we will convert into ints for subsequence
	std::vector<std::vector<bool>> bool_vecs = generate_bool_matrix(compare1, compare2);
	//inits 0 vector
	std::vector<std::vector<int>> for_return(compare1->size(), std::vector<int>(compare2->size(),0));

	// generates 1 or 0 on all indices of first row
	for (size_t i = 0; i < compare1->size(); ++i) {
		//if bool_vecs shows no match in index
		if (bool_vecs[i][0] == false) { 
			// if we are in top right corner
			if (i == 0) { for_return[0][0] = 0; } // init 0 in index
			else { for_return[i][0] = for_return[i - 1][0]; }
		}
		//bool_vecs shows match in index
		else { 
			// if we are in top right corner
			if (i == 0) { for_return[0][0] = 1; } //init 1 in index
			else { for_return[i][0] = std::max(for_return[i - 1][0], 1); }
		}
	}
	// generates 1 or 0 on all indices of first column, following first row
	for (size_t j = 1; j < compare2->size(); ++j) {
		//if bool_vecs shows no match in index
		if (bool_vecs[0][j] == false) {
			for_return[0][j] = for_return[0][j-1]; // init previous index
		}
		//bool_vecs shows match in index
		else { for_return[0][j] = std::max(for_return[0][j-1], 1); } // init previous index or 1
	}

	// walks all rows and columns and increments values based on bool_vecs matches
	for (size_t i = 1; i < compare1->size(); ++i) {
		for (size_t j = 1; j < compare2->size(); ++j) {
			//if bool_vecs shows no match in index, match left or up 
			if (bool_vecs[i][j] == false) { for_return[i][j] = std::max(for_return[i][j - 1], for_return[i - 1][j]); }
			//if bool_vecs shows match in index, increment left or up
			else { for_return[i][j] = 1 + for_return[i-1][j-1]; }
		}
	}

	//returns subsequence int matrix
	return for_return;
}

/**
Reads match_vecs to find longest common subsequence, and finds indices that get printed from the longest_word
and prints both the longest common subsequence and the length of that subsequence
@param match_vecs is the return from @generate_match_matrix
@param longest_word is the longer of the two comparison char vectors
*/
void compare::make_comparison(std::vector<std::vector<int>>* match_vecs, const std::vector<char>* longest_word) {
	//coordinates for bottom left corner of matrix
	size_t last_i = match_vecs->size() - 1;
	size_t last_j = (*match_vecs)[0].size() - 1;
	
	std::vector<size_t> indices;
	//unneccesary, but wanted to de-ambiguify name
	size_t i = last_i;
	size_t j = last_j;

	//while we are not in top-left or out-of-range
	while (i > 0 && j > 0) {
		//if index matches up index
		if ((*match_vecs)[i][j] == (*match_vecs)[i-1][j]) { --i; } // move up
		//if index matches left index
		else if ((*match_vecs)[i][j] == (*match_vecs)[i][j-1]) { --j; } // move left
		//else move diagonal and push back index
		else { indices.push_back(i); --i; --j; }
	}
	// check top left
	if ((*match_vecs)[0][0] > 0) { indices.push_back(0); }
	//reverse index matrix so we start from beginning
	std::reverse(indices.begin(), indices.end());

	//print subs len
	std::cout << "Common Subsequence Length: " << indices.size() << "\n";
	std::cout << "Overlap: \n";

	//print overlap based on index vector
	for (size_t i : indices) { std::cout << (*longest_word)[i]; }
}

/**
Compares vectors and returns longer vector
@param vec1 is arbitrary first char vector
@param vec2 is arbitrary second char vector
@return is the longer vector of two params
*/
const std::vector<char>* compare::longer_vector(const std::vector<char>* vec1, const std::vector<char>* vec2) {
	if (vec1->size() > vec2->size()) { return vec1; }
	else if (vec1->size() == vec2->size()) { return vec1; }
	else { return vec2; }
}

/**
Compares vectors and returns shorter vector
@param vec1 is arbitrary first char vector
@param vec2 is arbitrary second char vector
@return is the shorter vector of two params
*/
const std::vector<char>* compare::shorter_vector(const std::vector<char>* vec1, const std::vector<char>* vec2) {
	if (vec1->size() < vec2->size()) { return vec1; }
	else if (vec1->size() == vec2->size()) { return vec2; }
	else { return vec2; }
}

/**
Calls above functions in correct sequence as helper function, does some intermediary stuff
*/
void compare::call_funcs() {
	init_dir_vector();
	for (const auto& dirEntry : paths_for_comparison) { std::cout << dirEntry << "\n"; }
	std::vector<std::pair<fs::path, fs::path>> pairs_of_paths;
	// uses simple logic to pair all paths for comparison using <utility> header
	for (size_t i = 0; i < paths_for_comparison.size(); ++i) {
		for (size_t j = i + 1; j < paths_for_comparison.size(); ++j) {
			std::pair<fs::path, fs::path> tmp;
			tmp.first = paths_for_comparison[i];
			tmp.second = paths_for_comparison[j];
			pairs_of_paths.push_back(tmp);
		}
	}
	// pairing submissions notation, uses path.string() to make sure we cut at right part of path
	for (std::pair<fs::path, fs::path> _pair : pairs_of_paths) {
		std::cout << "Pairing submissions" << 
			_pair.first.string().substr(root_dir.string().size(), _pair.first.string().size()-1) 
			<< "-" << _pair.second.string().substr(root_dir.string().size(), _pair.second.string().size() - 1)
			<< "\n";

		//generates text vector
		const std::vector<char> str1 = generate_text_vector(_pair.first);
		const std::vector<char> str2 = generate_text_vector(_pair.second);

		//finds longer and shorter vectors to make sure we pass in correct ones
		const std::vector<char>* longer = longer_vector(&str1, &str2);
		const std::vector<char>* shorter = shorter_vector(&str1, &str2);

		//generates int matches
		std::vector<std::vector<int>> match_matrix = generate_match_matrix(longer, shorter);

		//prints comparison and line denoting end of comparison
		make_comparison(&match_matrix, longer);
		std::cout << "\n ----------------- \n";
	}
}
  

