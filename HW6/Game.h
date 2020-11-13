#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

class Game
{

public:


    Game(float _window_len, float _window_hgt) : window_len(_window_len), window_hgt(_window_hgt), player(_window_hgt/6), _enemy(_window_hgt / 6){}

    void initialize() {

        floor.setSize({ window_len,window_hgt / 6 });
        floor.setPosition(0, window_hgt - window_hgt / 6);
        floor.setFillColor(sf::Color::White);

        
        if (!sansation.loadFromFile("sansation.ttf")) { // cannot load
            std::cerr << "could not find sansation font" << std::endl; //throws error if could not find sansation.ttf
        }
        else {
            //loaded font, skip
        }

        player.initialize({ window_len,window_hgt });
        _enemy.initialize({ window_len,window_hgt });


        intro1.setFont(sansation); // set font to sansation, which we loaded from file
        intro1.setFillColor(sf::Color::White); //set color of msg object to UCLA Blue
        intro1.setCharacterSize(22); // set pt of msg object
        intro1.setString("Press any key to begin."); // set text of msg object
        intro1.setPosition(0, 0); // set pos of text based on window resolution

       
        intro2.setFont(sansation); // set font to sansation, which we loaded from file
        intro2.setFillColor(sf::Color::White); //set color of msg object to UCLA Blue
        intro2.setCharacterSize(22); // set pt of msg object
        intro2.setString("Press Up to jump."); // set text of msg object
        intro2.setPosition(0, intro1.getLocalBounds().height); // set pos of text based on window resolution

        
        intro3.setFont(sansation); // set font to sansation, which we loaded from file
        intro3.setFillColor(sf::Color::White); //set color of msg object to UCLA Blue
        intro3.setCharacterSize(22); // set pt of msg object
        intro3.setString("Get hit 3 times and lose."); // set text of msg object
        intro3.setPosition(0, intro2.getLocalBounds().height * 2); // set pos of text based on window resolution

        
        score.setFont(sansation);
        score.setFillColor(sf::Color::White);
        score.setCharacterSize(22);
        score.setString("Score: ");
        score.setPosition(0, 0);

        to_draw.push_back(&player); //draws circle @60hz (max)
        to_draw.push_back(&floor);
        to_draw.push_back(&_enemy);

        to_draw.push_back(&intro1);
        to_draw.push_back(&intro2);
        to_draw.push_back(&intro3);

    }

    void start(bool playing) {
        if (!playing) {
            to_draw.erase(std::remove(to_draw.begin(), to_draw.end(), &intro1), to_draw.end());
            to_draw.erase(std::remove(to_draw.begin(), to_draw.end(), &intro2), to_draw.end());
            to_draw.erase(std::remove(to_draw.begin(), to_draw.end(), &intro3), to_draw.end());
            to_draw.push_back(&score);
        }
    }

    std::vector<sf::Drawable*>* get_curr_drawings() {
        return &to_draw;
    }

    class player_char : public sf::Drawable, public sf::Transformable
    {
    public:
        player_char(float sqr_dims) : sqr_len_hgt(sqr_dims), rect_hgt(sqr_dims), rect_len(sqr_dims * 3) {}

        void initialize(sf::Vector2f window_dims) {
            rectangle.setSize(sf::Vector2f(rect_len, rect_hgt));
            rectangle.setFillColor(sf::Color::Red);

            square.setSize(sf::Vector2f(sqr_len_hgt, sqr_len_hgt));
            square.setFillColor(sf::Color::Red);

            rectangle.setPosition(0, window_dims.y - sqr_len_hgt * 2);
            square.setPosition(rect_len / 2 - sqr_len_hgt / 2, window_dims.y - sqr_len_hgt * 3);
        }

        float curr_y_pos() {
            return (rectangle.getLocalBounds().top - rectangle.getLocalBounds().height);
        }

    private:

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            // You can draw other high-level objects
            target.draw(rectangle);
            target.draw(square);

        }

        float rect_len;
        float rect_hgt;
        float sqr_len_hgt;

        sf::RectangleShape rectangle;
        sf::RectangleShape square;

    };

    class enemy : public sf::Shape
    {
    public:
        enemy(float sqr_dims) : sqr_len_hgt(sqr_dims) {}

        void initialize(sf::Vector2f window_dims) {
            square.setSize(sf::Vector2f(sqr_len_hgt, sqr_len_hgt));
            square.setFillColor(sf::Color::Yellow);

            square.setPosition(window_dims.x - sqr_len_hgt, window_dims.y - sqr_len_hgt * 2);
        }

        bool is_out_of_frame() {
            if (getPoint(0).x <= 0) {
                return true;
            }
            else { return false; }
        }

        bool is_at_player_x() {
            if (getPoint(0).x <= sqr_len_hgt*3) {
                return true;
            }
            else { return false; }
        }

        void move() {
            sf::Vector2f currPos = getPosition();
            setPosition({ currPos.x - 2, currPos.y });
        }

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            // Draw square when window.draw(enemy) is called
            target.draw(square);

        }

        virtual size_t getPointCount() const
        {
            return 4; // fixed, but could be an attribute of the class if needed
        }

        virtual sf::Vector2f getPoint(size_t index) const
        {
            sf::Vector2f currPos = getPosition();
            switch (index)
            {
            case(0):
                return currPos;
                break;
            case(1):
                currPos.x += sqr_len_hgt;
                return currPos;
                break;
            case(2):
                currPos.y += sqr_len_hgt;
                return currPos;
                break;
            case(3):
                currPos.x += sqr_len_hgt;
                currPos.y += sqr_len_hgt;
                return currPos;
                break;
            default:
                std::cout << "This point does not exist on the shape. \n";
                return (currPos);
                break;
            }

        }

        float sqr_len_hgt;

        sf::RectangleShape square;

    };

    void step(sf::Time elapsed) {
        
        do_removals();
        check_hits();
        move_enemies(elapsed);
        make_enemies(elapsed);

    }

private:
    std::vector<enemy*> enemies;
    enemy _enemy;
    player_char player;
    sf::RectangleShape floor;
    std::vector<sf::Drawable*> to_draw;
    sf::Font sansation;
    float window_hgt;
    float window_len;
    sf::Text intro1;
    sf::Text intro2;
    sf::Text intro3;
    sf::Text score;

    void do_removals() {
        for (enemy* _enemy : enemies) {
            if (_enemy->is_out_of_frame()) {
                enemies.erase(std::remove(enemies.begin(), enemies.end(), _enemy), enemies.end());
            }
            else { return; }
        }
    }

    bool check_hits() {
        bool hit = false;
        for (enemy* _enemy : enemies) {
            if (_enemy->is_at_player_x() && player.curr_y_pos() <= _enemy->getLocalBounds().height) {
                hit = true;
            }
        }
        return hit;
    }

    void move_enemies(sf::Time elapsed) {
        if ((elapsed.asMilliseconds() % 1500) < 15) {
            for (enemy* _enemy : enemies) {
                _enemy->move();
            }
        }
    }

    //void move_character{

    //}

    void make_enemies(sf::Time elapsed) {
        if ((elapsed.asMilliseconds() % 1500) < 60) {
            srand(time(nullptr));
            if ((rand() % 100) <= 49) {
                enemy* _new = new enemy(window_hgt / 6);
                enemies.push_back(_new);
                to_draw.push_back(_new);
                std::cout << "Enemy Created! \n"; 
            }
        }
    }

    //void manage_events{

    //}



};

#endif