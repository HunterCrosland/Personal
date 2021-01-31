#include "messenger.h"
#include <limits>

/**
checks if user has input q for program termination
@param is_q is a ref to a bool set in main()
*/
void detect_q(bool& is_q) {

	//init q and cin char
	char q;
	std::cin >> q;

	//continuously run until program closes and thread terminates
	while (true) {
		//if user did not input char or a char other than 'q'
		if (std::cin.fail() || q != 'q') {
			//clear cin and ignore
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			//accept another cin val
			std::cin >> q;
		}
		//user input q, set is_q ref bool to true and break while loop
		//to continue in main method
		else { is_q = true; break; }
	}
	
}

int main() {

	//init messenger and start threads
	messenger my_messenger;

	//init bool for quit_reader
	bool q_detected = false;
	
	//init thread for detect_q
	std::thread quit_reader(detect_q, std::ref(q_detected));

	//start detect_q
	quit_reader.join();

	//q detected, terminate program
	return 0;
}
