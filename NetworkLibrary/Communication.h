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

//#include "NetworkEnums.h" // (included in Command)
//#include "NetworkData.h" // (included in Command)

#include "InternalComm.h"


namespace Net
{


////////////////////////////////////////////////////////////
/// \brief This class is fully static, it only serve as 
/// end user interface to prevent them from using InternalComm
///
/// All the function in this class or just reflected from InternalComm
///
/// \see Net::InternalComm
///
////////////////////////////////////////////////////////////
class Communication
{
public:


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
	static Server* StartServer(const std::string& a_name, bool a_autoConnect = true, bool a_local = false); // start server in a new thread

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
	static Client* StartClient(const std::string& a_clientName); // start a client in a new thread

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
	static const std::vector<InfoServer*>& GetAvailableServers(int a_msTimeOut = -1); // get the list of all available servers (optional msTimeOut return only the server with recent communication)

	////////////////////////////////////////////////////////////
	/// \brief close the server if any 
	///
	////////////////////////////////////////////////////////////
	static void CloseServer();

	////////////////////////////////////////////////////////////
	/// \brief close the client if any
	///
	////////////////////////////////////////////////////////////
	static void CloseClient();

	////////////////////////////////////////////////////////////
	/// \brief stop all communication threads (server, client and update)
	///
	/// Use it at the end of the application
	///
	////////////////////////////////////////////////////////////
	static void ShutDownAllCommunications();


	////////////////////////////////////////////////////////////
	/// \brief check if a server exist at a non local address
	///
	/// \param a_nonLocalAddress the non local address to check
	///
	////////////////////////////////////////////////////////////
	static void CheckServerExistance(const std::string& a_nonLocalAddress);

	////////////////////////////////////////////////////////////
	/// \brief change the callback for new connections
	///
	/// \param a_newConnectionCallback the new callback
	///
	////////////////////////////////////////////////////////////
	static void SetNewConnectionCallback( void(*a_newConnectionCallback)(Connection*) );


	////////////////////////////////////////////////////////////
	/// \brief get the list of all not responding connections
	/// it can be client as well as server
	///
	/// \return the list of all not responding connections
	///
	////////////////////////////////////////////////////////////
	static std::vector<const Connection*> GetNotRespondingConnections();


	////////////////////////////////////////////////////////////
	/// \brief spawn a new object if we are from server side, else
	/// this will not do anything
	///
	/// \param args parameter package for calling the constructor
	/// of the new object
	///
	/// \tparam T the class of the object to instanciate
	///
	/// \tparam Args the parameter package to build the object
	///
	/// \return the new spawned object
	///
	////////////////////////////////////////////////////////////
	template<class T, class... Args>
	static T* SpawnObjectFromServer(Args&&... args)
	{
		return InternalComm::SpawnObjectFromServer<T>(std::forward<Args>(args)...);
	}



	////////////////////////////////////////////////////////////
	/// \brief add a new instanciable type to allow clients to
	/// syncronize new objects
	///
	/// \tparam the type that can now be instanciate
	///
	////////////////////////////////////////////////////////////
	template<class T>
	static void AddInstanciableType()
	{
		InternalComm::AddInstanciableType<T>();
	}


};
}

