#include "messenger.h"

//init mutex for thread locking in the local scope
std::mutex MUTEX;

//constructs with msgChanged == false
messenger::messenger() : msgChanged(false) {
	//init threads
	std::thread msg_reader(&messenger::set_msg, this);
	std::thread msg_printer(&messenger::print_msg, this);

	//detach threads
	msg_reader.detach();
	msg_printer.detach();
}

//default destructor
messenger::~messenger() = default;


void messenger::set_msg() {
	// init thread-local fstream and string
	thread_local std::fstream file;
	thread_local std::string new_msg;

	//continuously run until program closes and threads terminate
	while(true) {
		
		//try to open file
		file.open("to_cpp.txt");

		//if file is found
		if (file.good()) {
			//set string and msgChanged
			std::getline(file, new_msg);
			msg = new_msg;
			set_bool(true);

			//close file and remove it
			file.close();
			std::remove("to_cpp.txt");

			//sleep
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		//if file isn't found
		else {
			//close fstream and sleep
			file.close();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

}

void messenger::print_msg(){
	//continuously run until program closes and threads terminate
	while(true) {
		//if msg has not changed
		if (!get_bool()) {
			//sleep
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		//if msg has changed
		else { 
			//cout msg and reset msgChanged
			std::cout << get_msg() << "\n";
			set_bool(false);
		}
	}
}

//return msg
const std::string messenger::get_msg() const {
	return msg;
}

//set msgChanged
void messenger::set_bool(const bool _bool) {
	//init smart thread-locking mechanism w mutex
	std::lock_guard lock(MUTEX);

	//change msgChanged to desired bool
	msgChanged = _bool;
}

//return msgChanged
const bool messenger::get_bool() const {
	//init smart thread-locking mechanism w mutex
	std::lock_guard lock(MUTEX);

	//return msgChanged
	return msgChanged;
}

