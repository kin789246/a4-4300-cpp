#pragma once

#include "Animation.h"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Audio/Sound.hpp"
#include <map>
#include <string>

class Assets
{
    std::map<std::string, sf::Texture> m_textures;        
    std::map<std::string, Animation> m_animations;
    std::map<std::string, sf::Font> m_fonts;
    std::map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::map<std::string, sf::Sound> m_sounds;

    void addTexture(const std::string& name, const std::string& path);
    void addAnimation(const std::string& name, Animation animation);
    void addFont(const std::string& name, const std::string& path);
    void addSound(const std::string& name, const std::string& path);

    public:

    Assets();
    ~Assets();

    const std::map<std::string, sf::Texture>& getTextures() const;
    const std::map<std::string, Animation>& getAnimations() const;
    std::map<std::string, sf::Sound>& getSounds();

    const sf::Texture& getTexture(const std::string& name) const;
    const Animation& getAnimation(const std::string& name) const;
    const sf::Font& getFont(const std::string& name) const;
    sf::Sound& getSound(const std::string& soundName);
    
    void loadFromFile(const std::string& path);
};
