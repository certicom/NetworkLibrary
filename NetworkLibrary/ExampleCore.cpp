
#include "ExampleCore.h"


char LastChar(char a_char);

bool hasFocus(bool a_focus = false, bool erase = false);

void EventHandler(sf::RenderWindow& a_window)
{

	LastChar(0);

	sf::Event event;
	while (a_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			a_window.close();

		if (event.type == sf::Event::TextEntered)
		{
			
			if (event.text.unicode < 128)
				LastChar( static_cast<char>(event.text.unicode) );
		}

		if (event.type == sf::Event::LostFocus)
		{
			hasFocus(false, true);
		}

		if (event.type == sf::Event::GainedFocus)
		{
			hasFocus(true, true);
		}
		
	}
}


void GameLoop()
{
	for (PlayerObject* player : PlayerObject::s_player) // update the elements that need to be
	{
		player->update(hasFocus());
	}
}


void DisplayLoop(sf::RenderWindow& a_window, Net::Client* a_client)
{
	static std::string l_ipField = "";
	static bool l_ipFieldIsFocus = false;

	a_window.clear();

	std::string l_word = "Servers";
	WriteSomething(l_word, sf::Vector2f(50.0f, 110.0f), a_window, 24);

	sf::RectangleShape l_rectangleForServers (sf::Vector2f(160.f, 80.f));
	l_rectangleForServers.setFillColor(sf::Color(0, 255, 0, 25));
	l_rectangleForServers.setOutlineThickness(2);
	l_rectangleForServers.setOutlineColor(sf::Color(0, 255, 0));

	for (int i=0; i<5; i++)
	{
		l_rectangleForServers.setPosition(20.0f, 150.0f + (i*85.0f));

		a_window.draw(l_rectangleForServers);
	}



	for (int i = 0; i<Net::Communication::GetAvailableServers().size() && i<5; i++)
	{
		Net::InfoServer* l_server = Net::Communication::GetAvailableServers()[i];

		l_word = "Server name :" + l_server->m_name;
		WriteSomething(l_word, sf::Vector2f(25.0f, 160.0f + (i*85.0f)), a_window, 12);

		l_word = "Players : " + std::to_string(l_server->m_connectedClients) + " / " + std::to_string(l_server->m_maxClients);
		WriteSomething(l_word, sf::Vector2f(25.0f, 180.0f + (i*85.0f)), a_window, 12);

		l_word = "Address :" + l_server->m_address.toString();
		WriteSomething(l_word, sf::Vector2f(25.0f, 200.0f + (i*85.0f)), a_window, 12);

		if (!a_client->IsConnected())
		{
			sf::RectangleShape l_rectangleForconnect(sf::Vector2f(80.f, 80.f));
			l_rectangleForconnect.setFillColor(sf::Color(0, 255, 0, 25));
			l_rectangleForconnect.setOutlineThickness(2);
			l_rectangleForconnect.setOutlineColor(sf::Color(0, 255, 0));
			l_rectangleForconnect.setPosition(185.0f, 150.0f + (i*85.0f));
			a_window.draw(l_rectangleForconnect);

			sf::CircleShape l_triangle(35.f, 3);
			l_triangle.setRotation(90);
			l_triangle.setFillColor(sf::Color(255, 255, 255));
			l_triangle.setPosition(257.0f, 158.0f + (i*85.0f));
			a_window.draw(l_triangle);

			sf::Vector2i localPosition = sf::Mouse::getPosition(a_window);

			if (sf::IntRect(170, 150 + (i * 85), 80, 80).contains(localPosition))
			{
				a_window.draw(l_rectangleForconnect);

				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					std::cout << std::endl << "try to connect" << std::endl;

					a_client->Connect(Net::Communication::GetAvailableServers()[i], true);

					Sleep(500); // very very bad but just an app test

					PlayerObject::s_player[PlayerObject::s_player.size() - 1]->GainControl(); // take control of the last created player
				}
			}
		}


	}
	for (int i = Net::Communication::GetAvailableServers().size(); i < 5; i++)
	{
		l_word = "NONE";
		WriteSomething(l_word, sf::Vector2f(60.0f, 175.0f + (i*85.0f)),a_window, 24);
	}




	for (int i = 1; i < 6; i++)
	{
		sf::RectangleShape l_rectangleInput(sf::Vector2f(110.0f, 40.0f));
		l_rectangleInput.setFillColor(sf::Color(0, 125, 125, 125));
		l_rectangleInput.setOutlineThickness(2);
		l_rectangleInput.setOutlineColor(sf::Color(0, 125, 125));
		l_rectangleInput.setPosition(300.0f + (120 * i), 120.0f);
		a_window.draw(l_rectangleInput);

		l_word = "Input " + std::to_string(i);
		WriteSomething(l_word, sf::Vector2f(325.0f + (120 * i), 128.0f), a_window, 18);


		sf::Vector2i localPosition = sf::Mouse::getPosition(a_window);

		if (sf::IntRect(300.0f + (120 * i), 120.0f, 110.0f, 40.0f).contains(localPosition))
		{
			a_window.draw(l_rectangleInput);

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				switch (i)
				{
				case 1: a_client->CloseConnection(); break;
				case 2: break;
				case 3: break;
				case 4: break;
				case 5: break;
				}
			}
		}
	}








	l_word = "Find non local server";
	WriteSomething(l_word, sf::Vector2f(10.0f, 620.0f), a_window, 20);

	sf::RectangleShape l_rectangleIpField(sf::Vector2f(200.0f, 50.0f));
	l_rectangleIpField.setFillColor((l_ipFieldIsFocus ? sf::Color(0, 255, 0, 25) : sf::Color(255, 0, 0, 25)));
	l_rectangleIpField.setOutlineThickness(2);
	l_rectangleIpField.setOutlineColor((l_ipFieldIsFocus ? sf::Color(0, 255, 0) : sf::Color(255, 0, 0)));
	l_rectangleIpField.setPosition(10.0f, 650.0f);
	a_window.draw(l_rectangleIpField);

	if (l_ipFieldIsFocus)
	{
		char l_char = LastChar(0);

		if (l_char != 0)
		{
			if (l_char == 8)
			{
				if(l_ipField.size() > 0)
					l_ipField.resize(l_ipField.size() - 1);
			}
			else if(l_ipField.size() < 13 && ((l_char >= '0' && l_char <= '9' ) || l_char == '.')  )
			{
				l_ipField += l_char;
			}
		}

		if (l_ipField.size() != 0)
		{
			sf::RectangleShape l_rectangleForconnect(sf::Vector2f(50.f, 50.f));
			l_rectangleForconnect.setFillColor(sf::Color(0, 255, 0, 25));
			l_rectangleForconnect.setOutlineThickness(2);
			l_rectangleForconnect.setOutlineColor(sf::Color(0, 255, 0));
			l_rectangleForconnect.setPosition(210.0f, 650.0f);
			a_window.draw(l_rectangleForconnect);

			sf::CircleShape l_triangle(20.f, 3);
			l_triangle.setRotation(90);
			l_triangle.setFillColor(sf::Color(255, 255, 255));
			l_triangle.setPosition(255.0f, 655.0f);
			a_window.draw(l_triangle);

			sf::Vector2i localPosition = sf::Mouse::getPosition(a_window);

			if (sf::IntRect(210, 650.0f, 50, 50).contains(localPosition))
			{
				a_window.draw(l_rectangleForconnect);

				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					Net::Communication::CheckServerExistance(l_ipField);
					//a_client->SendFile("yyg.txt");

					l_ipFieldIsFocus = false;
					l_ipField.resize(0);
				}
			}
		}

	}

	if (l_ipField.size() > 0)
	{
		WriteSomething(l_ipField, sf::Vector2f(15.0f, 655.0f), a_window, 24);
	}
	else if(!l_ipFieldIsFocus)
	{
		l_word = "Ip address...";
		WriteSomething(l_word, sf::Vector2f(15.0f, 655.0f), a_window, 24);
	}


	sf::Vector2i localPosition = sf::Mouse::getPosition(a_window);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (sf::IntRect(10.0f, 650.0f, 200.0f, 50.0f).contains(localPosition))
		{
			l_ipFieldIsFocus = true;
		}
		else
		{
			l_ipFieldIsFocus = false;
		}
	}




	if (a_client->IsConnected())
	{
		for (PlayerObject* player : PlayerObject::s_player)
		{
			if (player->IsControlledByMe())
			{
				l_word = "Your are " + player->GetColorName() +" player";
				break;
			}
		}
	}
	else
	{
		l_word = "You are not connected";
	}

	WriteSomething(l_word, sf::Vector2f(590.0f, 350.0f), a_window, 24);


	sf::RectangleShape l_rectangleGame(Game_Size);
	l_rectangleGame.setFillColor(sf::Color(0, 125, 255, 25));
	l_rectangleGame.setOutlineThickness(2);
	l_rectangleGame.setOutlineColor(sf::Color(0, 125, 255));
	l_rectangleGame.setPosition(Game_Pos);
	a_window.draw(l_rectangleGame);


	// if we are not connected, a server can be running in background, but we don't display the game
	if (a_client->IsConnected())
	{
		for (PlayerObject* player : PlayerObject::s_player)
		{
			player->DrawPlayer(a_window);
		}
	}



	l_word = "Connection infos";
	WriteSomething(l_word, sf::Vector2f(1202.0f, 85.0f), a_window, 24);

	sf::RectangleShape l_rectangleForInfos(sf::Vector2f(200.0f, 300.0f));
	l_rectangleForInfos.setFillColor(   ( a_client->IsConnected() ? sf::Color(0, 255, 0, 25) : sf::Color(255, 0, 0, 25) )   );
	l_rectangleForInfos.setOutlineThickness(2);
	l_rectangleForInfos.setOutlineColor((a_client->IsConnected() ? sf::Color(0, 255, 0) : sf::Color(255, 0, 0)));
	l_rectangleForInfos.setPosition(1200.0f, 80.0f);
	a_window.draw(l_rectangleForInfos);

	const Net::ClientStat l_stats = a_client->GetStats();

	if (a_client->IsConnected())
	{
		int l_espace = 35;
		int l_startPos = 150;
		int l_fontSize = 15;

		l_word = "Address : " + (l_stats.m_connection->m_isLocalHost ? "Localhost" : a_client->GetServerConnection().m_ipAddress.toString());
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace *0) ), a_window, l_fontSize);

		std::string l_ternaire = l_stats.m_connection->m_isConsideredAlive ? "Active" : "Dead";
		l_word = "Status : " + l_ternaire;
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 1)), a_window, l_fontSize);

		l_word = "Name : " + l_stats.m_connection->m_name;
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 2)), a_window, l_fontSize);

		l_word = "Received : " + std::to_string(l_stats.m_ReceivedPackets);
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 3)), a_window, l_fontSize);

		l_word = "Transmited : " + std::to_string(l_stats.m_emittedPackets);
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 4)), a_window, l_fontSize);

		l_ternaire = l_stats.m_isInternet ? "Internet" : "Local";
		l_word = "Visibility : " + l_ternaire;
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 5)), a_window, l_fontSize);
	}


	
	l_word = "Network infos";
	WriteSomething(l_word, sf::Vector2f(1220.0f, 405.0f), a_window, 24);

	l_rectangleForInfos.setPosition(1200.0f, 400.0f);
	a_window.draw(l_rectangleForInfos);




	if (a_client->IsConnected())
	{
		int l_espace = 35;
		int l_startPos = 470;
		int l_fontSize = 15;

		l_word = "Connections : " + std::to_string(l_stats.m_serverInfo->m_connectedClients) + " / " + std::to_string(l_stats.m_serverInfo->m_maxClients);
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 0)), a_window, l_fontSize);

		l_word = "Last ping : " + std::to_string(l_stats.m_connection->m_lastPing.getElapsedTime().asMilliseconds()) + " ms";
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 1)), a_window, l_fontSize);

		l_word = "Clock offset : " + std::to_string(l_stats.m_clockOffset) + " ms";
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 2)), a_window, l_fontSize);

		l_word = "Current latency : " + std::to_string(l_stats.m_currentLatency) + " ms";;
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 3)), a_window, l_fontSize);

		l_word = "Average latency : " + std::to_string(l_stats.m_averageLatency) + " ms";
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 4)), a_window, l_fontSize);


		std::string l_protocolName = (l_stats.m_connection->m_isLocalHost ? "Direct" : (l_stats.m_connection->m_isUDPConnection ? "UDP" : "TCP"));
		l_word = "Connection type : " + l_protocolName;
		WriteSomething(l_word, sf::Vector2f(1207.0f, l_startPos + (l_espace * 5)), a_window, l_fontSize);
	}


	a_window.display();
}




void WriteSomething(std::string& a_string, sf::Vector2f pos, sf::RenderWindow& a_window, int a_size)
{
	
	static sf::Font font;
	static sf::Text text;

	static bool once = false;
	if (!once) { once = true;

		if (!font.loadFromFile("arial.ttf"))
		{
			
		}

		text.setFont(font);
		text.setFillColor(sf::Color::White);
		text.setStyle(sf::Text::Bold);
	}

	text.setCharacterSize(a_size);
	text.setString(a_string);
	text.setPosition(pos);

	a_window.draw(text);
	
	
}


// this things is absolutly awfull^^ of course here we do not care
char LastChar(char a_char)
{
	static char s_char;

	char l_char = s_char;

	s_char = a_char;
	
	return l_char;
}

bool hasFocus(bool a_focus, bool erase)
{
	static bool s_focus = true;

	bool l_focus = s_focus;

	if(erase)
		s_focus = a_focus;

	return l_focus;
}