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



namespace Net
{


////////////////////////////////////////////////////////////
/// \brief this structure give a compact way to get all the interresting
/// data about a server
///
/// As structure, this do not give any encapsulation security
///
////////////////////////////////////////////////////////////
struct NET InfoServer
{
public:

	sf::IpAddress m_address;         ///< The ip address of the server
	std::string m_name;              ///< The name of the server
	int m_connectedClients;          ///< The number of currently connected clients
	int m_maxClients;                ///< The maximum number of clients on the server
	std::string m_customInformation; ///< If the end user want that the server send some more informations, he can use this
	sf::Uint16 m_port;               ///< The port that must be used to communicate with this server
	sf::Clock m_lastPing;            ///< The last ping received from this server
};

}