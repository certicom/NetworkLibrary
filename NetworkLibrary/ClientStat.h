
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

#include "Connection.h"
#include "InfoServer.h"

namespace Net
{


////////////////////////////////////////////////////////////
/// \brief this structure give a compact way to get all the interresting
/// data about a client and its connection if any
///
/// As structure, this do not give any encapsulation security
///
/// TODO : definitly need a redesign of the structure
///
////////////////////////////////////////////////////////////
struct ClientStat
{
public:

	////////////////////////////////////////////////////////////
	/// \brief Constructor 
	///
	////////////////////////////////////////////////////////////
	ClientStat(Connection* a_connection) : m_emittedPackets(0), m_ReceivedPackets(0), m_clockOffset(0), m_serverInfo(NULL),
		                                   m_isInternet(true), m_currentLatency(0), m_averageLatency(0), m_connection(a_connection) {}


	////////////////////////////////////////////////////////////
	// Public Member data
	////////////////////////////////////////////////////////////
	int m_emittedPackets;  ///< The number of emitted packets from the client
	int m_ReceivedPackets; ///< the number of received packets from the client
	int m_clockOffset;     ///< The clock offset of a client
	bool m_isInternet;     ///< If the client is connect on a non local server
	int m_currentLatency;  ///< The current latency to communicate with the server
	int m_averageLatency;  ///< The average latency to communicate with the server

	const Connection* m_connection; ///< Infos about the connection
	InfoServer* m_serverInfo;       ///< Infos about the server itself
};

}