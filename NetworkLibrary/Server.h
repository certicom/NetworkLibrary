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

//#include "NetworkEnums.h" // (included in Communication)
//#include "NetworkData.h" // (included in Communication)
//#include "Command.h"// (included in Communication)

#include "InternalComm.h"
#include "Connection.h"
#include "NetworkObject.h" 


#include "UdpHandler.h"

namespace Net
{

////////////////////////////////////////////////////////////
/// \brief this class is capable to handle a connection with multiple
/// clients and to communicate with them.
///
/// A server automatically broacast on local network while is flag
/// isListening is activate, it allow client to simply find and connect
///
////////////////////////////////////////////////////////////
class Server
{

public:

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
	Server(const std::string& a_name, bool a_autoConnect, bool a_local, int a_timeout, int a_maxConnections);

	////////////////////////////////////////////////////////////
	/// \brief destructor
	///
	////////////////////////////////////////////////////////////
	~Server();

	////////////////////////////////////////////////////////////
	/// \brief Syncronize the clock of all connected client to
	/// the server clock
	///
	/// TODO : implement this
	///
	////////////////////////////////////////////////////////////
	void ClockSyncroForAllClients();

	////////////////////////////////////////////////////////////
	/// \brief Send a ping to a specific client that it will send back
	///
	/// TODO : Implement this function
	/// Use the Ping protocol with clock time
	///
	////////////////////////////////////////////////////////////
	void PingOutClient(Connection* a_client);

	////////////////////////////////////////////////////////////
	/// \brief send an update to a list of objects
	///
	/// \param a_data the list a object that we want to syncronized
	///
	/// \param a_byBroadcast if we use a broadcast to do that a save
	/// bandwidth (TODO : implement this)
	///
	////////////////////////////////////////////////////////////
	void SendUpdate(std::vector<NetworkData>& a_data, bool a_byBroadcast = false);

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
	void SendCommand(NetworkData& a_data, sf::Uint16 a_customCommand, bool a_byBroadcast = false);

	////////////////////////////////////////////////////////////
	/// \brief send a delete order to a list of objects
	///
	/// \param a_list the list of object ids that we want destroy
	///
	////////////////////////////////////////////////////////////
	void SendDelete(std::vector<sf::Uint16>& a_list);

	////////////////////////////////////////////////////////////
	/// \brief set if the server is currently listening
	///
	/// If the server is not listening, it will not broadcast and 
	/// will not accept new connections (TODO : implment this second part)
	///
	/// \param a_listening if the server must listening
	///
	////////////////////////////////////////////////////////////
	void SetListeningState(bool a_listening);

	////////////////////////////////////////////////////////////
	/// \brief set if the server will accept all new connections
	///
	/// TODO : implement this
	///
	/// \param a_autoAccept if the server must accept all connections
	///
	////////////////////////////////////////////////////////////
	void SetAutoAccept(bool a_autoAccept);

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
	void SetCustomInformation(std::string& a_infos);

	////////////////////////////////////////////////////////////
	/// \brief Get the connection infos that correspond to an specific
	/// ip address if this connection exist
	///
	/// \param a_address the ip address of the request
	///
	/// \return the connection with this ip address
	///
	////////////////////////////////////////////////////////////
	Connection* GetIdFromIP(sf::IpAddress& a_address);

	////////////////////////////////////////////////////////////
	/// \brief Get the connection infos that correspond to an specific
	/// client name if this connection exist
	///
	/// \param a_name the client name of the request
	///
	/// \return the connection with this client name
	///
	////////////////////////////////////////////////////////////
	Connection* GetIdFromName(std::string& a_name);


	////////////////////////////////////////////////////////////
	/// \brief get the list of all the clients of the server
	///
	/// \return the list of all clients
	///
	////////////////////////////////////////////////////////////
	const std::vector<Connection*>& GetClients() const;


	////////////////////////////////////////////////////////////
	/// \brief Close the connection with the server if it exist
	///
	/// \param a_connection The connection to close
	///
	////////////////////////////////////////////////////////////
	void CloseConnection(Connection* a_connection);

private:

	////////////////////////////////////////////////////////////
	/// \brief Get the current connected clients on the server
	/// 
	/// \return the number of connected clients on the server
	///
	////////////////////////////////////////////////////////////
	sf::Uint8 GetNumberOfConnectedClients();

	////////////////////////////////////////////////////////////
	/// \brief send a packet to all clients over the right 
	/// protocol (UDP or TCP)
	///
	/// \param a_packet the packet to send
	///
	////////////////////////////////////////////////////////////
	void SendPacket(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief send a packet to only one client over the right 
	/// protocol (UDP or TCP)
	///
	/// \param a_packet the packet to send
	///
	/// \param a_client the targeted client
	///
	////////////////////////////////////////////////////////////
	void SendPacketToOneClient(sf::Packet& a_packet, Connection* a_client);

	////////////////////////////////////////////////////////////
	/// \brief Handle received informations from a client
	///
	/// \param a_packet the received packet
	///
	/// \param a_idUser the connection at the origin of this packet 
	///
	////////////////////////////////////////////////////////////
	void ReceiveInformation(sf::Packet& a_packet, Connection* a_idUser);

	////////////////////////////////////////////////////////////
	/// \brief Handle a command received from a client
	///
	/// \param a_packet the received packet
	///
	/// \param a_idUser the connection at the origin of this packet 
	///
	////////////////////////////////////////////////////////////
	void ReceiveCommand(sf::Packet& a_packet, Connection* a_idUser);

	////////////////////////////////////////////////////////////
	/// \brief This is called when the connection was ended from
	/// client side
	///
	/// \param a_packet the received packet
	///
	/// \param a_idUser the connection at the origin of this packet 
	///
	////////////////////////////////////////////////////////////
	void ReceiveEndConnection(sf::Packet& a_packet, Connection* a_idUser);

	////////////////////////////////////////////////////////////
	/// \brief Handle a new connection request from a client
	///
	/// \param a_packet the received packet
	///
	/// \param a_idUser the connection at the origin of this packet 
	///
	////////////////////////////////////////////////////////////
	void ReceiveNewConnection(sf::Packet& a_packet, Connection* a_idUser);

	////////////////////////////////////////////////////////////
	/// \brief Receive a ping from a client
	///
	/// \param a_packet the received packet
	///
	/// \param a_idUser the connection at the origin of this packet 
	///
	////////////////////////////////////////////////////////////
	void ReceivePing(sf::Packet& a_packet, Connection* a_idUser);

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
	void ReceiveCheckServer(sf::Packet& a_packet, Connection* a_idUser);

	////////////////////////////////////////////////////////////
	/// \brief Handle the reception of a new connection
	/// from a TCP user
	///
	////////////////////////////////////////////////////////////
	void HandleNewTcpConnection();

	////////////////////////////////////////////////////////////
	/// \brief Handle the reception of a new Udp message
	///
	////////////////////////////////////////////////////////////
	void HandleNewUdpMessage();

	////////////////////////////////////////////////////////////
	/// \brief Do a broadcast with server informations
	///
	/// \param a_address by default a broadcast send the packet on the
	/// broadcast address, but for non local responses to a client
	/// we need to use the address of the client
	///
	////////////////////////////////////////////////////////////
	void Broadcast(const sf::IpAddress& a_address = sf::IpAddress::Broadcast);

	////////////////////////////////////////////////////////////
	/// \brief This function will handle any new objects that was not
	/// syncronized yet.
	///
	////////////////////////////////////////////////////////////
	void HandleNewObjects();
	
	////////////////////////////////////////////////////////////
	/// \brief Add a new client that wants to use the UDP protocol
	///
	/// TCP users are added inline btw
	///
	/// \param a_connection the new user connection
	///
	////////////////////////////////////////////////////////////
	void AddNewUdpUser(Connection* a_connection);

	////////////////////////////////////////////////////////////
	/// \brief When a new client is connected, we will send to 
	/// him all the network objects currently existing
	///
	/// TODO : open a new thread for this specific task
	///
	/// \param a_newConnection the client to syncronize
	///
	////////////////////////////////////////////////////////////
	void SyncroNewClient(Connection* a_newConnection);

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
	Connection* GiveTempUdpConnection(sf::IpAddress& a_address);


	////////////////////////////////////////////////////////////
	/// \brief Handle old clients, it send a ping to all clients 
	/// that have not communicated recently to make sure, they 
	/// are still alive, it delete dead connection and it close 
	/// very old connections that not responding
	///
	////////////////////////////////////////////////////////////
	void HandleOldClients();

	
	////////////////////////////////////////////////////////////
	/// \brief Receive and reflect a part of a file
	///
	/// \param a_packet the packet that contains the part of the file
	///
	/// \param a_idUser the user that send the packet
	///
	////////////////////////////////////////////////////////////
	void ReceiveFile(sf::Packet& a_packet, Connection* a_idUser);

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
	void RemoveUdpUserIfAny(sf::IpAddress& a_address);

	////////////////////////////////////////////////////////////
	/// \brief This function is called as thread for a server,
	/// this is the equivalent of main for server.
	///
	/// \param a_server the server at the origin of the thread
	///
	////////////////////////////////////////////////////////////
	static void ServerThread(Server* a_server);


	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////

	UdpHandler m_udpSystem;             ///< The system that handle all the udp communication
	sf::TcpListener m_listener;         ///< The TCP listener for listen new TCP connections
	sf::SocketSelector m_selector;      ///< The socket selector to wait on the socket simultaniously
	std::vector<Connection*> m_clients; ///< The list of all currently connected clients
	std::string m_serverName;           ///< The name of the server
	std::thread m_serverThread;         ///< The stored thread used to run the server
	std::string m_customInformation;    ///< More information about this server, this is custom data given by the user
	sf::Uint8 m_maxConnections;         ///< The maximum connection allowed by the server
	sf::Clock m_clock;                  ///< The clock of the server
	int m_clientTimeOut;                ///< The maximum time between ping before considering that a client is dead

	bool m_isRunning;    ///< Flag to know if the server is running
	bool m_isLocal;      ///< Flag to know if the server is local or internet
	bool m_isListening;  ///< Flag to know if the server is currently listening (mean it brodcast and accept new connections)
	bool m_isAutoAccept; ///< Flag to know if the server automatically accept new connections
};

}

