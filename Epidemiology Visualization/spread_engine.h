#ifndef  SPREAD_ENGINE_H
#define SPREAD_ENGINE_H

#include <vector>
#include <random>
#include <set>
#include <algorithm>
#include <iostream>


/**
@class spread_engine
@brief The spread engine class does all of the backend handling for the epidemiology
visualization, including configuring the networks for each person, calculating infections,
and calculating removals.

The spread_engine class stores several static constexpr values which correspond to 
experimental values for calculating spread as wellas network sizes for each person. 
In addition, it stores values corresponding to the S,I, and R values for morons and 
normal people, and vectors for looping through each of those groups, as well.

For its functions, it has getters which return S,I, and R values and days elapsed 
since the start of the simulation, and several initializer and step functions.
*/
class spread_engine
{
private:
	// the hazard rate of getting COVID-19 per day when interacting with a sick person
	static constexpr double beta = 0.02;
	// the risk reduction factor experienced by those interacting with someone wearing a mask
	static constexpr double mu = 0.34;
	// the daily rate people recover/die from the ill state and move into the removed group
	static constexpr double gamma = 1.0/14.0;

	// number contacts for normal people and morons
	static constexpr size_t normal_contacts = 9;
	static constexpr size_t moron_contacts = 20;

	// number days elapsed since starting simulation
	unsigned int elapsed_days;

	// number of people initially sick
	size_t initial_sick;

	// tracking variables for normal people
	size_t total_normal;
	size_t susceptible_normal;
	size_t infected_normal;
	size_t removed_normal;

	// tracking variables for morons
	size_t total_moron;
	size_t susceptible_moron;
	size_t infected_moron;
	size_t removed_moron;

	// detailed documentation in spread_engine.cpp
	struct person;
	 
	// vector declaration for people without a network,
	// gets cleared after all networks are configured
	std::vector<person*> need_contacts;

	// vector declarations for people types
	std::vector<person*> all_people;
	std::vector<person*> susceptible_people;
	std::vector<person*> infected_people;

	// we don't need to keep track of removed people,
	// since they are now removed from the sim
	//std::vector<person*> removed_people;

	

public:

	// default constructor simply initializes all size_ts stored privately to 0
	spread_engine();
	// default destructor
	~spread_engine();

	/**
	Returns random number within arg-specified interval
	
	This was taken from user Galik on
	https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c

	This code was not copywritten and is subject to sharing via Creative Commons

	@tparam from is the lower bound of the desired output
	@tparam to is the upper bound of the desired output
	@treturn is a variable type corresponding to the input type
	that is a random number between the to and from arguments
	*/
	template<typename Numeric, typename Generator = std::mt19937>
	Numeric random(Numeric from, Numeric to);

	/**
	Getter for returning days since start of sim
	@return is a size_t corresponding to days since start of sim
	*/
	const size_t get_days_elapsed() const;

	/**
	Getter for returning number of susceptible normal people
	@return is a size_t corresponding to number of susceptible normal people
	*/
	const size_t get_susceptible_normal() const;
	/**
	Getter for returning number of infected normal people
	@return is a size_t corresponding to number of infected normal people
	*/
	const size_t get_infected_normal() const;
	/**
	Getter for returning number of removed normal people
	@return is a size_t corresponding to number of removed normal people
	*/
	const size_t get_removed_normal() const;


	/**
	Getter for returning number of susceptible moron people
	@return is a size_t corresponding to number of susceptible moron people
	*/
	const size_t get_susceptible_moron() const;
	/**
	Getter for returning number of infected moron people
	@return is a size_t corresponding to number of infected moron people
	*/
	const size_t get_infected_moron() const;
	/**
	Getter for returning number of removed moron people
	@return is a size_t corresponding to number of removed moron people
	*/
	const size_t get_removed_moron() const;

	/**
	Sets initial size_ts as specified by the user
	@param num_normal is a const size_t corresponding to initial normal population
	@param num_moron is a const size_t corresponding to initial moron population
	@param num_sick is a const size_t corresponding to initial sick population
	*/
	void set_initial_populations(const size_t num_normal, const size_t num_moron, const size_t num_sick);

	/**
	@brief Creates person structs according to number specified by user and assigns 
	them attributes, i.e. is_moron, network size (0 by default), and places them 
	in their appropriate storage vectors
	*/
	void init_spread_network();

	/**
	@brief Generates network vectors for each person via random assignment through
	configuration network method.
	*/
	void populate_spread_network();

	/**
	@brief Loops through susceptible people vector and randomly infects them based
	on how many initial sick people were specified by the user
	*/
	void randomly_infect_healthy();

	/**
	@brief Loops through network of person who just got sick or removed, and updates
	each network stub accordingly
	@param for_updating is the person whose network needs updating
	@param is_sick specifies whether person just got sick or removed
	*/
	void update_people_contacts(person* for_updating, const bool is_sick);

	/**
	@brief Loops through susceptible people vector and randomly infects them based
	on each person's risk factor as determined by their networks and statuses of contacts
	*/
	void infect_healthy_people();

	/**
	@brief Loops through infected people vector and randomly removes them based
	on each person's recovery factor as determined by gamma, defined above
	*/
	void remove_infected_people();
	
	/**
	@brief caller function for interval calling of appropriate functions
	*/
	void tick();

};

#endif // ! SPREAD_ENGINE_H