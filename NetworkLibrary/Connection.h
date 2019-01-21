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

#include "NetworkEnums.h"

namespace Net
{


////////////////////////////////////////////////////////////
/// \brief Keep the information about a current connection
///
/// Server may have multiple of them, but client only one
/// As structure, this do not give any encapsulation security
///
/// TODO : is user allowed to use it ?  decide - structure or private access ?
///
////////////////////////////////////////////////////////////
class Connection
{

public:

	////////////////////////////////////////////////////////////
	/// \brief Constructor
	///
	////////////////////////////////////////////////////////////
	Connection();

	////////////////////////////////////////////////////////////
	/// \brief Copy constructor
	///
	/// TODO : verify why and where this is required
	///
	////////////////////////////////////////////////////////////
	Connection(const Connection& other);

	////////////////////////////////////////////////////////////
	/// \brief equal operator
	///
	/// TODO : verify why and where this is required
	///
	////////////////////////////////////////////////////////////
	Connection& operator=(const Connection& other);

	////////////////////////////////////////////////////////////
	/// \brief refuse this connection
	///
	////////////////////////////////////////////////////////////
	void RefuseConnection();

	////////////////////////////////////////////////////////////
	// Public member data
	////////////////////////////////////////////////////////////

	sf::TcpSocket m_TCPSocket; ///< If this connection uses TCP, the socket is here

	std::string m_name; ///< The name of the entity to which we are connected

	sf::Uint16 m_port; ///< The port to use to communicate with this entity

	sf::IpAddress m_ipAddress; ///< The ip address of the entity to wich we are connected

	bool m_isConsideredAlive; ///< Flag to know if the connection is still working

	bool m_isLocalHost; ///< Flag to know if the connection is localhosted

	bool m_isUDPConnection; ///< Flag to know if the connection uses UDP protocol

	sf::Clock m_lastPing; ///< The last time when the client was sending info

};
}
