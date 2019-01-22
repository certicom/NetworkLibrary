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
#include "stdafx.h"

#include "NetworkEnums.h"

#define BROADCAST_PORT 53000 // The port used for broadcasting

#define MASTER_PORT 53001 // the port to communicate with the master

namespace Net
{
	

////////////////////////////////////////////////////////////
/// \brief This class allow to use udp communication while
/// having multiple clients/servers on the same machine
///
/// Every entity that want communicate with Udp must have one
/// of this object typically clients end servers
///
////////////////////////////////////////////////////////////
class NET UdpHandler
{
public:

	////////////////////////////////////////////////////////////
	/// \brief The constructor, it will determine if we are master
	/// are slave as well as the port to use. If we are master
	/// it will start a listener thread
	///
	////////////////////////////////////////////////////////////
	UdpHandler();

	////////////////////////////////////////////////////////////
	/// \brief Get the 'effective' Udp socket. This socket is 
	/// supposed to be used like a normal one because everything
	/// is done to avoid collision and simulate rerouting of broadcasts
	///
	/// \return the udp socket to use for communication 
	///
	////////////////////////////////////////////////////////////
	sf::UdpSocket& GetUdpSocket();

	////////////////////////////////////////////////////////////
	/// \brief Get the port selected among the available ports
	/// to make sure we can communicate without collisions
	///
	/// \return the selected available port used to communicate
	/// with this entity
	///
	////////////////////////////////////////////////////////////
	sf::Uint16 GetUdpPort();

	////////////////////////////////////////////////////////////
	/// \brief Stop the listener thread of the master if it is
	/// running on this application
	///
	////////////////////////////////////////////////////////////
	static void StopUdpSharingSystem();

private:

	////////////////////////////////////////////////////////////
	/// \brief The function ran by the listener thread for the master
	///
	/// \param a_handler is the original handler(master) that 
	/// lunched the thread
	///
	////////////////////////////////////////////////////////////
	static void UdpThread(UdpHandler* a_handler);


	////////////////////////////////////////////////////////////
	// Static member data
	////////////////////////////////////////////////////////////

	static bool s_isThreadRunning; ///< Flag to know if the master thread is running on this handler

	static std::thread s_thread; ///< The master thread (only one per machine)

	////////////////////////////////////////////////////////////
	// member data
	////////////////////////////////////////////////////////////

	bool m_isMaster; ///< Flag to know if this handler is the master

	std::vector<sf::Uint16> m_slavesPorts; ///< The list of all the ports of the apps connected to this handler if it is the master

	sf::UdpSocket m_UdpSocket; ///< The udp socket that is given to the rest of the app to allow udp communication

	sf::UdpSocket m_UdpSocketForBroadcast; ///< The socket that listen for Broadcast messages from master side

	sf::UdpSocket m_UdpSocketForSlaves; ///< The socket that listen for slaves presence from master side

	sf::Uint16 m_port; ///< The port used to communicate with this entity
};
}

////////////////////////////////////////////////////////////
/// \class UdpHandler
///
/// The need to have the possibility to create a dedicated server
/// is the reason of this class.
/// Since a dedicated server often mean having a client and a
/// server on two different application but on the same machine,
/// we got multiple problems
/// - The extenal (non localhost) applications can not differenciate
/// those applications
/// - Those multiple localhost application can not listen on the same port
///
/// So here a different port is given to each application, then when
/// it will try to connect to something, it will send this port number
/// to indicate how communicate.
/// The other point of this app, is, since we can not all listen
/// on the same port and that broadcasts are always on the same port,
/// we create a system of internal connection slave/master to redirect
/// the broadcast that do not reach some of the applications
///
/// Some subtility :
/// There is one handler for each client and server, so in the configuration
/// of client + server there will be two handler in the same application.
/// But, that will act like two separate applications thank to this system
///
////////////////////////////////////////////////////////////