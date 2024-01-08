#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "Components.h"
#include "Scene_Menu.h"
#include "Scene_Zelda.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <string>

Scene_Zelda::Scene_Zelda(GameEngine* game, std::string& levelPath)
: Scene(game), m_levelPath(levelPath) {
    init(m_levelPath);
}

void Scene_Zelda::init(const std::string& levelPath) {
    loadLevel(levelPath);
    m_gridText.setCharacterSize(9);
    m_gridText.setFont(m_game->assets().getFont("Mario"));

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    // toggle follow camera
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW"); 
    // toggle drawing (T)extures
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE"); 
    // toggle drawing (C)ollision Box
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); 
    // toggle drawing (G)rid
    registerAction(sf::Keyboard::G, "TOGGLE_GRID"); 

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
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Could not load level config file!" << fileName << "\n";
        exit(-1);
    }

    std::string head;
    while (file >> head) {
        if (head == "Player") {
            file >> m_playerConfig.X >> m_playerConfig.Y
                >> m_playerConfig.CX >> m_playerConfig.CY
                >> m_playerConfig.SPEED
                >> m_playerConfig.HEALTH;
            std::cout << "Loaded Player\n";
            spawnPlayer();
        }
        else if (head == "Tile") {
            std::string name;
            int rx, ry, tx, ty, bm, bv;
            file >> name >> rx >> ry >> tx >> ty >> bm >> bv;
            auto tile = m_entityManager.addEntity("tile");
            tile->add<CAnimation>(m_game->assets().getAnimation(name), true);
            tile->add<CTransform>(getPosition(rx, ry, tx, ty));
            tile->add<CBoundingBox>(
                tile->get<CTransform>().pos,
                Vec2(64, 64),
                bm,
                bv
            );
            tile->add<CDraggable>();
            std::cout << "Loaded Tile " << name << std::endl;
        }
        else if (head == "NPC") {
            std::string name, ai;
            int rx, ry, tx, ty, bm, bv, h, d;
            file >> name >> rx >> ry >> tx >> ty >> bm >> bv >> h >> d >> ai;
            auto npc = m_entityManager.addEntity("npc");
            npc->add<CAnimation>(m_game->assets().getAnimation(name), true);
            npc->add<CTransform>(getPosition(rx, ry, tx, ty));
            npc->add<CBoundingBox>(
                npc->get<CTransform>().pos,
                Vec2(64, 64),
                false,
                false
            );
            npc->add<CHealth>(h, h);
            npc->add<CDamage>(d);
            if (ai == "Follow") {
                float s;
                file >> s;
                npc->add<CFollowPlayer>(getPosition(rx, ry, tx, ty), s);
            }
            else if (ai == "Patrol") {
                float s;
                int n;
                file >> s >> n;
                std::vector<Vec2> positions;
                for (int i=0; i<n; i++) {
                    int xi, yi;
                    file >> xi >> yi;
                    positions.push_back(Vec2(xi, yi));
                }
                npc->add<CPatrol>(positions, s);
            }
            std::cout << "Loaded NPC " << name << " with AI " << ai << std::endl;  
        }
        else {
            std::cerr << "head to " << head << "\n";
            std::cerr << "The config file format is incorrect!\n";
            exit(-1);
        }
    }
}

Vec2 Scene_Zelda::getPosition(int rx, int ry, int tx, int ty) const {
    // todo: implement this function, which takes in the room (rx, ry)
    // as well as the tile (tx, ty), and return the Vec2 game world
    // position of the center of the entity

    return Vec2(
        rx * (float)width() + tx * m_gridSize.x - m_gridSize.x / 2.0,
        ry * (float)height() + ty * m_gridSize.y - m_gridSize.y / 2.0
    );
}

void Scene_Zelda::spawnPlayer() {
    auto p = m_entityManager.addEntity("player");
    p->add<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    p->add<CAnimation>(m_game->assets().getAnimation("LinkStandDown"), true);
    p->add<CBoundingBox>(
        Vec2(m_playerConfig.X, m_playerConfig.Y), 
        Vec2(m_playerConfig.CX, m_playerConfig.CY),
        true, 
        false
    );
    p->add<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH / 2);
    p->add<CState>(PlayerState::STANDDOWN);
    // todo: implement this function so that it uses the parameters input
    // from the level file
    // Those parameters should be stored in the m_playerConfig variable
}

void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity) {
    // todo: implement the spawning of the sword, which:
    // - should spawn at the appropriate location based on player's facing direction
    auto sword = m_entityManager.addEntity("sword");
    Vec2 facing = entity->get<CTransform>().facing;
    Vec2 boxC = entity->get<CBoundingBox>().center;
    int swordL = 45, swordW = 15;
    Vec2 swordC = Vec2(
       boxC.x + facing.x * (swordL + 10), 
       boxC.y + facing.y * (swordL + 10) 
    );
    sword->add<CTransform>(swordC);
    if (facing.x != 0) {
        sword->add<CBoundingBox>(swordC, Vec2(swordL, swordW), false, false);
    }
    else if (facing.y != 0) {
        sword->add<CBoundingBox>(swordC, Vec2(swordW, swordL), false, false);
    }
    
    // - should be given the appropriate lifespan
    sword->add<CLifespan>(4, m_currentFrame);

    // - be given a damage value of 1
    sword->add<CDamage>(1);

    // - should play the slash sound
}

void Scene_Zelda::update() {
    m_entityManager.update();

    // todo: implement pause functionality
    if (!m_pause) {
        sDrag();
        sAI();
        sMovement();
        sStatus();
        sAnimation();
        sCollision();
        sCamera();
        m_currentFrame++;
    }
    
    sGUI();
    sRender();
}

void Scene_Zelda::sMovement() {
    // todo: 
    auto p = player();
    if (p) {
        p->get<CTransform>().velocity = Vec2(0, 0);
        if (
            p->get<CInput>().up && !p->get<CInput>().down &&
            !p->get<CInput>().left && !p->get<CInput>().right
            ) {
            p->get<CTransform>().velocity.y = -m_playerConfig.SPEED;
            p->get<CTransform>().facing.y = -1;
        }
        else if (
            !p->get<CInput>().up && p->get<CInput>().down &&
            !p->get<CInput>().left && !p->get<CInput>().right
        ) {
            p->get<CTransform>().velocity.y = m_playerConfig.SPEED;
            p->get<CTransform>().facing.y = 1;
        }
        else if (
            !p->get<CInput>().up && !p->get<CInput>().down &&
            p->get<CInput>().left && !p->get<CInput>().right
        ) {
            p->get<CTransform>().velocity.x = -m_playerConfig.SPEED;
            p->get<CTransform>().facing.x = -1;
        }
        else if (
            !p->get<CInput>().up && !p->get<CInput>().down &&
            !p->get<CInput>().left && p->get<CInput>().right
        ) {
            p->get<CTransform>().velocity.x = m_playerConfig.SPEED;
            p->get<CTransform>().facing.x = 1;
        }
        if (p->get<CInput>().attack) {
            spawnSword(p);
        }
    }

    for (auto e : m_entityManager.getEntities()) {
        if (e->has<CTransform>()) {
            e->get<CTransform>().prevPos = e->get<CTransform>().pos;
            e->get<CBoundingBox>().prevCenter = e->get<CBoundingBox>().center;
            e->get<CBoundingBox>().center += e->get<CTransform>().velocity;
            e->get<CTransform>().pos += e->get<CTransform>().velocity;
        }
    }
}

void Scene_Zelda::sGUI() {
    ImGui::Begin("Scene Properties");
    if (ImGui::BeginTabBar("MyTabBar")) {
        if (ImGui::BeginTabItem("Debug")) {
            ImGui::Checkbox("Draw Grid (G)", &m_drawGrid);
            ImGui::Checkbox("Draw Textures (T)", &m_drawTextures);
            ImGui::Checkbox("Draw Debug (C)", &m_drawCollision);
            ImGui::Checkbox("Follow Cam (Y)", &m_follow);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Tiles")) {
            // todo:
            ImGui::Text("Click a Tile to create on the Game, then drag to move.");
            static bool bm = true; 
            static bool bv = true;
            ImGui::Checkbox("Block Movement", &bm);
            ImGui::Checkbox("Block Vision", &bv);
            int i = 0;
            for (const auto& [name, anim] : m_game->assets().getAnimations()) {
                if (name.find("Tile") != std::string::npos) {
                    if (i++ % 5 != 0) ImGui::SameLine();
                    if (ImGui::ImageButton(anim.getSprite())) {
                        auto tile = m_entityManager.addEntity("tile");
                        tile->add<CAnimation>(
                            m_game->assets().getAnimation(name), true
                        );
                        auto view = m_game->window().getView().getCenter();
                        tile->add<CTransform>(Vec2(view.x, view.y));
                        tile->add<CBoundingBox>(
                            Vec2(view.x, view.y),
                            Vec2(64, 64), 
                            bm, 
                            bv
                        );
                        tile->add<CDraggable>();
                        std::cout << "create " << name
                            << " with block movement=" << bm
                            << " and block vision=" << bv << std::endl;
                    }
                    ImGui::SetItemTooltip("%s", name.c_str());
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Sounds")) {
            // todo:
            for (auto& [name, sound] : m_game->assets().getSounds()) {
                ImGui::PushID(name.c_str());
                if (ImGui::Button("play")) {
                    sound.play();
                }
                ImGui::PopID();
                ImGui::SameLine();
                ImGui::PushID(name.c_str());
                if (ImGui::Button("stop")) {
                    sound.stop();
                }
                ImGui::PopID();
                ImGui::SameLine();
                ImGui::Text("%s", name.c_str());
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Entity Manager")) {
            // todo:
            static ImGuiTreeNodeFlags tflags = ImGuiTreeNodeFlags_Framed;
            if (ImGui::TreeNodeEx("Entities by Tag", tflags))
            {
                for (auto& [tag, entityVec] : m_entityManager.getEntityMap()) {
                    if (ImGui::TreeNodeEx(tag.c_str(), tflags)) {
                        for (auto e: entityVec) {
                            ImGui::Text("%s", std::to_string(e->id()).c_str());
                            ImGui::SameLine();
                            ImGui::Text("%s", e->tag().c_str());
                            ImGui::SameLine();
                            ImGui::Text("(%.f, %.f)", 
                                e->get<CTransform>().pos.x,
                                e->get<CTransform>().pos.y
                            );
                        }

                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("All Entities", tflags)) {
                for (auto e : m_entityManager.getEntities()) {
                    ImGui::Text("%s", std::to_string(e->id()).c_str());
                    ImGui::SameLine();
                    ImGui::Text("%s", e->tag().c_str());
                    ImGui::SameLine();
                    ImGui::Text("(%.f, %.f)", 
                        e->get<CTransform>().pos.x,
                        e->get<CTransform>().pos.y
                    );
                }

                ImGui::TreePop();
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}

void Scene_Zelda::sDoAction(const Action& action) {
    // todo:
    if (action.name() == "MOUSE_MOVE") {
        m_mousePos = action.pos();
    }
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
        else if (action.name() == "UP") {
            if (!player()) return;
            player()->get<CInput>().up = true;
        }
        else if (action.name() == "DOWN") {
            if (!player()) return;
            player()->get<CInput>().down = true;
        }
        else if (action.name() == "LEFT") {
            if (!player()) return;
            player()->get<CInput>().left = true;
        }
        else if (action.name() == "RIGHT") {
            if (!player()) return;
            player()->get<CInput>().right = true;
        }
        else if (action.name() == "ATTACK") {
            if (!player()) return;
            player()->get<CInput>().attack = true;
        }
        else if (action.name() == "QUIT") { 
            onEnd();
        }
        else if (action.name() == "LEFT_CLICK") {
            // std::cout << action.toString() << std::endl; 
            Vec2 pos = posWinToWorld(action.pos());
            // std::cout << "world position = " << pos.x << " " << pos.y << " \n";
            if (m_eOnDragging) {
                m_eOnDragging->get<CDraggable>().dragging = false;
                m_eOnDragging = nullptr;
                return;
            }
            for (auto e : m_entityManager.getEntities()) {
                if (m_physics.IsInside(pos, e)) {
                    if (e->has<CDraggable>()) {
                        if (!m_eOnDragging) {
                            e->get<CDraggable>().dragging = true;
                            m_eOnDragging = e;
                        }
                    }
                }
            }
        }
    }
    else if (action.type() == "END") {
        if (action.name() == "UP") {
            if (!player()) return;
            player()->get<CInput>().up = false;
        }
        else if (action.name() == "DOWN") {
            if (!player()) return;
            player()->get<CInput>().down = false;
        }
        else if (action.name() == "LEFT") {
            if (!player()) return;
            player()->get<CInput>().left = false;
        }
        else if (action.name() == "RIGHT") {
            if (!player()) return;
            player()->get<CInput>().right = false;
        }
        else if (action.name() == "ATTACK") {
            if (!player()) return;
            player()->get<CInput>().attack = false;
        }
    }
}

void Scene_Zelda::sAI() {
    // todo: implement enemy AI
    // Follow AI
    // Patrol AI
}

void Scene_Zelda::sStatus() {
    // todo: implement lifespan and invincibility frames here
    for (auto e : m_entityManager.getEntities()) {
        if (e->has<CLifespan>()) {
            if (m_currentFrame - e->get<CLifespan>().frameCreated > 
                e->get<CLifespan>().lifespan) {
                e->destroy();
            }
        }
    }
}

void Scene_Zelda::sCollision() {
    // todo:
    // implement entity - tile collisions
    // player - enemy collisions with approciate damage calculations
    // sword - NPC collisions
    // entity - heart collisions and life gain logic
    // black tile collisions / 'teleporting'
    auto p = player();
    if (p) {
        // player and tiles
        for (auto t : m_entityManager.getEntities("tile")) {
            RectOverlap ro = m_physics.AisNearB(p, t, m_gridSize);
            switch (ro.direction) {
                case ODirection::UP:
                    // std::cout << "block player move down\n";
                    if (t->get<CBoundingBox>().blockMove) {
                        p->get<CTransform>().pos.y -= ro.overlap.y;
                        p->get<CBoundingBox>().center.y -= ro.overlap.y;
                    }
                    break;
                case ODirection::Down:
                    // std::cout << "block player move up\n";
                    if (t->get<CBoundingBox>().blockMove) {
                        p->get<CTransform>().pos.y += ro.overlap.y;
                        p->get<CBoundingBox>().center.y += ro.overlap.y;
                    }
                    break;
                case ODirection::LEFT:
                    // std::cout << "block player move right\n";
                    if (t->get<CBoundingBox>().blockMove) {
                        p->get<CTransform>().pos.x -= ro.overlap.x;
                        p->get<CBoundingBox>().center.x -= ro.overlap.x;
                    }
                    break;
                case ODirection::RIGHT:
                    // std::cout << "block player move left\n";
                    if (t->get<CBoundingBox>().blockMove) {
                        p->get<CTransform>().pos.x += ro.overlap.x;
                        p->get<CBoundingBox>().center.x += ro.overlap.x;
                    }
                    break;
                case ODirection::NONE:
                    break;
            }
        }

        // player and NPCs
        for (auto npc : m_entityManager.getEntities("npc")) {
            
        }
    }
}

void Scene_Zelda::sAnimation() {
    // todo:
    // player facing direction
    auto p = player();
    if (p) {
        if (p->get<CInput>().attack) {
            changePlayerStateTo("attack", p->get<CTransform>().facing);
        }
        // face right
        else if (p->get<CTransform>().velocity.x > 0) {
            p->get<CTransform>().facing = Vec2(1, 0);
            changePlayerStateTo("move", p->get<CTransform>().facing);
            p->get<CTransform>().scale.x = 1;
        }
        // face left
        else if (p->get<CTransform>().velocity.x < 0) {
            p->get<CTransform>().facing = Vec2(-1, 0);
            changePlayerStateTo("move", p->get<CTransform>().facing);
            p->get<CTransform>().scale.x = -1;
        }
        // face up
        else if (p->get<CTransform>().velocity.y < 0) {
            p->get<CTransform>().facing = Vec2(0, -1);
            changePlayerStateTo("move", p->get<CTransform>().facing);
        }
        // face down
        else if (p->get<CTransform>().velocity.y > 0) {
            p->get<CTransform>().facing = Vec2(0, 1);
            changePlayerStateTo("move", p->get<CTransform>().facing);
        } 
        // stand
        else if (p->get<CTransform>().velocity.x == 0 &&
            p->get<CTransform>().velocity.y == 0) {
            changePlayerStateTo("stand", p->get<CTransform>().facing);
        }

        if (p->get<CState>().changeAnimate) {
            switch (p->get<CState>().state) {
                case PlayerState::STANDDOWN:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkStandDown"),
                        true
                    );
                    break;
                case PlayerState::STANDUP:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkStandUp"),
                        true
                    );
                    break;
                case PlayerState::STANDRIGHT:
                case PlayerState::STANDLEFT:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkStandRight"),
                        true
                    );
                    break;
                case PlayerState::MOVEDOWN:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkMoveDown"),
                        true
                    );
                    break;
                case PlayerState::MOVEUP:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkMoveUp"),
                        true
                    );
                    break;
                case PlayerState::MOVERIGHT:
                case PlayerState::MOVELEFT:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkMoveRight"),
                        true
                    );
                    break;
                case PlayerState::ATTACKDOWN:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkAtkDown"),
                        true
                    );
                    break;
                case PlayerState::ATTACKUP:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkAtkUp"),
                        true
                    );
                    break;
                case PlayerState::ATTACKRIGHT:
                case PlayerState::ATTACKLEFT:
                    p->add<CAnimation>(
                        m_game->assets().getAnimation("LinkAtkRight"),
                        true
                    );
                    break;
            }
        }
    }
    // sword animation based on player facing
    // the sword should move if the player changes direction mid swing
    // destruction of entities with non-repeating finished animations
    for (auto e : m_entityManager.getEntities()) {
        if (e->has<CAnimation>()) {
            e->get<CAnimation>().animation.update();
        }
    }
}

void Scene_Zelda::sCamera() {
    // todo: camera view logic
    // get the current view, which we will modify in the if-statement below
    sf::View view = m_game->window().getView();

    auto p = player();
    if (!p) return;
    auto& pPos = p->get<CTransform>().pos;
    if (m_follow) {
        // player follow camera
        view.setCenter(pPos.x, pPos.y);
    }
    else {
        // room-based camera
        Vec2 r = getRoomXY(pPos);
        view.setCenter(
            r.x * (float)width() + width() / 2.0, 
            r.y * (float)height() + height() / 2.0
        );
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
        // draw bounding box
        sf::CircleShape dot(4);
        for (auto e : m_entityManager.getEntities()) {
            if (e->has<CBoundingBox>()) {
                auto& box = e->get<CBoundingBox>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x-1, box.size.y-1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(box.center.x, box.center.y);
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
                
                // draw line between player and npc
                if (!player()) continue;
                if (e->tag() == "npc") {
                    auto& ePos = e->get<CTransform>().pos;
                    auto view = m_game->window().getView().getCenter();
                    if (ePos.x >= view.x - (float)width()/2.0&&
                        ePos.x <= view.x + (float)width()/2.0 &&
                        ePos.y >= view.y - (float)height()/2.0 &&
                        ePos.y <= view.y + (float)height()/2.0 
                    ) {
                        drawLine(
                            player()->get<CTransform>().pos,
                            e->get<CTransform>().pos
                        );
                    }
                }
            }

            // draw patrol points
            if (e->has<CFollowPlayer>()) {
                auto& h = e->get<CFollowPlayer>().home;
                dot.setPosition(h.x, h.y);
                m_game->window().draw(dot);
            }
            if (e->has<CPatrol>()) {
                for (auto p : e->get<CPatrol>().positions) {
                    Vec2 r = getRoomXY(e->get<CTransform>().pos);
                    Vec2 pos = getPosition(r.x, r.y, p.x, p.y);
                    dot.setPosition(pos.x, pos.y);
                    m_game->window().draw(dot);
                }
            }
        }
    }

    // draw the grid so that can easily debug
    if (m_drawGrid) {
        float leftX = m_game->window().getView().getCenter().x - width() / 2.0;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);
        float topY = m_game->window().getView().getCenter().y - height() / 2.0;
        float bottomY = topY + height() + m_gridSize.y;
        float nextGridY = topY - ((int)topY % (int)m_gridSize.x);

        // draw room coordinate
        auto p = player();
        if (p) {
            Vec2 r = getRoomXY(p->get<CTransform>().pos);
            m_gridText.setString(
                "room \n" + std::to_string((int)r.x) + " " +
                std::to_string((int)r.y)
            );
            m_gridText.setPosition(
                leftX + m_gridSize.x + 3,
                topY + m_gridSize.y / 2
            );
            m_game->window().draw(m_gridText);
        }

        for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
            drawLine(Vec2(x, topY), Vec2(x, bottomY));
        }

        for (float y=nextGridY; y < bottomY; y += m_gridSize.y) {
            drawLine(Vec2(leftX, y), Vec2(rightX, y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
                int w = width();
                int h = height();
                std::string xCell = std::to_string(
                    ((((int)x % w) + w) % w) / (int)m_gridSize.x
                );
                std::string yCell = std::to_string(
                    ((((int)y % h) + h) % h) / (int)m_gridSize.y
                );
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x+3, y+2);
                m_game->window().draw(m_gridText);
            }
        }
    }
}

std::shared_ptr<Entity> Scene_Zelda::player() {
    for (auto e : m_entityManager.getEntities("player")) {
        return e;
    }
    return nullptr;
}

void Scene_Zelda::changePlayerStateTo(
    const std::string& state, 
    const Vec2& facing
) {
    auto p = player();
    if (!p) return;
    auto& prev = p->get<CState>().preState; 
    PlayerState s;
    if (state == "stand") {
        if (facing == Vec2(1, 0)) {
            s = PlayerState::STANDRIGHT;
        }
        else if (facing == Vec2(-1, 0)) {
            s = PlayerState::STANDLEFT;
        }
        else if (facing == Vec2(0, -1)) {
            s = PlayerState::STANDUP;
        }
        else if (facing == Vec2(0, 1)) {
            s = PlayerState::STANDDOWN;
        }
    }
    if (state == "move") {
        if (facing == Vec2(1, 0)) {
            s = PlayerState::MOVERIGHT;
        }
        else if (facing == Vec2(-1, 0)) {
            s = PlayerState::MOVELEFT;
        }
        else if (facing == Vec2(0, -1)) {
            s = PlayerState::MOVEUP;
        }
        else if (facing == Vec2(0, 1)) {
            s = PlayerState::MOVEDOWN;
        }
    }
    Vec2 offset = p->get<CBoundingBox>().center;
    if (state == "attack") {
        if (facing == Vec2(1, 0)) {
            s = PlayerState::ATTACKRIGHT;
            offset.x -= 10;
        }
        else if (facing == Vec2(-1, 0)) {
            s = PlayerState::ATTACKLEFT;
            offset.x += 10;
        }
        else if (facing == Vec2(0, -1)) {
            s = PlayerState::ATTACKUP;
            offset.y += 10;
        }
        else if (facing == Vec2(0, 1)) {
            s = PlayerState::ATTACKDOWN;
            offset.y -= 10;
        }
    }
    if (prev != s) {
        prev = p->get<CState>().state;
        p->get<CState>().state = s; 
        // change player bounding box for attack animation
        if (state == "attack") {
            p->get<CBoundingBox>().center = offset;
        }
        else {
            p->get<CBoundingBox>().center = p->get<CTransform>().pos;
        }
        p->get<CState>().changeAnimate = true;
    }
    else { 
        p->get<CState>().changeAnimate = false;
    }
}

Vec2 Scene_Zelda::posWinToWorld(const Vec2& pos) {
    auto view = m_game->window().getView();
    float wx = view.getCenter().x - width() / 2.0;
    float wy = view.getCenter().y - height() / 2.0;
    return Vec2(pos.x + wx, pos.y + wy);
}

void Scene_Zelda::sDrag() {
    for (auto e : m_entityManager.getEntities()) {
        if (e->has<CDraggable>()) {
            if (e->get<CDraggable>().dragging) {
                Vec2 wPos = posWinToWorld(m_mousePos);
                e->get<CTransform>().pos = wPos;
                e->get<CBoundingBox>().center = wPos;
            }
        }
    }
}

Vec2 Scene_Zelda::getRoomXY(const Vec2& pos) {
    auto winSize = m_game->window().getSize();
    int roomX = pos.x / winSize.x; 
    int roomY = pos.y / winSize.y;
    if (pos.x < 0) roomX--;
    if (pos.y < 0) roomY--;
    return { (float)roomX, (float)roomY };
}
