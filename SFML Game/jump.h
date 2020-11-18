#ifndef JUMP_H
#define JUMP_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <string>

/**
@class Game is all functions and private variables necessary for 
    game infrastructure
*/
class Game
{

public:

    /**
    Game constructor
    @param _window_len is the x dimension of the window
    @param _window_hgt is the y dimension of the window
    */
    Game(const float _window_len, const float _window_hgt);

    //Default destructor
    ~Game() = default;

    /**
    Initializes all private variables of Game that are not init'ed in constructor
    */
    void initialize();

    /**
    Swaps intro text with score text and informs main.cpp that game can begin
    @param playing is a const bool that ensures function only does stuff when game is 
        not being played
    */
    void start(const bool playing);

    /**
    Plays jump.wav
    */
    void play_jump_sound();

    /**
    Returns life count of player_char player in private
    @return int for above desc
    */
    const int check_lives();

    /**
    Creates "Game over" text for drawing, appends to drawable vector
    */
    void end_game();

    /**
    Returns drawable vector to loop over for graphical rendering in main
    @return std::vector<sf::Drawable*>* for above desc
    */
    const std::vector<sf::Drawable*>* get_curr_drawings();

    /**
    @class player_char is a class that houses the player shapes, as well as 
        some variables for logic such as lives and their respective getters
    */
    class player_char : public sf::Drawable, public sf::Transformable
    {
    public:
        /**
        Constructor for player char which accepts the len and hgt dimensions
            of the upper square, which is the basis for all other dimensions
            in the player's shape
        */
        player_char(float sqr_dims);

        //default destructor
        ~player_char() = default;

        /**
        Inits all appropriate dimensions of the player shape, colors, and positions
        @param window_dims is a vector of the window dimensions, which allow the
            positions of the player shape to be set accurately
        */
        void initialize(sf::Vector2f window_dims);

        /**
        Returns a const float corresponding to y position of player shape for collision checking
        @return a const float corresponding to y position of player shape for collision checking
        */
        const float curr_min_y_pos();

        /**
        Returns a const int corresponding to lives remaining
        @return a const int (private int lives)
        */
        const int check_lives();

        /**
        Updates player position to allow jumping based on a function housed within jump()
        @param jumping is a bool to ensure position should be updating
        @param start is a time var to check against clock for positioning of player
        @param clock is the global game clock against which to check start
        */
        void jump(bool& jumping, const sf::Time start, sf::Clock& clock);

        /**
        Updates player lives if a collision occurs
        */
        void decrement_lives();

    private:

        /**
        draw function allows for calls to window.draw(player)
        @param target the render window, called within window.draw()
        @param states are the render window states, called within window.draw()
        */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        //shape dims
        float rect_len;
        float rect_hgt;
        float sqr_len_hgt;

        //constant vars for jump function
        static constexpr float T = 1;
        static constexpr float T_star = 1.1;

        //shape declarations for player_char overall shape
        sf::RectangleShape rectangle;
        sf::RectangleShape square;

        //default positions of rectangle and square
        sf::Vector2f rect_def_pos;
        sf::Vector2f sqr_def_pos;

        int lives;

    };


    /**
    @class enemy is a class that houses the enemy shapes, as well as
        some variables for logic
    */
    class enemy : public sf::Drawable, public sf::Transformable
    {
    public:
        /**
        Constructor for enemy which accepts the len and hgt dimensions
            of the square
        */
        enemy(float sqr_dims);

        //default destructor
        ~enemy() = default;

        /**
        Inits all appropriate dimensions of the enemy shape, colors, and positions
        @param window_dims is a vector of the window dimensions, which allow the
            positions of the player shape to be set accurately
        */
        void initialize(sf::Vector2f window_dims);

        /**
        Returns a const float corresponding to height of enemy shape for collision checking
        @return a const float corresponding to height of enemy shape for collision checking
        */
        const float get_hgt();

        /**
        Returns a const bool corresponding to position of enemy shape for removal checking
        @return a const bool that corresponds to whether the enemy is outside of the window frame
        */
        const bool is_out_of_frame();

        /**
        Returns a const bool corresponding to position of enemy shape for removal checking
        @return a const bool that corresponds to whether the enemy is within the player's x coords
        */
        const bool is_at_player_x();

        /**
        Decrements the enemy shape position to move shape across screen
        */
        void move();

    private:

        /**
        draw function allows for calls to window.draw(player)
        @param target the render window, called within window.draw()
        @param states are the render window states, called within window.draw()
        */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        //dimensions and square decl
        float sqr_len_hgt;
        sf::RectangleShape square;

    };
    
    /**
    @brief game progression function, makes calls to do_removals and check hits, interval calls to move_enemies and make_enemies,
        and a conditional call to jump
    @param one_point_five_elapsed asks whether 1.5s has elapsed since last step call 
    @param point_oh_one_elapsed asks whether 0.01s has elapsed since last step call
    @param jumping asks whether char is currently jumping
    @param elapsed records how much time has elapsed since player started jumping
    @param clock is the global game clock
    */
    void step(bool one_point_five_elapsed, bool point_oh_one_elapsed, bool& jumping, sf::Time& elapsed,
        sf::Clock& clock);

private:
    //vector of enemies for removal management
    std::vector<enemy*> enemies;

    player_char player;

    sf::RectangleShape floor;

    //vector of Drawables for graphical rendering in the window
    std::vector<sf::Drawable*> to_draw;
    
    //SoundBuffer and Sound declarations 
    sf::SoundBuffer hit_sound_buffer;
    sf::SoundBuffer jump_sound_buffer;
    sf::Sound hit_sound;
    sf::Sound jump_sound;


    sf::Font sansation;

    float window_hgt;
    float window_len;

    //all text declarations for display
    sf::Text intro1;
    sf::Text intro2;
    sf::Text intro3;
    sf::Text score;
    sf::Text game_over;

    int score_num;

    /**
    loops through enemies vector to see if removals are needed
    */
    void do_removals();

    /**
    loops through enemies vector to see if any enemies are currently colliding with player object
    @return a boolean to inform Game that player has been hit to play a sound and decrement lives
    */
    const bool check_hits();

    /**
    loops through enemies vector and calls move() on all of them
    */
    void move_enemies();

    /**
    @brief generates enemies randomly based on a Bernoulli distribution random generator,
    if successful, creates new enemies, calls initialize, pushes them to enemies
    vector and to_draw vector
    */
    void make_enemies();

};

#endif