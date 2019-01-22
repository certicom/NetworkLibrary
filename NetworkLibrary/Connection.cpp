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
#include "Connection.h"


namespace Net
{

////////////////////////////////////////////////////////////
/// \brief equal operator
///
/// TODO : verify why and where this is required
///
////////////////////////////////////////////////////////////
Connection& Connection::operator=(const Connection& other)
{
	throw NetworkException("Error : illegal operation, connection = !");
}


////////////////////////////////////////////////////////////
/// \brief Copy constructor
///
/// TODO : verify why and where this is required
///
////////////////////////////////////////////////////////////
Connection::Connection(const Connection& other)
{
	throw NetworkException("Error : illegal operation, connection copy!");
}

////////////////////////////////////////////////////////////
/// \brief Constructor
///
////////////////////////////////////////////////////////////
Connection::Connection()
{

}

////////////////////////////////////////////////////////////
/// \brief refuse this connection
///
////////////////////////////////////////////////////////////
void Connection::RefuseConnection()
{
	if (m_isConsideredAlive)
	{
		m_isConsideredAlive = false;
	}
}

}