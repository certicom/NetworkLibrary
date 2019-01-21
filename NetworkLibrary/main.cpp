

// include the main library
#include "NetworkLibrary.h" 


// include the depandency of this example
#include "PlayerObject.h" 
#include "ExampleCore.h"


// You can change the current configuration by chosing one of these tree
#define NETWORK_CONFIG SERVER_ONLY // SERVER_ONLY || CLIENT_ONLY || CLIENT_AND_SERVER


void InitInstanciableTypes()
{
	Net::Communication::AddInstanciableType<PlayerObject>();
	// etc...
}



// Usefull only from server side
// A callback can be sent to the communication class to do something for each new connection
// In this case we create a new player for each connection
void NewConnectionCallBack(Net::Connection* a_connection) 
{
	// you can now call a_connection->CloseConnection() to refuse this connection
	// If you close this connection here, the client will not be syncronize

	std::cout << "New connection, I create a player !" << std::endl;

	PlayerColor l_color = (PlayerColor)PlayerObject::s_player.size();

	Net::Communication::SpawnObjectFromServer<PlayerObject>(l_color, PlayerObject::GetRandomPosition() );
}




int main()
{

	Net::Communication::SetNewConnectionCallback(NewConnectionCallBack);
	InitInstanciableTypes();

#if NETWORK_CONFIG == SERVER_ONLY || NETWORK_CONFIG == CLIENT_AND_SERVER
	
		Net::Server* l_server = Net::Communication::StartServer("MyServer", true, true);
	
#endif
	


#if NETWORK_CONFIG == CLIENT_ONLY || NETWORK_CONFIG == CLIENT_AND_SERVER
	
		Net::Client* l_client = Net::Communication::StartClient("Im_a_Client");
	
#endif


#if NETWORK_CONFIG != SERVER_ONLY

	sf::RenderWindow window(sf::VideoMode(1500, 800), "Network library example");
	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		EventHandler(window); //Event handler from the player

		GameLoop(); // very commun for game to use a game loop

		DisplayLoop(window, l_client); // the display loop (very simple app, so game loop and display loop are on the same thread)
	}

#else

	while (true)
	{
		GameLoop(); // in server only, we only need to simulate the game, but we do not handle player event and we do not display the game

		Sleep(16); // 60 fps, for a real app use a real delta time like sf::clock 
	}

#endif

	Net::Communication::ShutDownAllCommunications();
	return 0;
}