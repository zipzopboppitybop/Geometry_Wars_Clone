#include "Game.h"
#include <fstream>
#include <iostream>

Game::Game(const std::string& config)
{
    init(config);
}

void Game::init(const std::string& path)
{
    // TODO: read in config file here
    // use the premade PlayerConfig, EnemyConfig and BulletConfig variables
    std::ifstream fin(path);
    // fin >> m_playerConfig.SR >>
    // set up default window parameters
    m_window.create(sf::VideoMode(1280, 720), "Geometry Wars Clone");
    m_window.setFramerateLimit(60);

    spawnPlayer();
}

void Game::run()
{
    //TODO add pause here
    // some systems should function while paused(rendering)
    // some systems shouldn't (movement input)

    while (m_running)
    {
        m_entities.update();

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sRender();

        //increment the current frame;
        // may need to be moved when pause implemented
        m_currentFrame++;
    }
}

void Game::setPaused(bool paused)
{
    //TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
    //TODO: finish adding all properties of the player with the correct values from the config

    // We Create every entity by calling EntityManager.addEnitity(tag)
    //This returns a std::shared_pty<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    //Give this entity a transform so it spawns at middle of screen with velocity (1,1) and angle 0
    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;

    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(1.0f, 1.0f), 0.0f);

    //The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickenss 4
    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    // input component to control player
    entity->cInput = std::make_shared<CInput>();

    //make this entity the player
    m_player = entity;
}

// spawn an enemy at random position
void Game::spawnEnemy()
{
    //TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
    // the enemy must be spawn completely within the bounds of the window

    auto entity = m_entities.addEntity("enemy");

    float ex = rand() % m_window.getSize().x / 2.0f;
    float ey = rand() % m_window.getSize().y / 2.0f;

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(1.0f, 1.0f), 0.0f);

    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    //record when the most enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity>  e)
{
    //TODO: spawn small enemies at the location of input enemy e
   // when we create the smaller enemy, we have to read the values of the original enemy
   // - spawn a number of small enemies equal to the vertices of the original enemy
   // - set each small enemy ot thte same color as the original, half the size
   // - small enemies are worth double points of the original enemy

}


// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
    // TODO: implement the spawning of a bullet which travels toward target
    // -bullet speed is given as a scalar speed
    // - you must set the velocity using formula in notes

    auto bullet = m_entities.addEntity("bullet");
    auto entity_stuff = Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y);
    float distance = target.dist(entity_stuff);
    float angle = target.angle(entity_stuff);
    auto normalize = target.normalize(entity_stuff);
    std::cout << distance << "\n";
    bullet->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(8 * normalize.x, 8 * normalize.y), angle);
    bullet->cShape = std::make_shared<CShape>(10, 8, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 2);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    // TODO: implement weapon
}

void Game::sMovement()
{
    //TODO: implement all entity movement in this function
    // you should read the m_player->cInput movement component to determine if the player is moving

    // player movement
    m_player->cTransform->velocity = { 0,0 };

    if (m_player->cInput->up)
    {
        m_player->cTransform->velocity.y = -5;
    }

    if (m_player->cInput->down)
    {
        m_player->cTransform->velocity.y = 5;
    }

    if (m_player->cInput->right)
    {
        m_player->cTransform->velocity.x = 5;
    }

    if (m_player->cInput->left)
    {
        m_player->cTransform->velocity.x = -5;
    }
    
    // move all entities in m_entities vector
    for (auto e : m_entities.getEntities())
    {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    //m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    //m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

    // bullet movement

    
}

void Game::sLifeSpan()
{
    //TODO: implent all lifespan functionality
    // for all entities
    // if entity has no lifespan component, skip it
    // if entity has > 0 remaining lifespan, subtract 1
    // if it has lifespan and is alive sclae its alpha channel properly
    // if it has lifesapn and its time is up destroy it
}

void Game::sCollision()
{
    //TODO: implement all proper collisions between entities
    // use collision radius not shape radius
}

void Game::sEnemySpawner()
{
    // Spawn an enemy by subtracting the last time an enemy was spawned and the current frame to equal 3 seconds
    if (m_currentFrame - m_lastEnemySpawnTime == 180) spawnEnemy();
}


// Renders the game
void Game::sRender()
{
    // Clear the window every frame so that the same entity doesn't get drawn over and over
    m_window.clear();

    for (auto e : m_entities.getEntities())
    {
        // set the position of the shape based on the entity's transform->pos
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

        //set the rotation of the shape based on the entity's transform->angle
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);

        // draw entities sf::CircleShape
        m_window.draw(e->cShape->circle);
    }

    m_window.display();
}

void Game::sUserInput()
{
    //TODO: handle user input here
    // not that you should only be setting the player's input component variables here
    // you should not implement the player's movement logic here
    // the movement system will read the variables you set in this function

    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        // Movement Inputs
        // this event is triggered when the W key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = true;
                break;
            default:break;
            }
        }

        // this event is triggered when the W key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = false;
                break;
            default:break;
            }
        }

        // this event is triggered when the S key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::S:
                m_player->cInput->down = true;
                break;
            default:break;
            }
        }

        // this event is triggered when the S key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::S:
                m_player->cInput->down = false;
                break;
            default:break;
            }
        }

        // this event is triggered when the D key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::D:
                m_player->cInput->right = true;
                break;
            default:break;
            }
        }

        // this event is triggered when the D key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::D:
                m_player->cInput->right = false;
                break;
            default:break;
            }
        }

        // this event is triggered when the A key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::A:
                m_player->cInput->left = true;
                break;
            default:break;
            }
        }

        // this event is triggered when the A key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::A:
                m_player->cInput->left = false;
                break;
            default:break;
            }
        }

        // Attack Input
        // this event is triggered when left mouse button is pressed
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }
        }


    }
}