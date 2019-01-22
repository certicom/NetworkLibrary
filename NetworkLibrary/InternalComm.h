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

#include "Command.h"
#include "InfoServer.h"


namespace Net
{


////////////////////////////////////////////////////////////
// Forward definitions
////////////////////////////////////////////////////////////
class Server;
class Client;
class NetworkObject;
class Connection;

////////////////////////////////////////////////////////////
// Aliasing
////////////////////////////////////////////////////////////
typedef NetworkObject* (*FactoryMethod)();



////////////////////////////////////////////////////////////
/// \brief This class is fully static. It handle everything
/// that the user can do as well as everything that the 
/// server and the client can do both.
///
////////////////////////////////////////////////////////////
class NET InternalComm
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
	static Server* StartServer(const std::string& a_name, bool a_autoConnect = true, bool a_local = false);

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
	static Client* StartClient(const std::string& a_clientName);

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
	static const std::vector<InfoServer*>& GetAvailableServers(int a_msTimeOut = -1);

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
	/// \brief stop the thread that update network objetcs
	///
	////////////////////////////////////////////////////////////
	static void StopUpdateThread();

	////////////////////////////////////////////////////////////
	/// \brief stop all communication threads (server, client and update)
	///
	/// Use it at the end of the application
	///
	////////////////////////////////////////////////////////////
	static void ShutDownAllCommunications();

	////////////////////////////////////////////////////////////
	/// \brief change the callback for new connections
	///
	/// \param a_newConnectionCallback the new callback
	///
	////////////////////////////////////////////////////////////
	static void SetNewConnectionCallback(void(*a_newConnectionCallback)(Connection*));

	////////////////////////////////////////////////////////////
	/// \brief update the list of all know server
	///
	/// \param a_infoServer the info server we just received
	///
	////////////////////////////////////////////////////////////
	static void AddOrUpdateInfoServer(InfoServer* a_infoServer);

	////////////////////////////////////////////////////////////
	/// \brief Write an object data in a packet for transmition
	/// this uses the Object Protocol (Read Me)
	///
	/// \param a_packet the packet in wich we will write
	///
	/// \param a_data the data we need to write
	///
	////////////////////////////////////////////////////////////
	static void WriteObject(sf::Packet& a_packet, const NetworkData& a_data);

	////////////////////////////////////////////////////////////
	/// \brief Read an object in a packet and put the data in a NetworkData
	/// this uses the Object Protocol (Read Me)
	///
	/// \param a_packet the packet we need to read
	///
	/// \param a_data the data in wich we will put the variables read.
	///
	/// \return if the reading is a success
	///
	////////////////////////////////////////////////////////////
	static bool ReadObject(sf::Packet& a_packet, NetworkData& a_data);


	////////////////////////////////////////////////////////////
	/// \brief Write the a variable in a packet, this uses the Variable Protocol (Read Me)
	///
	/// \param a_packet the packet in wich we will write the variable
	///
	/// \param a_data the data that we will write
	///
	////////////////////////////////////////////////////////////
	static void WriteVariable(sf::Packet& a_packet, const Data& a_data);

	////////////////////////////////////////////////////////////
	/// \brief Read the next variable in a packet, this uses the Variable Protocol (Read Me)
	///
	/// \param a_packet the packet in wich we will read the variable
	///
	/// \return the data that contains the variable
	///
	////////////////////////////////////////////////////////////
	static Data ReadVariable(sf::Packet& a_packet);

	////////////////////////////////////////////////////////////
	/// \brief Read a received packet and put the data in a NetworkData
	///
	/// \param a_packet the packet we need to read
	///
	/// \param a_data the data in wich we will put the variables read.
	///
	/// \return if the reading is a success
	///
	////////////////////////////////////////////////////////////
	static bool ReadCommand(sf::Packet& a_packet, NetworkData& a_data);

	////////////////////////////////////////////////////////////
	/// \brief Write a command data in a packet for transmition
	///
	/// \param a_packet the packet in wich we will write
	///
	/// \param a_data the data we need to write
	///
	////////////////////////////////////////////////////////////
	static void WriteCommand(sf::Packet& a_packet, const NetworkData& a_data);

	////////////////////////////////////////////////////////////
	/// \brief [Client side] Send a received update from the server to
	/// the specified object in the data
	///
	/// \param a_data the received data
	///
	/// \param a_forceId if the network id will be overrides
	///
	////////////////////////////////////////////////////////////
	static void SendUpdateToObject(NetworkData& a_data, bool a_forceId);

	////////////////////////////////////////////////////////////
	/// \brief [Client side] Send a received command from the server to
	/// the specified object in the command
	///
	/// \param a_command the received command
	///
	////////////////////////////////////////////////////////////
	static void SendCommandToObject(Command& a_command);

	////////////////////////////////////////////////////////////
	/// \brief check if a server exist at a non local address
	///
	/// \param a_nonLocalAddress the non local address to check
	///
	////////////////////////////////////////////////////////////
	static void CheckServerExistance(const std::string& a_nonLocalAddress);

	////////////////////////////////////////////////////////////
	/// \brief [Client side] Send a command to the server, this command
	/// comes from an network object
	///
	/// \param a_origin the objet at the origin of the command
	///
	/// \param a_codeCommand the user code gave to this command
	///
	/// \param a_data the data of the command
	///
	////////////////////////////////////////////////////////////
	static void SendCommand(NetworkObject* a_origin, int a_codeCommand, NetworkData& a_data);

	////////////////////////////////////////////////////////////
	/// \brief [Server side] Get the list of all new objects that 
	/// have not been handle yet
	///
	/// \return the list of unhandled new objects
	///
	////////////////////////////////////////////////////////////
	static std::vector<NetworkObject*>& GetNewObjects();

	////////////////////////////////////////////////////////////
	/// \brief [Server side] indicate that the server has finished
	/// to syncronized new objects for all clients
	///
	////////////////////////////////////////////////////////////
	static void NewObjectsWasHandled();

	////////////////////////////////////////////////////////////
	/// \brief this function is the internal connection callback
	/// it will call the user connection callback
	///
	/// \param a_connection the new connection
	///
	////////////////////////////////////////////////////////////
	static void NewConnectionCallBack(Connection* a_connection);

	////////////////////////////////////////////////////////////
	/// \brief Get if a server is running on this application
	///
	/// \return if a server is running on this application
	///
	////////////////////////////////////////////////////////////
	static bool HasRunningServer();

	////////////////////////////////////////////////////////////
	/// \brief get the flag CanInstanciate, this is a use safety to
	/// prevent user to manually instanciate NetworkObjects
	///
	/// \return if a NetworkObject can be created
	///
	////////////////////////////////////////////////////////////
	static bool IsInstanciable();


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
	static T* SpawnObjectFromServer(Args&&... args) //  spawn an object that will be auto syncro (only if we are from server side)
	{
		if (s_server != NULL /*&& s_factoryMap.find(typeid(T).name())*/)
		{
			s_canInstanciate = true;

			T* l_obj = new T(std::forward<Args>(args)...);

			s_canInstanciate = false;

			l_obj->SetTypeName(typeid(T).name());

			s_newObjects.push_back(l_obj);

			return l_obj;
		}

		return NULL;
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
		// TODO : check if polymorphism is capable of doing that automatically from the constructor of NetworkObject (Seems not)
		s_factoryMap[typeid(T).name()] = &InternalComm::InstanciateType<T>;
	}

	////////////////////////////////////////////////////////////
	/// \brief [Client side] this function is internally used to instanciate
	/// network objects from client side 
	///
	/// \param a_typeName the name of the type that must be instanciate (typeid(T).name())
	///
	/// \return A pointer to the new network object
	///
	////////////////////////////////////////////////////////////
	static NetworkObject* InstanciateType(const std::string& a_typeName)
	{
		return (*s_factoryMap[a_typeName])();
	}



private:

	////////////////////////////////////////////////////////////
	/// \brief [Client side] This function is call to instanciate
	/// an object of a specific type
	///
	/// \tparam T the type of the object to instanciate
	///
	/// \return A pointer to the new network object
	///
	////////////////////////////////////////////////////////////
	template<class T>
	static NetworkObject* InstanciateType()
	{
		s_canInstanciate = true;

		NetworkObject* l_obj = new T();

		s_canInstanciate = false;

		return l_obj;
	}


	////////////////////////////////////////////////////////////
	// Static member data
	////////////////////////////////////////////////////////////

	static std::vector<InfoServer*> s_availableServers; ///< The list of all the servers that currently broacasting

	static std::vector<InfoServer*> s_availableServersCustom; ///< Same list as s_availableServers, but store the result of the previous request for variable persistancy

	static Server* s_server; ///< Server singleton for now, remember the running server on this application

	static Client* s_client; ///< Client singleton for now, remember the running client on this application

	static std::thread s_updateThread; ///< The stored update thread for update network objects

	static std::vector<NetworkObject*> s_newObjects; ///< The list of all the new objects recently created and not handled yet

	static std::map<std::string, FactoryMethod> s_factoryMap; ///< This map is a bit tricky, it store a pointer of a template function capable of instanciate a type

	static void(*s_newConnectionCallback)(Connection*); ///< The pointer to the callback function to call when a new connection append

	static bool s_canInstanciate; ///< Flag to know if a new network object can be instanciate (use safety)

	static bool s_isInit; ///< Flag to know if the thread for updating object is currently running 

};

}

// cpp link includes
#include "NetworkObject.h"
#include "Server.h"
#include "Client.h"