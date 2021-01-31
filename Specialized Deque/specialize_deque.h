#ifndef SPECIALIZE_DEQUE_H
#define SPECIALIZE_DEQUE_H

#include <memory>
#include <climits>
#include <stdexcept>
#include <iostream>

namespace pic10c {




	template<typename T>

	/**
		@class deque
		@brief The deque class creates an iterable functional double ended queue for arbitrary types. 
			It allows for back and front pushing, as well as retrieval from both ends. 
	*/
	class deque {
	public:
		//default constructor, sets cap to 1, size to 0
		deque() : data(std::make_unique<T[]>(1)), cap(1), sz(0),
		left(0), right(0){}

		//default deconstructor; since we are using smart pointers, no extra stuff is required here
		~deque() = default;

		//swap constructor
		deque(deque&& rhs) noexcept : deque() {
			
			std::swap(cap, rhs.cap);
			std::swap(sz, rhs.sz);
			std::swap(left, rhs.left);
			std::swap(right, rhs.right);

			swap(rhs.data);
		}

		//copy constructor
		deque(const deque& rhs) noexcept : deque() {
			cap = rhs.cap;
			sz = rhs.sz;
			std::unique_ptr<T[]> tmp = std::make_unique<T[]>(cap);
			data.swap(tmp);
			copy_vals(rhs);
			left = 0;
			right = sz - 1;
		}

		//copy operator
		deque& operator=(const deque& rhs) {
			cap = rhs.cap;
			sz = rhs.sz;
			std::unique_ptr<T[]> tmp = std::make_unique<T[]>(cap);
			data.swap(tmp);
			copy_vals(rhs);
			left = 0;
			right = sz - 1;
		}

		//move operator
		deque& operator=(deque&& rhs) {
			cap = std::move(rhs.cap);
			sz = std::move(rhs.sz);
			right = std::move(rhs.right);
			data  = std::move(rhs.data);
		}

		/**
		swaps data arrays between deques
		@param rhs is a deque with which @this will swap its values
		*/
		void swap(std::unique_ptr<T[]>& rhs) {
			this->data.swap(rhs);
		}

		/**
		allows for copies without destruction of data due to unique_ptr 
		@param rhs is a deque from which @this will copy its values
		*/
		void copy_vals(const deque& rhs) {
			auto f = this->begin();
			//want new deque to have ptr-friendly traversal to start
			for (auto i = rhs.begin(), e = rhs.end(); i != e; ++i) {
				(*f = *i);
				++f;
			}
		}

		/**
		places value corresponding to new_data in front of queue
		@param new_data is a T& corresponding to new value for deque
		*/
		void push_front(const T& new_data) {
			// if capacity is reached, we need to reallocate
			if (cap == sz) {
				// create tmp deque for val swapping
				deque tmp = *this;

				// double capacity, create new data array full of zeros
				cap *= 2;
				std::unique_ptr<T[]> tmp2 = std::make_unique<T[]>(cap);
				data.swap(tmp2);
				
				// due to copy behavior, new left is zero, new right is tmp.right
				left = 0;
				right = tmp.right;

				// copy vals as opposed to swapping to maintain capacity
				this->copy_vals(tmp);

				// place new value before old starting value, allows for wrapping
				iterator start = begin();
				start--;
				*start = new_data;

				// update left
				left = start.currIndex;

				sz += 1;
				return;
			}

			// if data is empty, simply place new value at begin
			else if (sz == 0) {
				
				iterator start = begin();
				*start = new_data;

				sz += 1;
				return;
			}

			// if capacity is not reached, place at one before start
			else {
				
				// place new value before old starting value, allows for wrapping
				iterator start = begin();
				start--;
				*start = new_data;

				// update left
				left = start.currIndex;

				sz += 1;
				return;
			}

		
		}

		/**
		places value corresponding to new_data in back of queue
		@param new_data is a T& corresponding to new value for deque
		*/
		void push_back(const T& new_data) {
			// if capacity is reached, we need to reallocate
			if (cap == sz) {

				// create tmp deque for val swapping
				deque tmp = *this;

				// double capacity, create new data array full of zeros
				cap *= 2;
				std::unique_ptr<T[]> tmp2 = std::make_unique<T[]>(cap);
				data.swap(tmp2);

				// due to copy behavior, new left is zero, new right is tmp.right
				left = 0;
				right = tmp.right;

				// copy vals as opposed to swapping to maintain capacity
				this->copy_vals(tmp);

				// place new value after old end value
				iterator end(right,this);
				end.currIndex += 1;
				*end = new_data;

				// update right
				right = end.currIndex;

				sz += 1;
				return;
			}

			// if data is empty, simply place new value at end
			else if (sz == 0) {

				iterator end(right,this);
				*end = new_data;

				sz += 1;
				return;
			}

			// if capacity is not reached, place at one after end
			else {

				// place new value after old end value
				iterator end(right,this);
				end.currIndex += 1;
				*end = new_data;

				// update right
				right = end.currIndex;

				sz += 1;
				return;
			}


		}

		/**
		returns T& from the end of the array and removes it from array
		@return T& from the end of the array
		*/
		T& pop_back() {

			// grab value from end of array and erase it 
			iterator end(right,this);
			T& for_return = *end;
			*end = 0;

			// update right
			this->right = (--end).currIndex;

			sz -= 1;
			return for_return;
		}

		/**
		returns T& from the beginning of the array and removes it from array
		@return T& from the beginning of the array
		*/
		T& pop_front() {

			// grab value from beginning of array and erase it
			iterator start(left, this);
			T& for_return = *start;
			*start = 0;

			// update right
			this->left = (++start).currIndex;

			sz -= 1;
			return for_return;
		}

		/**
		returns size_t corresponding to number of elements in the array
		@return size_t corresponding to number of elements in the array
		*/
		size_t size() {
			return sz;
		}

		/**
		returns size_t corresponding to capacity of the array
		@return size_t corresponding to capacity of the array
		*/
		size_t capacity() {
			return cap;
		}

		/**
		@class iterator
		@brief The iterator class creates a functional iterator for vals in the specialize_deque

		The iterator class privately contains a size_t representing its current index, a pointer to a specialize_deque representing its
		container, and a declaration for specialize_deque as a friend class so that specialize_deque can access currIndex
		It publicly contains a constructor, destructor, and operator overloading for -- (prefix and postfix), ++ (prefix and postfix),
		<, ==, !=, *, and ->. The combination of the aforementioned operators allow for range based traversal of a specialize_deque, and 
		assignment of values in the deque from the iterator.
		*/

		class iterator {

		private:
			size_t currIndex;
			deque* container;
			friend class deque;

			/**
			returns size_t corresponding to next index in the array,
			used in increment operator
			@return size_t corresponding to next index in the array
			*/
			size_t next_index(size_t index) {
				// conditionally returns past-the-end index if we have reached
				// the right value of container, accounts for wrapping
				if (index != container->right) {
					return (index + 1) % container->cap;
				}
				else {
					return container->cap + 1;
				}
			}

			/**
			returns size_t corresponding to last index in the array,
			used in decrement operator
			@return size_t corresponding to last index in the array
			*/
			size_t previous_index(size_t index) {
				// returns previous index, accounts for wrapping
				if (index == 0) {
					return container->cap - 1;
				}
				return index - 1;
			}

		public:
			//constructor, takes index and container values
			iterator(size_t _currIndex, deque* _container) : currIndex(_currIndex), container(_container) {};
			
			//default deconstructor
			~iterator() {};

			// postfix increment operator
			iterator operator++(int) {
				iterator tmp = *this;
				++* this;
				return tmp;
			}      

			// postfix decrement operator
			iterator operator--(int) {
				iterator tmp = *this;
				--* this;
				return tmp;
			}      

			// prefix increment operator
			iterator& operator++() {
				currIndex = next_index(currIndex);
				return *this;
			}

			// prefix decrement operator
			iterator& operator--() {
				currIndex = previous_index(currIndex);
				return *this;
			}


			// comparison operator for usage in equality checks
			bool operator<(const iterator& it1) const {
				return (container->data[currIndex] < container->data[it1.currIndex]);
			}

			// Declare and define equals and not equals operators.
			bool operator==(const iterator& it1) const {
				if (!(container->data[currIndex] < container->data[it1.currIndex]) &&
					!(container->data[it1.currIndex] < container->data[currIndex])) {
					return true; // utilizes < to check equality
				}
				else { return false; }
			}

			// nots the equals operator
			bool operator!=(const iterator& it1) const {
				return !(*this==it1);
			}

			// Declare and define dereferencing and arrow operator.
		
			T& operator*() const { return container->data[currIndex]; }

			T* operator->() const { return &container->data[currIndex]; }

		};

		/**
		@class const_iterator
		@brief The iterator class creates a functional iterator for vals in the specialize_deque

		The iterator class privately contains a size_t representing its current index, a pointer to a specialize_deque representing its
		container, and a declaration for specialize_deque as a friend class so that specialize_deque can access currIndex
		It publicly contains a constructor, destructor, and operator overloading for -- (prefix and postfix), ++ (prefix and postfix),
		<, ==, !=, *, and ->. The combination of the aforementioned operators allow for range based traversal of a specialize_deque
		*/
		class const_iterator {

		private:
			size_t currIndex;
			const deque* container;
			friend class deque;

			/**
			returns size_t corresponding to next index in the array,
			used in increment operator
			@return size_t corresponding to next index in the array
			*/
			size_t next_index(size_t index) {
				if (index != container->right) {
					return (index + 1) % container->cap;
				}
				else {
					return container->cap + 1;
				}
			}

			/**
			returns size_t corresponding to last index in the array,
			used in decrement operator
			@return size_t corresponding to last index in the array
			*/
			size_t previous_index(size_t index) {
				if (index != container->left) {
					if (index == 0) {
						return container->cap - 1;
					}
					return index - 1;
				}
				else {
					return container->cap;
				}
			}

		public:
			//constructor, takes index and container values
			const_iterator(size_t _currIndex, const deque* _container) : currIndex(_currIndex), container(_container) {};
			
			//default deconstructor
			~const_iterator() {};

			// postfix increment operator
			const_iterator operator++(int) {
				const_iterator tmp = *this;
				++* this;
				return tmp;
			}

			// postfix decrement operator
			const_iterator operator--(int) {
				const_iterator tmp = *this;
				--* this;
				return tmp;
			}

			// prefix increment operator
			const_iterator& operator++() {
				currIndex = next_index(currIndex);
				return *this;
			}

			// prefix decrement operator
			const_iterator& operator--() {
				currIndex = previous_index(currIndex);
				return *this;
			}


			// comparison operator for usage in equality checks
			bool operator<(const const_iterator& it1) const {
				return (container->data[currIndex] < container->data[it1.currIndex]);
			}

			// Declare and define equals and not equals operators.
			bool operator==(const const_iterator& it1) const {
				if (!(container->data[currIndex] < container->data[it1.currIndex]) &&
					!(container->data[it1.currIndex] < container->data[currIndex])) {
					return true; // utilizes < to check equality
				}
				else { return false; }
			}

			// nots the equals operator
			bool operator!=(const const_iterator& it1) const {
				return !(*this == it1);
			}

			// Declare and define dereferencing  and arrow operator.

			const T& operator*() const { return container->data[currIndex]; }

			const T* operator->() const { return &container->data[currIndex]; }

		};

		/**
		returns iterator corresponding to first index in the array
		@return iterator corresponding to first index in the array
		*/
		iterator begin() {
			//left value is "first" value
			iterator for_return(left, this);
			return for_return;
		}

		/**
		returns iterator corresponding to one-past-the-end index in the array
		@return iterator corresponding to one-past-the-end index in the array
		*/
		iterator end() {
			iterator for_return(cap + 1, this);
			return for_return;
		}

		/**
		returns const-iterator corresponding to first index in the array
		@return const-iterator corresponding to first index in the array
		*/
		const_iterator begin() const {
			const_iterator for_return(left, this);
			return for_return;
		}
		
		/**
		returns const-iterator corresponding to one-past-the-end index in the array
		@return const-iterator corresponding to one-past-the-end index in the array
		*/
		const_iterator end() const {
			const_iterator for_return(cap + 1, this);
			return for_return;
		}

		/**
		bracket access operator
		@param index is the index of the data array we are looking for
		@return T& at index of array specified by index
		*/
		T& operator[](size_t index) { 
			if (index >= cap) { throw std::range_error("Index out of range!"); }
			else { return data[index]; }
		}

		/**
		const bracket access operator
		@param index is the index of the data array we are looking for
		@return const T& at index of array specified by index
		*/
		const T& operator[](size_t index) const { 
			if (index >= cap) { throw std::range_error("Index out of range!"); }
			else { return &data[index]; }
		}

	private:
		std::unique_ptr<T[]> data;
		size_t cap;
		size_t sz;
		size_t left;
		size_t right;
	};

	template<>
	/**
		@class deque<bool>
		@brief The deque<bool> class creates an iterable functional double ended queue for bools.
			It allows for back and front pushing, as well as retrieval from both ends.
			It is optimized so that each bool in the deque occupies only one bit.
	*/
	class deque<bool> {
		
	public:
		//default constructor, sets cap to 1, size to CHAR_BIT
		deque() : data(std::make_unique<unsigned char[]>(CHAR_BIT)), cap(1), sz(0),
			left(0), right(0) {}

		//default deconstructor; since we are using smart pointers, no extra stuff is required here
		~deque() = default;

		//swap constructor
		deque(deque<bool> && rhs) noexcept : deque<bool>() {

			std::swap(cap, rhs.cap);
			std::swap(sz, rhs.sz);
			std::swap(left, rhs.left);
			std::swap(right, rhs.right);

			swap(rhs.data);
		}

		//copy constructor
		deque(const deque<bool> & rhs) noexcept : deque<bool>() {
			cap = rhs.cap;
			sz = rhs.sz;
			std::unique_ptr<unsigned char[]> tmp = std::make_unique<unsigned char[]>(cap);
			data.swap(tmp);			copy_vals(rhs);
			left = 0;
			right = sz - 1;
		}

		//copy operator
		deque<bool>& operator=(const deque<bool> & rhs) {
			cap = rhs.cap;
			sz = rhs.sz;
			std::unique_ptr<unsigned char[]> tmp = std::make_unique<unsigned char[]>(cap);
			data.swap(tmp);
			copy_vals(rhs);
			left = 0;
			right = sz - 1;
			return *this;
		}

		//move operator
		deque<bool>& operator=(deque<bool>&& rhs) noexcept {
			cap = std::move(rhs.cap);
			sz = std::move(rhs.sz);
			right = std::move(rhs.right);
			data = std::move(rhs.data);
			return *this;
		}

		/**
		swaps data arrays between deques
		@param rhs is a deque with which @this will swap its values
		*/
		void swap(std::unique_ptr<unsigned char[]> & rhs) {
			this->data.swap(rhs);
		}

		/**
		allows for copies without destruction of data due to unique_ptr
		@param rhs is a deque from which @this will copy its values
		*/
		void copy_vals(const deque<bool> & rhs) {
			//want new deque to have ptr-friendly traversal to start
			auto f = this->begin();
			for (auto i = rhs.begin(), e = rhs.end(); i != e; ++i) {
				//set 0,1,... to rhs.begin(), rhs.begin() + 1,...
				*f = *i;
				++f;
			}
		}

		
		/**
		returns size_t corresponding to number of elements in the array
		@return size_t corresponding to number of elements in the array
		*/
		size_t size() {
			return sz;
		}

		/**
		returns size_t corresponding to capacity of the array
		@return size_t corresponding to capacity of the array
		*/
		size_t capacity() {
			return cap;
		}

		/**
			@class proxy
			@brief the proxy class creates a bit proxy for booleans inserted into our deque.
				It contains = and bool conversion operators, and a ptr to a bit for storage at
				size_t pos.
		*/
		class proxy {
		private:
			friend class deque<bool>;
			unsigned char* ptr;
			size_t pos;
			inline proxy(unsigned char* _ptr, size_t _pos) : ptr(_ptr), pos(_pos) {}

		public:
			inline const proxy& operator=(bool b) const {
				if (b) {
					*ptr = (*ptr |  (1 << pos));
				}
				else {
					*ptr = (*ptr | ~(1 << pos));
				}
				return *this;
			}
			operator bool() const {
				unsigned char& u = *ptr;
				return (u >> pos) & 1;
			}
		};

		/**
			@class const_proxy
			@brief the proxy class creates a bit proxy for booleans inserted into our deque.
				It contains a bool conversion operator, and a ptr to a bit for storage at
				size_t pos.
		*/
		class const_proxy {
		private:
			friend class deque<bool>;
			const unsigned char* ptr;
			size_t pos;
			inline const_proxy(const unsigned char* _ptr, size_t _pos) : ptr(_ptr), pos(_pos) {}

		public:
			operator bool() const {
				const unsigned char& u = *ptr;
				return (u >> pos) & 1;
			}
		};

		/**
		bracket access operator
		@param index is the index of the data array we are looking for
		@return proxy for boolean at index of array specified by index
		*/
		proxy operator[](size_t index) { return { &data[index/8], index%8 }; }
		
		/**
		const bracket access operator
		@param index is the index of the data array we are looking for
		@return const proxy for boolean at index of array specified by index
		*/
		const_proxy operator[](size_t index) const { return { &data[index / 8], index%8 }; }

		class iterator {

		private:
			size_t currIndex;
			deque<bool>* container;
			friend class deque<bool>;

			/**
			returns size_t corresponding to next index in the array,
			used in increment operator
			@return size_t corresponding to next index in the array
			*/
			size_t next_index(size_t index) {
				// conditionally returns past-the-end index if we have reached
				// the right value of container, accounts for wrapping
				if (index != container->right) {
					return (index + 1) % container->cap;
				}
				else {
					return container->cap + 1;
				}
			}

			/**
			returns size_t corresponding to last index in the array,
			used in decrement operator
			@return size_t corresponding to last index in the array
			*/
			size_t previous_index(size_t index) {
				// returns previous index, accounts for wrapping
				if (index == 0) {
					return container->cap - 1;
				}
				return index - 1;
			}

		public:
			//constructor, takes index and container values
			iterator(size_t _currIndex, deque* _container) : currIndex(_currIndex), container(_container) {};

			//default deconstructor
			~iterator() {};

			// postfix increment operator
			iterator operator++(int) {
				iterator tmp = *this;
				++* this;
				return tmp;
			}

			// postfix decrement operator
			iterator operator--(int) {
				iterator tmp = *this;
				--* this;
				return tmp;
			}

			// prefix increment operator
			iterator& operator++() {
				currIndex = next_index(currIndex);
				return *this;
			}

			// prefix decrement operator
			iterator& operator--() {
				currIndex = previous_index(currIndex);
				return *this;
			}


			// comparison operator for usage in equality checks
			bool operator<(const iterator& it1) const {
				return (**this < *it1);
			}

			// Declare and define equals and not equals operators.
			bool operator==(const iterator& it1) const {
				if (!(**this < *it1) &&
					!(*it1 < **this)) {
					return true; // utilizes < to check equality
				}
				else { return false; }
			}

			// nots the equals operator
			bool operator!=(const iterator& it1) const {
				return !(*this == it1);
			}

			// Declare and define dereferencing and arrow operator.

			proxy operator*() const { return { &(container->data[currIndex / 8]) , static_cast<size_t>(currIndex % 8) };}
			
			/*bool operator*() const {
				bool for_return;
				proxy for_convert{ &(container->data[currIndex / 8]) , static_cast<size_t>(currIndex % 8) };
				for_return = for_convert;
				return for_return;
			}*/

		};

		/**
		@class const_iterator
		@brief The iterator class creates a functional iterator for vals in the specialize_deque

		The iterator class privately contains a size_t representing its current index, a pointer to a specialize_deque representing its
		container, and a declaration for specialize_deque as a friend class so that specialize_deque can access currIndex
		It publicly contains a constructor, destructor, and operator overloading for -- (prefix and postfix), ++ (prefix and postfix),
		<, ==, !=, *, and ->. The combination of the aforementioned operators allow for range based traversal of a specialize_deque
		*/
		class const_iterator {

		private:
			size_t currIndex;
			const deque* container;
			friend class deque;

			/**
			returns size_t corresponding to next index in the array,
			used in increment operator
			@return size_t corresponding to next index in the array
			*/
			size_t next_index(size_t index) {
				if (index != container->right) {
					return (index + 1) % container->cap;
				}
				else {
					return container->cap + 1;
				}
			}

			/**
			returns size_t corresponding to last index in the array,
			used in decrement operator
			@return size_t corresponding to last index in the array
			*/
			size_t previous_index(size_t index) {
				if (index != container->left) {
					if (index == 0) {
						return container->cap - 1;
					}
					return index - 1;
				}
				else {
					return container->cap;
				}
			}

		public:
			//constructor, takes index and container values
			const_iterator(size_t _currIndex, const deque* _container) : currIndex(_currIndex), container(_container) {};

			//default deconstructor
			~const_iterator() {};

			// postfix increment operator
			const_iterator operator++(int) {
				const_iterator tmp = *this;
				++* this;
				return tmp;
			}

			// postfix decrement operator
			const_iterator operator--(int) {
				const_iterator tmp = *this;
				--* this;
				return tmp;
			}

			// prefix increment operator
			const_iterator& operator++() {
				currIndex = next_index(currIndex);
				return *this;
			}

			// prefix decrement operator
			const_iterator& operator--() {
				currIndex = previous_index(currIndex);
				return *this;
			}


			// comparison operator for usage in equality checks
			bool operator<(const const_iterator& it1) const {
				return (**this < *it1);
			}

			// Declare and define equals and not equals operators.
			bool operator==(const const_iterator& it1) const {
				if (!(**this < *it1) &&
					!(*it1 < **this)) {
					return true; // utilizes < to check equality
				}
				else { return false; }
			}

			// nots the equals operator
			bool operator!=(const const_iterator& it1) const {
				return !(*this == it1);
			}

			// Declare and define dereferencing  and arrow operator.

			proxy operator*() const { return { &(container->data[currIndex / 8]) , static_cast<size_t>(currIndex % 8) }; }

			/*bool operator*() const { 
				bool for_return;
				proxy for_convert { &(container->data[currIndex / 8]) , static_cast<size_t>(currIndex % 8) }; 
				for_return = for_convert;
				return for_return;
			}*/

		};

		/**
		returns iterator corresponding to first index in the array
		@return iterator corresponding to first index in the array
		*/
		iterator begin() {
			//left value is "first" value
			iterator for_return(left, this);
			return for_return;
		}

		/**
		returns iterator corresponding to one-past-the-end index in the array
		@return iterator corresponding to one-past-the-end index in the array
		*/
		iterator end() {
			iterator for_return(cap + 1, this);
			return for_return;
		}

		/**
		returns const-iterator corresponding to first index in the array
		@return const-iterator corresponding to first index in the array
		*/
		const_iterator begin() const {
			const_iterator for_return(left, this);
			return for_return;
		}

		/**
		returns const-iterator corresponding to one-past-the-end index in the array
		@return const-iterator corresponding to one-past-the-end index in the array
		*/
		const_iterator end() const {
			const_iterator for_return(cap + 1, this);
			return for_return;
		}

		/**
		places value corresponding to new_data in front of queue
		@param new_data is a T& corresponding to new value for deque
		*/
		void push_front(const bool& new_data) {
			// if capacity is reached, we need to reallocate
			if (cap == sz) {
				// create tmp deque for val swapping
				deque<bool> tmp = *this;

				// double capacity, create new data array full of zeros
				cap *= 2;
				std::unique_ptr<unsigned char[]> tmp2 = std::make_unique<unsigned char[]>(cap);
				data.swap(tmp2);

				// due to copy behavior, new left is zero, new right is tmp.right
				left = 0;
				right = tmp.right;

				// copy vals as opposed to swapping to maintain capacity
				this->copy_vals(tmp);

				//proxy_new_data 

				// place new value before old starting value, allows for wrapping
				iterator start = begin();
				start--;
				*start = new_data;

				// update left
				left = start.currIndex;

				sz += 1;
				return;
			}

			// if data is empty, simply place new value at begin
			else if (sz == 0) {

				iterator start = begin();
				*start = new_data;

				sz += 1;
				return;
			}

			// if capacity is not reached, place at one before start
			else {

				// place new value before old starting value, allows for wrapping
				iterator start = begin();
				start--;
				*start = new_data;

				// update left
				left = start.currIndex;

				sz += 1;
				return;
			}


		}

		/**
		places value corresponding to new_data in back of queue
		@param new_data is a T& corresponding to new value for deque
		*/
		void push_back(const bool& new_data) {
			// if capacity is reached, we need to reallocate
			if (cap == sz) {

				// create tmp deque for val swapping
				deque tmp = *this;

				// double capacity, create new data array full of zeros
				cap *= 2;
				std::unique_ptr<unsigned char[]> tmp2 = std::make_unique<unsigned char[]>(cap);
				data.swap(tmp2);

				// due to copy behavior, new left is zero, new right is tmp.right
				left = 0;
				right = tmp.right;

				for (auto i = this->begin(), e = this->end(); i != e; ++i) {
					//set 0,1,... to rhs.begin(), rhs.begin() + 1,...
					std::cout << *i;
				}

				// copy vals as opposed to swapping to maintain capacity
				this->copy_vals(tmp);

				// place new value after old end value
				iterator end(right, this);
				end.currIndex += 1;
				*end = new_data;

				// update right
				right = end.currIndex;

				sz += 1;
				return;
			}

			// if data is empty, simply place new value at end
			else if (sz == 0) {

				iterator end(right, this);
				*end = new_data;

				sz += 1;
				return;
			}

			// if capacity is not reached, place at one after end
			else {

				// place new value after old end value
				iterator end(right, this);
				end.currIndex += 1;
				*end = new_data;

				// update right
				right = end.currIndex;

				sz += 1;
				return;
			}


		}

		/**
		returns T& from the end of the array and removes it from array
		@return T& from the end of the array
		*/
		bool& pop_back() {

			// grab value from end of array and erase it 
			iterator end(right, this);
			bool for_return = *end;
			*end = 0;

			// update right
			this->right = (--end).currIndex;

			sz -= 1;
			return for_return;
		}

		/**
		returns T& from the beginning of the array and removes it from array
		@return T& from the beginning of the array
		*/
		bool& pop_front() {

			// grab value from beginning of array and erase it
			iterator start(left, this);
			bool for_return = *start;
			*start = 0;

			// update right
			this->left = (++start).currIndex;

			sz -= 1;
			return for_return;
		}


	private:
		std::unique_ptr<unsigned char[]> data;
		size_t cap;
		size_t sz;
		size_t left;
		size_t right;
	};

}
#endif