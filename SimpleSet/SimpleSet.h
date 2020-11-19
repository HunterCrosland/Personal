#ifndef SIMPLESET_H
#define SIMPLESET_H

#include <vector>

namespace pic10c {

	template<typename T>
	class simple_set {

	private:

		std::vector<T> data;

	public:
		//default constructor which defaults data to empty vector
		simple_set() : data(std::vector<T>()) {}
		//destructor makes a swap with a newly-inititialized empty vector
		~simple_set() {
			std::vector<T>().swap(data);
		}

		/**
		Conditionally inserts new_data into class data storage vector if data does not exist in vector a priori
		@param new_data a const ref to a T value for insertion into class data storage vector
		*/
		void insert(const T& new_data) {
			bool found = false;
			// check if data exists in storage vector already and flag found if so
			for (const T& check : data) {
				if (check == new_data) {
					found = true;
				}
			}
			// if data not found, add to storage vector
			if (!found) { data.push_back(new_data); }
		}

		/**
		Loops over storage vector to erase a value specified in args, if value exists in storage vector
		@param delete_data a const ref to a T value for deletion from class data storage vector
		*/
		void erase(const T& delete_data) {
			for (auto it = data.begin(); it != data.end(); ++it) {
				if (*it == delete_data) { data.erase(it); }
			}
		}

		/**
		Loops over storage vector to find a value specified in args, returns true if found
		@param find_data a const ref to a T value for discovery in class data storage vector
		*/
		bool find(const T& find_data) {
			bool found = false;
			//check find_data against all data values in data 
			for (const T& check : data) {
				if (check == find_data) {
					found = true;
				}
			}
			return found;
		}

		/**
		Returns size of storage vector
		*/
		size_t size() {
			return data.size();
		}
	};

	template<typename T>
	class simple_set<T*> {
	private:
		std::vector<T*> data;
	public:
		//default constructor which defaults data to empty vector
		simple_set() : data(std::vector<T*>()) {}
		//destructor makes a swap with a newly-initialized empty vector
		~simple_set() {
			std::vector<T*>().swap(data);
		}

		/**
		Conditionally inserts new_data into class data storage vector if data does not exist in vector a priori
		@param new_data a const pointer to a T val for insertion into class data storage vector
		*/
		void insert(T *const new_data) {
			bool found = false;
			for (const T* check : data) {
				if (*check == *new_data) {
					found = true;
				}
			}
			if (!found) { data.push_back(new_data); }
		}

		/**
		Loops over storage vector to erase a value specified in args, if value exists in storage vector
		@param delete_data a const ref to a T value for deletion from class data storage vector
		*/
		void erase(const T& delete_data) {
			for (auto it = data.begin(); it != data.end(); ++it) {
				if (*(*it) == delete_data) { data.erase(it); break; }
			}
		}

		/**
		Loops over storage vector to find a value specified in args, returns true if found
		@param find_data a const ref to a T value for discovery in class data storage vector
		*/
		bool find(const T& find_data) {
			bool found = false;
			for (const T* check : data) {
				if (*check == find_data) {
					found = true;
				}
			}
			return found;
		}

		/**
		Returns size of storage vector
		*/
		size_t size() {
			return data.size();
		}
	};

	template<>
	class simple_set<bool> {
	private:

		std::vector<bool> data;

	public:
		//default constructor which defaults data to empty vector
		simple_set() : data(std::vector<bool>()) {}
		//destructor makes a swap with a newly-initialized empty vector
		~simple_set() {
			std::vector<bool>().swap(data);
		}

		/**
		Conditionally inserts new_data into class data storage vector if data does not exist in vector a priori
			If data does exist, deletes existing data
		@param new_data a const ref to a bool value for insertion into class data storage vector
		*/
		void insert(const bool& new_data) {
			for (auto it = data.begin(); it != data.end(); ++it) {
				if (*it == new_data) { data.erase(it); }
				return;
			}
			data.push_back(new_data);
		}

		/**
		Loops over storage vector to erase a value specified in args, if value exists in storage vector
		@param delete_data a const ref to a bool value for deletion from class data storage vector
		*/
		void erase(const bool& delete_data) {
			for (auto it = data.begin(); it != data.end(); ++it) {
				if (*it == delete_data) { data.erase(it); }
			}

		}

		/**
		Loops over storage vector to find a value specified in args, returns true if found
		@param find_data a const ref to a bool value for discovery in class data storage vector
		*/
		bool find(const bool& find_data) {
			bool found = false;
			for (const bool check : data) {
				if (check == find_data) {
					found = true;
				}
			}
			return found;
		}

		/**
		Returns size of storage vector
		*/
		size_t size() {
			return data.size();
		}
	};

}


#endif