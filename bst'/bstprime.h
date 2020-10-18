#ifndef BST_H
#define BST_H

#include<utility>
#include<functional>
#include<stdexcept>

namespace pic10c {
	template<typename T, typename C = std::less<T>>
	class bst {
	private:
		node* root; // the root of the tree
		void deletebst(node* forDestruction) {} // to recursively delete the tree
		void traverseInsert(node*); // to help with copying

	public:
		bst<T, C>() noexcept; // default constructor for empty tree
		~bst<T, C>() { deletebst(root); } // destructor

		bst<T, C>(const bst& rhs) noexcept : bst() {
			root = rhs.root;
		}; // copy constructor
		bst<T, C>(bst&& rhs) noexcept : bst() {
			std::swap(root, rhs.root);
		}; // move constructor

		bst& operator=(const bst& rhs) {
			root = rhs.root;
			return *this;
		}
		bst& operator=(bst&& rhs) {
			root = std::move(rhs.root);
			return *this;
		} // assignment operators

		bool find(const T&) const; // check if contains a T
		friend void swap(bst&& rhs) {
			std::swap(this, rhs);
		} // swap two bsts

		iterator begin() {
			iterator x = new iterator(root, this);
			return x;
		} // iterator to begin position

		iterator end() const; // iterator to past-the-end position
		void insert(T); // to add a value to the tree

		//creates template for variadic parameters
		template<typename... args>
		void emplace(args&&... arg1) {
			//forwards args from variadic list into T constructor
			T forAddition(std::forward<args>(arg1)...);
			//calls insert to use standard method of insertion on newly constructed T
			this->insert(forAddition);
		}

		void erase(iterator); // to erase a value from the tree

		//bool operator<(const bst::iterator&, const bst::iterator&);
		//bool operator==(const bst::iterator&, const bst::iterator&);
		//bool operator!=(const bst::iterator&, const bst::iterator&);


	template<typename T, typename C = std::less<T>>
	class node {
		friend bst; // tree member functions may search through nodes
		friend iterator; // to be able to advance by checking node values
	private:
		node* left, * right; // children
		T val; // data value stored
		node(T _val) : val(_val) {}; // constructor: take by value and move it
		void insertNode(node* forInsertion) {
			if (pred(forInsertion->val)) {
				this->left = forInsertion;
			}
			else { this->right = forInsertion; }
		}// member function for inserting node
		
	};

	template<typename T, typename C = std::less<T>>
	class iterator {
		friend bst; // to allow iterator modifications by bst operations
	private:
		iterator(node* _curr, const bst* _container) : curr(_curr), container(_container) {}; // constructor
		node* curr; // current position
		const bst* container; // holding container
	public:
		iterator& operator++(); // prefix ++
		iterator operator++(int); // postfix ++
		iterator& operator--(); // prefix - -
		iterator operator--(int); // postfix - -
		const T& operator*() const; // dereference operator
		const T* operator->() const { // arrow operator
			return &(curr->val);
		}

		friend bool operator<(const iterator& it1, const iterator& it2) {
			return pred(it1->address->value, it1->address->value);
		}
		friend bool operator==(const iterator& it1, const iterator& it2) {
			if !(it1->address->value < it2->address->value) && !(it2->address->value < it1->address->value) {
				return true;
			}
			else { return false; }
		}
		friend bool operator!=(const iterator& it1, const iterator& it2) {
			return !(it1->address->value == it2->address->value);
		}
	};
	};
	

}

	




#endif