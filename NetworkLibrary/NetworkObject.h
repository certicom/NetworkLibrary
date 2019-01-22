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

//#include "NetworkEnums.h" // (included in network data)
//#include "Data.h" // (included in network data)
#include "Command.h"
#include "NetworkData.h"
#include "InternalComm.h"
#include "NetworkStruct.h"

namespace Net
{


////////////////////////////////////////////////////////////
/// \brief Absctract class. Inherit from this class allow objects
/// to be automatically syncronized with the server and all clients
///
////////////////////////////////////////////////////////////
class NET NetworkObject
{

public:

	////////////////////////////////////////////////////////////
	/// \brief constructor, also the end of recursion constructor
	/// from the variadic constructor
	///
	/// TODO : put this in protected and in cpp (at least the link is badly resolve with the variadic function)
	///
	////////////////////////////////////////////////////////////
	NetworkObject() : m_networkId(GetUniqueId())
	{
		if (!InternalComm::IsInstanciable()) //Use safety, end users are not supposed to instanciate manually a network object
			throw NetworkException("Network objects can only be instanciate with 'SpawnObjectFromServer' or 'InstanciateType'!");

		s_networkObjectsList[m_networkId] = this;

		m_priority = CP_Normal;

		m_syncronizedData.SetId(m_networkId);

		m_lastSyncronizedData = new NetworkData();

		m_instanciateTypeName = "";
	}

	////////////////////////////////////////////////////////////
	/// \brief destructor
	///
	////////////////////////////////////////////////////////////
	~NetworkObject();

	////////////////////////////////////////////////////////////
	/// \brief Must be overrided, must handle the reception of commands
	/// that comes from this object, but may be from a different client 
	///
	/// \param a_command the command to interpret
	///
	////////////////////////////////////////////////////////////
	virtual void ReceiveCommand(Command& a_command) = 0;

	////////////////////////////////////////////////////////////
	/// \brief callback when the object receive an update. The user
	/// can override this function if he wants
	///
	////////////////////////////////////////////////////////////
	virtual void WasUpdated();

	////////////////////////////////////////////////////////////
	/// \brief The function that internally handle update, it will
	/// automatically change the value of some variables
	///
	/// \param a_data the received data to update the object
	///
	/// \param a_forceId if we override the network id 
	///
	////////////////////////////////////////////////////////////
	void ReceiveUpdate(NetworkData& a_data, bool a_forceId);

	////////////////////////////////////////////////////////////
	/// \brief get the network id of this object
	///
	/// \return the network id
	///
	////////////////////////////////////////////////////////////
	int GetId() const;

	////////////////////////////////////////////////////////////
	/// \brief get the list of all data that must be kept syncronized
	///
	/// \return the list of all syncronizable data
	///
	////////////////////////////////////////////////////////////
	const NetworkData& GetSyncronizableData() const;

	////////////////////////////////////////////////////////////
	/// \brief [Server side] consider this object up to date to avoid
	/// resending the same data
	///
	////////////////////////////////////////////////////////////
	void ConsiderUpToDate();

	////////////////////////////////////////////////////////////
	/// \brief get the polymorphic type name of this object
	///
	/// \return the type name of the parent object
	///
	////////////////////////////////////////////////////////////
	const std::string& GetTypeName() const;

	////////////////////////////////////////////////////////////
	/// \brief set the polymorphic type name of this object
	///
	/// \param a_name the type name
	///
	////////////////////////////////////////////////////////////
	void SetTypeName(const std::string& a_name);


	////////////////////////////////////////////////////////////
	/// \brief get the list of all existing network objects
	///
	/// \return the list of all existing network objects
	///
	////////////////////////////////////////////////////////////
	static std::map<sf::Uint16, NetworkObject*>& GetObjectList();

	////////////////////////////////////////////////////////////
	/// \brief start the update thread
	///
	/// \param a_thread the thread wich will be used
	///
	////////////////////////////////////////////////////////////
	static void StartUpdateThread(std::thread& a_thread);

	////////////////////////////////////////////////////////////
	/// \brief stop the update thread
	///
	/// \return if the thread was running and if we must wait for the thread to end
	///
	////////////////////////////////////////////////////////////
	static bool StopUpdateThread();

protected:


	////////////////////////////////////////////////////////////
	/// \brief Variatic constructor
	///
	/// TODO : understand why I can not write T& t instead of T* t
	///
	/// \param t the current variable
	///
	/// \param rest the rest of the variable in the parameter pack
	///
	/// \tparam T a network compatible type of variable
	///
	/// \tparam Args as parameter package
	///
	////////////////////////////////////////////////////////////
	template <class T, class... Args>
	NetworkObject(T* t, Args... rest) : NetworkObject(rest...)
	{
		if (typeid(T).hash_code() == typeid(NetworkStruct).hash_code() )
		{
			AddStructure(t);
		}
		else
		{
			Data l_data = Data(m_syncronizedData.GetData().size(), *t);

			m_syncronizedData << l_data;

			m_lastSyncronizedData->operator<<(l_data.CopyValueData()); // remember only the value
		}
	}


	////////////////////////////////////////////////////////////
	/// \brief set the frequency and the priority at which 
	/// the object will be updated
	///
	/// \param a_priority the communication priority
	///
	////////////////////////////////////////////////////////////
	void SetCommunicationPriority(CommunicationPriority a_priority);

	////////////////////////////////////////////////////////////
	/// \brief [Client side] Send a command to the server
	///
	/// \param a_codeCommand the custom code of this command
	///
	/// \param a_data the main data of the command 
	///
	////////////////////////////////////////////////////////////
	void SendCommand(int a_codeCommand, NetworkData& a_data);

private:

	////////////////////////////////////////////////////////////
	/// \brief This add a struture in the list of syncronizable
	/// structure
	///
	/// \param a structure this is a void*, but it will be cast into
	/// NetworkStruct*
	///
	////////////////////////////////////////////////////////////
	void AddStructure(void* a_structure);

	////////////////////////////////////////////////////////////
	/// \brief [Server side] Get a unique id to attribute to a new object
	///
	/// From client side, the returned value and the unique id will always
	/// be override by the server
	///
	/// \return a unique id
	///
	////////////////////////////////////////////////////////////
	static int GetUniqueId();


	////////////////////////////////////////////////////////////
	/// \brief [Server side] function for threading the update of the objects
	///
	////////////////////////////////////////////////////////////
	static void ThreadForUpdate();


	////////////////////////////////////////////////////////////
	// Satic member data
	////////////////////////////////////////////////////////////

	static std::map<sf::Uint16, NetworkObject*> s_networkObjectsList; ///< The list of all network object currently instanciated

	static bool s_threadIsRunning; ///< Flag to know if the update thread is running

	static int s_currentId; ///< The last Id given to an object, this incremental member to ensure that all object will receive a unique id


	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////

	int m_networkId; ///< The syncronized unique id of the object (Given by the server, if not it will be oautomatically overrided)

	CommunicationPriority m_priority; ///< The frequency/priority where the data will be updated

	NetworkData m_syncronizedData; ///< The list of all the data that must be syncronized on this object

	NetworkData* m_lastSyncronizedData; ///< The list that contains the value of the syncronizable variables of the last update

	std::vector<NetworkStruct*> m_structs; ///< The list of all syncronizable sub structure of the object

	std::string m_instanciateTypeName; ///< The polymorphic type name of this object


};

}