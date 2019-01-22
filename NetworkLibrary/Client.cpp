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
#include "stdafx.h"
#include "Client.h"

namespace Net
{



////////////////////////////////////////////////////////////
/// \brief Constructor for client.
///
/// End user should call the CreateClient function to 
/// Correctly create a new client
///
/// \code
/// Net::Client* l_client = Net::Communication::StartClient("Name");
/// \endcode
///
/// \param a_clientName the name that will be used for represent this client
///
////////////////////////////////////////////////////////////
Client::Client(const std::string& a_clientName) : m_stats(&m_server)
{
	std::cout << "I am client" << std::endl;

	m_isRunning = true;

	m_clientName = a_clientName;

	m_isConnected = false;

	m_offsetTime = 0;

	m_server.m_isConsideredAlive = false; // we start not connected

	m_clientThread = std::thread(Client::ClientThread, this);	
}


////////////////////////////////////////////////////////////
/// \brief Destructor of the client
///
/// This will automatically stop the thread that the client 
/// uses for receive and send data
///
////////////////////////////////////////////////////////////
Client::~Client()
{
	if (m_isRunning) // for localhost connections this is not true
	{
		m_isRunning = false;

		m_clientThread.join(); // we now expect that the thread will end soon
	}
}


////////////////////////////////////////////////////////////
/// \brief Send a ping to the server that it will send back
///
/// TODO : Implement this function
/// Use the Ping protocol with clock time
///
////////////////////////////////////////////////////////////
void Client::PingOutServer()
{
	if (!m_server.m_isConsideredAlive || !m_isConnected)
		throw NetworkException("Error : Cannot send without connection");

	sf::Packet l_packet;

	l_packet << (sf::Uint16)CT_Ping << m_clock.getElapsedTime().asMilliseconds() << true;

	SendPacket(l_packet);
}


////////////////////////////////////////////////////////////
/// \brief Get the name of this client 
///
/// \return the name of the client
///
////////////////////////////////////////////////////////////
const std::string& Client::GetName() const
{
	return m_clientName;
}


////////////////////////////////////////////////////////////
/// \brief Send a packet to the server
///
/// \param a_packet the packet that contains all data
///
////////////////////////////////////////////////////////////
void Client::SendPacket(sf::Packet& a_packet)
{
	m_stats.m_emittedPackets++;
	
	if (m_server.m_isUDPConnection) // UDP
	{
		m_udpSystem.GetUdpSocket().send(a_packet, m_server.m_ipAddress, m_server.m_port);
	}
	else // TCP
	{
		m_server.m_TCPSocket.send(a_packet);
	}
}


////////////////////////////////////////////////////////////
/// \brief Send a command to the server
///
/// \param a_data the package that contains all the data
///
/// \param a_customCommand a code/id given by the user to
/// know waht is the purpose of this command
///
////////////////////////////////////////////////////////////
void Client::SendCommand(NetworkData& a_data, sf::Uint16 a_customCommand)
{
	if(!m_server.m_isConsideredAlive || !m_isConnected)
		throw NetworkException("Error : Cannot send without connection");

	sf::Packet l_packet;

 	l_packet << (sf::Uint16)CT_CustomCommand << m_clientName << a_customCommand;

	InternalComm::WriteCommand(l_packet, a_data);

	SendPacket(l_packet);
}


////////////////////////////////////////////////////////////
/// \brief Called when the socket selector receive a packet
/// 
/// This function just read the protocol code and dispatch the packet
/// to the concerned function
///
/// \param a_packet the packet that contains data
///
////////////////////////////////////////////////////////////
void Client::Receive(sf::Packet& a_packet)
{
	sf::Uint16 l_command;

	if (! (a_packet >> l_command))
	{
		throw NetworkException("Error : reading has failed");
	}

	m_stats.m_ReceivedPackets++;

	m_server.m_lastPing.restart();

	switch (l_command)
	{
	case CT_CustomCommand: ReceiveCommand(a_packet);	   break;
	case CT_DeleteObject:  ReceiveDelete(a_packet);		   break;
	case CT_NewObject:     ReceiveCreate(a_packet);		   break;
	case CT_UpdateObjects: ReceiveUpdate(a_packet);		   break;
	case CT_Ping:          ReceivePing(a_packet);		   break;
	case CT_File:          ReceiveFile(a_packet);		   break;
	case CT_EndConnection: ReceiveEndConnection(a_packet); break;

	case CT_Broadcast: /* for now, we don't care about broadcast of the connected server */  break;

	default: throw NetworkException("Error : Bad command type"); break;
	}
}



////////////////////////////////////////////////////////////
/// \brief This is called when the connection was ended from
/// server side
///
/// \param a_packet the packet that contains data without the protocol code
///
////////////////////////////////////////////////////////////
void Client::ReceiveEndConnection(sf::Packet& a_packet)
{
	m_server.m_isConsideredAlive = false;

	if (!m_server.m_isUDPConnection)
		m_server.m_TCPSocket.disconnect();

	m_isConnected = false;
}

////////////////////////////////////////////////////////////
/// \brief Called after first step of reading packet, if the
/// protocol code of the packet indicate a ping
///
/// \param a_packet the packet that contains data without the protocol code
///
////////////////////////////////////////////////////////////
void Client::ReceivePing(sf::Packet& a_packet)
{
	sf::Int32 l_clockTime;

	bool l_mustBeReflected;

	a_packet >> l_clockTime >> l_mustBeReflected;


	if (l_mustBeReflected)
	{
		sf::Packet l_packet;

		l_packet << (sf::Uint16)CT_Ping << m_clock.getElapsedTime().asMilliseconds() << false;

		SendPacket(l_packet); // reflect it
	}
}



////////////////////////////////////////////////////////////
/// \brief check if a server exist at a non local address
///
/// \param a_nonLocalAddress the non local address to check
///
////////////////////////////////////////////////////////////
void Client::CheckServerExistance(sf::IpAddress& a_nonLocalAddress)
{
	sf::Packet l_packet;

	l_packet << (sf::Uint16)CT_CheckServer;

	m_udpSystem.GetUdpSocket().send(l_packet, a_nonLocalAddress, BROADCAST_PORT);
}



////////////////////////////////////////////////////////////
/// \brief Get if the client must ignore most of the data
/// from the server because the server already handle the game core
///
/// \return if a server already handle game core on this app
///
////////////////////////////////////////////////////////////
bool Client::IsClientAndServer() const
{
	return (InternalComm::HasRunningServer() && m_server.m_isLocalHost);
}


////////////////////////////////////////////////////////////
/// \brief Allow to know if the client is connected to a server
///
/// \return if the client is connected to a server
///
////////////////////////////////////////////////////////////
bool Client::IsConnected() const
{
	return m_isConnected;
}


////////////////////////////////////////////////////////////
/// \brief Get the connection with the server
///
/// \return the connection with the server
///
////////////////////////////////////////////////////////////
const Connection& Client::GetServerConnection() const
{
	return m_server;
}


////////////////////////////////////////////////////////////
/// \brief Close the connection with the server if it exist
///
////////////////////////////////////////////////////////////
void Client::CloseConnection()
{
	if (m_isConnected)
	{
		sf::Packet l_packet;

		l_packet << (sf::Uint16)CT_EndConnection;

		SendPacket(l_packet);

		if (!m_server.m_isUDPConnection)
			m_server.m_TCPSocket.disconnect();

		m_isConnected = false;
	}
}

////////////////////////////////////////////////////////////
/// \brief Called after first step of reading packet, if the
/// protocol code of the packet indicate an update
///
/// \param a_packet the packet that contains data without the protocol code
///
////////////////////////////////////////////////////////////
void Client::ReceiveUpdate(sf::Packet& a_packet)
{

}


////////////////////////////////////////////////////////////
/// \brief Get all the interresting information about this client
/// and its connection
///
/// \return the stats of the client
///
////////////////////////////////////////////////////////////
const ClientStat& Client::GetStats() const
{
	return m_stats;
}

////////////////////////////////////////////////////////////
/// \brief Called after first step of reading packet, if the
/// protocol code of the packet indicate a creation
///
/// \param a_packet the packet that contains data without the protocol code
///
////////////////////////////////////////////////////////////
void Client::ReceiveCreate(sf::Packet& a_packet)
{

	if (IsClientAndServer()) // in case of a listener server, we ignore the creation
		return;

	sf::Uint8 l_numberObject;

	if (!(a_packet >> l_numberObject))
	{
		throw NetworkException("Error : reading creator has failed");
	}

	for (int i = 0; i < l_numberObject; i++)
	{
		std::string l_typeName;
		NetworkData l_data;

		if (!(a_packet >> l_typeName))
		{
			throw NetworkException("Error : Unreadable type name");
		}

		if (!InternalComm::ReadObject(a_packet, l_data))
		{
			throw NetworkException("Error : reading new object has failed");
		}
		
		NetworkObject* l_object = InternalComm::InstanciateType(l_typeName);
		l_object->SetTypeName(l_typeName);
		l_object->ReceiveUpdate(l_data, true);
	}
}


////////////////////////////////////////////////////////////
/// \brief Called after first step of reading packet, if the
/// protocol code of the packet indicate a deletion
///
/// \param a_packet the packet that contains data without the protocol code
///
////////////////////////////////////////////////////////////
void Client::ReceiveDelete(sf::Packet& a_packet)
{

}


////////////////////////////////////////////////////////////
/// \brief Called after first step of reading packet, if the
/// protocol code of the packet indicate a command
///
/// \param a_packet the packet that contains data without the protocol code
///
////////////////////////////////////////////////////////////
void Client::ReceiveCommand(sf::Packet& a_packet)
{
	if (IsClientAndServer()) // in case of an listener server, we ignore the command, since the server already send it to the concerned object
		return;

	NetworkData l_data;

	std::string l_serverName;
	sf::Uint16 l_command;

	if(! (a_packet >> l_serverName >> l_command))
		throw NetworkException("Error : reading command has failed");

	if (InternalComm::ReadCommand(a_packet, l_data))
	{
		InternalComm::SendCommandToObject(Command(l_command, l_data));
	}
	else
	{
		throw NetworkException("Error : reading command has failed");
	}
}


////////////////////////////////////////////////////////////
/// \brief Called after first step of reading packet, if the
/// protocol code of the packet indicate a broadcast
///
/// \param a_packet the packet that contains data without the protocol code
///
/// \param a_serverSource the ip of the server who emitted the broadcast
///
////////////////////////////////////////////////////////////
void Client::ReceiveBroadcast(sf::Packet& a_packet, sf::IpAddress& a_serverSource)
{
	sf::Uint16 l_Command;
	std::string l_stringAddress;
	sf::Uint16 l_port;
	sf::Uint8 l_currentConnections;
	sf::Uint8 l_maxConnections;
	std::string l_serverName;
	std::string l_custom;

	if (a_packet >> l_Command >> l_stringAddress >> l_port >> l_serverName >> l_currentConnections >> l_maxConnections >> l_custom)
	{
		if (l_Command == CT_Broadcast)
		{
			std::cout << ".";

			InfoServer* l_infoServer = new InfoServer(); // TODO : consider server id as ip+port and not just ip

			l_infoServer->m_address = sf::IpAddress(l_stringAddress);
			l_infoServer->m_port = l_port;
			l_infoServer->m_connectedClients = l_currentConnections;
			l_infoServer->m_maxClients = l_maxConnections;
			l_infoServer->m_name = l_serverName;
			l_infoServer->m_customInformation = l_custom;

			InternalComm::AddOrUpdateInfoServer(l_infoServer); // we keep the list of available servers updated
		}
		// else : we ignore everyone that doesn't try to broadcast
	}
	// else we just ignore any bad formated broadcast
}



////////////////////////////////////////////////////////////
/// \brief Send a file that will be syncronized on all the clients
///
/// \param a_fileName the file name
///
////////////////////////////////////////////////////////////
void Client::SendFile(const std::string& a_fileName)
{
	std::ifstream l_file(a_fileName, std::ios::in);

	if (l_file) // open the file
	{
		char l_char;

		sf::Packet l_packet;

		int l_currentPacketSize = 0;

		// send a fisrt packet with just the name of the file
		l_packet << (sf::Uint16)CT_File << true << a_fileName;
		SendPacket(l_packet);

		while (l_file.get(l_char)) // while there is still a character in the file
		{
			if (l_currentPacketSize == 0) // init the packet
			{
				l_packet = sf::Packet();

				l_packet << (sf::Uint16)CT_File << false << a_fileName;
			}
			l_packet << l_char; // add the char in the packet

			l_currentPacketSize++; // imcremente the size

			if (l_currentPacketSize >= 256) // if we have reach the maximum size
			{
				SendPacket(l_packet); // send the packet

				l_currentPacketSize = 0; // reset the packet
			}
		}

		if(l_currentPacketSize != 0) // handle a non complete packet
			SendPacket(l_packet); 

		l_file.close();
	}

	else
		throw NetworkException("Error : Can not open the file");
}


////////////////////////////////////////////////////////////
/// \brief Receive a part of a file
///
/// \param a_packet the data for the part of the file
///
////////////////////////////////////////////////////////////
void Client::ReceiveFile(sf::Packet& a_packet)
{
	bool l_startFile;

	std::string l_fileName;

	a_packet >> l_startFile >> l_fileName;

	if (l_startFile)
	{
		std::ofstream l_file(l_fileName);
		l_file.close();
	}
	else
	{
		std::ofstream l_file(l_fileName, std::ofstream::app);

		if (l_file)
		{
			int l_count = 0;
			sf::Int8 l_char;
			while (a_packet >> l_char)
			{
				l_count++;
				if (l_count % 4 == 0) // TODO : understand why we receive 32bits data instead of 8
				{
					l_file.put(l_char);
				}
			}
				
			l_file.close();
		}
	}
}


////////////////////////////////////////////////////////////
/// \brief Get the information that the connected server send
/// by broadcast
///
////////////////////////////////////////////////////////////
InfoServer* Client::GetInfoOfTheConnection()
{
	for (InfoServer* info : InternalComm::GetAvailableServers())
	{
		if (info->m_address.toInteger() == m_server.m_ipAddress.toInteger())

			return info;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
/// \brief Connect this client to an existing server
///
/// If the connection ip is localHost, this function will call ConnectInLocalHost
///
/// \param a_server the informations about the server to connect. Those informations
/// can be found with the GetAvailableServers function
///
/// \param a_TcpConnect is a flag that indicate if the connection should
/// uses the TCP protocol (For the moment very recommended)
///
/// \return If the connection is a sucess
///
/// \see Client.ConnectInLocalHost
/// \see Communication::GetAvailableServers
///
////////////////////////////////////////////////////////////
bool Client::Connect(const InfoServer* a_server, bool a_TcpConnect)
{

	if (a_server->m_connectedClients >= a_server->m_maxClients) // don't even try to connect if the server is full
		return false;

	sf::Packet l_packet; // prepare a message for the server to indicate clearly who I'am

	l_packet << (sf::Uint16)CT_NewConnection << m_clientName << m_udpSystem.GetUdpPort();

	if (a_TcpConnect)
	{
		sf::Socket::Status status = m_server.m_TCPSocket.connect(a_server->m_address, a_server->m_port); //  TODO : Check if this is as wrong as UDP port system
		if (status != sf::Socket::Done)
		{
			return false;
		}

		std::cout << "I m now connected !" << std::endl;

		m_server.m_isUDPConnection = false;

		m_selector.add(m_server.m_TCPSocket);
	}
	else
	{
		m_server.m_isUDPConnection = true;
	}

	m_server.m_ipAddress = a_server->m_address;
	m_server.m_isConsideredAlive = true;
	m_server.m_name = a_server->m_name;
	m_server.m_isLocalHost = a_server->m_address.toInteger() == sf::IpAddress::getLocalAddress().toInteger();
	m_isConnected = true;

	m_stats.m_serverInfo = GetInfoOfTheConnection();

	SendPacket(l_packet);

	return true;
}



////////////////////////////////////////////////////////////
/// \brief This function is called as thread for each client,
/// this is the equivalent of main for clients.
///
/// \param a_client the client at the origin of the thread
///
////////////////////////////////////////////////////////////
void Client::ClientThread(Client* a_client)
{
	try
	{
		a_client->m_selector.add(a_client->m_udpSystem.GetUdpSocket());

		while (a_client->m_isRunning)
		{
			if (a_client->m_selector.wait(sf::milliseconds(50))) // 50ms loop to allow the thread to stop quickly
			{

				if (a_client->m_selector.isReady(a_client->m_server.m_TCPSocket)) //TCP
				{
					sf::Packet l_packet;

					a_client->m_server.m_TCPSocket.receive(l_packet);

					a_client->Receive(l_packet); // received data are supposed to be processed in the ReceiveInformation method
				}

				if (a_client->m_selector.isReady(a_client->m_udpSystem.GetUdpSocket())) // Udp
				{
					sf::Packet l_packet;
					sf::IpAddress l_ipAddress;
					unsigned short l_port;

					a_client->m_udpSystem.GetUdpSocket().receive(l_packet, l_ipAddress, l_port);

					if (l_ipAddress.toInteger() == a_client->m_server.m_ipAddress.toInteger() && a_client->m_server.m_isUDPConnection)
					{
						a_client->Receive(l_packet); // received data are supposed to be processed in the ReceiveInformation method
					}
					else // if we receive a message from an unknow source (probably a broadcasting server)
					{
						a_client->ReceiveBroadcast(l_packet, l_ipAddress); //we supposed that this is a broadcast, but if not this function will do nothing
					}
				}
				
			}
		}
	}
	catch (const NetworkException& ex)
	{
		std::cout << "Crash by exception : " << ex.what() << std::endl;
	}

	std::cout << "client Stop" << std::endl;
}

}




