#ifndef SIMPLESET_H
#define SIMPLESET_H

#include <vector>


template<typename T>
class simple_set {
private:
	std::vector<T> data;
public:
	simple_set() : data(std::vector<T>()) {}
	~simple_set() {
		std::vector<T>().swap(data);
	}
	void insert(const T& new_data) {
		bool found = false;
		for (const T check : data) {
			if(check == new_data) {
				found = true;
			}
		}
		if(!found) { data.push_back(new_data); }
	}
	void erase(const T& delete_data) {
		for (std::vector<T>::iterator it; it != data.end(); ++it) {
			if (data[it] == delete_data) { data.erase(it); }
		}
	}
	bool find(const T& find_data) {
		bool found = false;
		for (const T check : data) {
			if (check == find_data) {
				found = true;
			}
		}
		if (found) { return found; }
	}
	size_t size() {
		return data.size();
	}
};

template<typename T>
class simple_set<T*> {
private:
	std::vector<T*> data;
public:
	simple_set() : data(std::vector<T*>()) {}
	~simple_set() {
		std::vector<T*>().swap(data);
	}
	void insert(const T* new_data) {
		bool found = false;
		for (const T* check : data) {
			if (*check == *new_data) {
				found = true;
			}
		}
		if (!found) { data.push_back(new_data); }
	}
	void erase(const T& delete_data) {
		for (std::vector<T*>::iterator it; it != data.end(); ++it) {
			if (*(data[it]) == delete_data) { data.erase(it); }
		}
		
	}

	bool find(const T& find_data) {
		bool found = false;
		for (const T* check : data) {
			if (*check == find_data) {
				found = true;
			}
		}
		return found;
	}
	size_t size() {
		return data.size();
	}
};

template<>
class simple_set<bool> {
private:
	std::vector<bool> data;
public:
	simple_set() : data(std::vector<bool>()) {}
	~simple_set() {
		std::vector<bool>().swap(data);
	}
	void insert(bool new_data) {
		bool found = false;
		for (size_t i = 0; i < data.size(); ++i) {
			if (new_data == data[i]) {
				data.erase(i);
				return;
			}
		}
	}

	void erase(const bool delete_data) {
		for (std::vector<bool>::iterator it; it != data.end(); ++it) {
			if (data[it]) == delete_data) { data.erase(it); }
		}

	}
	bool find(const bool find_data) {
		bool found = false;
		for (const bool check : data) {
			if (check == find_data) {
				found = true;
			}
		}
		if (found) { return found; }
	}
	size_t size() {
		return data.size();
	}
};


#endif