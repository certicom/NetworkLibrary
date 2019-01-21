
#include "PlayerObject.h"



std::vector<PlayerObject*> PlayerObject::s_player;


////////////////////////////////////////////////////////////
/// Only the following four functions are usefull to 
/// understand how this library works with overriding
/// NetworkObject. 
///
////////////////////////////////////////////////////////////

// Default constructor (Client side)
PlayerObject::PlayerObject() : NetworkObject(&m_colorCode, &m_position.x, &m_position.y)
{
	s_player.push_back(this);
}

// Real constructor (Serveur side)
PlayerObject::PlayerObject(PlayerColor a_color, sf::Vector2f a_startPos) : NetworkObject(&m_colorCode, &m_position.x, &m_position.y)
{
	s_player.push_back(this);

	m_isMe = false;

	m_colorCode = a_color;

	m_position = a_startPos;
}


// update loop that send command if necessary
void PlayerObject::update(bool a_hasFocus)
{
	
	if (!m_isMe || !a_hasFocus) // we don't do the control part if I not supposed to control this charater
		return;

	sf::Uint8 l_direction = PD_None;


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		l_direction = PD_Left;
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		l_direction = PD_Right;
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		l_direction = PD_Up;
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		l_direction = PD_Down;
	

	if (l_direction != PD_None)
	{
		Net::NetworkData l_data; // create the command data
		l_data << Net::Data(0, l_direction); // while there is info to add to build the command, add data
		SendCommand(PC_Move, l_data); // send the command

		// Note : In general games prefer to send the key up/down informations to avoid sending to much data
		// but here it is easier to understand
	}
}



// The command receiver
void PlayerObject::ReceiveCommand(Net::Command& a_command)
{
	switch (a_command.GetCommandCode())
	{
		case PC_Move:
		{
			sf::Vector2f l_movement;

			switch (a_command.GetDataFromId<sf::Uint8>(0)) // GetDataFromId is the easy way to get back the data that you put in the command (sf::Uint8 is the type and 0 the id)
			{
				case PD_Left: l_movement = sf::Vector2f(-2, 0); break;
				case PD_Right:l_movement = sf::Vector2f(+2, 0); break;
				case PD_Up:   l_movement = sf::Vector2f(0, -2); break;
				case PD_Down: l_movement = sf::Vector2f(0, +2); break;

				default: break;
			}

			// Since V0.6 the system for Accept or Reject was moved directly in the concerned objects. Here we MUST call Accept() or Reject()
			// This system of reject or accept is complementary with the game simulation of all the clients and must not override it.
			if (IsMovementPossible(l_movement))
			{
				m_position += l_movement;
				a_command.Accept();
			}
			else
			{
				a_command.Reject();
			}

			break;
		}

		default: a_command.Reject(); break;
	}
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////



PlayerObject::~PlayerObject()
{
	for (int i = 0; i < s_player.size(); i++)
	{
		if (s_player[i] == this)
		{
			s_player.erase(s_player.begin() + i);
		}
	}
}


// gain the control of the player
void PlayerObject::GainControl()
{
	for (PlayerObject* player : s_player)
	{
		player->m_isMe = false;
	}

	m_isMe = true;
}


void PlayerObject::DrawPlayer(sf::RenderWindow& a_window)
{
	sf::RectangleShape l_player(Player_Size);
	l_player.setFillColor(GetPlayerColor());
	l_player.setPosition(m_position + Game_Pos);
	a_window.draw(l_player);
}


bool PlayerObject::IsControlledByMe()
{
	return m_isMe;
}


sf::Vector2f PlayerObject::GetRandomPosition()
{
	bool l_good = false;

	sf::Vector2f l_randomVector;

	while (!l_good)
	{
		l_randomVector.x = rand() / (RAND_MAX / (Game_Size.x - Player_Size.x));
		l_randomVector.y = rand() / (RAND_MAX / (Game_Size.y - Player_Size.y));

		l_good = true;
		for (PlayerObject* player : s_player) // player collider
		{

			if (l_randomVector.x + Player_Size.x > player->m_position.x && l_randomVector.x < player->m_position.x + Player_Size.x &&
				l_randomVector.y + Player_Size.y > player->m_position.y && l_randomVector.y < player->m_position.y + Player_Size.y)
			{
				l_good = false;
			}
		}
	}

	return l_randomVector;
}



bool PlayerObject::IsMovementPossible(sf::Vector2f a_movement)
{
	sf::Vector2f l_futurePos = a_movement + m_position;

	if (l_futurePos.x < 0 || l_futurePos.x > Game_Size.x - Player_Size.x || l_futurePos.y < 0 || l_futurePos.y > Game_Size.y - Player_Size.y) // game collider
		return false;


	for (PlayerObject* player : s_player) // player collider
	{
		if (player != this)
		{
			if (l_futurePos.x + Player_Size.x > player->m_position.x && l_futurePos.x < player->m_position.x + Player_Size.x &&
				l_futurePos.y + Player_Size.y > player->m_position.y && l_futurePos.y < player->m_position.y + Player_Size.y)
			{
				return false;
			}
		}
	}

	return true;
}

sf::Color PlayerObject::GetPlayerColor()
{
	switch (m_colorCode)
	{
	case PC_Blue: return sf::Color::Blue;
	case PC_Red: return sf::Color::Red;
	case PC_Green: return sf::Color::Green;
	case PC_Yellow: return sf::Color::Yellow;
	case PC_Cyan: return sf::Color::Cyan;
	case PC_Magenta: return sf::Color::Magenta;
	default: return sf::Color::White;

	}
}

std::string PlayerObject::GetColorName()
{
	switch (m_colorCode)
	{
	case PC_Blue: return "blue";
	case PC_Red: return "red";
	case PC_Green: return "green";
	case PC_Yellow: return "yellow";
	case PC_Cyan: return "cyan";
	case PC_Magenta: return "magenta";
	default: return "white";

	}
}