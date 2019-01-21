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



////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////
// TODO : give to the end user a interface to modify thoses
#define CONNECTION_TIMEOUT 1500//ms time for a connection before considered as not responding

#define SERVER_MAX_CONNECTIONS 10


namespace Net
{


////////////////////////////////////////////////////////////
/// \brief The priority of an network object
///
/// TODO : implement real update system with working priorities
///
////////////////////////////////////////////////////////////
enum CommunicationPriority
{
	CP_VeryLow,
	CP_Low,
	CP_Normal,
	CP_Hight,
	CP_VeryHight
};


////////////////////////////////////////////////////////////
/// \brief all the type supported for network communication
///
/// TODO : VERY SOON - add 64/16/8b types (8?)
///
////////////////////////////////////////////////////////////
enum DataType
{
	DT_bool,
	DT_float,
	DT_double,
	DT_string,
	DT_Int32,
	DT_Uint32,
	DT_Uint8

};


////////////////////////////////////////////////////////////
/// \brief the list of all possible protocol code 
///
////////////////////////////////////////////////////////////
enum CommandType
{
	CT_NewConnection,
	CT_EndConnection,
	CT_UpdateObjects,
	CT_NewObject,
	CT_DeleteObject,
	CT_Broadcast,
	CT_CheckServer,
	CT_CustomCommand,
	CT_Ping,
	CT_File,
	CT_ClockSyncro
};

////////////////////////////////////////////////////////////
/// \brief handle all the exception that can append in this library
///
////////////////////////////////////////////////////////////
class NetworkException : std::exception
{

public:

	////////////////////////////////////////////////////////////
	/// \brief constructor for custom Network exceptions
	///
	////////////////////////////////////////////////////////////
	NetworkException(const char* msg)
	{
		m_msg = msg;
	}

	////////////////////////////////////////////////////////////
	/// \brief What override from standard exception
	///
	////////////////////////////////////////////////////////////
	const char * what() const throw ()
	{
		return m_msg;
	}

private:

	const char* m_msg; ///< Exception informations

};

}