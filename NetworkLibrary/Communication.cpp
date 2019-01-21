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


#include "Communication.h"

namespace Net
{


////////////////////////////////////////////////////////////
/// \brief Start a new server 
///
/// TODO : handle the case where there is already a running server
///
/// \param a_name the name of the server
///
/// \param a_autoConnect If the server automatically accept all connections (TODO : false case)
///
/// \param a_local if the server is local (TODO : usefull ? broadcast ?)
///
/// \return A pointer to the new server
///
////////////////////////////////////////////////////////////
Server* Communication::StartServer(const std::string& a_name, bool a_autoConnect, bool a_local)
{
	return InternalComm::StartServer(a_name, a_autoConnect, a_local);
}


////////////////////////////////////////////////////////////
/// \brief get the list of all available servers
///
/// if timeout is used, the life duration of the returned list 
/// is guarranted only until the next call of this function
/// TODO : better life time or auto timeout
///
/// \param a_msTimeOut maximum time since the last communication of this server
///
/// \return The list of all the live servers
///
////////////////////////////////////////////////////////////
const std::vector<InfoServer*>& Communication::GetAvailableServers(int a_msTimeOut)
{
	return InternalComm::GetAvailableServers(a_msTimeOut);
}


////////////////////////////////////////////////////////////
/// \brief change the callback for new connections
///
/// \param a_newConnectionCallback the new callback
///
////////////////////////////////////////////////////////////
void Communication::SetNewConnectionCallback(void(*a_newConnectionCallback)(Connection*))
{
	InternalComm::SetNewConnectionCallback(a_newConnectionCallback);
}


////////////////////////////////////////////////////////////
/// \brief start a new client 
///
/// TODO : handle the case where there is already a running client
///
/// \param a_name the name of the client
///
/// \return A pointer to the new client
///
////////////////////////////////////////////////////////////
Client* Communication::StartClient(const std::string& a_clientName)
{
	return InternalComm::StartClient(a_clientName);
}

////////////////////////////////////////////////////////////
/// \brief check if a server exist at a non local address
///
/// \param a_nonLocalAddress the non local address to check
///
////////////////////////////////////////////////////////////
void Communication::CheckServerExistance(const std::string& a_nonLocalAddress)
{
	InternalComm::CheckServerExistance(a_nonLocalAddress);
}

////////////////////////////////////////////////////////////
/// \brief stop all communication threads (server, client and update)
///
/// Use it at the end of the application
///
////////////////////////////////////////////////////////////
void Communication::ShutDownAllCommunications()
{
	InternalComm::ShutDownAllCommunications();
}

////////////////////////////////////////////////////////////
/// \brief close the server if any 
///
////////////////////////////////////////////////////////////
void Communication::CloseServer()
{
	InternalComm::CloseServer();
}

////////////////////////////////////////////////////////////
/// \brief close the client if any
///
////////////////////////////////////////////////////////////
void Communication::CloseClient()
{
	InternalComm::CloseClient();
}

////////////////////////////////////////////////////////////
/// \brief get the list of all not responding connections
/// it can be client as well as server
///
/// \return the list of all not responding connections
///
////////////////////////////////////////////////////////////
std::vector<const Connection*> Communication::GetNotRespondingConnections()
{
	return InternalComm::GetNotRespondingConnections();
}

}
