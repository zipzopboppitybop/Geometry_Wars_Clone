#include "Game.h"
#include <fstream>
#include <iostream>

Game::Game(const std::string& config)
{
    init(config);
}

void Game::init(const std::string& path)
{
    std::ifstream fin(path);
    std::string temp;
    while (fin >> temp)
    {
        if (temp == "Window")
        {
            fin >> m_windowConfig.W >> m_windowConfig.H >> m_windowConfig.FR >> m_windowConfig.FS;
        }
        
        if (temp == "Player")
        {
            fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
        }

        if (temp == "Enemy")
        {
            fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SP;
        }

        if (temp == "Bullet")
        {
            fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }
            
    }

    if (m_windowConfig.FS == 1)
    {
        m_windowConfig.FS = sf::Style::Fullscreen;
    }
    else
    {
        m_windowConfig.FS = sf::Style::Default;
    }
    
    // set up default window parameters
    m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H), "Geometry Wars Clone", m_windowConfig.FS);
    m_window.setFramerateLimit(m_windowConfig.FR);

    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        m_entities.update();

        if (!m_paused)
        {
            sEnemySpawner();
            sMovement();
            sCollision();
            m_currentFrame++;
            sLifeSpan();
        }

        sRender();
        sUserInput();

        // respawn player if they die
        if (!m_player->isActive())
        {
            spawnPlayer();
        }
    }
}

void Game::setPaused()
{
    //TODO
    m_paused = !m_paused;
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
    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

    // input component to control player
    entity->cInput = std::make_shared<CInput>();

    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    //make this entity the player
    m_player = entity;

    
}

// spawn an enemy at random position
void Game::spawnEnemy()
{
    //TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
    // the enemy must be spawn completely within the bounds of the window

    auto entity = m_entities.addEntity("enemy");

    float ex = rand() % m_window.getSize().x ;
    float ey = rand() % m_window.getSize().y ;
    auto center = Vec2(m_window.getSize().x, m_window.getSize().y);
    auto enemyPos = Vec2(ex, ey);
    float angle = -1 + (rand() % (1 + 1 + 1));
    int randomV = m_enemyConfig.VMIN + (rand() % m_enemyConfig.VMAX);
    float randomS = m_enemyConfig.SMIN + (rand() % 1 + m_enemyConfig.SMAX - m_enemyConfig.SMIN);
    float randomFR = 0 + (rand() % 255);
    float randomFG = 0 + (rand() % 255);
    float randomFB = 0 + (rand() % 255);
    auto normalize = center.normalize(enemyPos);

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(randomS * normalize.x, randomS * normalize.y), angle);
    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, randomV, sf::Color(randomFR, randomFG, randomFB), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
    

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
    auto bullet = m_entities.addEntity("bullet");
    auto entity_stuff = Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y);
    float angle = target.angle(entity_stuff);
    auto normalize = target.normalize(entity_stuff);
    bullet->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(m_bulletConfig.S * normalize.x, m_bulletConfig.S * normalize.y), angle);
    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bullet->cShape = std::make_shared<CShape>( m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
    bullet->cLifeSpan = std::make_shared<CLifeSpan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    // TODO: implement weapon
      
    auto bulletOne = m_entities.addEntity("bullet");
    auto entity_stuff = Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y);
    bulletOne->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(10, -.05), -.05);
    bulletOne->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bulletOne->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_bulletConfig.OT);
    bulletOne->cLifeSpan = std::make_shared<CLifeSpan>(10);

    auto bulletTwo = m_entities.addEntity("bullet");
    bulletTwo->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(.008, -10), -1.5);
    bulletTwo->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bulletTwo->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_bulletConfig.OT);
    bulletTwo->cLifeSpan = std::make_shared<CLifeSpan>(10);

    auto bulletThree = m_entities.addEntity("bullet");
    bulletThree->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(-10, -.08), -3);
    bulletThree->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bulletThree->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_bulletConfig.OT);
    bulletThree->cLifeSpan = std::make_shared<CLifeSpan>(10);

    auto bulletFour = m_entities.addEntity("bullet");
    bulletFour->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(-.08, 10), 1.6);
    bulletFour->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bulletFour->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_bulletConfig.OT);
    bulletFour->cLifeSpan = std::make_shared<CLifeSpan>(10);

    auto bulletFive = m_entities.addEntity("bullet");
    bulletFive->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(-10, 10), 2);
    bulletFive->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bulletFive->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_bulletConfig.OT);
    bulletFive->cLifeSpan = std::make_shared<CLifeSpan>(10);

    auto bulletSix = m_entities.addEntity("bullet");
    bulletSix->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(10, -10), -2);
    bulletSix->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bulletSix->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_bulletConfig.OT);
    bulletSix->cLifeSpan = std::make_shared<CLifeSpan>(10);

    auto bulletSeven = m_entities.addEntity("bullet");
    bulletSeven->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(10, 10), 2);
    bulletSeven->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bulletSeven->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_bulletConfig.OT);
    bulletSeven->cLifeSpan = std::make_shared<CLifeSpan>(10);

    auto bulletEight = m_entities.addEntity("bullet");
    bulletEight->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(-10, -10), 2);
    bulletEight->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bulletEight->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_bulletConfig.OT);
    bulletEight->cLifeSpan = std::make_shared<CLifeSpan>(10);

    
}

void Game::sMovement()
{
    // player movement
    m_player->cTransform->velocity = { 0,0 };

    if (m_player->cInput->up)
    {
        m_player->cTransform->velocity.y += -m_playerConfig.S;

        if (m_player->cTransform->pos.y - m_player->cCollision->radius < 0)
        {
            m_player->cTransform->velocity.y = 0;
        }
    }

    if (m_player->cInput->down)
    {
        m_player->cTransform->velocity.y += m_playerConfig.S;

        if (m_player->cTransform->pos.y + m_player->cCollision->radius  > m_window.getSize().y)
        {
            m_player->cTransform->velocity.y = 0;

        }
    }

    if (m_player->cInput->right)
    {
        m_player->cTransform->velocity.x += m_playerConfig.S;

        if (m_player->cTransform->pos.x + m_player->cCollision->radius  > m_window.getSize().x)
        {
            m_player->cTransform->velocity.x = 0;

        }
    }

    if (m_player->cInput->left)
    {
        m_player->cTransform->velocity.x += -m_playerConfig.S;

        if (m_player->cTransform->pos.x - m_player->cCollision->radius < 0)
        {
            m_player->cTransform->velocity.x = 0;

        }
    }
    
    // move all entities in m_entities vector
    for (auto e : m_entities.getEntities())
    {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }
}

void Game::sLifeSpan()
{
    //TODO: implent all lifespan functionality
    // for all entities
    // if entity has no lifespan component, skip it
    // if entity has > 0 remaining lifespan, subtract 1
    // if it has lifespan and is alive sclae its alpha channel properly
    // if it has lifesapn and its time is up destroy it

    for (auto e : m_entities.getEntities())
    {
        if (e->cLifeSpan)
        {
            e->cLifeSpan->remaining--;

            if (e->cLifeSpan->remaining == 0)
            {
                e->destroy();
            }
        }

    }

}

void Game::sCollision()
{

    
    // if bullet hits enemy they both die
    for (auto b : m_entities.getEntities("bullet"))
    {
        for (auto e : m_entities.getEntities("enemy"))
        {
           auto enemy_stuff = Vec2(e->cTransform->pos.x, e->cTransform->pos.y);
           auto bullet_stuff = Vec2(b->cTransform->pos.x, b->cTransform->pos.y);
           float dist = bullet_stuff.dist(enemy_stuff);
           if (dist < b->cCollision->radius + e->cCollision->radius)
           {
               b->destroy();
               e->destroy();
           }
        }
    }

    // if enemy hits player they both die
    for (auto e : m_entities.getEntities("enemy"))
    {
       auto enemy_stuff = Vec2(e->cTransform->pos.x, e->cTransform->pos.y);
       auto player_stuff = Vec2(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
       float dist = player_stuff.dist(enemy_stuff);
       if (dist < e->cCollision->radius + m_player->cCollision->radius)
       {
          e->destroy();
          m_player->destroy();
       }
    }

    // if enemy hits walls they will bounce off
    for (auto e : m_entities.getEntities("enemy"))
    {
        if (e->cTransform->pos.x - e->cCollision->radius < 0)
        {
            e->cTransform->velocity.x *= -1;
        }

        if (e->cTransform->pos.x + e->cCollision->radius > m_window.getSize().x)
        {
            e->cTransform->velocity.x *= -1;
        }

        if (e->cTransform->pos.y - e->cCollision->radius < 0)
        {
            e->cTransform->velocity.y *= -1;
        }

        if (e->cTransform->pos.y + e->cCollision->radius > m_window.getSize().y)
        {
            e->cTransform->velocity.y *= -1;
        }
    }
}

void Game::sEnemySpawner()
{
    // Spawn an enemy by subtracting the last time an enemy was spawned and the current frame
    if (m_currentFrame - m_lastEnemySpawnTime == m_enemyConfig.SP) spawnEnemy();
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
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        // Pause Input
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Space)
            {
                setPaused();
            }
        }
         
        // Movement Inputs Pressed
        if (!m_paused)
        {
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::W:
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = true;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = true;
                    break;
                default:break;
                }
            }

            // Movement Inputs Released
            if (event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = false;
                    break;
                default:break;
                }
            }

            // Attack Inputs
            if (event.type == sf::Event::MouseButtonPressed)
            {
                switch (event.mouseButton.button)
                {
                case sf::Mouse::Left:
                    spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
                    break;
                case sf::Mouse::Right:
                    spawnSpecialWeapon(m_player);
                    break;
                default:break;
                }
            }
        }
    }
}