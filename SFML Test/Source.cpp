#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{

    //init render window with 400x400 res, Hello World title
    sf::RenderWindow window(sf::VideoMode(400, 400), "Hello World");
    //minimizes graphical compute utilization
    window.setFramerateLimit(60);
    
    sf::CircleShape shape(150.f); //init circle with radius 150
    shape.setOrigin(shape.getRadius(), shape.getRadius()); //set origin of circle for centering
    shape.setPosition(200, 200); //set pos of circle based on window resolution
    shape.setFillColor(sf::Color{ 255,209,0,255 }); //set color of circle to UCLA Gold


    //below assumes comic.ttf file is at same dir as Source.cpp, which we are allowed to assume based on
    //forum post on CCLE
    sf::Font comic_sans;
    if (!comic_sans.loadFromFile("comic.ttf")) { // cannot load
        std::cerr << "could not find comic sans font" << std::endl; //throws error if could not find comic.ttf
    }
    else {
        //loaded font, skip
    }

    sf::Text msg;
    msg.setFont(comic_sans); // set font to comic sans, which we loaded from file
    msg.setFillColor(sf::Color(39, 116, 174, 255)); //set color of msg object to UCLA Blue
    msg.setCharacterSize(50); // set pt of msg object
    msg.setString("PIC 10C"); // set text of msg object
    msg.setOrigin(msg.getLocalBounds().width / 2, msg.getLocalBounds().height); // Convention should be height/2,           
    msg.setPosition(200,200); // set pos of text based on window resolution     //but it looks off when I do that

    while (window.isOpen()) // ensures window is open
    {
        sf::Event event; // inits event queue
        while (window.pollEvent(event)) // asks for events from queue
        {
            if (event.type == sf::Event::Closed) //detects closure request
                window.close(); //closes window
        }

        window.clear(sf::Color(39, 116, 174, 255)); //sets window background color and refreshes window @60hz (max)
        window.draw(shape); //draws circle @60hz (max)
        window.draw(msg); //draws text @60hz (max)
        window.display(); //displays window @60hz (max)
    }

    return 0;
}