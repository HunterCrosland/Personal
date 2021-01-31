#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "jump.h"



int main()
{

    //init render window with 450x100 res, Hello World title
    float window_len = 450;
    float window_hgt = 100;
    sf::RenderWindow window(sf::VideoMode(window_len, window_hgt), "Jumping Game");
    //minimizes graphical compute utilization
    window.setFramerateLimit(60);

    sf::Clock clock;
    sf::Time time1 = sf::seconds(0);
    sf::Time time2 = sf::seconds(0);
    sf::Time time3 = sf::seconds(0);

    Game game(window_len,window_hgt);
    game.initialize();
    
    bool point_oh_one_elapsed   = false;
    bool one_point_five_elapsed = false;
    bool playing = false;
    bool jumping = false;
    bool hasJumped = false;
    bool gameOver = false;
    bool gameEnded = false;

    while (window.isOpen()) // ensures window is open
    {
        if (game.check_lives() <= 0) {
            gameOver = true;
        }
        
        sf::Event event; // inits event queue
        while (window.pollEvent(event)) // asks for events from queue
        {
           
            switch (event.type)
            {
            case(sf::Event::Closed):
                window.close();
                break;
            case(sf::Event::KeyPressed):
                if(!playing && !gameOver) {
                    game.start(playing);
                    playing = true;
                }
                else if (playing && gameOver) {
                    //do nothing
                }
                else {
                    if (event.key.code == sf::Keyboard::Up) {
                        jumping = true;
                        if (!hasJumped) { 
                        time3 = clock.getElapsedTime(); 
                        hasJumped = true; 
                        game.play_jump_sound();
                        }
                        
                        }
                    }
                break;

            default:
                break;
            }
        }

        
        window.clear(sf::Color::Black); //sets window background color and refreshes window @60hz (max)

        const std::vector<sf::Drawable*>* to_draw = game.get_curr_drawings();

        for (sf::Drawable* draw_me : *to_draw) {
            window.draw(*draw_me);
        }
        if(playing && !gameOver) {
            if (clock.getElapsedTime().asMilliseconds() - time1.asMilliseconds() < 1500) {
                one_point_five_elapsed = false;
            }
            else {
                one_point_five_elapsed = true;
                time1 = clock.getElapsedTime();
            }
            if (clock.getElapsedTime().asMilliseconds() - time2.asMilliseconds() < 10) {
                point_oh_one_elapsed = false;
            }
            else {
                point_oh_one_elapsed = true;
                time2 = clock.getElapsedTime();
            }

            game.step(one_point_five_elapsed, point_oh_one_elapsed, jumping, time3, clock);
            
            if (clock.getElapsedTime().asSeconds() - time3.asSeconds() >= 1.1f) { hasJumped = false; }
        }
        else if (playing && gameOver && !gameEnded) {
            game.end_game();
            gameEnded = true;
        }
        
        window.display(); //displays window @60hz (max)
    }

    return 0;
}