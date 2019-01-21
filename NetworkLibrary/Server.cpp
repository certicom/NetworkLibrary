////////////////////////////////////////////////////////////
//
// Net - Network library for games
// copyleft 2018 - 2019 Alexandre Lepoittevin
//
// This library is provided in open source without any license.
// This code is available as demonstration purpose, and do not implied any working warranty
// and availability of features.
//
// This library can be freely altered and redistributed for any purpose at the only condition
// to not alter this notice and to not misrepresent the author.
//
////////////////////////////////////////////////////////////

#include "Server.h"

namespace Net
{


////////////////////////////////////////////////////////////
/// \brief Constructor
///
/// Run in a thread
///
/// \param a_name the server name
///
/// \param a_autoConnect if all connections are automatically accepted 
/// (TODO : implement the false case)
///
/// \param a_local if the server is local (TODO : really usefull ?)
///
/// \param a_timeout the time for wich we consider a client as 
/// disconnected without communication (TODO : implement this)
///
/// \param a_maxConnections the maximum connection autorized
///
/// \param a_commandHandler the callback that handle command
///
////////////////////////////////////////////////////////////
Server::Server(const std::string& a_name, bool a_autoConnect, bool a_local, int a_timeout, int a_maxConnections)
{
	std::cout << "I am server" << std::endl;

	m_isRunning = true;

	m_isLocal = a_local;

	m_isAutoAccept = a_autoConnect;

	m_serverName = a_name;

	m_clientTimeOut = a_timeout;

	m_isListening = true;

	m_maxConnections = a_maxConnections;

	m_serverThread = std::thread(Server::ServerThread, this);
}


////////////////////////////////////////////////////////////
/// \brief destructor
///
////////////////////////////////////////////////////////////
Server::~Server()
{
	m_isRunning = false;

	m_serverThread.join(); // we now expect that the thread will end soon

	for (Connection* connection : m_clients) // TODO : send an end of connection message
		delete connection;
}


////////////////////////////////////////////////////////////
/// \brief Syncronize the clock of all connected client to
/// the server clock
///
/// TODO : implement this
///
////////////////////////////////////////////////////////////
void Server::ClockSyncroForAllClients()
{

}


////////////////////////////////////////////////////////////
/// \brief Send a ping to a specific client that it will send back
///
/// TODO : Implement this function
/// Use the Ping protocol with clock time
///
////////////////////////////////////////////////////////////
void Server::PingOutClient(Connection* a_client)
{
	sf::Packet l_packet;

	l_packet << (sf::Uint16)CT_Ping << m_clock.getElapsedTime().asMilliseconds() << true;

	SendPacketToOneClient(l_packet, a_client);
}

////////////////////////////////////////////////////////////
/// \brief Get the current connected clients on the server
/// 
/// \return the number of connected clients on the server
///
////////////////////////////////////////////////////////////
sf::Uint8 Server::GetNumberOfConnectedClients()
{
	sf::Uint8 l_connectedClients = 0;

	for (Connection* connection : m_clients)
	{
		if (connection->m_isConsideredAlive)
			l_connectedClients++;
	}

	return l_connectedClients;
}

////////////////////////////////////////////////////////////
/// \brief set the custom informations of the server
///
/// Allow the user to give more custom informations about
/// this server when it broadcast, like the map type.
///
/// TODO : Make them usefull
/// TODO : Better way than a string to parse ?
///
/// \param a_infos the custom infos
///
////////////////////////////////////////////////////////////
void Server::SetCustomInformation(std::string& a_infos)
{
	m_customInformation = a_infos;
}


////////////////////////////////////////////////////////////
/// \brief Do a broadcast with server informations
///
/// \param a_address by default a broadcast send the packet on the
/// broadcast address, but for non local responses to a client
/// we need to use the address of the client
///
////////////////////////////////////////////////////////////
void Server::Broadcast(const sf::IpAddress& a_address)
{
	sf::Packet l_packet;
	sf::Uint16 l_broadcastCommand = CT_Broadcast;

	l_packet << l_broadcastCommand << sf::IpAddress::getLocalAddress().toString() << m_udpSystem.GetUdpPort() << m_serverName
		     << GetNumberOfConnectedClients() << m_maxConnections << m_customInformation;

	std::cout << ".";
	m_udpSystem.GetUdpSocket().send(l_packet, a_address, BROADCAST_PORT);
}


////////////////////////////////////////////////////////////
/// \brief send an update to a list of objects
///
/// \param a_data the list a object that we want to syncronized
///
/// \param a_byBroadcast if we use a broadcast to do that a save
/// bandwidth (TODO : implement this)
///
////////////////////////////////////////////////////////////
void Server::SendUpdate( std::vector<NetworkData>& a_data, bool a_byBroadcast)
{
	sf::Packet l_packet;

	l_packet << (sf::Uint16)CT_UpdateObjects << a_data.size();

	for (NetworkData& data : a_data)
	{
		InternalComm::WriteObject(l_packet, data);
	}

	SendPacket(l_packet);
}



////////////////////////////////////////////////////////////
/// \brief send a command to an object
///
/// \param a_data the main data of the command
///
/// \param a_customCommand the custom code of the command
///
/// \param a_ignoreLocalhost ignore the localhost client while
/// sending the command to all clients
///
/// \param a_byBroadcast if we use a broadcast to do that a save
/// bandwidth (TODO : implement this)
///
////////////////////////////////////////////////////////////
void Server::SendCommand(NetworkData& a_data, sf::Uint16 a_customCommand, bool a_byBroadcast)
{
	sf::Packet l_packet;

	l_packet << (sf::Uint16)CT_CustomCommand << m_serverName << a_customCommand;

	InternalComm::WriteCommand(l_packet, a_data);

	SendPacket(l_packet);
}


////////////////////////////////////////////////////////////
/// \brief send a delete order to a list of objects
///
/// \param a_list the list of object ids that we want destroy
///
////////////////////////////////////////////////////////////
void Server::SendDelete(std::vector<sf::Uint16>& a_list)
{
	sf::Packet l_packet;

	l_packet << (sf::Uint16)CT_DeleteObject;

	for (sf::Uint16 id : a_list)
	{
		l_packet << id;
	}

	SendPacket(l_packet);
}


////////////////////////////////////////////////////////////
/// \brief send a packet to all clients over the right 
/// protocol (UDP or TCP)
///
/// \param a_packet the packet to send
///
////////////////////////////////////////////////////////////
void Server::SendPacket(sf::Packet& a_packet)
{
	for (Connection* connection : m_clients) //  send the command to every clients currently connected
	{
		SendPacketToOneClient(a_packet, connection);
	}
}




////////////////////////////////////////////////////////////
/// \brief send a packet to only one client over the right 
/// protocol (UDP or TCP)
///
/// \param a_packet the packet to send
///
/// \param a_client the targeted client
///
////////////////////////////////////////////////////////////
void Server::SendPacketToOneClient(sf::Packet& a_packet, Connection* a_client)
{	
	if (a_client->m_isUDPConnection) // UDP
	{
		m_udpSystem.GetUdpSocket().send(a_packet, a_client->m_ipAddress, a_client->m_port);
	}
	else // TCP
	{
		a_client->m_TCPSocket.send(a_packet);
	}
}


////////////////////////////////////////////////////////////
/// \brief This function will handle any new objects that was not
/// syncronized yet.
///
////////////////////////////////////////////////////////////
void Server::HandleNewObjects()
{
	std::vector<NetworkObject*> l_newObjects = InternalComm::GetNewObjects();

	if (l_newObjects.size() != 0)
	{

		sf::Packet l_packet;

		sf::Uint8 l_nbObjects = l_newObjects.size();

		l_packet << (sf::Uint16)CT_NewObject << l_nbObjects;

		

		for (NetworkObject* object : l_newObjects)
		{
			l_packet << object->GetTypeName();

			InternalComm::WriteObject(l_packet, object->GetSyncronizableData());
		}

		SendPacket(l_packet);

		for (NetworkObject* object : l_newObjects)
		{
			object->ConsiderUpToDate(); //  we send it, so we can consider that everything is up to date
		}

		InternalComm::NewObjectsWasHandled();
	}
}




////////////////////////////////////////////////////////////
/// \brief Handle received informations from a client
///
/// \param a_packet the received packet
///
/// \param a_idUser the connection at the origin of this packet 
///
////////////////////////////////////////////////////////////
void Server::ReceiveInformation(sf::Packet& a_packet, Connection* a_idUser)
{
	sf::Uint16 l_command;

	try
	{
		if (!(a_packet >> l_command))
		{
			throw NetworkException("Error : Unreadable message (Command reading)!");
		}
			
		a_idUser->m_lastPing.restart();

		switch (l_command)
		{
		case CT_Broadcast: 

			if(!a_idUser->m_isConsideredAlive)// broadcast is not a connection request, so we delete the temp connection
				delete a_idUser;

			break;

		case CT_CustomCommand: ReceiveCommand(a_packet, a_idUser);       break; 
		case CT_NewConnection: ReceiveNewConnection(a_packet, a_idUser); break;
		case CT_Ping:          ReceivePing(a_packet, a_idUser);          break;
		case CT_File:          ReceiveFile(a_packet, a_idUser);          break;
		case CT_CheckServer:   ReceiveCheckServer(a_packet, a_idUser);   break;
		case CT_EndConnection: ReceiveEndConnection(a_packet, a_idUser); break;

		// Update, create and delete objects are not accepted by the server
		default: throw NetworkException("Error : Unreadable message (Command type)!");
		}

	}
	catch (const std::exception&)
	{
		// TODO : the message must be resend
	}

}


////////////////////////////////////////////////////////////
/// \brief This is called when the connection was ended from
/// client side
///
/// \param a_packet the received packet
///
/// \param a_idUser the connection at the origin of this packet 
///
////////////////////////////////////////////////////////////
void Server::ReceiveEndConnection(sf::Packet& a_packet, Connection* a_idUser)
{
	a_idUser->m_isConsideredAlive = false;

	if (!a_idUser->m_isUDPConnection)
		a_idUser->m_TCPSocket.disconnect();
}


////////////////////////////////////////////////////////////
/// \brief Receive a ping from a client
///
/// \param a_packet the received packet
///
/// \param a_idUser the connection at the origin of this packet 
///
////////////////////////////////////////////////////////////
void Server::ReceivePing(sf::Packet& a_packet, Connection* a_idUser)
{
	sf::Int32 l_clockTime;

	bool l_mustBeReflected;

	a_packet >> l_clockTime >> l_mustBeReflected;


	if (l_mustBeReflected)
	{
		sf::Packet l_packet;

		l_packet << (sf::Uint16)CT_Ping << m_clock.getElapsedTime().asMilliseconds() << false;

		SendPacketToOneClient(l_packet, a_idUser); // reflect it
	}
}


////////////////////////////////////////////////////////////
/// \brief When the server is non local, the simplest way for a 
/// client to connect to the server is to used a direct request
/// This function handle this kind of calls
///
/// \param a_packet the received packet
///
/// \param a_idUser the connection at the origin of this packet
/// (supposed temporary)
///
////////////////////////////////////////////////////////////
void Server::ReceiveCheckServer(sf::Packet& a_packet, Connection* a_idUser)
{
	if(m_isListening) // if we do not want new connection we do not answer

		Broadcast(a_idUser->m_ipAddress);

	//a_idUser is supposed to be a temporary connection, since this is just a call
	delete a_idUser;
}

////////////////////////////////////////////////////////////
/// \brief Handle a command received from a client
///
/// \param a_packet the received packet
///
/// \param a_idUser the connection at the origin of this packet 
///
////////////////////////////////////////////////////////////
void Server::ReceiveCommand(sf::Packet& a_packet, Connection* a_idUser)
{
	sf::Uint16 l_customCommand;
	std::string l_userName;

	if (!(a_packet >> l_userName >> l_customCommand))
		throw NetworkException("Error : unreadable message (Name reading)!");


	if (l_userName != a_idUser->m_name) // TODO: better authentication
		throw NetworkException("Error : Authentication error!");


	NetworkData l_data;

	if (InternalComm::ReadCommand(a_packet, l_data))
	{
		Command l_structCommand(l_customCommand, l_data);

		// let the game core decide what to do with this command
		// this is supposed to be a client side function, but here we simulate a client
		InternalComm::SendCommandToObject(l_structCommand);

		if(!l_structCommand.IsHandled())
			throw NetworkException("Error : You must accept ou reject commands in the ReceiveCommand function of your NetworkObjects");

		// if an object must be created because of this command, the info must be sent before reflecting the command 
		// since the creations will be part of the thread, any new object must be syncro exacly here 
		HandleNewObjects();

		if (l_structCommand.IsValidate())
		{
			SendCommand(l_data, l_customCommand); // reflect the command to all clients (lighter that sending the effects)
		}
	}
	else
	{
		throw NetworkException("Error : Reading command has failed!");
	}
}

////////////////////////////////////////////////////////////
/// \brief Handle a new connection request from a client
///
/// \param a_packet the received packet
///
/// \param a_idUser the connection at the origin of this packet 
///
////////////////////////////////////////////////////////////
void Server::ReceiveNewConnection(sf::Packet& a_packet, Connection* a_idUser)
{
	if (!m_isListening) // if we do not accept new connection, we return now
	{
		if (a_idUser->m_isUDPConnection) // udp new connections are not supposed to arrive here, but we make sure there is no problem
			delete &a_idUser; // the temp connection must be delete

		return;
	}

	if (a_idUser->m_isConsideredAlive && a_idUser->m_isUDPConnection) // udp new connections are not supposed to be marked as alive
		throw NetworkException("Error : This connection already exist!");

	if (!(a_packet >> a_idUser->m_name >> a_idUser->m_port))
	{
		if (a_idUser->m_isUDPConnection)
			delete &a_idUser; // the temp connection must be delete

		throw NetworkException("Error : New connection failed!");
	}

	// if we arrive here, we consider the connection as accepted
	if (a_idUser->m_isUDPConnection)
		AddNewUdpUser(a_idUser); // we add this new connection (only if UDP, because if we receive a TCP new connection, the connection is already store)
	

	// TODO : NewConnectionCallBack before SyncroNewClient generate 2 players find a way to avoid doing SyncroNewClient if the connexion was refused
	if (a_idUser->m_isConsideredAlive) // We do not syncro the client, if it was refused
	{
		SyncroNewClient(a_idUser); // we send the current state of the app to this new client
	}
	else
	{
		CloseConnection(a_idUser);
	}

	InternalComm::NewConnectionCallBack(a_idUser); // the user can close the connection here


}

////////////////////////////////////////////////////////////
/// \brief set if the server is currently listening
///
/// If the server is not listening, it will not broadcast and 
/// will not accept new connections (TODO : implment this second part)
///
/// \param a_listening if the server must listening
///
////////////////////////////////////////////////////////////
void Server::SetListeningState(bool a_listening)
{
	m_isListening = a_listening;
}

////////////////////////////////////////////////////////////
/// \brief set if the server will accept all new connections
///
/// TODO : implement this
///
/// \param a_autoAccept if the server must accept all connections
///
////////////////////////////////////////////////////////////
void Server::SetAutoAccept(bool a_autoAccept)
{

}

////////////////////////////////////////////////////////////
/// \brief get the list of all the clients of the server
///
/// \return the list of all clients
///
////////////////////////////////////////////////////////////
const std::vector<Connection*>& Server::GetClients() const
{
	return m_clients;
}


////////////////////////////////////////////////////////////
/// \brief Close the connection with the server if it exist
///
/// \param a_connection The connection to close
///
////////////////////////////////////////////////////////////
void Server::CloseConnection(Connection* a_connection)
{
	for (Connection* connection : m_clients)
	{
		if (a_connection == connection)
		{
			sf::Packet l_packet;

			l_packet << (sf::Uint16)CT_EndConnection;

			SendPacketToOneClient(l_packet, connection);

			if (!connection->m_isUDPConnection)
				connection->m_TCPSocket.disconnect();

			break;
		}
	}
}

////////////////////////////////////////////////////////////
/// \brief Handle old clients, it send a ping to all clients 
/// that have not communicated recently to make sure, they 
/// are still alive, it delete dead connection and it close 
/// very old connections that not responding
///
////////////////////////////////////////////////////////////
void Server::HandleOldClients()
{
	int l_connectionToDelete = -1;

	for (int i = 0; i < m_clients.size(); i++) // TODO : again, vector seems to not be the best collection here
	{
		int l_lastPing = m_clients[i]->m_lastPing.getElapsedTime().asMilliseconds();

		if (!m_clients[i]->m_isConsideredAlive && l_lastPing >= 150) // delete dead connections
		{
			l_connectionToDelete = i;
		}

		// clients that not responding are put in the not responding list that the user can access
		// if (l_lastPing >= CLIENT_TIMEOUT) 
		// finally decided to use an automatic list

		if (l_lastPing >= 600) // make sure this client is alive because it have not communicated recently
		{
			PingOutClient(m_clients[i]);
		}
	}

	if (l_connectionToDelete != -1) //we almost never have to delete multiple client at the same time, so one by one (50 ms)
	{
		m_clients.erase(m_clients.begin() + l_connectionToDelete);
	}
}



////////////////////////////////////////////////////////////
/// \brief Receive and reflect a part of a file
///
/// \param a_packet the packet that contains the part of the file
///
/// \param a_idUser the user that send the packet
///
////////////////////////////////////////////////////////////
void Server::ReceiveFile(sf::Packet& a_packet, Connection* a_idUser)
{
	// TODO : add a way to control the incoming file, because the client must be able to send a file too

	for (Connection* client : m_clients)
	{
		if (client != a_idUser)
		{
			SendPacketToOneClient(a_packet, client);
		}
	}

}


////////////////////////////////////////////////////////////
/// \brief Get the connection infos that correspond to an specific
/// client name if this connection exist
///
/// \param a_name the client name of the request
///
/// \return the connection with this client name
///
////////////////////////////////////////////////////////////
Connection* Server::GetIdFromName(std::string& a_name)
{
	for (Connection* connection : m_clients)
	{
		if (connection->m_name == a_name)
		{
			return connection;
		}
	}

	throw NetworkException("Error : Unable to find the requested ID!");
}


////////////////////////////////////////////////////////////
/// \brief remove a UDP user from its ip address 
///
/// When a user is connected with TCP protocol, we need
/// to make he was not already connected with UDP. If so
/// we remove this previous connection from the list.
///
/// \param a_address ip address of the new client
///
////////////////////////////////////////////////////////////
void Server::RemoveUdpUserIfAny(sf::IpAddress& a_address)
{
	for (unsigned int i = 0; i < m_clients.size(); i++)
	{
		if (m_clients[i]->m_ipAddress.toInteger() == a_address.toInteger() && m_clients[i]->m_isUDPConnection)
		{
			delete m_clients[i];
			m_clients.erase(m_clients.begin() + i); // may be we only set it to m_IsConsideredAlive = false ? or just dont change it ?
			break;
		}
	}
}


////////////////////////////////////////////////////////////
/// \brief Create a temporary/dead connection that use the UDP
/// protocol and with a given ip address
///
/// Since UDP is connectionless, we are never sure if a UDP packet
/// from an unknow source is a connection request or something else
/// so we need to create a temporary connection that will be closed
/// automatically if we do not consider it as a valid connection
///
/// \param a_address the address of the unknow source
///
/// \return A temporary connection
///
////////////////////////////////////////////////////////////
Connection* Server::GiveTempUdpConnection(sf::IpAddress& a_address)
{
	Connection* l_connection = new Connection();

	l_connection->m_ipAddress = a_address; // we only set the ip address, the name will be change in receiveInformation
	l_connection->m_isUDPConnection = true;
	l_connection->m_isConsideredAlive = false; // temp, so not alive

	return l_connection;
}


////////////////////////////////////////////////////////////
/// \brief Add a new client that wants to use the UDP protocol
///
/// TCP users are added inline btw
///
/// \param a_connection the new user connection
///
////////////////////////////////////////////////////////////
void Server::AddNewUdpUser(Connection* a_connection)
{
	a_connection->m_isUDPConnection = true;
	a_connection->m_isConsideredAlive = true;
	a_connection->m_isLocalHost = false;

	m_clients.push_back(a_connection);
}


////////////////////////////////////////////////////////////
/// \brief Get the connection infos that correspond to an specific
/// ip address if this connection exist
///
/// \param a_address the ip address of the request
///
/// \return the connection with this ip address
///
////////////////////////////////////////////////////////////
Connection* Server::GetIdFromIP(sf::IpAddress& a_address)
{
	for (Connection* connection : m_clients)
	{
		if (connection->m_ipAddress.toInteger() == a_address.toInteger())
		{
			return connection;
		}
	}

	return NULL;
}



////////////////////////////////////////////////////////////
/// \brief When a new client is connected, we will send to 
/// him all the network objects currently existing
///
/// TODO : open a new thread for this specific task
///
/// \param a_newConnection the client to syncronize
///
////////////////////////////////////////////////////////////
void Server::SyncroNewClient(Connection* a_newConnection)
{
	// TODO : make this initial syncronization as an indepandant thread for cases where there are lot of objects to send

	if (a_newConnection->m_isLocalHost) // we just ignore the initial syncro with localhost clients (in all case localhost clients are not supposed to arrived here)
		return;

	// to avoid too large packet, we only send 10 objects per packet
	int l_numberObject = NetworkObject::GetObjectList().size();

	int l_currentIndex = 0;

	sf::Packet l_packet;

	for (std::pair<sf::Uint16, NetworkObject*> object : NetworkObject::GetObjectList())
	{
		if (l_currentIndex % 10 == 0)
		{
			sf::Uint8 l_nbObjInThisPacket = ((l_numberObject - l_currentIndex) >= 10 ? 10 : (l_numberObject - l_currentIndex));

			if (l_currentIndex != 0) // send before reset only if it not the begining
			{
				SendPacketToOneClient(l_packet, a_newConnection);
			}

			l_packet = sf::Packet(); // reset packet

			l_packet << (sf::Uint16)CT_NewObject << l_nbObjInThisPacket;
		}
		
		l_packet << object.second->GetTypeName();

		InternalComm::WriteObject(l_packet, object.second->GetSyncronizableData());

		l_currentIndex++;
	}

	if (l_numberObject % 10 != 0) // case where we need to send a packet that contain less that 10 objects at the end
	{
		SendPacketToOneClient(l_packet, a_newConnection);
	}
}



////////////////////////////////////////////////////////////
/// \brief Handle the reception of a new connection
/// from a TCP user
///
////////////////////////////////////////////////////////////
void Server::HandleNewTcpConnection()
{
	
	Connection* l_connection = new Connection(); // prepare the new connection

	if (m_listener.accept(l_connection->m_TCPSocket) != sf::Socket::Done)
	{
		throw NetworkException("Error : Cannot connect to the new client!");
	}

	if (!m_isListening)
	{
		CloseConnection(l_connection);// we immediatly disconect from the the new user if we are not listening
		return;
	}
	
	std::cout << std::endl << "New TCP client !" << std::endl;

	l_connection->m_isUDPConnection = false;
	l_connection->m_isConsideredAlive = true;
	l_connection->m_isLocalHost = false;
	// l_connection.m_ipAddress; // in TCP we don't care about ipAddress (for now)

	RemoveUdpUserIfAny(l_connection->m_TCPSocket.getRemoteAddress()); // we make sure that the previous UDP connection on the same ip is deleted (why not let them both ?)

	m_selector.add(l_connection->m_TCPSocket);// we now wait any data from this new TCP connection

	m_clients.push_back(l_connection); // remember this new connection
}



////////////////////////////////////////////////////////////
/// \brief Handle the reception of a new Udp message
///
////////////////////////////////////////////////////////////
void Server::HandleNewUdpMessage()
{
	sf::Packet l_packet;
	sf::IpAddress l_ipAddress;
	unsigned short l_port;

	m_udpSystem.GetUdpSocket().receive(l_packet, l_ipAddress, l_port);

	Connection* l_connection;

	l_connection = GetIdFromIP(l_ipAddress);

	bool  l_newEntity = false;

	if (l_connection == NULL) // not a user that we know
	{
		l_connection = GiveTempUdpConnection(l_ipAddress); // now it exist temporarily

		l_newEntity = true;
	}


	if (m_isListening || !l_newEntity) // receive data except from new entities when we do not listen
	{
		ReceiveInformation(l_packet, l_connection); // received data are supposed to be processed in the ReceiveInformation method
	}
	else if(l_newEntity)
	{
		delete l_connection; // this connection becomes useless
	}
}


////////////////////////////////////////////////////////////
/// \brief This function is called as thread for a server,
/// this is the equivalent of main for server.
///
/// \param a_server the server at the origin of the thread
///
////////////////////////////////////////////////////////////
void Server::ServerThread(Server* a_server)
{
	try
	{
		// Start to listen
		sf::Clock l_clock; // starts the broadcast clock

	    // bind listener to the port (TCP side)
		if (a_server->m_listener.listen(a_server->m_udpSystem.GetUdpPort()) != sf::Socket::Done)
		{
			throw NetworkException("Error : Server listener fail!");
		}

		a_server->m_selector.add(a_server->m_listener);

		a_server->m_selector.add(a_server->m_udpSystem.GetUdpSocket());
		
		while (a_server->m_isRunning)
		{

			if (a_server->m_selector.wait(sf::milliseconds(50))) // 50ms loop to allow the thread to stop quickly
			{

				if (a_server->m_selector.isReady(a_server->m_listener)) // Listener for TCP connections
				{
					a_server->HandleNewTcpConnection();
				}

				if (a_server->m_selector.isReady(a_server->m_udpSystem.GetUdpSocket())) // Udp
				{
					a_server->HandleNewUdpMessage();
				}
				

				for (Connection* TCPconnection : a_server->m_clients)
				{
					if (a_server->m_selector.isReady(TCPconnection->m_TCPSocket)) // TCP
					{
						sf::Packet l_packet;

						TCPconnection->m_TCPSocket.receive(l_packet);

						a_server->ReceiveInformation(l_packet, TCPconnection); // received data are supposed to be processed in the ReceiveInformation method
					}
				}
			}

			a_server->HandleNewObjects(); // in case new objects was created indepandently of clients actions

			a_server->HandleOldClients();

			// also broadcast regullary (every 0.5s)
			sf::Time l_elapsedTime = l_clock.getElapsedTime();
			if (l_elapsedTime.asMilliseconds() >= 500)
			{
				if (a_server->m_isListening)
				{
					a_server->Broadcast();
				}
				l_clock.restart();
			}

		}
	}
	catch (const NetworkException& ex)
	{
		std::cout << std::endl << "Crash by exception : " << ex.what() << std::endl;
	}

	std::cout << std::endl << "server Stop" << std::endl;
}

}