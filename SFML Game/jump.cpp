#include "jump.h"



    Game::Game(const float _window_len, const float _window_hgt) : window_len(_window_len), window_hgt(_window_hgt), player(_window_hgt / 6), score_num(0) {}

    void Game::initialize() {

        //sets up floor for drawing
        floor.setSize({ window_len,window_hgt / 6 });
        floor.setPosition(0, window_hgt - window_hgt / 6);
        floor.setFillColor(sf::Color::White);

        //load font
        if (!sansation.loadFromFile("sansation.ttf")) { // cannot load
            std::cerr << "could not find sansation font" << std::endl; //throws error if could not find sansation.ttf
        }
        else {
            //loaded font, skip
        }

        //load jump sound
        if (!jump_sound_buffer.loadFromFile("jump.wav")) {
            std::cerr << "could not find jump sound" << std::endl;
        }
        else {
            //load buffer into sf::Sound
            jump_sound.setBuffer(jump_sound_buffer);
        }

        //load hit sound
        if (!hit_sound_buffer.loadFromFile("hit.wav")) {
            std::cerr << "could not find hit sound" << std::endl;
        }
        else {
            //load buffer into sf::Sound
            hit_sound.setBuffer(hit_sound_buffer);
        }

        //call initialize to setup player obj
        player.initialize({ window_len,window_hgt });


        //set up intro text line 1
        intro1.setFont(sansation); // set font to sansation, which we loaded from file
        intro1.setFillColor(sf::Color::White); //set color of msg object to UCLA Blue
        intro1.setCharacterSize(22); // set pt of msg object
        intro1.setString("Press any key to begin."); // set text of msg object
        intro1.setPosition(0, 0); // set pos of text based on window resolution

        //set up intro text line 2
        intro2.setFont(sansation); // set font to sansation, which we loaded from file
        intro2.setFillColor(sf::Color::White); //set color of msg object to UCLA Blue
        intro2.setCharacterSize(22); // set pt of msg object
        intro2.setString("Press Up to jump."); // set text of msg object
        intro2.setPosition(0, intro1.getLocalBounds().height); // set pos of text based on window resolution

        //set up intro text line 3
        intro3.setFont(sansation); // set font to sansation, which we loaded from file
        intro3.setFillColor(sf::Color::White); //set color of msg object to UCLA Blue
        intro3.setCharacterSize(22); // set pt of msg object
        intro3.setString("Get hit 3 times and lose."); // set text of msg object
        intro3.setPosition(0, intro2.getLocalBounds().height * 2); // set pos of text based on window resolution

        //set up score text
        score.setFont(sansation);
        score.setFillColor(sf::Color::White);
        score.setCharacterSize(22);
        std::string score_str("Score: " + std::to_string(score_num));
        score.setString(score_str);
        score.setPosition(0, 0);

        //pushes player and floor objects to to_draw for rendering
        to_draw.push_back(&player);
        to_draw.push_back(&floor);

        //pushes intro text objects to to_draw for rendering
        to_draw.push_back(&intro1);
        to_draw.push_back(&intro2);
        to_draw.push_back(&intro3);

    }

    void Game::start(const bool playing) {
        if (!playing) {
            //erase all intro text
            to_draw.erase(std::remove(to_draw.begin(), to_draw.end(), &intro1), to_draw.end());
            to_draw.erase(std::remove(to_draw.begin(), to_draw.end(), &intro2), to_draw.end());
            to_draw.erase(std::remove(to_draw.begin(), to_draw.end(), &intro3), to_draw.end());

            //push score text to to_draw
            to_draw.push_back(&score);
        }
    }

    void Game::play_jump_sound(){
        jump_sound.play();
    }

    const int Game::check_lives() {
        return player.check_lives();
    }

    void Game::end_game() {
        //setup game over text
        game_over.setFont(sansation);
        game_over.setFillColor(sf::Color::White);
        game_over.setCharacterSize(22);
        game_over.setString("Game Over!");
        game_over.setPosition(window_len / 2 - game_over.getLocalBounds().width / 2,
            window_hgt / 2 - game_over.getLocalBounds().height); // set pos of text based on window resolution

        //push game over text to to_draw for rendering
        to_draw.push_back(&game_over);
    }

    const std::vector<sf::Drawable*>* Game::get_curr_drawings() {
        return &to_draw;
    }

    //PLAYER_CHAR CLASS

    //player_char constructor
    Game::player_char::player_char(float sqr_dims) : sqr_len_hgt(sqr_dims), rect_hgt(sqr_dims), rect_len(sqr_dims * 3), lives(3) {}

    void Game::player_char::initialize(sf::Vector2f window_dims) {
        //set dims and color of base
        rectangle.setSize(sf::Vector2f(rect_len, rect_hgt));
        rectangle.setFillColor(sf::Color::Red);

        //set dims and color of head
        square.setSize(sf::Vector2f(sqr_len_hgt, sqr_len_hgt));
        square.setFillColor(sf::Color::Red);

        //set position of base and head
        rectangle.setPosition(0, window_dims.y - sqr_len_hgt * 2);
        square.setPosition(rect_len / 2 - sqr_len_hgt / 2, window_dims.y - sqr_len_hgt * 3);

        //set default positions of base and head
        rect_def_pos = (sf::Vector2f{ 0, window_dims.y - sqr_len_hgt * 2 });
        sqr_def_pos = (sf::Vector2f{ rect_len / 2 - sqr_len_hgt / 2, window_dims.y - sqr_len_hgt * 3 });
    }

    const float Game::player_char::curr_min_y_pos() {
        return (rectangle.getPosition().y + rect_hgt);
    }

    const int Game::player_char::check_lives() {
        return lives;
    }

    void Game::player_char::jump(bool& jumping, const sf::Time start, sf::Clock& clock) {

        //t and h variables for our piecewise function
        float t = (clock.getElapsedTime().asSeconds() - start.asSeconds());
        float h = (8 * sqr_len_hgt * t * (T - t));

        //piecewise function:

        //if t <= T* , h = 8bt(T-t)/T^2
        if (jumping && (h > 0) && (t <= T_star)) {
            rectangle.setPosition(sf::Vector2f{ rect_def_pos.x,rect_def_pos.y - h });
            square.setPosition(sf::Vector2f{ sqr_def_pos.x,sqr_def_pos.y - h });
        }
        //if T < t < T*, h = 0
        else if (jumping && T < t < T_star) {
            rectangle.setPosition(rect_def_pos);
            square.setPosition(sqr_def_pos);
            return;
        }
        // player is no longer jumping
        else {
            jumping = false;
        }
    }

    void Game::player_char::decrement_lives() {
        lives -= 1;
    }


    void Game::player_char::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // window.draw() draws base and head now
        target.draw(rectangle);
        target.draw(square);

    }

    //ENEMY CLASS

    //enemy constructor
    Game::enemy::enemy(float sqr_dims) : sqr_len_hgt(sqr_dims) {}

    void Game::enemy::initialize(sf::Vector2f window_dims) {
        
        //set size and color of square that represents enemy
        square.setSize(sf::Vector2f(sqr_len_hgt, sqr_len_hgt));
        square.setFillColor(sf::Color::Yellow);
       
        //sets spawn position
        square.setPosition(window_dims.x, window_dims.y - sqr_len_hgt * 2);
        //setTextureRect();
        
    }

    const float Game::enemy::get_hgt() {
        return sqr_len_hgt;
    }

    const bool Game::enemy::is_out_of_frame() {
        // makes sure square is fully out of frame before removal
        if (square.getPosition().x <= (-sqr_len_hgt)) {
            return true;
        }
        else { return false; }
    }

    const bool Game::enemy::is_at_player_x() {
        //checks if enemy x is within player x range
        if (square.getPosition().x <= sqr_len_hgt * 3) {
            return true;
        }
        else { return false; }
    }

    void Game::enemy::move() {
        // moves enemy 2 pixels left
        sf::Vector2f currPos = square.getPosition();
        square.setPosition({ currPos.x - 2, currPos.y });
    }

    void Game::enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const 
    {
        // window.draw() now draws square representing enemy
        target.draw(square);

    }




    void Game::step(bool one_point_five_elapsed, bool point_oh_one_elapsed, bool& jumping, sf::Time& elapsed,
        sf::Clock& clock) {
        //only call jump if player is currently jumping
        if (jumping) {
            player.jump(jumping, elapsed, clock);
        }
        //move_enemies if 0.01s has passed since last move_enemies call
        if (point_oh_one_elapsed) { move_enemies(); }

        //always call do_removals
        do_removals();

        //always call check_hits
        bool hit = check_hits();

        //step also houses player hit sound play and life decrement
        if (hit) {
            hit_sound.play();
            player.decrement_lives();
        }

        //call make_enemies if 1.5s has passed since last make_enemies call
        if (one_point_five_elapsed) { make_enemies(); }

    }


    void Game::do_removals() {
        for (enemy* _enemy : enemies) {
            if (_enemy->is_out_of_frame()) {

                //if enemy out of frame, remove it from enemies and to_draw
                enemies.erase(std::remove(enemies.begin(), enemies.end(), _enemy), enemies.end());
                to_draw.erase(std::remove(to_draw.begin(), to_draw.end(), _enemy), to_draw.end());

                //free enemy from memory
                _enemy = nullptr;
                free(_enemy);

                //player scores
                ++score_num;

                //increment score in str
                std::string score_str("Score: " + std::to_string(score_num));
                score.setString(score_str);
            }
            else { return; }
        }
    }

    const bool Game::check_hits() {
        bool hit = false;
        for (enemy* _enemy : enemies) {
            if (_enemy->is_at_player_x() && (player.curr_min_y_pos() >= (window_hgt - (_enemy->get_hgt() + (window_hgt / 6))))) {
                enemies.erase(std::remove(enemies.begin(), enemies.end(), _enemy), enemies.end());
                to_draw.erase(std::remove(to_draw.begin(), to_draw.end(), _enemy), to_draw.end());
                hit = true;
            }
        }
        return hit;
    }

    void Game::move_enemies() {

        //move all enemies in enemies vector
        for (enemy* _enemy : enemies) {
            _enemy->move();
        }
    }

    void Game::make_enemies() {
        //creates random generators
        std::random_device rd;
        std::mt19937 gen(rd());
        // give "true" 1/2 of the time
        // give "false" 1/2 of the time
        std::bernoulli_distribution d(0.5);

        if (d(gen)) {
            //create new enemy and init 
            enemy* _new = new enemy(window_hgt / 6);
            _new->initialize(sf::Vector2f({ window_len,window_hgt }));

            //put enemy into appropriate vectors
            enemies.push_back(_new);
            to_draw.push_back(_new);
        }
    }
