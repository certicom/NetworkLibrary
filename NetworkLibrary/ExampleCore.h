#pragma once

#include "NetworkLibrary.h"


#define Game_Pos sf::Vector2f(280.0f, 400.0f)

#define Game_Size sf::Vector2f(800.0f, 300.0f)

void DisplayLoop(sf::RenderWindow& a_window, Net::Client* a_client);

void GameLoop();

void EventHandler(sf::RenderWindow& a_window);

void WriteSomething(std::string& a_string, sf::Vector2f pos, sf::RenderWindow& a_window, int a_size);

#include "PlayerObject.h"