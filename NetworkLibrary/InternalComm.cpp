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
#include "InternalComm.h"

namespace Net
{

Server* InternalComm::s_server = NULL; ///< Server singleton for now, remember the running server on this machine

Client* InternalComm::s_client = NULL; ///< Client singleton for now, remember the running client on this machine

std::vector<InfoServer*> InternalComm::s_availableServers; ///< The list of all the servers that currently broacasting

std::vector<InfoServer*> InternalComm::s_availableServersCustom; ///< Same list as s_availableServers, but store the result of the previous request for variable persistancy

std::unordered_set<std::string> InternalComm::s_syncronizedFiles; ///< The list of all files that must be syncronized on all clients

std::thread InternalComm::s_updateThread; ///< The stored update thread for update network objects

bool InternalComm::s_canInstanciate = false; ///< Flag to know if a new network object can be instanciate (use safety)

std::vector<NetworkObject*> InternalComm::s_newObjects; ///< The list of all the new objects recently created and not handled yet

std::map<std::string, FactoryMethod> InternalComm::s_factoryMap; ///< This map is a bit tricky, it store a pointer of a template function capable of instanciate a type

void(*InternalComm::s_newConnectionCallback)(Connection*) = NULL; ///< The pointer to the callback function to call when a new connection append


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
Server* InternalComm::StartServer(const std::string& a_name, bool a_autoConnect, bool a_local)
{
	NetworkObject::StartUpdateThread(s_updateThread);

	s_server = new Server(a_name, a_autoConnect, a_local, CONNECTION_TIMEOUT, SERVER_MAX_CONNECTIONS);
	return s_server;
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
const std::vector<InfoServer*>& InternalComm::GetAvailableServers(int a_msTimeOut)
{
	if (a_msTimeOut == -1)
		return s_availableServers;

	s_availableServersCustom.clear();

	for (InfoServer* server : s_availableServers)
	{
		// TODO : restore this
		/*
		if (server->m_lastPing.getElapsedTime().asMilliseconds() < a_msTimeOut)
		{
			s_availableServersCustom.push_back(server);
		}
		*/

		s_availableServersCustom.push_back(server);
	}

	return s_availableServersCustom;
}



////////////////////////////////////////////////////////////
/// \brief this function is the internal connection callback
/// it will call the user connection callback
///
/// \param a_connection the new connection
///
////////////////////////////////////////////////////////////
void InternalComm::NewConnectionCallBack(Connection* a_connection)
{
	if (s_newConnectionCallback != NULL)
		s_newConnectionCallback(a_connection);
}


////////////////////////////////////////////////////////////
/// \brief get the flag CanInstanciate, this is a use safety to
/// prevent user to manually instanciate NetworkObjects
///
/// \return if a NetworkObject can be created
///
////////////////////////////////////////////////////////////
bool InternalComm::IsInstanciable()
{
	return s_canInstanciate;
}

////////////////////////////////////////////////////////////
/// \brief change the callback for new connections
///
/// \param a_newConnectionCallback the new callback
///
////////////////////////////////////////////////////////////
void InternalComm::SetNewConnectionCallback(void(*a_newConnectionCallback)(Connection*))
{
	s_newConnectionCallback = a_newConnectionCallback;
}


////////////////////////////////////////////////////////////
/// \brief get the list of all files that must be syncronized
///
/// \return the list of all files that must be syncronized
///
////////////////////////////////////////////////////////////
std::unordered_set<std::string>& InternalComm::GetSyncronizedFiles()
{
	return s_syncronizedFiles;
}

////////////////////////////////////////////////////////////
/// \brief Add a file taht will be syncronized on each client 
/// that will connect
/// if the server.exe and the client.exe are in the same
/// repertory, this will not append (because already here)
///
/// \param a_filePath the path of the file to syncronize
///
////////////////////////////////////////////////////////////
void InternalComm::AddSyncronizedFile(const std::string& a_filePath)
{

	if (s_syncronizedFiles.find(a_filePath) == s_syncronizedFiles.end()) // since we check for duplicate, unordered_set is not really usefull, but what ever
	{
		s_syncronizedFiles.insert(a_filePath);
		
		if(s_server != NULL)
			s_server->AddSyncronizedFile(a_filePath);
	}
	else
	{
		throw NetworkException("Error : This file was already syncronized, use ResyncronizeFile instead if you want to update it");
	}
}


////////////////////////////////////////////////////////////
/// \brief If a file was already be syncronized, but that
/// it receive some important changes, this will resyncronize
/// all the file on all clients

/// \param a_filePath the path of the file to syncronize
///
////////////////////////////////////////////////////////////
void InternalComm::ResyncronizeFile(const std::string& a_filePath)
{
	if (s_syncronizedFiles.find(a_filePath) != s_syncronizedFiles.end())
	{
		if (s_server != NULL)
			s_server->ResyncronizeFile(a_filePath);
	}
	else
	{
		throw NetworkException("Error : Can not resyncronize a file that was not syncronized in a first place");
	}
}

////////////////////////////////////////////////////////////
/// \brief Get if a server is running on this application
///
/// \return if a server is running on this application
///
////////////////////////////////////////////////////////////
bool InternalComm::HasRunningServer()
{
	return (s_server != NULL);
}

////////////////////////////////////////////////////////////
/// \brief check if a server exist at a non local address
///
/// \param a_nonLocalAddress the non local address to check
///
////////////////////////////////////////////////////////////
void InternalComm::CheckServerExistance(const std::string& a_nonLocalAddress)
{
	sf::IpAddress l_address(a_nonLocalAddress);

	if (s_client != NULL)
	{
		s_client->CheckServerExistance(l_address);
	}
}

////////////////////////////////////////////////////////////
/// \brief update the list of all know server
///
/// \param a_infoServer the info server we just received
///
////////////////////////////////////////////////////////////
void InternalComm::AddOrUpdateInfoServer(InfoServer* a_infoServer)
{
	for (InfoServer* infoServer : s_availableServers)
	{
		if (infoServer->m_address.toInteger() == a_infoServer->m_address.toInteger() && infoServer->m_port == a_infoServer->m_port) // id : Ip + port = avoid blended servers
		{
			//update
			infoServer->m_connectedClients = a_infoServer->m_connectedClients;
			infoServer->m_customInformation = a_infoServer->m_customInformation;
			infoServer->m_maxClients = a_infoServer->m_maxClients;
			infoServer->m_name = a_infoServer->m_name;

			infoServer->m_lastPing.restart();

			delete a_infoServer;

			return;// hard return but we don't need to do the rest of the loop
		}
	}

	// if we arrive here, we must add a new info server
	s_availableServers.push_back(a_infoServer);

}

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
void InternalComm::SendCommand(NetworkObject* a_origin, int a_codeCommand, NetworkData& a_data)
{
	a_data.SetId(a_origin->GetId());

	s_client->SendCommand(a_data, a_codeCommand);
}


////////////////////////////////////////////////////////////
/// \brief get the list of all not responding connections
/// it can be client as well as server
///
/// \return the list of all not responding connections
///
////////////////////////////////////////////////////////////
std::vector<const Connection*> InternalComm::GetNotRespondingConnections()
{
	//this return a copy of the vector, but it is not very heavy because
	// it only contains pointers and very few elements

	std::vector<const Connection*> l_list;

	if (s_client != NULL)
	{
		if (s_client->GetServerConnection().m_lastPing.getElapsedTime().asMilliseconds() >= CONNECTION_TIMEOUT)
		{
			l_list.push_back(&s_client->GetServerConnection());
		}
	}

	if (s_server != NULL)
	{
		for (const Connection* client : s_server->GetClients())
		{
			if (client->m_lastPing.getElapsedTime().asMilliseconds() >= CONNECTION_TIMEOUT)
			{
				l_list.push_back(client);
			}
		}
	}

	return l_list;
}


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
bool InternalComm::ReadCommand(sf::Packet& a_packet, NetworkData& a_data)
{
	// for now similar to an object
	return ReadObject(a_packet, a_data);
}


////////////////////////////////////////////////////////////
/// \brief Write a command data in a packet for transmition
///
/// \param a_packet the packet in wich we will write
///
/// \param a_data the data we need to write
///
////////////////////////////////////////////////////////////
void InternalComm::WriteCommand(sf::Packet& a_packet, const NetworkData& a_data)
{
	// for now similar to an object
	WriteObject(a_packet, a_data);
}


////////////////////////////////////////////////////////////
/// \brief [Client side] Send a received update from the server to
/// the specified object in the data
///
/// \param a_data the received data
///
/// \param a_forceId if the network id will be overrides
///
////////////////////////////////////////////////////////////
void InternalComm::SendUpdateToObject(NetworkData& a_data, bool a_forceId)
{
	NetworkObject::GetObjectList()[a_data.GetId()]->ReceiveUpdate(a_data, a_forceId);
}


////////////////////////////////////////////////////////////
/// \brief [Server side] Get the list of all new objects that 
/// have not been handle yet
///
/// \return the list of unhandled new objects
///
////////////////////////////////////////////////////////////
std::vector<NetworkObject*>& InternalComm::GetNewObjects()
{
	return s_newObjects;
}


////////////////////////////////////////////////////////////
/// \brief [Server side] indicate that the server has finished
/// to syncronized new objects for all clients
///
////////////////////////////////////////////////////////////
void InternalComm::NewObjectsWasHandled()
{
	s_newObjects.clear();
}


////////////////////////////////////////////////////////////
/// \brief [Client side] Send a received command from the server to
/// the specified object in the command
///
/// \param a_command the received command
///
////////////////////////////////////////////////////////////
void InternalComm::SendCommandToObject(Command& a_command)
{
	NetworkObject::GetObjectList()[a_command.GetData().GetId()]->ReceiveCommand(a_command);
}

////////////////////////////////////////////////////////////
/// \brief Write an object data in a packet for transmition
/// this uses the Object Protocol (Read Me)
///
/// \param a_packet the packet in wich we will write
///
/// \param a_data the data we need to write
///
////////////////////////////////////////////////////////////
void InternalComm::WriteObject(sf::Packet& a_packet, const NetworkData& a_data)
{
	sf::Uint8 l_nbData = a_data.GetData().size();

	a_packet << a_data.GetId() << l_nbData;

	for (const Data& data : a_data.GetData())
	{
		WriteVariable(a_packet, data);
	}
}

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
bool InternalComm::ReadObject(sf::Packet& a_packet, NetworkData& a_data)
{
	sf::Uint8 l_numberVariable;
	sf::Uint16 l_idObject;

	if (!(a_packet >> l_idObject >> l_numberVariable))
	{
		return false;
	}
	a_data.SetId(l_idObject);

	try
	{
		for (int j = 0; j < l_numberVariable; j++)
		{
			a_data << ReadVariable(a_packet);
		}
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////
/// \brief Write the a variable in a packet, this uses the Variable Protocol (Read Me)
///
/// \param a_packet the packet in wich we will write the variable
///
/// \param a_data the data that we will write
///
////////////////////////////////////////////////////////////
void InternalComm::WriteVariable(sf::Packet& a_packet, const Data& a_data)
{

	// TODO : add support for Color (4*8bits = 32bits)
	// TODO : add support for vector2f (2*32bits) (no support for vector2 as double)
	// TODO : reduce the size of the function (1 line per case)

	a_packet << a_data.m_id << (sf::Uint8)a_data.m_type;

	switch (a_data.m_type)
	{
	case DT_bool:
	{
		const bool* l_bool = static_cast<const bool*>(a_data.m_data);
		a_packet << *l_bool;
		break;
	}
	case DT_float:
	{
		const float* l_float = static_cast<const float*>(a_data.m_data);
		a_packet << *l_float;
		break;
	}
	case DT_double:
	{
		const double* l_double = static_cast<const double*>(a_data.m_data);
		a_packet << *l_double;
		break;
	}
	case DT_string:
	{
		const std::string* l_string = static_cast<const std::string*>(a_data.m_data);
		a_packet << *l_string;
		break;
	}
	case DT_Int32:
	{
		const sf::Int32* l_int32 = static_cast<const sf::Int32*>(a_data.m_data);
		a_packet << *l_int32;
		break;
	}
	case DT_Uint32:
	{
		const sf::Uint32* l_uint32 = static_cast<const sf::Uint32*>(a_data.m_data);;
		a_packet << *l_uint32;
		break;
	}
	case DT_Uint8:
	{
		const sf::Uint8* l_uint8 = static_cast<const sf::Uint8*>(a_data.m_data);
		a_packet << *l_uint8;
		break;
	}

	default:
		throw NetworkException("Error : Bad type while reading!");
		break;
	}

}

////////////////////////////////////////////////////////////
/// \brief Read the next variable in a packet, this uses the Variable Protocol (Read Me)
///
/// \param a_packet the packet in wich we will read the variable
///
/// \return the data that contains the variable
///
////////////////////////////////////////////////////////////
Data InternalComm::ReadVariable(sf::Packet& a_packet)
{
	sf::Uint8 l_variableId;
	sf::Uint8 l_variableType;

	a_packet >> l_variableId >> l_variableType;

	DataType l_castedType = (DataType)l_variableType; // work without this line, but cleaner

	switch (l_castedType)
	{
	case DT_bool:
	{
		bool l_bool;
		a_packet >> l_bool;
		return Data(l_variableId, l_bool, true);
	}
	case DT_float:
	{
		float l_float;
		a_packet >> l_float;
		return Data(l_variableId, l_float, true);
	}
	case DT_double:
	{
		double l_double;
		a_packet >> l_double;
		return Data(l_variableId, l_double, true);
	}
	case DT_string:
	{
		std::string l_string;
		a_packet >> l_string;
		return Data(l_variableId, l_string, true);
	}
	case DT_Int32:
	{
		sf::Int32 l_int32;
		a_packet >> l_int32;
		return Data(l_variableId, l_int32, true);
	}
	case DT_Uint32:
	{
		sf::Uint32 l_uint32;
		a_packet >> l_uint32;
		return Data(l_variableId, l_uint32, true);
	}
	case DT_Uint8:
	{
		sf::Uint8 l_uint8;
		a_packet >> l_uint8;
		return Data(l_variableId, l_uint8, true);
	}

	default:
		throw NetworkException("Error : Bad type while reading!");
		break;
	}
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
Client* InternalComm::StartClient(const std::string& a_clientName)
{
	s_client = new Client(a_clientName);
	return s_client;
}


////////////////////////////////////////////////////////////
/// \brief stop all communication threads (server, client and update)
///
/// Use it at the end of the application
///
////////////////////////////////////////////////////////////
void InternalComm::ShutDownAllCommunications()
{
	UdpHandler::StopUdpSharingSystem();

	StopUpdateThread();
	CloseServer();
	CloseClient();

	for (InfoServer* server : s_availableServers)
		delete server;
}


////////////////////////////////////////////////////////////
/// \brief stop the thread that update network objetcs
///
////////////////////////////////////////////////////////////
void InternalComm::StopUpdateThread()
{
	if (NetworkObject::StopUpdateThread())
		s_updateThread.join(); // should be ended soon
}

////////////////////////////////////////////////////////////
/// \brief close the server if any 
///
////////////////////////////////////////////////////////////
void InternalComm::CloseServer()
{
	if (s_server != NULL)
		delete s_server;
}

////////////////////////////////////////////////////////////
/// \brief close the client if any
///
////////////////////////////////////////////////////////////
void InternalComm::CloseClient()
{
	if (s_client != NULL)
		delete s_client;
}

}