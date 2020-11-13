#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Game.h"



int main()
{

    //init render window with 450x100 res, Hello World title
    float window_len = 450;
    float window_hgt = 100;
    sf::RenderWindow window(sf::VideoMode(window_len, window_hgt), "Jumping Game");
    //minimizes graphical compute utilization
    window.setFramerateLimit(60);

    //shape.setOrigin(shape.getRadius(), shape.getRadius()); //set origin of circle for centering
    //shape.setPosition(200, 200); //set pos of circle based on window resolution
    //shape.setFillColor(sf::Color{ 255,209,0,255 }); //set color of circle to UCLA Gold

    

    

    sf::Clock clock;
    Game game(window_len,window_hgt);
    game.initialize();

    while (window.isOpen()) // ensures window is open
    {
        bool playing = false;
        sf::Event event; // inits event queue
        while (window.pollEvent(event)) // asks for events from queue
        {
           
            switch (event.type)
            {
            case(sf::Event::Closed):
                window.close();
                break;
            case(sf::Event::KeyPressed):
                if(!playing) {
                    game.start(playing);
                    playing = true;
                }
                break;
            default:
                break;
            }
        }

        sf::Time time = clock.getElapsedTime();
        window.clear(sf::Color::Black); //sets window background color and refreshes window @60hz (max)

        std::vector<sf::Drawable*>* to_draw = game.get_curr_drawings();

        for (sf::Drawable* draw_me : *to_draw) {
            window.draw(*draw_me);
        }
        
        game.step(time);
        window.display(); //displays window @60hz (max)
    }

    return 0;
}