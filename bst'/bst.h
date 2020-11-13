#ifndef BST_H
#define BST_H

#include<utility>
#include<functional>
#include<stdexcept>

namespace pic10c
{
	/** 
	template required for arbitrary type storage in the bst, and C allows for user to define
	a comparison function appropriate for whatever type T may be 
	 */
	template<typename T, typename C = std::less<T>>
	
	/**
	@class bst
	@brief The bst class is a Binary Search Tree of templated T values

	The bst class contains a pointer to a root node, defaulting to nullptr, a pred comparison function, defaulting to std::less<T>, rule of 5
	constructors and operator overloading, a recursive destructor, insert/emplace functions for creation of nodes, an erase function to elim
	nodes pointed to by iterators (defined later), begin and end functions to return iterators to beginning and end (past-the-end)
	nodes of the tree, a size function that recursively parses the tree nodes to count its members, a find function that returns
	an iterator if any node in the tree contains a value T passed into find.
	*/
	class bst {
	
	public:
		
	/**
	@class node
	@brief The node class is a nested class in bst for holding T values and providing infrastructure for the tree

	The node class contains a value of type T, pointers to children and parent, and the pred C comparator. node also declares iterator
	and bst as friend classes so that node->left, etc. can be utilized by functions in those two classes.
	Note also contains a constructor for values of type T, with a default left,right,parent construction that
	defaults said pointers to nullptr, and pred to whatever value of pred is contained in bst. It also contains a destructor, and functions
	size that allows for the bst::size function to operate properly, insertNode that allows for the bst::insert
	and bst::emplace functions to operate properly, find for the bst::find function to operate properly, and a recursive
	destruction function to allow for tree branch-cutting.
	*/
		class node {
		private:
			T value;
			node* left;
			node* right;
			node* parent;
			C pred;
			friend class iterator;
			friend class bst;
			
		public:
			node(T data, C _pred) : value(data), left(nullptr), right(nullptr), parent(nullptr), pred(_pred) {};
			~node() {}
			
			//following concstructors are not strictly necessary, but abide by rule of 5.
			//swap constructor
			node(node&& rhs) noexcept : node() {
				std::swap(value, rhs.value);
				std::swap(left, rhs.left);
				std::swap(right, rhs.right);
				std::swap(parent, rhs.parent);
				std::swap(pred, rhs.pred);
			};

			//copy constructor
			node(const node& rhs) noexcept : node() {
				root = rhs.root;
				value = rhs.value;
				left = rhs.left;
				right = rhs.right;
				parent = rhs.parent;
				pred = rhs.pred;
			};

			//copy operator
			node& operator=(const node& rhs) {
				root = rhs.root;
				value = rhs.value;
				left = rhs.left;
				right = rhs.right;
				parent = rhs.parent;
				pred = rhs.pred;
				return *this;
			}

			//move operator
			node& operator=(node&& rhs) {
				root = std::move(rhs.root);
				value = std::move(rhs.value);
				left = std::move(rhs.left);
				right = std::move(rhs.right);
				parent = std::move(rhs.parent);
				pred = std::move(rhs.pred);
				return *this;
			}

			/**	
			Recursive size function to count children
			@param start is a const pointer to the root node for counting
			@return is a size_t representing number of non-nullptr nodes in the tree
			*/
			size_t size(const node* start) const {
				if (start == nullptr) { return 0; } // checks if new root is nullptr
				return (1 + size(start->left) + size(start->right)); //if not recursively counts children
			}

			/**	
			Safe node insertion function used by bst::insert()
			@param forInsertion is the node we want to insert
			@param refNode is a node under which we want to insert forInsertion
			@param prevNode allows us to assign a child to the node under which forInsertion is inserted and a parent to forInsertion
			@param leftInsert is a simple mechanism to ensure child is placed on correct side of parent
			*/
			void insertNode(node* forInsertion, node* refNode, node* prevNode = nullptr, bool leftInsert = true) {
				if (refNode == nullptr) { // if node does not exist
					refNode = forInsertion; // create node w value specified
					refNode->parent = (prevNode);
					if (leftInsert) { prevNode->left = (refNode); } // if child should be left of parent, insert left
					else{prevNode->right = (refNode);} // insert right instead

				}
				else { //if node exists
					if (pred(forInsertion->value, refNode->value)) { // check if forInsertion is less or greater than parent
						try {
							insertNode(forInsertion, refNode->left, refNode); // recursively insert left
						}
						catch (...) {} // std::cerr << "bad_alloc caught: " << ba.what() << '\n'; is what I want, but no iostream allowed
					}
					else {
						try {
							insertNode(forInsertion, refNode->right, refNode, false); // recursively insert right
						}
						catch (...) {} // std::cerr << "bad_alloc caught: " << ba.what() << '\n'; is what I want, but no iostream allowed
					}
				}
				}

			/**
			Find function used by bst::find()
			@param T is the value we are looking for in the tree
			@return is a node corresponding to the value, if found
			*/

			node* find(T data) {
				//if input smaller than current node value
				if (pred(data,value)) {
					if (left == nullptr) { //cannot continue 
						return this; //return this for comparison
					}
					//call find recursively to further cycle through bst and search for value
					else {
						return left->find(value);
					}
				}
				//if input larger than current node value
				else if (pred(value,data)) {
					if (right == nullptr) { //cannot continue
						return this; //return this for comparison
					}
					//call find recursively to further cycle through bst and search for value
					else {
						return right->find(value);
					}
				}
				//data and value match
				else {
					return this; //return this for comparison
				}
			}

			/**
			recursively destroy for tree-branch cutting, used in ~bst()
			@param forDestruction starting node we want to recursively destroy
			*/

			void recursively_destroy(node* forDestruction) {

				if (!forDestruction) { return; } // doesn't exist, go back

				recursively_destroy(forDestruction->left);
				recursively_destroy(forDestruction->right); 
				forDestruction = nullptr; // set node to nullptr
				free(forDestruction); // free memory from node

			}

			/**
			locates the parent of the last node of the tree, allowing for nullifying erasure node
			@param root is where we want to start looking for the last node
			@param forAssignment is a node* reference to which we will assign the location of the parent
			of the last node of the tree
			@param nodesFound is how many nodes have been found in our recursion
			@param nodesToBeFound is the depth of the tree we are traveling
			*/
			void lastNodeParent(node* root, node*& forAssignment, size_t nodesFound, size_t nodesToBeFound) {

				//this size_t exists to facilitate counting. 
				size_t forPassage = nodesFound + 1;
				
				//as long as the last node is at the end of the tree, correction can happen later, so I chose R and L
				//in arbitrary order here
				if (root->right != nullptr) { lastNodeParent(root->right, forAssignment, forPassage, nodesToBeFound); }
				if (root->left != nullptr)  { lastNodeParent( root -> left , forAssignment, forPassage, nodesToBeFound);  }

				//if we are at max depth, assign 
				if(nodesFound == nodesToBeFound) { forAssignment = root->parent; }
				else { return; }
			}

			/**
			determines depth of tree from the specified root node
			@param root is where we want to start looking for the last node
			@param level is the number of levels under the root for the current function call
			@param lastLevel gets set only when a new node level has been reached, and is a ref
			*/
			void depthFinder(node* root, size_t level, size_t& lastLevel) {
				if (root != nullptr) {
					depthFinder(root->left, ++level, lastLevel);
						if (level > lastLevel) {
							lastLevel = level;
						}
					depthFinder(root->right,level,lastLevel);
				}
			}

			/**
			helper function for depthFinder, inits the lastLevel size_t for depthFinder
			@param root is a node from which we want to start searching for depth.
			@return lastLevel is the depth of the tree determined by depthFinder
			*/
			size_t heightOfTree(node* root) {
				size_t lastLevel = 0;
				depthFinder(root, 0, lastLevel);
				return lastLevel;
			}

			/**
			swaps values of node at erasure point and last node in tree
			*/
			void node_swap() {
				//initializes values for heightOfTree() function
				size_t nodesFound = 1; //current node counts as a node found
				size_t nodesToBeFound = heightOfTree(this);
				
				//initializes node* lastNodesParent to be the parent of the deepest node
				node* lastNodesParent = this;
				//calls lastNodeParent to reassign lastNodesParent to the appropriate place
				this->lastNodeParent(this, lastNodesParent, nodesFound,nodesToBeFound);

				//prefers left child for value reassignment
				if (lastNodesParent->left != nullptr) {
					value = lastNodesParent->left->value;
					lastNodesParent->left = nullptr;
				}
				else if (lastNodesParent->right != nullptr) {
					value = lastNodesParent->right->value;
					lastNodesParent->right = nullptr;
				}
			}

		};


		/**
		@class iterator
		@brief The iterator class creates a functional iterator for nodes in the bst

		The iterator class privately contains a pointer to a node representing its current location, a pointer to a bst representing its 
		container, and a declaration for bst as a friend class so that bst can access currNode
		It publicly contains a constructor, destructor, and operator overloading for -- (prefix and postfix), ++ (prefix and postfix),
		<, ==, !=, *, and ->. The combination of the aforementioned operators allow for range based traversal of a bst.
		*/

		class iterator {
			
		private:
			node* currNode;
			const bst* container;
			friend class bst;

		public:
			iterator(node* _currNode, const bst* _container) : currNode(_currNode), container(_container){};
			~iterator();

			const iterator operator++(int) { 
				return (++currNode);
			}      // Postfix increment operator.

			const iterator operator--(int) { 
				return (--currNode); 
			}      // Postfix decrement operator.


			iterator& operator++() {
				//if current has a right child
				if (this->currNode->right != nullptr) {
					//move to right
					this->currNode = this->currNode->right;
					//while a left child exists
					while (this->currNode->left != nullptr) {
						//move to left
						this->currNode = this->currNode->left;
					}
					return *this;
				}
				else {
					//else, while current parent exists AND is the right child of parent
					while ((this->currNode->parent != nullptr) && (this->currNode->parent->right == this->currNode)) {
						//move to parent
						this->currNode = this->currNode->parent;
					}
					//if current's parent still exists (aka not root)
					if (this->currNode->parent != nullptr) {
						//move to parent and end
						this->currNode = this->currNode->parent;
						return *this;
					}
					//else, return one space past end
					else {
						this->currNode = nullptr;
						return *this;
					}
				}
			}

			//overloaded prefix --
			iterator& operator--() {
				//if current has a left child
				if (this->currNode->left != nullptr) {
					//move to left
					this->currNode = this->currNode->left;
					//while a right child exists
					while (this->currNode->right != nullptr) {
						//move to right
						this->currNode = this->currNode->right;
					}
					return *this;
				}
				else {
					//else, while current parent exists AND is the left child of parent
					while ((this->currNode->parent != nullptr) && (this->currNode->parent->left == this->currNode)) {
						//move to parent
						this->currNode = this->currNode->parent;
					}
					//if current's parent still exists (aka not root)
					if (this->currNode->parent != nullptr) {
						//move to parent and end
						this->currNode = this->currNode->parent;
						return *this;
					}
					//else, return one space past end
					else {
						this->currNode = nullptr;
						return *this;
					}
				}
			}



			bool operator<(const iterator& it1) const {
				// pred for comparison of lhs and rhs
				return pred(currNode->value, it1.currNode->value);
			}

			// Declare and define equals and not equals operators.
			bool operator==(const iterator& it1) const {
				if(it1.currNode != nullptr && currNode != nullptr){ //ensures no bad access
					if (!(currNode < it1.currNode) &&
						!(it1.currNode < currNode)) {
						return true; // utilizes < to check equality
					}
					else { return false; }
				}
				else { return false; } //if currNodes are nullptr returns false by default
			}
			bool operator!=(const iterator& it1) const {
				if (it1.currNode != nullptr && currNode != nullptr) { //ensures no bad access
					return !(currNode->value == it1.currNode->value); //utilizes == to check inequality
				}
				else {
					//if one currNode is nullptr and other is not, return true
					if ( (it1.currNode == nullptr && currNode != nullptr ) ||
						(it1.currNode != nullptr && currNode == nullptr)) { return true; } 
					else { return false; }
				}
			}

			// Declare and define dereferencing  and arrow operator.

			const T& operator*() const { return this->currNode->value; }

			const T* operator->() const { return &this->currNode->value; }



		};

		//constructors, which default pred to less
		bst<T, C>();
		bst<T, C>(C _pred) : pred(_pred), root(nullptr) {};

		//swap constructor
		bst<T, C>(bst&& rhs) noexcept : bst() {
			std::swap(root, rhs.root);
		};

		//copy constructor
		bst<T, C>(const bst& rhs) noexcept : bst() {
			root = rhs.root;
		};

		//copy operator
		bst& operator=(const bst& rhs) {
			root = rhs.root;
			return *this;
		}

		//move operator
		bst& operator=(bst&& rhs) {
			root = std::move(rhs.root);
			return *this;
		}

		//swap func
		void swap(bst&& rhs) {
			std::swap(this, rhs);
		}

		//destructor which calls recursive destruction on root
		~bst() {
			if (root != nullptr) { 
				try { root->recursively_destroy(root); }
				catch(...) {} // std::cerr << "bad_alloc caught: " << ba.what() << '\n'; is what I want, but no iostream allowed
			}
				 
			}

		//insertion call for 1 arg which forwards to 2 arg insert with root as refNode
		void insert(const T& data) {
			insert(data, root);
		}

		/**
		insertion call for 2 args which initializes a pointer to a new node with type T data and calls
		insertNode on specified refNode for proper insertion
		@tparam data is an arbitrary type of data for insertion in the tree
		@param refNode is the node on which we want to call insertNode later for insertion
		*/
		void insert(const T& data, node*& refNode) {
			//initialization of new node
			node* new_node = new node(data, pred);
			//if root, or specified refNode, is nullptr, place new_node at refNode (root by default)
			if (refNode == nullptr) { refNode = new_node; }
			//else call the recursive insert_node function on new_node to find appropriate location
			else { 
				node* new_node2 = refNode->find(data); //call find to ensure no duplicates
				if (new_node2->value!=data) { //compare find return
					refNode->insertNode(new_node, refNode);
				}
				else { return; }
			}
		}


		//creates template for variadic parameters
		template<typename... args>
		/**
		creates a new object based on variadic list of params and calls insert for tree insertion
		@tparam arg1 is our variadic list of parameters that are appropriately forwarded to create
		an object, and then inserted into the tree
		*/
		void emplace(args&&... arg1) {
			//forwards args from variadic list into T constructor
			T forAddition(std::forward<args>(arg1)...);
			//calls insert to use standard method of insertion on newly constructed T
			this->insert(forAddition);
		}

		/**
		returns starting point for the tree. based on how we defined our iterator traversal, 
		this is an iterator that points to the root node.
		@return startNode is root node iterator
		*/
		iterator begin() {
			node* smallestVal = root;
			while (smallestVal->left != nullptr) { smallestVal = smallestVal->left; }
			iterator startNode(smallestVal, this);
			return startNode;
		}

		/**
		returns end point for the tree. based on how we defined our iterator traversal,
		this is an iterator that points to a nullptr.
		@return endNode is a nullptr iterator
		*/
		iterator end() {
			iterator endNode(nullptr, this);
			return endNode;
		}

		/**
		returns iterator corresponding to specified data in the tree
		@tparam data is an arbitrary type of data for locating in the tree
		@param first is an iterator asking where to start looking, defaults to root iterator
		@return an iterator corresponding to nullptr or the value specified
		*/
		iterator find(const T& data, iterator first = begin()) {
			//calls find on node in first to check against data
			node* checkVal = first->currNode->find();
			//initializes an iterator for return
			iterator forReturn(first->currNode,this);
			if (checkVal->value != data) {
				//sets y ptr to nullptr if data is not found
				forReturn->currNode = nullptr; 
				return forReturn;
			}
			else { return forReturn; }
		}

		/**
		strongly inspired and commented by https://www.geeksforgeeks.org/fix-two-swapped-nodes-of-bst/
		utility/helper function for correctBst(), finds two swapped nodes of the tree
		@param root is where we want to start looking for the swapped nodes
		@param first is a pointer to the first BST rule violation node
		@param middle is a pointer to the second BST rule violation node
		@param last is a pointer to an optional third BST rule violation node
		@param prev is a pointer to a node pointer representing the last node we looked at
		*/
		void correctBSTUtil(node* root, node** first,
			node** middle, node** last,
			node** prev)
		{
			if (root != nullptr)
			{
				// Recur for the left subtree 
				if (root->left != nullptr) { correctBSTUtil(root->left, first, middle, last, prev); }
				
				// If this node is smaller than the previous node, it's violating 
				// the BST rule. 
				if (*prev != nullptr) {
 					if (root->pred(root->value, (*prev)->value))
					{
						// If this is first violation, mark these two nodes as 
						// 'first' and 'middle' 
						if (!*first)
						{
							*first = *prev;
							*middle = root;
						}

						// If this is second violation, mark this node as last 
						else
							*last = root;
					}

					
				}
				// Mark this node as previous 
				*prev = root;
		
				// Recur for the right subtree 
				if(root->right != nullptr) { correctBSTUtil(root->right, first, middle, last, prev); }
			}
		}

		/**
		strongly inspired and commented by https://www.geeksforgeeks.org/fix-two-swapped-nodes-of-bst/
		driver function for correctBSTUtil(), swaps nodes violating BST rules
		@param root is where we want to start looking for the swapped nodes
		*/
		void correctBST(node* root)
		{
			// Initialize pointers needed for correctBSTUtil() 
			node* first, * middle, * last, * prev;
			first = middle = last = prev = NULL;

			// Set the poiters to find out two nodes 
			correctBSTUtil(root, &first, &middle, &last, &prev);

			// Fix (or correct) the tree 
			if (first && last) {
				T tmp = first->value;
				first->value = last->value;
				last->value = tmp;
			}

			else if (first && middle) {// Adjacent nodes swapped 
				T tmp = first->value;
				first->value = middle->value;
				middle->value = tmp;
			}

			// else nodes have not been swapped, passed tree is really BST. 
		}

		/**
		erases current node in iterator and replaces with child or nullptr
		*/
		void erase(iterator& it) {
			it.currNode->node_swap();
			correctBST(it.currNode);
		}

		//calls root size function
		size_t size() {
			return root->size(root);
		}

	private:
		node* root;
		C pred;
		friend class node;
		friend class iterator;

	
	};

	
	//inline stuff for proper building, recommended by VS 
	template<typename T, typename C>
	inline bst<T, C>::bst()
	{
	}

	template<typename T, typename C>
	inline bst<T, C>::iterator::~iterator()
	{
	}

}


#endif




