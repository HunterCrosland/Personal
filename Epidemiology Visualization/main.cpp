#include "spread_engine.h"
#include <SFML/Graphics.hpp>
#include <string>

int main() {

	// init render window with 400x400 res, Epidemiology Visualization title
	static constexpr float window_len = 400;
	static constexpr float window_hgt = 400;
	sf::RenderWindow window(sf::VideoMode(window_len, window_hgt), "Epidemiology Visualization");
	// minimizes graphical compute utilization
	window.setFramerateLimit(60);

    // load sansation font
    sf::Font sansation;
    if (!sansation.loadFromFile("sansation.ttf")) { // cannot load
        std::cerr << "could not find sansation font" << std::endl; //throws error if could not find sansation.ttf
    }
    else {
        //loaded font, skip
    }

    // init our vector for things to draw with SFML
    std::vector<sf::Drawable*> to_draw;

    // init intro texts
    sf::Text intro1;
    sf::Text intro2;
    sf::Text intro3;
    sf::Text intro4;

    // init user input and day_count texts
    sf::Text user_input1;
    sf::Text user_input2;
    sf::Text user_input3;
    sf::Text day_count;
    
    // convenience vector for all of the above texts
    std::vector<sf::Text*> intro_text{ &intro1, &intro2, &intro3, &intro4, 
        &user_input1, &user_input2, &user_input3, &day_count };

    // for all text objects in the intro_text vector
    for (sf::Text* my_text : intro_text) {
        my_text->setFont(sansation); // set font to sansation, which we loaded from file
        my_text->setFillColor(sf::Color::White); //set color of msg object to White
        my_text->setCharacterSize(22); // set pt of msg object
    }

    // set strings for our intro text
    intro1.setString("normal: ");
    intro2.setString("moron: ");
    intro3.setString("sick: ");
    intro4.setString("day: ");

    // set positions for our intro text
    intro1.setPosition(0, 0); // set pos of text to top left
    intro2.setPosition(0, intro1.getLocalBounds().height + 5); // set pos of text to below intro2 with small buffer
    intro3.setPosition(0, intro1.getLocalBounds().height * 2 + 10); // set pos of text to below intro3 with small buffer
    intro4.setPosition(0, intro1.getLocalBounds().height * 5); // set pos of text to below intro4 with large buffer

    // set positions for our user_input text
    user_input1.setPosition(intro1.getLocalBounds().width + 10, 0); //set pos of text to right of input1
    user_input2.setPosition(intro2.getLocalBounds().width + 10, intro1.getLocalBounds().height + 5); //set pos of text to right of input2
    user_input3.setPosition(intro3.getLocalBounds().width + 10, intro1.getLocalBounds().height * 2 + 10); //set pos of text to right of input3
    day_count.setPosition(intro4.getLocalBounds().width + 10, intro1.getLocalBounds().height * 5); //set pos of text to right of input4

    // start with to_draw having our user_input1 and intro1 text
    to_draw.push_back(&intro1);
    to_draw.push_back(&user_input1);

    // init and set string of all S,I, and R text objects
    sf::Text normal_s; normal_s.setString("Normal S");
    sf::Text normal_i; normal_i.setString("Normal I");
    sf::Text normal_r; normal_r.setString("Normal R");

    sf::Text moron_s; moron_s.setString("Moron S");
    sf::Text moron_i; moron_i.setString("Moron I");
    sf::Text moron_r; moron_r.setString("Moron R");

    // convenience vector for all of the above texts
    std::vector<sf::Text*> sir_text{ &normal_s, &normal_i, &normal_r,
        &moron_s, & moron_i, & moron_r };

    // for all of the S,I,R text objects
    for (sf::Text* my_text : sir_text) {
        my_text->setFont(sansation); // set font to sansation, which we loaded from file
        my_text->setFillColor(sf::Color::White); //set color of msg object to White
        my_text->setCharacterSize(30); // set pt of msg object

    }

    // set positions for our S,I,R text
    moron_r.setPosition(0, window_hgt - moron_r.getGlobalBounds().height - 10); //set pos to bottom left
    moron_i.setPosition(0, window_hgt - moron_r.getGlobalBounds().height * 2 - 15); // set pos to above R_m
    moron_s.setPosition(0, window_hgt - moron_r.getGlobalBounds().height * 3 - 20); // set pos to above I_m

    normal_r.setPosition(0, window_hgt - moron_r.getGlobalBounds().height * 4 - 25); // set pos to above S_m
    normal_i.setPosition(0, window_hgt - moron_r.getGlobalBounds().height * 5 - 30); // set pos to above R_n
    normal_s.setPosition(0, window_hgt - moron_r.getGlobalBounds().height * 6 - 35); // set pos to above I_n

    // set maximum bar length for visualization (1/2 window len)
    float max_bar_len = (1.f / 2) * window_len;

    // init and set color of S,I,R bars
    sf::RectangleShape normal_s_bar; normal_s_bar.setFillColor(sf::Color::Green);
    sf::RectangleShape normal_i_bar; normal_i_bar.setFillColor(sf::Color::Red);
    sf::RectangleShape normal_r_bar; normal_r_bar.setFillColor(sf::Color::Blue);

    sf::RectangleShape moron_s_bar; moron_s_bar.setFillColor(sf::Color::Green);
    sf::RectangleShape moron_i_bar; moron_i_bar.setFillColor(sf::Color::Red);
    sf::RectangleShape moron_r_bar; moron_r_bar.setFillColor(sf::Color::Blue);

    // convenience vector for S,I,R bars
    std::vector<sf::RectangleShape*> sir_bars{ &normal_s_bar, &normal_i_bar, &normal_r_bar,
        &moron_s_bar, &moron_i_bar, &moron_r_bar };

    // for all S,I,R bars
    for (auto bar : sir_bars) {
        bar->setSize(sf::Vector2f(1, 22.f)); //set size so that the position setting doesn't mess up
    }

    // set each bar to the right of each corresponding text
    // i.e. normal_s_bar is set to the right of normal_s text object
    normal_s_bar.setPosition(sf::Vector2f(normal_s.getGlobalBounds().left + normal_s.getLocalBounds().width + 50,
        normal_s.getGlobalBounds().top));

    normal_i_bar.setPosition(sf::Vector2f(normal_s_bar.getGlobalBounds().left,
        normal_s_bar.getGlobalBounds().top + normal_s_bar.getLocalBounds().height + 5));

    normal_r_bar.setPosition(sf::Vector2f(normal_i_bar.getGlobalBounds().left,
        normal_i_bar.getGlobalBounds().top + normal_i_bar.getLocalBounds().height + 5));

    moron_s_bar.setPosition(sf::Vector2f(normal_r_bar.getGlobalBounds().left,
        normal_r_bar.getGlobalBounds().top + normal_r_bar.getLocalBounds().height + 5));

    moron_i_bar.setPosition(sf::Vector2f(moron_s_bar.getGlobalBounds().left,
        moron_s_bar.getGlobalBounds().top + moron_s_bar.getLocalBounds().height + 5));

    moron_r_bar.setPosition(sf::Vector2f(moron_i_bar.getGlobalBounds().left,
        moron_i_bar.getGlobalBounds().top + moron_i_bar.getLocalBounds().height + 5));

    // init line_count to track how many times user has pressed enter
    size_t line_count = 0;
    // forward declaration of user_input string
    std::string userInput;

    // forward declaration of user input populations
    size_t initial_normal_population;
    size_t initial_moron_population;
    size_t initial_sick_population;

    // convenience vector for initial populations
    std::vector<size_t*> initial_pops{ &initial_normal_population,&initial_moron_population,&initial_sick_population, };

    // bools to check whether simulation has started 
    // and to ensure initialization happens only once
    bool sim_has_started = false;
    bool sim_has_initialized = false;
 
    // clock for progressing simulation once per second
    sf::Clock clock;
    sf::Time curr_time = sf::seconds(0);

    // initialize spread_engine 
    spread_engine my_SE;

    while (window.isOpen()) // ensures window is open
    {

        sf::Event event; // inits event queue
        while (window.pollEvent(event)) // asks for events from queue
        {

            switch (event.type)
            {
                // if window_close is called, close window
            case(sf::Event::Closed):
                window.close();
                break;
            case(sf::Event::KeyPressed):
                // if user presses enter
                if (event.key.code == sf::Keyboard::Enter) {
                    // increment line count
                    line_count++;
                    if (line_count < 4) {
                        // push back intro text and user input text from
                        // convenience vector
                        to_draw.push_back(intro_text[line_count]);
                        to_draw.push_back(intro_text[line_count + 4]);
                        // set initial pops based on user input
                        *initial_pops[line_count - 1] = std::stoi(userInput);
                    } 
                    // reset userInput string
                    userInput = "";
                    // if user has pressed enter 3 times
                    if (line_count == 3) {
                        // for all drawables in S,I,R text vector
                        for (sf::Text* for_drawing : sir_text) {
                            // add S,I,R text to vector for SFML drawing
                            to_draw.push_back(for_drawing);
                        }
                        // specify that simulation has started
                        sim_has_started = true;
                    }

                }
                break;
            // user enters text
            case(sf::Event::TextEntered):
                if (event.text.unicode < 0x80 && event.text.unicode != '\b') // it's printable
                    // and is not backspace
                {
                    
                    // if user has pressed enter less than three times
                    if (line_count < 3) {
                        // add new text to userInput string
                        userInput += (char)event.text.unicode;
                        // and set sfml text object string to userInput
                        intro_text[line_count + 4]->setString(userInput);
                    }
                }
                // if user entered backspace and string isn't empty
                if (event.text.unicode == '\b' && userInput.size() > 0) {
                    
                    // if user has pressed enter less than three times
                    if (line_count < 3) {
                        // erase last character from userInput string
                        userInput.erase(userInput.size() - 1, 1);
                        // and set sfml text object string to userInput
                        intro_text[line_count + 4]->setString(userInput);
                    }
                }
                break;
            default:
                break;
            }
        }

        window.clear(sf::Color::Black); //sets window background color and refreshes window @60hz (max)

        // draw all drawables from to_draw vector
        for (sf::Drawable* draw_me : to_draw) {
            window.draw(*draw_me);
        }

        // if simulation has started (i.e. user has pressed enter 3 times)
        if (sim_has_started) {

            // if one second has passed)
            if (clock.getElapsedTime().asMilliseconds() - curr_time.asMilliseconds() > 999) {
                // if sim has not yet been initialized
                if (!sim_has_initialized) {
                    // set initial pops based on user input
                    my_SE.set_initial_populations(initial_normal_population, initial_moron_population, initial_sick_population);
                    // initialize spread network
                    my_SE.init_spread_network();
                    // populate each person's network
                    my_SE.populate_spread_network();
                    // randomly infect healthy people
                    my_SE.randomly_infect_healthy();
                    // set initialized to true to ensure we don't re-initialize
                    sim_has_initialized = true;
                    // allow sim to draw sir_bars
                    for (auto bar : sir_bars) {
                        to_draw.push_back(bar);
                    }
                }

                
                // total population size_t for reference so that each bar is proportional to total population
                size_t initial_total_population = initial_moron_population + initial_normal_population;
                
                //set each bar width to ( (its corresponding population / total population) * maximum length)
                normal_s_bar.setSize(sf::Vector2f(
                    max_bar_len * (static_cast<float>(my_SE.get_susceptible_normal()) / initial_total_population), 22.f));
                normal_i_bar.setSize(sf::Vector2f(
                    max_bar_len * (static_cast<float>(my_SE.get_infected_normal()) / initial_total_population), 22.f));
                normal_r_bar.setSize(sf::Vector2f(
                    max_bar_len * (static_cast<float>(my_SE.get_removed_normal()) / initial_total_population), 22.f));
                moron_s_bar.setSize(sf::Vector2f(
                    max_bar_len * (static_cast<float>(my_SE.get_susceptible_moron()) / initial_total_population), 22.f));
                moron_i_bar.setSize(sf::Vector2f(
                    max_bar_len * (static_cast<float>(my_SE.get_infected_moron()) / initial_total_population), 22.f));
                moron_r_bar.setSize(sf::Vector2f(
                    max_bar_len * (static_cast<float>(my_SE.get_removed_moron()) / initial_total_population), 22.f));

                // reset current time for second counting
                curr_time = clock.getElapsedTime();
                // set day counting string to new days elapsed
                day_count.setString(std::to_string(my_SE.get_days_elapsed()));


                // call forwarding function to continue the simulation
                my_SE.tick();
            }
        }
       

        window.display(); //displays window @60hz (max)
    }

    return 0;

}