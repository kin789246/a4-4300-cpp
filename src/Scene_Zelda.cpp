#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "Scene_Menu.h"
#include "Scene_Zelda.h"
#include <memory>
#include <iostream>

Scene_Zelda::Scene_Zelda(GameEngine* game, std::string& levelPath)
: Scene(game) {
    init(m_levelPath);
}

void Scene_Zelda::init(const std::string& levelPath) {
    loadLevel(levelPath);
    m_gridText.setCharacterSize(9);
    m_gridText.setFont(m_game->assets().getFont("Mario"));

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW"); // toggle follow camera
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE"); // toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // toggle drawing (C)ollision Box
    registerAction(sf::Keyboard::G, "TOGGLE_GRID"); // toggle drawing (G)rid
                                                    //
    // todo: register the actions to play the game

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Space, "ATTACK");
}

void Scene_Zelda::loadLevel(const std::string& fileName) {
    m_entityManager = EntityManager();
    
    // todo: Load the level file and put all entities in the manager
    // use the getPosition() function below to convert room-tile coords
    // to game world coords
    
    spawnPlayer();
}

Vec2 Scene_Zelda::getPosition(int rx, int ry, int tx, int ty) const {
    // todo: implement this function, which takes in the room (rx, ry)
    // as well as the tile (tx, ty), and return the Vec2 game world
    // position of the center of the entity

    return Vec2(0, 0);
}

void Scene_Zelda::spawnPlayer() {
    auto p = m_entityManager.addEntity("player");
    p->add<CTransform>(Vec2(640, 480));
    p->add<CAnimation>(m_game->assets().getAnimation("LinkStandDown"), true);
    p->add<CBoundingBox>(Vec2(64, 64), true, false);
    p->add<CHealth>(7, 3);
    // todo: implement this function so that it uses the parameters input
    // from the level file
    // Those parameters should be stored in the m_playerConfig variable
}

void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity) {
    // todo: implement the spawning of the sword, which:
    // - should be given the appropriate lifespan
    // - should spawn at the appropriate location based on player's facing direction
    // - be given a damage value of 1
    // - should play the slash sound
}

void Scene_Zelda::update() {
    m_entityManager.update();

    // todo: implement pause functionality
    if (!m_pause) {
        sAI();
        sMovement();
        sStatus();
        sCollision();
        sCamera();
        m_currentFrame++;
    }
    
    sAnimation();
    sGUI();
    sRender();
}

void Scene_Zelda::sMovement() {
    // todo: 
}

void Scene_Zelda::sGUI() {
    ImGui::Begin("Scene Properties");
    if (ImGui::BeginTabBar("MyTabBar")) {
        if (ImGui::BeginTabItem("Debug")) {
            ImGui::Checkbox("Draw Grid", &m_drawGrid);
            ImGui::Checkbox("Draw Textures", &m_drawTextures);
            ImGui::Checkbox("Draw Debug", &m_drawCollision);
            ImGui::Checkbox("Follow Cam", &m_follow);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Animations")) {
            // todo:
            ImGui::Text("Do this");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Entity Manager")) {
            // todo:
            ImGui::Text("Do this");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}

void Scene_Zelda::sDoAction(const Action& action) {
    // todo:
    if (action.type() == "START") {
        if (action.name() == "TOGGLE_TEXTURE") {
            m_drawTextures = !m_drawTextures; 
        }
        else if (action.name() == "TOGGLE_COLLISION") { 
            m_drawCollision = !m_drawCollision; 
        }
        else if (action.name() == "TOGGLE_GRID") { 
            m_drawGrid = !m_drawGrid; 
        }
        else if (action.name() == "TOGGLE_FOLLOW") { 
            m_follow = !m_follow; 
        }
        else if (action.name() == "PAUSE") { 
            setPaused(!m_pause);
        }
        else if (action.name() == "QUIT") { 
            onEnd();
        }
    }
    else if (action.type() == "END") {
    }
}

void Scene_Zelda::sAI() {
    // todo: implement enemy AI
    // Follow AI
    // Patrol AI
}

void Scene_Zelda::sStatus() {
    // todo: implement lifespan and invincibility frames here
}

void Scene_Zelda::sCollision() {
    // todo:
    // implement entity - tile collisions
    // player - enemy collisions with approciate damage calculations
    // sword - NPC collisions
    // entity - heart collisions and life gain logic
    // black tile collisions / 'teleporting'
}

void Scene_Zelda::sAnimation() {
    // todo:
    // player facing direction
    // sword animation based on player facing
    // the sword should move if the player changes direction mid swing
    // destruction of entities with non-repeating finished animations
}

void Scene_Zelda::sCamera() {
    // todo: camera view logic
    // get the current view, which we will modify in the if-statement below
    sf::View view = m_game->window().getView();

    if (m_follow) {
        // player follow camera
    }
    else {
        // room-based camera
    }

    // then set the window view
    m_game->window().setView(view);
}

void Scene_Zelda::onEnd() {
    // todo: when the scene ends, change back to the MENU scene
    // stop the music
    // play the menu music
    // use m_game->changeScene(correct params);
    m_game->changeScene( "MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Zelda::setPaused(bool pause) {
    m_pause = pause;
}

void Scene_Zelda::sRender() {
    m_game->window().clear(sf::Color(255, 192, 122)); 
    sf::RectangleShape tick({ 1.0f, 6.0f });
    tick.setFillColor(sf::Color::Black);

    // draw all Entity textures / animations
    if (m_drawTextures) {
        for (auto e : m_entityManager.getEntities()) {
            auto& transform = e->get<CTransform>();
            sf::Color c = sf::Color::White;
            if (e->has<CInvicibility>()) {
                c = sf::Color(255, 255, 255, 128);
            }
            if (e->has<CAnimation>()) {
                auto& animation = e->get<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(
                    transform.pos.x, transform.pos.y
                );
                animation.getSprite().setScale(
                    transform.scale.x, transform.scale.y
                );
                animation.getSprite().setColor(c);
                m_game->window().draw(animation.getSprite());
            }
        }

        for (auto e : m_entityManager.getEntities()) {
            auto& transform = e->get<CTransform>();
            if (e->has<CHealth>()) {
                auto& h = e->get<CHealth>();
                Vec2 size(64, 6);
                sf::RectangleShape rect({ size.x, size.y });
                rect.setPosition(
                    transform.pos.x - 32,
                    transform.pos.y - 48
                );
                rect.setFillColor(sf::Color(96, 96, 96));
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(2);
                m_game->window().draw(rect);

                float ratio = (float)(h.current) / h.max;
                size.x *= ratio;
                rect.setSize({ size.x, size.y });
                rect.setFillColor(sf::Color(255, 0, 0));
                rect.setOutlineThickness(0);
                m_game->window().draw(rect);

                for (int i=0; i<h.max; i++) {
                    tick.setPosition(
                        rect.getPosition() + sf::Vector2f(i * 64 * 1.0 / h.max, 0)
                    );
                    m_game->window().draw(tick);
                }
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangle shape
    if (m_drawCollision) {
        sf::CircleShape dot(4);
        for (auto e : m_entityManager.getEntities()) {
            if (e->has<CBoundingBox>()) {
                auto& box = e->get<CBoundingBox>();
                auto& transform = e->get<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x-1, box.size.y-1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                if (box.blockMove && box.blockVision) {
                    rect.setOutlineColor(sf::Color::Black);
                }
                if (box.blockMove && !box.blockVision) {
                    rect.setOutlineColor(sf::Color::Blue);
                }
                if (!box.blockMove && box.blockVision) {
                    rect.setOutlineColor(sf::Color::Red);
                }
                if (!box.blockMove && !box.blockVision) {
                    rect.setOutlineColor(sf::Color::White);
                }
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    // draw the grid so that can easily debug
    if (m_drawGrid) {
        float leftX = m_game->window().getView().getCenter().x - width() / 2.0;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
            drawLine(Vec2(x, 0), Vec2(x, height()));
        }

        for (float y=0; y < height(); y += m_gridSize.y) {
            drawLine(Vec2(leftX, y), Vec2(rightX, y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x+3, y+2);
                m_game->window().draw(m_gridText);
            }
        }
    }
}
