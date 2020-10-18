#ifndef BST_H
#define BST_H

#include<utility>
#include<functional>
#include<stdexcept>

namespace pic10c
{
	template<typename T, typename C = std::less<T>>
	class bst {
	public:
		
		class node {
		private:
			T value;
			node* left;
			node* right;
			node* parent;
			C pred;
			
		public:
			node(T data, C _pred) : value(data), left(nullptr), right(nullptr), parent(nullptr), pred(_pred) {};
			~node() {}

			void setLeft(node* _left) { this->left = _left; }
			void setRight(node* _right) { this->right = _right; }
			void setParent(node* _parent) { this->parent = _parent; }

			node* getLeft() { return this->left; }
			node* getRight() { return this->right; }
			node* getParent() { return this->parent; }
			T& getValue() { return this->value; }
			
			size_t size(node* start) {
				if (start == nullptr) { return 0; }
				return (1 + size(start->left) + size(start->right));
			}

			void insertNode(node* forInsertion, node* refNode, node* prevNode = nullptr, bool leftInsert = true) {
				if (refNode == nullptr) {
					refNode = forInsertion;
					refNode->setParent(prevNode);
					if (leftInsert) { prevNode->setLeft(refNode); }
					else{prevNode->setRight(refNode);}

				}
				else {
					if (pred(forInsertion->value, refNode->value)) {
						try {
							insertNode(forInsertion, refNode->left, refNode);
						}
						catch (...) {} // std::cerr << "bad_alloc caught: " << ba.what() << '\n'; is what I want, but no iostream allowed
					}
					else {
						try {
							insertNode(forInsertion, refNode->right, refNode, false);
						}
						catch (...) {} // std::cerr << "bad_alloc caught: " << ba.what() << '\n'; is what I want, but no iostream allowed
					}
				}
				}// member function for inserting node

			node* find(T data) {
				//if input int value is smaller than current node data must continue to the left
				if (pred(data,value)) {
					//if left is nullptr cannot continue therefore return false
					if (left == nullptr) {
						return this;
					}
					//call find recursively to further cycle through BinarySearchTree system and search for TreeNode
					else {
						return left->find(value);
					}
				}
				//if input int value is larger than current node data must continue to the right
				else if (pred(value,data)) {
					//if right is nullptr cannot continue therefore return false
					if (right == nullptr) {
						return this;
					}
					//call find recursively to further cycle through BinarySearchTree system and search for TreeNode
					else {
						return right->find(value);
					}
				}
				//if data==value return true
				else {
					return this;
				}
			}
		};



		class iterator {
		private:
			friend class node;
			friend class bst;
			node* address;
			const bst* container;

		public:
			iterator(node* _address, const bst* _container) : address(_address), container(_container){};

			node* getAddress() { return address; }

			int operator*() const { return this->address; }

			// Declare and define prefix and postfix increment operators.
			const iterator& operator++() { 
				this->address = this->address->getRight();
				return *this;
			}       // Prefix increment operator.
			const iterator operator++(int) { 
				return (++address);
			}      // Postfix increment operator.

			// Declare and define prefix and postfix decrement operators.
			const iterator& operator--() { 
				this->address = this->address->getLeft(); 
				return *this; 
			}  // Prefix decrement operator.

			const iterator operator--(int) { 
				return (--address); 
			}      // Postfix decrement operator.

			friend bool operator<(const iterator&, const iterator&);

			// Declare and define equals and not equals operators.
			friend bool operator==(const iterator&, const iterator&);
			friend bool operator!=(const iterator&, const iterator&);
			// Declare and define dereferencing  and arrow operator.

			T& operator*() { return this->address->getValue(); }

			T* operator->() const { return *this; }



		};

		friend bool operator<(const iterator& it1, const iterator& it2) {
			return pred(it1.address->value, it1.address->value);
		}
		friend bool operator==(const iterator& it1, const iterator& it2) {
			if (!(it1.address->value < it2.address->value) &&
				!(it2.address->value < it1.address->value)) {
				return true;
			}
			else { return false; }
		}
		friend bool operator!=(const iterator& it1, const iterator& it2) {
			return !(it1.address->getValue() == it2.address->getValue());
		}


		//constructors, which default pred to less
		bst<T, C>();
		bst<T, C>(C _pred) : pred(_pred), root(nullptr) {};

		bst<T, C>(bst&& rhs) noexcept : bst() {
			std::swap(root, rhs.root);
		};

		bst<T, C>(const bst& rhs) noexcept : bst() {
			root = rhs.root;
		};

		bst& operator=(const bst& rhs) {
			root = rhs.root;
			return *this;
		}

		bst& operator=(bst&& rhs) {
			root = std::move(rhs.root);
			return *this;
		}

		node* firstNode() const { return root; }

		void swap(bst&& rhs) {
			std::swap(this, rhs);
		}

		void recursively_destroy(node* forDestruction) {

			if (forDestruction->getLeft() != nullptr) {
				recursively_destroy(forDestruction->getLeft());
			}
			if (forDestruction->getRight() != nullptr) {
				recursively_destroy(forDestruction->getRight());
			}
			delete forDestruction;
			 
		}
		~bst() {
			if (root != nullptr) { 
				try{ recursively_destroy(root); }
				catch(...) {} // std::cerr << "bad_alloc caught: " << ba.what() << '\n'; is what I want, but no iostream allowed
				delete this;
			}
				 
			}

		//insertion call, which calls our recursive promotion function
		void insert(const T& data) {
			insert(data, root);
		}
		void insert(const T& data, node*& refNode) {
			node* new_node = new node(data, pred);
			new_node->setLeft(nullptr);
			new_node->setRight(nullptr);
			new_node->setParent(nullptr);
			if (refNode == nullptr) { refNode = new_node; }
			//else call the recursive insert_node function on new_node to find appropriate location
			else { 
				node* new_node2 = refNode->find(data);
				if (new_node2->getValue()!=data) {
					refNode->insertNode(new_node, refNode);
				}
				else { return; }
			}
		}


		//creates template for variadic parameters
		template<typename... args>
		void emplace(args&&... arg1) {
			//forwards args from variadic list into T constructor
			T forAddition(std::forward<args>(arg1)...);
			//calls insert to use standard method of insertion on newly constructed T
			this->insert(forAddition);
		}


		iterator begin() {
			iterator x(root, this);
			return x;
		}

		iterator end() {
			iterator x(root, this);
			iterator y(root, this);
			while (x.address != nullptr) {
				++x;
				if (x.address != nullptr) { y = x; }
			}
			return y;
		}

		iterator find(const T& data, iterator first = begin()) {
			node* x = first->address->find();
			iterator y(first->address,this);
			if (x->value != data) {
				y->address = nullptr; 
				return y;
			}
			else { return x; }
		}

		void erase(iterator& it) {
			delete it->address;
		}

		//size function, which checks size of values vector
		//interior from https://cs.phyley.com/binary-tree/find-size
		size_t size() {
			return root->size(root);
		}
	private:
		node* root;
		C pred;

	
	};

	

	template<typename T, typename C>
	inline bst<T, C>::bst()
	{
	}

}


#endif




