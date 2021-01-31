#include "spread_engine.h"
#include <cmath>

// default constructor inits all size_t values to 0
spread_engine::spread_engine() : 
	initial_sick(0), elapsed_days(0), 
	total_normal(0), susceptible_normal(0), infected_normal(0), removed_normal(0),
	total_moron(0),  susceptible_moron(0),  infected_moron(0),  removed_moron(0) {}

// default destructor
spread_engine::~spread_engine() = default;

template<typename Numeric, typename Generator>
Numeric spread_engine::random(Numeric from, Numeric to)
{
	// inits thread_local generator via random header
	thread_local static Generator gen(std::random_device{}());

	// inits distribution type based on Numeric type
	using dist_type = typename std::conditional<
		std::is_integral<Numeric>::value,
		std::uniform_int_distribution<Numeric>,
		std::uniform_real_distribution<Numeric>
	>::type;

	// inits distribution based on dist_type
	thread_local static dist_type dist;

	// returns Numeric from dist 
	return dist(gen, typename dist_type::param_type{ from,to });
}


void spread_engine::set_initial_populations(const size_t num_normal, const size_t num_moron, const size_t num_sick) {
	//sets private size_t vals based on user input
	total_normal = num_normal;
	total_moron = num_moron;
	initial_sick = num_sick;
}

//standard getters that return private member vars
const size_t spread_engine::get_days_elapsed() const { return elapsed_days; }

const size_t spread_engine::get_susceptible_normal() const { return susceptible_normal; }
const size_t spread_engine::get_infected_normal() const { return infected_normal; }
const size_t spread_engine::get_removed_normal() const { return removed_normal; }

const size_t spread_engine::get_susceptible_moron() const { return susceptible_moron; }
const size_t spread_engine::get_infected_moron() const { return infected_moron; }
const size_t spread_engine::get_removed_moron() const { return removed_moron; }

/**
@struct person
@brief The person struct stores a set of variables, as well as
a network vector, for each person in the simulation.
*/
struct spread_engine::person {

	// bool for determining if person is a moron
	bool is_moron = false;
	// network size is fixed at 9 or 20 based on is_moron, set later
	size_t network_size = 0;

	// size_ts to track number of morons and normals in network
	size_t ill_normal_contacts = 0;
	size_t ill_moron_contacts = 0;

	// network vector based on configuration network 
	std::vector<person*> network;
};

void spread_engine::init_spread_network() {
	// define temp size_t to make sure we get enough morons (set in user input)
	size_t current_morons = 0;
	
	// generate people acc to number of normal and number of moron
	for (size_t i = 0; i < (total_normal + total_moron); ++i) {

		// init person 
		person* new_person = new person;

		// if we do not yet have enough morons
		if (current_morons < total_moron) {
			// make new_person a moron
			new_person->is_moron = true;
			++current_morons;
		} // else, new_person is a normal by default

		// push back new_person to susceptible people and all people
		all_people.push_back(new_person);
		susceptible_people.push_back(new_person);

		// push back new_person need_contacts for network generation
		need_contacts.push_back(new_person);
	}

	// before we initially infect, all normal and morons are susceptible
	susceptible_normal = total_normal;
	susceptible_moron = total_moron;

}

void spread_engine::populate_spread_network() {
	
	std::random_device rd;
	std::mt19937 g(rd());
	// to ensure randomness in assignment we shuffle need_contacts
	// (otherwise morons always get assigned first)
	std::shuffle(need_contacts.begin(), need_contacts.end(), g);

	// while networks are not filled
	while (need_contacts.size() > 1) {

		// first we fill first person in the need_contacts vector 
		person* current_person = need_contacts[0];
		// create set so we uniquely assign people from the need_contacts 
		// vector to each person's network
		std::set<size_t> indices_for_networking;

		// size_t which corresponds to desired number of network contacts
		// based on if person at first index is a moron and how many contacts 
		// person has already
		size_t desired_num_indices = [&current_person]{

			// if moron, return C_m = 20 minus current contacts
			if (current_person->is_moron) { return (moron_contacts  - current_person->network_size); }
			// else is normal, return C_m = 9 minus current contacts
			else                          { return (normal_contacts - current_person->network_size); }
		
		} ();

		// upper limit size_t for random generator
		size_t upper_limit = need_contacts.size() - 1;

		// while we have less indices than we want in the set
		while ((indices_for_networking.size() < desired_num_indices) && 
			// and the set is not larger than the need_contacts vector minus 1
			!(need_contacts.size() - 1 <= indices_for_networking.size())) {
			// insert random index from one to upper limit (0 is current person)
			indices_for_networking.insert(spread_engine::random(static_cast<size_t>(1), upper_limit));
		}

		// for all indices
		for (size_t index : indices_for_networking) {
			
			// put person at index in need_contacts into current_person's network
			current_person->network.push_back(need_contacts[index]);
			// put current_person into person at index's network
			need_contacts[index]->network.push_back(current_person);

			//increase network_size of both
			current_person->network_size++;
			need_contacts[index]->network_size++;

			// must check if we have filled the network of need_contacts[index]
				// if person is moron and network size == max_moron contacts OR
			if ((need_contacts[index]->is_moron && need_contacts[index]->network_size == moron_contacts) ||
				// if person is normal and network size == max_normal contacts
				(!need_contacts[index]->is_moron && need_contacts[index]->network_size == normal_contacts)) {
				// swap with nullptr (need to preserve need_contacts.size()
				need_contacts[index]=nullptr;
			}

		}

		// once person at index 0 has a full network, erase them from need_contacts
		need_contacts.erase(need_contacts.begin());

		// remove all nullptr values that are now full, if there are any
		need_contacts.erase(std::remove(need_contacts.begin(), need_contacts.end(), nullptr),
			need_contacts.end());

	}

	// get rid of anything left in need_contacts and get rid of memory allocation for it
	need_contacts.clear();
	need_contacts.shrink_to_fit();

}

void spread_engine::randomly_infect_healthy() {
	// if user specified they wanted initial_sick to be less than total population
	if (initial_sick < all_people.size()) {
		
		// init set for indices of first sick people
		std::set<size_t> indices_of_sick;

		// set upper limit for random number generator
		size_t upper_limit = all_people.size() - 1;

		// insert random values within the range of all_people into indices_of_sick
		// until we have # size corresponding to initial_sick
		while (indices_of_sick.size() < initial_sick) {
			indices_of_sick.insert(spread_engine::random(static_cast<size_t>(0),upper_limit));
		}

		// for every index, mark person as infected, remove them from susceptible,
		// and place them into infected
		for (size_t index : indices_of_sick) {
			susceptible_people[index] = nullptr;
			update_people_contacts(all_people[index], true);
		}
	}

	// else user specified they wanted either whole population or 
	// more than whole population sick, so just make everyone sick
	else {
		// for person in all person, mark them sick, put them in infected,
		// and update their network
		for (person* my_person: all_people) {
			update_people_contacts(my_person, true);
		}
		// remove them from susceptible
		for (size_t i = 0; i < susceptible_people.size(); ++i) {
			susceptible_people[i] = nullptr;
		}
	}
	
	// remove all nullptrs from susceptible_people that we created above
	susceptible_people.erase(std::remove(susceptible_people.begin(), susceptible_people.end(), nullptr),
		susceptible_people.end());

}


void spread_engine::update_people_contacts(person* for_updating, bool is_sick) {
	
	// if person is supposed to be infected
	if (is_sick) {

		// add them to infected people vector
		infected_people.push_back(for_updating);

		// if the person is a moron
		if (for_updating->is_moron) {
			// for everyone in their network
			for (person* has_infected_contact : for_updating->network) {
				// add one moron ill contact to each
				has_infected_contact->ill_moron_contacts++;
			}
			// decrement susceptible moron count and increment infected moron count
			susceptible_moron--;
			infected_moron++;
		}
		else { //else the new person is a normal
			// for everyone in their network
			for (person* has_infected_contact : for_updating->network) {
				// add one normal ill contact to each
				has_infected_contact->ill_normal_contacts++;
			}
			// decrement susceptible normal count and increment infected normal count
			susceptible_normal--;
			infected_normal++;
		}

		
	}

	// else the person is supposed to be removed
	else {

		// if the person is a moron
		if (for_updating->is_moron) {
			// for everyone in their network
			for (person* has_infected_contact : for_updating->network) {
				// subtract one moron ill contact from each
				has_infected_contact->ill_moron_contacts--;
			}
			// decrement infected moron count and increment removed moron count
			infected_moron--;
			removed_moron++;
		}
		// else the person is a normal
		else {
			// for everyone in their network
			for (person* has_infected_contact : for_updating->network) {
				// subtract one normal ill contact from each
				has_infected_contact->ill_normal_contacts--;
			}
			// decrement infected normal count and increment removed normal count
			infected_normal--;
			removed_normal++;
		}	
	}
}

void spread_engine::infect_healthy_people() {

	// delta_t (really not strictly necessary due to being 1.0)
	// for exp() expression
	static constexpr double delta_t = 1.0;

	// for everyone in the susceptible_people vector
	for (size_t i = 0; i < susceptible_people.size(); ++i) {
		// if person has any contact with ill people in their network
		if (susceptible_people[i]->ill_moron_contacts + susceptible_people[i]->ill_normal_contacts > 0) {
			// n = b(u*(ill normals) + ill morons)
			//set eta based on above expression
			double eta = beta * ((mu * susceptible_people[i]->ill_normal_contacts)
				+ susceptible_people[i]->ill_moron_contacts);
			
			// probability of getting sick is 
			// 1 - e^( -eta * delta_t)
			double prb_get_sick = 1.0 - std::exp(-(eta)*delta_t);

			// if a random number between a non-zero double and one is less than or
			// equal to our probability, (happens {prb_get_sick * 100}% of the time)
			// update person as sick and take them out of susceptible people vector
			if (spread_engine::random(0.000001,1.0) <= prb_get_sick) {
				update_people_contacts(susceptible_people[i], true);
				susceptible_people[i] = nullptr;
			}
		}
		
	}

	// remove all nullptrs from susceptible_people that we created above
	susceptible_people.erase(std::remove(susceptible_people.begin(), susceptible_people.end(), nullptr),
		susceptible_people.end());
}

void spread_engine::remove_infected_people() {

	// delta_t (really not strictly necessary due to being 1.0)
	// for exp() expression
	static constexpr double delta_t = 1.0;

	// for everyone in the infected_people vector
	for (size_t i = 0; i < infected_people.size(); ++i) {
		// if a random number between a non-zero double and one is less than or
		// equal to gamma, (happens {gamma * 100}% of the time)
		// update person as renmoved and take them out of infected people vector
		if (spread_engine::random(0.000001, 1.0) <= gamma) {
			update_people_contacts(infected_people[i], false);
			infected_people[i] = nullptr;
		}
	}

	// remove all nullptrs from infected_people that we created above
	infected_people.erase(std::remove(infected_people.begin(), infected_people.end(), nullptr),
		infected_people.end());
}

void spread_engine::tick() {

	//call infect and remove functions
	infect_healthy_people();
	remove_infected_people();
	//increment elapsed days
	++elapsed_days;

}