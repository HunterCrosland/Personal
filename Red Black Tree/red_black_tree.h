/*******************************************************
 * Copyright (C) Hunter Crosland <hunter@croslands.com>
 *
 * This file is part of my work at UCLA.
 *
 * rbt.h can not be copied and/or distributed without the express
 * permission of Hunter Crosland
 *******************************************************/

 /**
	The following is an implementation of the red-black tree data structure.
*/

#ifndef RBT_H
#define RBT_H

#include<utility>
#include<stdexcept>
#include<iostream>

namespace pic10c
{
	/**
	template required for arbitrary type storage in the rbt, and C allows for user to define
	a comparison function appropriate for whatever type T may be
	 */
	template<typename T, typename compare_type = std::less<T>>

	/**
	@class rbt
	@brief The rbt class is a Binary Search Tree of templated T values

	The rbt class contains a pointer to a root node, defaulting to nullptr, a pred comparison function, defaulting to std::less<T>, rule of 5
	constructors and operator overloading, a recursive destructor, insert/emplace functions for creation of nodes, an erase function to elim
	nodes pointed to by iterators (defined later), begin and end functions to return iterators to beginning and end (past-the-end)
	nodes of the tree, a size function that recursively parses the tree nodes to count its members, a find function that returns
	an iterator if any node in the tree contains a value T passed into find.
	*/
	class rbt {

	public:

		/**
		@class node
		@brief The node class is a nested class in rbt for holding T values and providing infrastructure for the tree

		The node class contains a value of type T, pointers to children and parent, and the pred C comparator. node also declares iterator
		and rbt as friend classes so that node->left, etc. can be utilized by functions in those two classes.
		Note also contains a constructor for values of type T, with a default left,right,parent construction that
		defaults said pointers to nullptr, and pred to whatever value of pred is contained in rbt. It also contains a destructor, and functions
		size that allows for the rbt::size function to operate properly, insertNode that allows for the rbt::insert
		and rbt::emplace functions to operate properly, find for the rbt::find function to operate properly, and a recursive
		destruction function to allow for tree branch-cutting.
		*/
		class node {
		private:
			T value;
			node* left;
			node* right;
			node* parent;
			node* sibling;
			node* grandparent;
			node* uncle;
			compare_type pred;

			bool black;
			rbt* container;

			//friend class templated_iterator;
			friend class rbt;

		public:
			node(T data, compare_type _pred, rbt* _container) : value(data), left(nullptr), right(nullptr), parent(nullptr),
				sibling(nullptr), grandparent(nullptr), uncle(nullptr), pred(_pred), container(_container), black(false) {};
			~node() {}

			//following constructors are not strictly necessary, but abide by rule of 5.
			//swap constructor
			node(node&& rhs) noexcept : node() {
				std::swap(value, rhs.value);
				std::swap(left, rhs.left);
				std::swap(right, rhs.right);
				std::swap(parent, rhs.parent);
				std::swap(sibling, rhs.sibling);
				std::swap(grandparent, rhs.grandparent);
				std::swap(uncle, rhs.uncle);
				std::swap(pred, rhs.pred);
				std::swap(container, rhs.container);
				std::swap(black, rhs.black);
			};

			//copy constructor for deep copies
			node(const node& rhs) noexcept : node() {
				this = node_copy(*rhs);
				set_parents_siblings_uncles(this);
				container = rhs.container;
				pred = rhs.pred;
				black = rhs.black;
			};

			//copy operator for deep copies
			node& operator=(const node& rhs) {
				this = node_copy(*rhs);
				set_parents_siblings_uncles(this);
				pred = rhs.pred;
				container = rhs.container;
				black = rhs.black;
				return *this;
			}

			//move operator
			node& operator=(node&& rhs) {
				value = std::move(rhs.value);
				left = std::move(rhs.left);
				right = std::move(rhs.right);
				parent = std::move(rhs.parent);
				sibling = std::move(rhs.sibling);
				grandparent = std::move(rhs.grandparent);
				uncle = std::move(rhs.uncle);
				pred = std::move(rhs.pred);
				container = std::move(rhs.container);
				black = std::move(rhs.black);
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

			/*
			Recursive copy function for creation of deep copies
			@param to_copy is a const node* that will be copied 
			@return is the new node* whose data arose from to_copy
			*/
			node* node_copy(const node* to_copy) {
				//past the end checker
				if (to_copy == nullptr) {
					return nullptr;
				}
				node* new_node = new node;
				new_node->value = to_copy->value;

				//recursively call left and right children for deep copying
				new_node->left->node_copy(to_copy->left);
				new_node->right->node_copy(to_copy->right);
				return new_node;
			}

			void set_parents_siblings_uncles(node* curr) {
				if (curr == nullptr) { return; }

				if (curr->left != nullptr) {
					curr->left->sibling = curr->right;
					if (curr->right != nullptr) { 
						if (curr->left->sibling != nullptr) { 
							curr->left->sibling->sibling = nullptr; 
						}
						curr->right->sibling = curr->left; 
					}
					curr->left->parent = curr;
					curr->left->grandparent = curr->parent;
					curr->left->uncle = curr->sibling;
					set_parents_siblings_uncles(curr->left);
				}
				if (curr->right != nullptr) {
					curr->right->sibling = curr->left;
					if (curr->left != nullptr) { 
						if (curr->right->sibling != nullptr) {
							curr->right->sibling->sibling = nullptr;
						}
						curr->left->sibling = curr->right; 
					}
					curr->right->parent = curr;
					curr->right->grandparent = curr->parent;
					curr->right->uncle = curr->sibling;
					set_parents_siblings_uncles(curr->right);
				}
			}


			/**
			Safe node insertion function used by rbt::insert()
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
					else { prevNode->right = (refNode); } // insert right instead
					refNode->set_parents_siblings_uncles(refNode->parent);
					if (!(refNode->parent->black)) { 
						colorCorrect(refNode);
						refNode->set_parents_siblings_uncles(refNode->parent);
					}
					

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

			/*
			Left rotation function to help rebalance trees
			@brief promotes node on which rotation is called to its parent's rank and demotes its parent to its child,
			rotation happens such that child on right of parent moves to parent's location and parent moves left and down,
			children of new parent become children of old parent
			*/
			void rotateLeft() {
				//checks if parent is root
				if (this->grandparent != nullptr) {
					//checks if parent is right child
					if (this->grandparent->right == this->parent) {
						//symbolic copy of grandparent for syntactical ease
						node* _grandparent = this->grandparent;  
						//symbolic copy of parent for syntactical ease
						node* _parent = this->parent;
						
						//performs rotation
						_grandparent->right = this;
						this->parent = _grandparent;
						_parent->right = this->left;
						if (_parent->right != nullptr) { _parent->right->parent = parent; }
						this->left = _parent;
						_parent->parent = this;

						//performs recoloring
						bool b1 = _parent->black;
						bool b2 = this->black;
						this->black = b1;
						_parent->black = b2;

						//resets relationships for future rebalancing
						set_parents_siblings_uncles(_parent->parent);
						set_parents_siblings_uncles(_grandparent);
						
					}
					//checks if parent is left child
					else if (this->grandparent->left == this->parent) {
						//symbolic copy of grandparent for syntactical ease
						node* _grandparent = this->grandparent;
						//symbolic copy of parent for syntactical ease
						node* _parent = this->parent;

						//performs rotation
						_grandparent->left = this;
						this->parent = _grandparent;
						_parent->right = this->left;
						if (_parent->right != nullptr) { _parent->right->parent = _parent; }
						this->left = _parent;
						_parent->parent = this;

						//performs recoloring
						bool b1 = _parent->black;
						bool b2 = this->black;
						this->black = b1;
						_parent->black = b2;

						//resets relationships for future rebalancing
						set_parents_siblings_uncles(_parent->parent);
						set_parents_siblings_uncles(_grandparent);
					}

					//grandparent exists, but parent is not child of grandparent -- something went wrong
					else { std::cout << "something went wrong during left rotation."; }
				}

				//parent is root
				else {
					//symbolic copy of parent for syntactical ease
					node* _parent = this->parent;

					//performs rotation
					_parent->right = this->left;
					if (_parent->right != nullptr) { _parent->right->parent = _parent; }
					this->left = _parent;
					_parent->parent = this;

					//this is now root node
					this->parent = nullptr;
					this->container->root = this;
					this->sibling = nullptr;
					this->uncle = nullptr;

					//performs recoloring
					bool b1 = _parent->black;
					bool b2 = this->black;
					this->black = b1;
					_parent->black = b2;

					//resets relationships for future rebalancing
					set_parents_siblings_uncles(this);
				}
				
			}

			/*
			Right rotation function to help rebalance trees
			@brief promotes node on which rotation is called to its parent's rank and demotes its parent to its child,
			rotation happens such that child on left of parent moves to parent's location and parent moves right and down,
			children of new parent become children of old parent
			*/
			void rotateRight() {
				//checks if parent is root
				if (this->grandparent != nullptr) {
					//parent is right child
					if (this->grandparent->right == this->parent) {
						//symbolic copy of grandparent for syntactical ease
						node* _grandparent = this->grandparent;
						//symbolic copy of parent for syntactical ease
						node* _parent = this->parent;

						//performs rotation
						_grandparent->right = this;
						this->parent = _grandparent;
						_parent->left = this->right;
						if (_parent->left != nullptr) { _parent->left->parent = parent; }
						this->right = _parent;
						_parent->parent = this;

						//performs recoloring
						bool b1 = _parent->black;
						bool b2 = this->black;
						this->black = b1;
						_parent->black = b2;

						//resets relationships for future rebalancing
						set_parents_siblings_uncles(_parent->parent);
						set_parents_siblings_uncles(_grandparent);
					}
					//parent is left child
					else if (this->grandparent->left == this->parent) {
						//symbolic copy of grandparent for syntactical ease
						node* _grandparent = this->grandparent;
						//symbolic copy of parent for syntactical ease
						node* _parent = this->parent;

						//performs rotation
						_grandparent->left = this;
						this->parent = _grandparent;
						_parent->left = this->right;
						if (_parent->left != nullptr) { _parent->left->parent = _parent; }
						this->right = _parent;
						_parent->parent = this;

						//performs recoloring
						bool b1 = _parent->black;
						bool b2 = this->black;
						this->black = b1;
						_parent->black = b2;

						//resets relationships for future rebalancing
						set_parents_siblings_uncles(_parent->parent);
						set_parents_siblings_uncles(_grandparent);
					}
					else { std::cout << "something went wrong during right rotation."; }
				}

				//parent is root
				else {
					//symbolic copy of grandparent for syntactical ease
					node* _parent = this->parent;

					//performs rotation
					_parent->left = this->right;
					if (_parent->left != nullptr) { _parent->left->parent = _parent; }
					this->right = _parent;
					_parent->parent = this;

					//this is now root node
					this->parent = nullptr;
					this->container->root = this;
					this->sibling = nullptr;
					this->uncle = nullptr;

					//performs recoloring
					bool b1 = _parent->black;
					bool b2 = this->black;
					this->black = b1;
					_parent->black = b2;

					//resets relationships for future rebalancing
					set_parents_siblings_uncles(this);
				}
			}

			/**
			Color correction function to aid in insertion
			@brief function recolors if the tree only needs to be recolored, otherwise 
			calls rebalancing (rotation) functions to sort the tree.
			@param refNode is a semi-arbitrary entry-point for color correction
			*/
			void colorCorrect(node* refNode) {

				//if rootNode
				if (refNode->parent == nullptr) { 
					refNode->black = true; // root must be black
					refNode->container->root = refNode; // node is now container's root
					return; 
				}
				if (refNode->parent->black) { return; } // arbitrary call, nothing needs to be done
				else {
					//if refNode has uncle, ask control flow for rotations/recursions
					if (refNode->uncle != nullptr) {
						//if node has black uncle, we need rotations on a case-by-case basis
						if (refNode->uncle->black) {
							// right right case, color correction happens in rotation
							if (refNode->parent->right == refNode && refNode->grandparent->right == refNode->parent) {
								refNode->parent->rotateLeft();
							}
							// right left case, color correction happens in rotation
							else if (refNode->parent->left == refNode && refNode->grandparent->right == refNode->parent) {
								refNode->rotateRight();
								refNode->rotateLeft();
							}
							// left left case, color correction happens in rotation
							else if (refNode->parent->left == refNode && refNode->grandparent->left == refNode->parent) {
								refNode->parent->rotateRight();
							}
							//left right case, color correction happens in rotation
							else if (refNode->parent->right == refNode && refNode->grandparent->left == refNode->parent) {
								refNode->rotateLeft();
								refNode->rotateRight();
							}

						}
						//if node has red uncle, we can simply recolor and recurse
						else {
							refNode->parent->black = true;
							refNode->uncle->black = true;
							refNode->grandparent->black = false;
							colorCorrect(refNode->grandparent);
						}
					}
					//if node does not have an uncle, we simply need to rotate.
					else {
						if (refNode->grandparent->right == refNode->parent) {
							//color correction happens in rotation
							refNode->parent->rotateLeft();
						}
						else {
							//color correction happens in rotation
							refNode->parent->rotateRight();
						}
					}
				}
			}

			/**
			Find function used by iterator rbt::find()
			@param T is the value we are looking for in the tree
			@return is a node corresponding to the value, if found
			*/

			node* find(T data) {
				//if input smaller than current node value
				if (pred(data, value)) {
					if (left == nullptr) { //cannot continue 
						return nullptr; // value not found, return nullptr
					}
					//call find recursively to further cycle through rbt and search for value
					else {
						return left->find(data);
					}
				}
				//if input larger than current node value
				else if (pred(value, data)) {
					if (right == nullptr) { //cannot continue
						return nullptr; // value not found, return nullptr
					}
					//call find recursively to further cycle through rbt and search for value
					else {
						return right->find(data);
					}
				}
				//data and value match
				else {
					return this; //return this for comparison
				}
			}

			/**
			recursively destroy for tree-branch cutting, used in ~rbt()
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
				if (root->left != nullptr) { lastNodeParent(root->left, forAssignment, forPassage, nodesToBeFound); }

				//if we are at max depth, assign 
				if (nodesFound == nodesToBeFound) { forAssignment = root->parent; }
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
					depthFinder(root->right, level, lastLevel);
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
			helper function for node_remove, deals with dbl black correction
			@brief deals with several cases based on a strict control flow for alleviating double-black
			@param dbl_black_node is a node corresponding to the node colored double-black.
			*/
			void dbl_black_delete(node* dbl_black_node) {

				//simply discard double-blackness of root
				if (dbl_black_node->parent == nullptr) { return; }

				//symbolic nodes for syntactical ease
				node* _sibling = dbl_black_node->sibling;
				node* _parent = dbl_black_node->parent;

				//promote double-blackness if dbl_black_node has no siblings
				if (_sibling == nullptr) {
					dbl_black_delete(_parent);
				}
				else {
					//if sibling is black
					if (_sibling->black == false) {
						// and sibling is left child
						if (_parent->left == _sibling) {
							//requires rotation, color correction happens in rotation
							_sibling->rotateRight();
						}
						// and sibling is right child
						else {
							//requires rotation, color correction happens in rotation
							_sibling->rotateLeft();
						}
						//re-call delete, since double-blackness needs alleviation still
						dbl_black_delete(dbl_black_node);
					}
					//if sibling is red
					else {
						//check sibling children status
						if (_sibling->left != nullptr && _sibling->left->black == false) {
							if (_parent->left == _sibling) {
								//requires rotation, color correction happens in rotation
								_sibling->rotateRight();
							}
							else {
								//requires rotation, color correction happens in rotation
								_sibling->rotateLeft();
							}
							//recolor parent
							_parent->black = true;
						}
						else if (_sibling->right != nullptr && _sibling->right->black == false) {
							if (_parent->left == _sibling) {
								//requires rotation, color correction happens in rotation
								_sibling->right->rotateLeft();
								_sibling->rotateRight();
							}
							else {
								//requires rotation, color correction happens in rotation
								_sibling->rotateLeft();
							}
							//recolor parent
							_parent->black = true;
						}

						//sibling has two children
						else if (_sibling->right != nullptr && _sibling->left != nullptr){
							// recolor sibling
							_sibling->black = false;
							// promote double-blackness
							if (_parent->black == true) {
								dbl_black_delete(_parent);
							}
							// recolor parent, done
							else { _parent->black = true; }
						}

						//sibling has no children
						else {
							//parent is red, recolor
							if (!(parent->black)) {
							_parent->black = true;
							_sibling->black = false;
							}
							//parent is black, promote double-blackness and recolor sibling
							else { 
								_sibling->black = false;
								dbl_black_delete(parent); 
							}
						}
					}


				}
			}

			/**
			function to remove nodes and re-balance tree
			@brief function is called on desired node to be deleted by erase in the templated_iterator class.
			*/
			void node_remove() {
				//node to be removed is black
				if (this->black) { // all black cases
					//has no kids, is black, follow double black procedure
					if (this->right == nullptr && this->left == nullptr) {
						//node for deletion is root
						if (this->parent == nullptr) {
							this->container->root = nullptr;
							return;
							//returns for ptr freedom
						}
						//node for deletion is not root
						else {
							//mark node double black, call double black helper function
							dbl_black_delete(this);
							if (this->parent->left == this) {
								//reset relations
								this->parent->left = nullptr;
								if (this->sibling != nullptr) { this->sibling->sibling = nullptr; }
							}
							else {
								//reset relations
								this->parent->right = nullptr;
								if (this->sibling != nullptr) { this->sibling->sibling = nullptr; }
							}
						}
					}
					// has one child (left)
					else if (this->right == nullptr && this->left != nullptr) {
						//root check
						if (this->parent == nullptr) {
							//reset container's root
							this->left->container->root = this->left;

							//reset relations, color new root black
							this->left->parent = nullptr;
							if (this->left->black) { return; }
							else { this->left->black = true; return; }
							
							//returns for ptr freedom
						}
						// sub-case 1: delete node and child are black
						if (this->left->black) {
							//mark node as double black, call double black helper function
							dbl_black_delete(this);

							//reset relations
							if (this->parent->left == this) {
								this->parent->left = this->left;
								set_parents_siblings_uncles(this->parent);
							}
							else {
								this->parent->right = this->left;
								set_parents_siblings_uncles(this->parent);							
							}
						}
						// sub-case 2: delete node is black, child is red
						else {
							//sub-case 1: this is left child of parent, replacing this w left child
							if (this->parent->left == this) {

								//set new relations
								this->parent->left = this->left;
								this->left->parent = this->parent;
								this->left->black = true;

								//need to set new grandparents and uncles for later rebalancing
								set_parents_siblings_uncles(this->left->parent);
								if (this->left->right != nullptr) { set_parents_siblings_uncles(this->left); }
								else if (this->left->left != nullptr) { set_parents_siblings_uncles(this->left); }
							}
							//sub-case 2: this is right child of parent, replacing this w left child
							else {

								//set new relations
								this->parent->right = this->left;
								this->left->parent = this->parent;
								this->left->black = true;

								//need to set new grandparents and uncles for later rebalancing
								set_parents_siblings_uncles(this->left->parent);
								if (this->left->right != nullptr) { set_parents_siblings_uncles(this->left); }
								else if (this->left->left != nullptr) { set_parents_siblings_uncles(this->left); }
							}
						}
					
					}
					// has one child (right)
					else if (this->right != nullptr && this->left == nullptr) {
						//root check
						if (this->parent == nullptr) {
							//reset container's root
							this->right->container->root = this->right;

							//reset relations, color new root black
							this->right->parent = nullptr;
							if (this->right->black) { return; }
							else { this->right->black = true; return; }
							//returns for ptr freedom
						}
						// sub-case 1: delete note and child are black
						if (this->right->black) {
							//mark node as double black, call double black helper function
							dbl_black_delete(this);

							//reset relations
							if (this->parent->left == this) {
								this->parent->left = this->right;
								set_parents_siblings_uncles(this->parent);
							}
							else {
								this->parent->right = this->right;
								set_parents_siblings_uncles(this->parent);
							}
						}
						//sub-case 2: delete node is black, child is red
						else {
							//sub-case 1: this is left child of parent, replacing this w right child
							if (this->parent->left == this) {

								//set new relations
								this->parent->left = this->right;
								this->right->parent = this->parent;
								this->right->black = true;

								//need to set new grandparents and uncles for later rebalancing
								set_parents_siblings_uncles(this->right->parent);
								if (this->right->right != nullptr) { set_parents_siblings_uncles(this->right); }
								else if (this->right->left != nullptr) { set_parents_siblings_uncles(this->right); }
							}
							//sub-case 2: this is right child of parent, replacing this w right child
							else {

								//set new relations
								this->parent->right = this->right;
								this->right->parent = this->parent;
								this->right->black = true;

								//need to set new grandparents and uncles for later rebalancing
								set_parents_siblings_uncles(this->right->parent);
								if (this->right->right != nullptr) { set_parents_siblings_uncles(this->right); }
								else if (this->right->left != nullptr) { set_parents_siblings_uncles(this->right); }
							}
						}

					}
					// has two children, recurse
					else {
						node* _root = this;
						node* _copy = this->left;
						while (_copy->right != nullptr) {
							_copy = _copy->right;
						}

						//need to replace current value with copy value
						//(greatest progeny from left child)
						_root->value = _copy->value;

						//kill copy, which has donated its value 
						_copy->node_remove();
					}
				
				}
				//node to be removed is red
				else {
					//has no kids, is red, just delete, no effect on black depth
					if (this->right == nullptr && this->left == nullptr) {
						//reset relations if need be
						if (this->parent->right == this) { this->parent->right = nullptr; }
						else { this->parent->left = nullptr; }
						if (this->sibling != nullptr) { this->sibling->sibling = nullptr; }
					}
					// has one child (left)
					else if (this->right == nullptr && this->left != nullptr) {
						// sub-case 1: delete node is red and child is black
						if (this->left->black) {
							//sub-case 1: this is left child of parent, replacing this w left child
							if (this->parent->left == this) {

								//set new relations
								this->parent->left = this->left;
								this->left->parent = this->parent;

								//need to set new grandparents and uncles for later rebalancing
								set_parents_siblings_uncles(this->left->parent);
								if (this->left->right != nullptr) { set_parents_siblings_uncles(this->left); }
								else if (this->left->left != nullptr) { set_parents_siblings_uncles(this->left); }
							}
							//sub-case 2: this is right child of parent, replacing this w left child
							else {

								//set new relations
								this->parent->right = this->left;
								this->left->parent = this->parent;

								//need to set new grandparents and uncles for later rebalancing
								set_parents_siblings_uncles(this->left->parent);
								if (this->left->right != nullptr) { set_parents_siblings_uncles(this->left); }
								else if (this->left->left != nullptr) { set_parents_siblings_uncles(this->left); }
							}
						}
						// sub-case 2: delete node and child are red, something went wrong in insertion, abort()
						else {
							try
							{
								throw 20;
							}
							catch (const std::exception&)
							{
								std::cout << "Insertion or deletion were incorrect, red-red occurred.";
							}
						}

					}
					// has one child (right)
					else if (this->right != nullptr && this->left == nullptr) {
						// sub-case 1: delete is red and child is black
						if (this->right->black) {
							//sub-case 1: this is left child of parent, replacing this w right child
							if (this->parent->left == this) {

								//set new relations
								this->parent->left = this->right;
								this->right->parent = this->parent;

								//need to set new grandparents and uncles for later rebalancing
								set_parents_siblings_uncles(this->right->parent);
								if (this->right->right != nullptr) { set_parents_siblings_uncles(this->right); }
								else if (this->right->left != nullptr) { set_parents_siblings_uncles(this->right); }
							}
							//sub-case 2: this is right child of parent, replacing this w right child
							else {

								//set new relations
								this->parent->right = this->right;
								this->right->parent = this->parent;

								//need to set new grandparents and uncles for later rebalancing
								set_parents_siblings_uncles(this->right->parent);
								if (this->right->right != nullptr) { set_parents_siblings_uncles(this->right); }
								else if (this->right->left != nullptr) { set_parents_siblings_uncles(this->right); }
							}
						}
						//sub-case 2: delete node and child are red, impossible, abort()
						else {
							try
							{
								throw 20;
							}
							catch (const std::exception&)
							{
								std::cout << "Insertion or deletion were incorrect, red-red occurred.";
							}
						}

					}
					// has two children, recurse
					else {
						node* _root = this;
						node* _copy = this->left;

						//need to replace current value with copy value
						//(greatest progeny from left child)
						while (_copy->right != nullptr) {
							_copy = _copy->right;
						}
						//kill copy, which has donated its value
						_root->value = _copy->value;
						_copy->node_remove();
					}
				} // all red cases
			}

			
		};


		/**
		@class iterator
		@brief The iterator class creates a functional iterator for nodes in the rbt

		The iterator class privately contains a pointer to a node representing its current location, a pointer to a rbt representing its
		container, and a declaration for rbt as a friend class so that rbt can access currNode
		It publicly contains a constructor, destructor, and operator overloading for -- (prefix and postfix), ++ (prefix and postfix),
		<, ==, !=, *, and ->. The combination of the aforementioned operators allow for range based traversal of a rbt.
		*/

		template <bool isConst = false> class templated_iterator {

		private:
			node* currNode;
			const rbt* container;
			friend class rbt;

		public:
			templated_iterator(node* _currNode, const rbt* _container) : currNode(_currNode), container(_container) {};
			~templated_iterator() {};

			//templated_iterator(const templated_iterator<const node*>& rhs) = default;
			templated_iterator(const templated_iterator<true>& rhs) : currNode(rhs.currNode) {}
	

			const templated_iterator operator++(int) {
				return (++currNode);
			}      // Postfix increment operator.

			const templated_iterator operator--(int) {
				return (--currNode);
			}      // Postfix decrement operator.


			templated_iterator& operator++() {
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
			templated_iterator& operator--() {
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



			bool operator<(const templated_iterator& it1) const {
				// pred for comparison of lhs and rhs
				return pred(currNode->value, it1.currNode->value);
			}

			// Declare and define equals and not equals operators.
			bool operator==(const templated_iterator<false>& it1) const {
				if (it1.currNode != nullptr && currNode != nullptr) { //ensures no bad access
					if (!(currNode < it1.currNode) &&
						!(it1.currNode < currNode)) {
						return true; // utilizes < to check equality
					}
					else { return false; }
				}
				else { return false; } //if currNodes are nullptr returns false by default
			}
			bool operator==(const templated_iterator<true>& it1) const {
				if (it1.currNode != nullptr && currNode != nullptr) { //ensures no bad access
					if (!(currNode < it1.currNode) &&
						!(it1.currNode < currNode)) {
						return true; // utilizes < to check equality
					}
					else { return false; }
				}
				else { return false; } //if currNodes are nullptr returns false by default
			}
			bool operator!=(const templated_iterator<false>& it1) const {
				if (it1.currNode != nullptr && currNode != nullptr) { //ensures no bad access
					return !(currNode->value == it1.currNode->value); //utilizes == to check inequality
				}
				else {
					//if one currNode is nullptr and other is not, return true
					if ((it1.currNode == nullptr && currNode != nullptr) ||
						(it1.currNode != nullptr && currNode == nullptr)) {
						return true;
					}
					else { return false; }
				}
			}
			bool operator!=(const templated_iterator<true>& it1) const {
				if (it1.currNode != nullptr && currNode != nullptr) { //ensures no bad access
					return !(currNode->value == it1.currNode->value); //utilizes == to check inequality
				}
				else {
					//if one currNode is nullptr and other is not, return true
					if ((it1.currNode == nullptr && currNode != nullptr) ||
						(it1.currNode != nullptr && currNode == nullptr)) {
						return true;
					}
					else { return false; }
				}
			}

			// Declare and define dereferencing  and arrow operator.

			const T& operator*() const { return this->currNode->value; }

			const T* operator->() const { return &this->currNode->value; }

		};

		
		//typedef templated_iterator<const node*> const_iterator;

		using iterator = templated_iterator<false>;
		using const_iterator = templated_iterator<true>;

		//constructors, which default pred to less
		rbt<T, compare_type>() : pred(compare_type{}), root(nullptr) {};
		rbt<T, compare_type>(compare_type _pred) : pred(_pred), root(nullptr) {};

		//swap constructor
		rbt<T, compare_type>(rbt&& rhs) noexcept : rbt() {
			std::swap(root, rhs.root);
		};

		//copy constructor
		rbt<T, compare_type>(const rbt& rhs) noexcept : rbt() {
			root = rhs.root;
		};

		//copy operator
		rbt& operator=(const rbt& rhs) {
			root = rhs.root;
			return *this;
		}

		//move operator
		rbt& operator=(rbt&& rhs) {
			root = std::move(rhs.root);
			return *this;
		}

		//swap func
		void swap(rbt&& rhs) {
			std::swap(this, rhs);
		}

		//destructor which calls recursive destruction on root
		~rbt() {
			if (root != nullptr) {
				try { root->recursively_destroy(root); }
				catch (...) {} // std::cerr << "bad_alloc caught: " << ba.what() << '\n'; is what I want, but no iostream allowed
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
 		void insert(const T& data, node* refNode) {
			//initialization of new node
			node* new_node = new node(data, pred, this);
			//if root is nullptr, place new_node at root
			if (refNode == nullptr) { 
				new_node->black = true;  
				if (new_node->parent == nullptr) { 
					new_node->container->root = new_node; 
				}
			}
			//else call the recursive insertNode function on new_node to find appropriate location
			else {
				node* new_node2 = refNode->find(data); //call find to ensure no duplicates
				if (new_node2 == nullptr) { //compare find return
					refNode->insertNode(new_node, refNode);
				}
				else { return; }
			}
		}

		/**
		forwarding function for recursive_print function,
		also prints line for ease of eyes
		*/
		void print() {
			recursive_print(root, 0);
			std::cout << "----------------------";
		}

		/**
		recursive function to print tree in a left-right fashion
		@param start is a root from which we will print from left to right
		@param num_spaces is the number of spaces to print before current tree node
		*/
		void recursive_print(const node* start, size_t num_spaces) {
			//already printed root
			if (start == nullptr) {
				return;
			}

			//6 is arbitrary number of spaces to print for aesthetic
			num_spaces += 6;

			//recursively print value to right
			recursive_print(start->right, num_spaces);

			std::cout << '\n';
			//print spaces
			for (int i = 6; i < num_spaces; ++i) {
				std::cout << " ";
			}
			//is root, print -
			if (start->parent == nullptr) {
				std::cout << "-";
			}
			//is right child, print /
			else if (start == start->parent->right) {
				std::cout << '/';
			}

			/* is left child, print \ */
			else {
				std::cout << '\\';
			}
			std::cout << start->value;

			//print colors
			if (!(start->black)) {
				std::cout << "(r)\n";
			}
			else {
				std::cout << "(b)\n";
			}
			//recursively print to left
			recursive_print(start->left, num_spaces);
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
		this is a const_iterator that points to the root node.
		@return startNode is root node const_iterator
		*/
		iterator begin() {
			node* leftmostVal = root;
			while (leftmostVal->left != nullptr) { leftmostVal = leftmostVal->left; }
			iterator startNode(leftmostVal, this);
			return startNode;
		}

		const_iterator begin() const {
			node* leftmostVal = root;
			while (leftmostVal->left != nullptr) { leftmostVal = leftmostVal->left; }
			const_iterator startNode(leftmostVal, this);
			return startNode;
		}


		/**
		returns end point for the tree. based on how we defined our iterator traversal,
		this is a const_iterator that points to a nullptr.
		@return endNode is a nullptr const_iterator
		*/
		const_iterator end() const {
			const_iterator endNode(nullptr, this);
			return endNode;
		}

		/**
		returns end point for the tree. based on how we defined our iterator traversal,
		this is an iterator that points to a nullptr.
		@return endNode is a nullptr const_iterator
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
		iterator find(const T& data) const {
			const_iterator first(root,this);
			//calls find on node in first to check against data
			node* checkVal = first.currNode->find(data);
			//initializes an iterator for return
			const_iterator forReturn(first.currNode, this);
			if (checkVal->value != data) {
				//sets y ptr to nullptr if data is not found
				forReturn.currNode = nullptr;
				return forReturn;
			}
			else { return forReturn; }
		}

		iterator find(const T& data) {
			iterator first(root,this);
			//calls find on node in first to check against data
			node* checkVal = first.currNode->find(data);
			//initializes an iterator for return
			iterator forReturn(checkVal, this);
			if (checkVal == nullptr) {
				//sets y ptr to nullptr if data is not found
				forReturn.currNode = nullptr;
				return forReturn;
			}
			else { return forReturn; }
		}

		/**
		@param it is a ref to an iterator pointing to value for erasure
		erases current node in iterator and replaces with child or nullptr
		*/
		void erase(iterator& it) {
			//forwards to node removal function
			if (it.currNode != nullptr) { it.currNode->node_remove(); }
			
			//prints error for values not in tree
			else {
				int x = -1;
				try
				{
					throw x;
				}
				catch (int x )
				{
					std::cout << "\n Desired erasure was not in tree. Ignoring erasure call. \n";
				}
			}
			//frees node
			node_delete(it.currNode);
		}

		/**
		@param it is a const ref to an iterator pointing to value for erasure
		erases current node in iterator and replaces with child or nullptr
		*/
		void erase(const iterator& it) {
			if (it.currNode != nullptr) { it.currNode->node_remove(); }

			//prints error for values not in tree
			else {
				int x = -1;
				try
				{
					throw x;
				}
				catch (int x)
				{
					std::cout << "\n Desired erasure was not in tree. Ignoring erasure call. \n";
				}
			}
			//frees node
			node_delete(it.currNode);
		}

		/**
		@param currNode is a pointer to a node for erasure
		erases current node in iterator and replaces with child or nullptr
		*/
		void node_delete(node* currNode) {
			currNode = nullptr;
			free(currNode);
		}

		//calls root size function
		size_t size() {
			size_of = root->size(root);
			return size_of;
		}

	private:
		node* root;
		compare_type pred;
		size_t size_of;
		friend class node;
		friend iterator;
		friend const_iterator;


	};

}


#endif




