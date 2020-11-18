#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include<vector>
#include<utility>
#include<stdexcept>

namespace pic10c 
{
	template<typename T, typename C = std::less<T>>
	class max_heap {
	private:
		std::vector<T> values;
		C pred;



		/*
		Following Code (until public) was inspired by 
		https://www.techiedelight.com/min-heap-max-heap-implementation-c/
		*/
		//define positions for the heap
		int parent(const int i) { return (i - 1) / 2; }
		int left  (const int i) { return (2*i + 1);   }
		int right (const int i) { return (2*i + 2);   }

		//downward recursion for correction
		void parent_demotion(const int& i) 
		{
			//define children of parent, and parent as max
			int l = left(i);
			int r = right(i);
			int max = i;

			//check if children are larger or smaller, and set max if so
			if (l < this->size() && pred(values[i],values[l])) max = l;
			if (r < this->size() && pred(values[i],values[r])) max = r;

			//swap parent with larger or smaller child, and recursively repeat
			if (max != i) {
				std::swap(values[i], values[max]);
				parent_demotion(max);
			}
		}

		//upward recursion for insertion
		void child_promotion(const int& i)
		{
			//check if parent is smaller
			if (pred(values[parent(i)], values[i]))
			{
				//swap if parent is larger, then do it again
				std::swap(values[i], values[parent(i)]);
				child_promotion(parent(i));
			}
		}

	public:
		//constructor, which defaults pred to less
		max_heap<T, C>();
		max_heap<T, C>(C _pred) : pred(_pred) {};
		
		
		//insertion call, which calls our recursive promotion function
		void insert(const T& data) {
			values.push_back(data);
			child_promotion(this->size() - 1);
		}
		
		//pop call, which calls our recursive correction function
		void pop() {
			if (this->size() > 0) { //ensures heap is not empty
				//replaces front of vector with value from back, then pops vector.
				//this allows for the correction function to fix heap order
				values[0] = values.back();
				values.pop_back();
				parent_demotion(0);
			}

			//throws error for empty heap
			else { throw std::logic_error("Heap is out of elements to pop!"); }
		}

		//creates template for variadic parameters
		template<typename... args>
		void emplace(args&&... arg1) {
			//forwards args from variadic list into T constructor
			T forAddition (std::forward<args>(arg1)...);
			//calls insert to use standard method of insertion on newly constructed T
			this->insert(forAddition);
		}
	
		//top function, which returns max value from heap
		T top() const {
			if (this->size() > 0) { //ensures heap is not empty
				return values[0];
			}
			//throws error for empty heap
			else { throw std::logic_error("Heap is out of elements for top!"); }
		}

		//size function, which checks size of values vector
		int size() const {
			return values.size();
		}

	};

	
	template<typename T, typename C>
	inline max_heap<T, C>::max_heap()
	{
	}

}

#endif
