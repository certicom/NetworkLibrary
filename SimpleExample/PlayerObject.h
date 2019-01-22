#pragma once
#include "NetworkLibrary.h"

#define Player_Size sf::Vector2f(50.0f, 50.0f)

enum PlayerColor
{
	PC_Blue,
	PC_Red,
	PC_Green,
	PC_Yellow,
	PC_Cyan,
	PC_Magenta
};


enum PlayerCommand
{
	PC_Move
};


enum PlayerDirection
{
	PD_None,
	PD_Up,
	PD_Down,
	PD_Left,
	PD_Right
};

class PlayerObject : public Net::NetworkObject
{
public:

	PlayerObject(); // all network objects must declare this kind of constructor to be easily instanciable

	PlayerObject(PlayerColor a_color, sf::Vector2f a_startPos);

	~PlayerObject();

	virtual void ReceiveCommand(Net::Command& a_command);

	void DrawPlayer(sf::RenderWindow& a_window);

	void update(bool a_hasFocus); // loop update

	sf::Color GetPlayerColor();

	void GainControl(); // gain the control of the player

	bool IsControlledByMe();

	std::string GetColorName();

	static std::vector<PlayerObject*> s_player; // list of all connected players (better private but just a simple app)

	static sf::Vector2f GetRandomPosition();


private:

	bool IsMovementPossible(sf::Vector2f a_movement);

	sf::Uint8 m_colorCode;

	sf::Vector2f m_position; // position of the player

	bool m_isMe; // if I control this player
};


#include "ExampleCore.h"