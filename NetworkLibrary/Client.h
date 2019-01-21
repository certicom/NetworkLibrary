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


#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "StaticHeader.h"

//#include "NetworkEnums.h" //(included in server)
//#include "Communication.h" //(included in server)

#include "Connection.h" //(included in server)
#include "Server.h"
#include "ClientStat.h"

#include "UdpHandler.h"

namespace Net
{

////////////////////////////////////////////////////////////
/// \brief this class is capable to handle a connection with a server
/// as well as data communication with it.
///
/// A client does not immeditly connect, but it always listen all the 
/// available servers automatically
///
////////////////////////////////////////////////////////////
class Client
{

public:


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
	Client(const std::string& a_clientName);

	////////////////////////////////////////////////////////////
	/// \brief Destructor of the client
	///
	/// This will automatically stop the thread that the client 
	/// uses for receive and send data
	///
	////////////////////////////////////////////////////////////
	~Client();

	////////////////////////////////////////////////////////////
	/// \brief Send a ping to the server that it will send back
	///
	/// TODO : Implement this function
	/// Use the Ping protocol with clock time
	///
	////////////////////////////////////////////////////////////
	void PingOutServer();

	////////////////////////////////////////////////////////////
	/// \brief Send a command to the server
	///
	/// \param a_data the package that contains all the data
	///
	/// \param a_customCommand a code/id given by the user to
	/// know waht is the purpose of this command
	///
	////////////////////////////////////////////////////////////
	void SendCommand(NetworkData& a_data, sf::Uint16 a_customCommand);

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
	bool Connect(const InfoServer* a_server, bool a_TcpConnect = false);


	////////////////////////////////////////////////////////////
	/// \brief check if a server exist at a non local address
	///
	/// \param a_nonLocalAddress the non local address to check
	///
	////////////////////////////////////////////////////////////
	void CheckServerExistance(sf::IpAddress& a_nonLocalAddress);

	////////////////////////////////////////////////////////////
	/// \brief Allow to know if the client is connected to a server
	///
	/// \return if the client is connected to a server
	///
	////////////////////////////////////////////////////////////
	bool IsConnected() const;

	////////////////////////////////////////////////////////////
	/// \brief Get the connection with the server
	///
	/// \return the connection with the server
	///
	////////////////////////////////////////////////////////////
	const Connection& GetServerConnection() const;

	////////////////////////////////////////////////////////////
	/// \brief Send a file that will be syncronized on all the clients
	///
	/// \param a_fileName the file name
	///
	////////////////////////////////////////////////////////////
	void SendFile(const std::string& a_fileName);

	////////////////////////////////////////////////////////////
	/// \brief Receive a part of a file
	///
	/// \param a_packet the data for the part of the file
	///
	////////////////////////////////////////////////////////////
	void ReceiveFile(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief Get the name of this client 
	///
	/// \return the name of the client
	///
	////////////////////////////////////////////////////////////
	const std::string& GetName() const;

	////////////////////////////////////////////////////////////
	/// \brief Get all the interresting information about this client
	/// and its connection
	///
	/// \return the stats of the client
	///
	////////////////////////////////////////////////////////////
	const ClientStat& GetStats() const;


	////////////////////////////////////////////////////////////
	/// \brief Close the connection with the server if it exist
	///
	////////////////////////////////////////////////////////////
	void CloseConnection();

private:

	////////////////////////////////////////////////////////////
	/// \brief This function is called as thread for each client,
	/// this is the equivalent of main for clients.
	///
	/// \param a_client the client at the origin of the thread
	///
	////////////////////////////////////////////////////////////
	static void ClientThread(Client* a_client);


	////////////////////////////////////////////////////////////
	/// \brief Send a packet to the server
	///
	/// \param a_packet the packet that contains all data
	///
	////////////////////////////////////////////////////////////
	void SendPacket(sf::Packet& a_packet);


	////////////////////////////////////////////////////////////
	/// \brief Called when the socket selector receive a packet
	/// 
	/// This function just read the protocol code and dispatch the packet
	/// to the concerned function
	///
	/// \param a_packet the packet that contains data
	///
	////////////////////////////////////////////////////////////
	void Receive(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief This is called when the connection was ended from
	/// server side
	///
	/// \param a_packet the packet that contains data without the protocol code
	///
	////////////////////////////////////////////////////////////
	void ReceiveEndConnection(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief Called after first step of reading packet, if the
	/// protocol code of the packet indicate an update
	///
	/// \param a_packet the packet that contains data without the protocol code
	///
	////////////////////////////////////////////////////////////
	void ReceiveUpdate(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief Called after first step of reading packet, if the
	/// protocol code of the packet indicate a creation
	///
	/// \param a_packet the packet that contains data without the protocol code
	///
	////////////////////////////////////////////////////////////
	void ReceiveCreate(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief Called after first step of reading packet, if the
	/// protocol code of the packet indicate a deletion
	///
	/// \param a_packet the packet that contains data without the protocol code
	///
	////////////////////////////////////////////////////////////
	void ReceiveDelete(sf::Packet& a_packet);


	////////////////////////////////////////////////////////////
	/// \brief Get if the client must ignore most of the data
	/// from the server because the server already handle the game core
	///
	/// \return if a server already handle game core on this app
	///
	////////////////////////////////////////////////////////////
	bool IsClientAndServer() const;

	////////////////////////////////////////////////////////////
	/// \brief Called after first step of reading packet, if the
	/// protocol code of the packet indicate a command
	///
	/// IMPORTANT : note that localhost client never receive command by this way
	///
	/// \param a_packet the packet that contains data without the protocol code
	///
	////////////////////////////////////////////////////////////
	void ReceiveCommand(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief Called after first step of reading packet, if the
	/// protocol code of the packet indicate a ping
	///
	/// \param a_packet the packet that contains data without the protocol code
	///
	////////////////////////////////////////////////////////////
	void ReceivePing(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief Called after first step of reading packet, if the
	/// protocol code of the packet indicate a broadcast
	///
	/// \param a_packet the packet that contains data without the protocol code
	///
	/// \param a_serverSource the ip of the server who emitted the broadcast
	///
	////////////////////////////////////////////////////////////
	void ReceiveBroadcast(sf::Packet& a_packet, sf::IpAddress& a_serverSource);

	////////////////////////////////////////////////////////////
	/// \brief Get the information that the connected server send
	/// by broadcast
	///
	////////////////////////////////////////////////////////////
	InfoServer* GetInfoOfTheConnection();




	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////
	bool m_isConnected; ///< Flag to know if this client is conected to any server

	bool m_isUsingUDPOnly; ///< Flag to know if the main communication is one with UDP protocol

	bool m_isRunning; ///< Flag to know if the client work and run fine

	int m_offsetTime; ///< The (theoritical) clock offset between the client and the server (in ms)

	std::thread m_clientThread; ///< The thread used by this client for data communication

	Connection m_server; ///< Remember the connection to the server (only one per client)

	sf::SocketSelector m_selector; ///< The selector for sockets since the client can communicate with the UDP and its TCP sockets

	std::string m_clientName; ///< The client name sent in many packets. This can be used in addition to the ip address for authentication
							  
	ClientStat m_stats; ///< Contains all informations that the end user might want about this client

	sf::Clock m_clock; ///< The clock of the client

	UdpHandler m_udpSystem; ///< The system that handle all the udp communication
};

}