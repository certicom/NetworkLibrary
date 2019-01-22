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
#include "NetworkObject.h"

namespace Net
{

std::map<sf::Uint16, NetworkObject*> NetworkObject::s_networkObjectsList; ///< The list of all network object currently instanciated

bool NetworkObject::s_threadIsRunning = false; ///< Flag to know if the update thread is running

int NetworkObject::s_currentId = 0; ///< The last Id given to an object, this incremental member to ensure that all object will receive a unique id


////////////////////////////////////////////////////////////
/// \brief start the update thread
///
/// \param a_thread the thread wich will be used
///
////////////////////////////////////////////////////////////
void NetworkObject::StartUpdateThread(std::thread& a_thread)
{
	if (!s_threadIsRunning)
	{
		s_threadIsRunning = true;
		a_thread = std::thread(ThreadForUpdate);
	}
}


////////////////////////////////////////////////////////////
/// \brief stop the update thread
///
/// \return if the thread was running and if we must wait for the thread to end
///
////////////////////////////////////////////////////////////
bool NetworkObject::StopUpdateThread()
{
	if (s_threadIsRunning)
	{
		s_threadIsRunning = false;
		return true;
	}
	return false;
}



////////////////////////////////////////////////////////////
/// \brief [Server side] function for threading the update of the objects
///
////////////////////////////////////////////////////////////
void NetworkObject::ThreadForUpdate()
{
	while (s_threadIsRunning)
	{
		// TODO : update objects


		Sleep(16); // 16 for update
	}
}


////////////////////////////////////////////////////////////
/// \brief set the frequency and the priority at which 
/// the object will be updated
///
/// \param a_priority the communication priority
///
////////////////////////////////////////////////////////////
void NetworkObject::SetCommunicationPriority(CommunicationPriority a_priority)
{
	m_priority = a_priority;
}

////////////////////////////////////////////////////////////
/// \brief destructor
///
////////////////////////////////////////////////////////////
NetworkObject::~NetworkObject()
{
	s_networkObjectsList.erase(m_networkId);

	delete m_lastSyncronizedData;
}

////////////////////////////////////////////////////////////
/// \brief get the list of all existing network objects
///
/// \return the list of all existing network objects
///
////////////////////////////////////////////////////////////
std::map<sf::Uint16, NetworkObject*>& NetworkObject::GetObjectList()
{
	return s_networkObjectsList;
}


////////////////////////////////////////////////////////////
/// \brief set the polymorphic type name of this object
///
/// \param a_name the type name
///
////////////////////////////////////////////////////////////
void NetworkObject::SetTypeName(const std::string& a_name)
{
	m_instanciateTypeName = a_name;
}


////////////////////////////////////////////////////////////
/// \brief get the polymorphic type name of this object
///
/// \return the type name of the parent object
///
////////////////////////////////////////////////////////////
const std::string& NetworkObject::GetTypeName() const
{
	return m_instanciateTypeName;
}


////////////////////////////////////////////////////////////
/// \brief This add a struture in the list of syncronizable
/// structure
///
/// \param a structure this is a void*, but it will be cast into
/// NetworkStruct*
///
////////////////////////////////////////////////////////////
void NetworkObject::AddStructure(void* a_structure)
{
	m_structs.push_back(static_cast<NetworkStruct*>(a_structure));
}


////////////////////////////////////////////////////////////
/// \brief get the list of all data that must be kept syncronized
///
/// \return the list of all syncronizable data
///
////////////////////////////////////////////////////////////
const NetworkData& NetworkObject::GetSyncronizableData() const
{
	return m_syncronizedData;
}


////////////////////////////////////////////////////////////
/// \brief [Server side] consider this object up to date to avoid
/// resending the same data
///
////////////////////////////////////////////////////////////
void NetworkObject::ConsiderUpToDate()
{
	delete m_lastSyncronizedData;

	m_lastSyncronizedData = new NetworkData();

	for (const Data& data : m_syncronizedData.GetData())
	{
		m_lastSyncronizedData->operator<<(data.CopyValueData()); // remember only the current value
	}

}


////////////////////////////////////////////////////////////
/// \brief The function that internally handle update, it will
/// automatically change the value of some variables
///
/// \param a_data the received data to update the object
///
/// \param a_forceId if we override the network id 
///
////////////////////////////////////////////////////////////
void NetworkObject::ReceiveUpdate(NetworkData& a_data, bool a_forceId)
{

	if (a_forceId) //  replace the id
	{
		s_networkObjectsList.erase(m_networkId);  // remove

		m_networkId = a_data.GetId();             // change

		s_networkObjectsList[m_networkId] = this; // replace
	}

	// quadratic complexity : TODO Use map for NetworkData instead of vector
	for (const Data& newData : a_data.GetData())
	{
		for (Data& oldData : m_syncronizedData.GetAlterableData())
		{
			if (newData.m_id == oldData.m_id)
			{
				oldData.OverrideData(newData);
				break;
			}
		}
	}
	
	WasUpdated(); // call back for user processing
}



////////////////////////////////////////////////////////////
/// \brief [Client side] Send a command to the server
///
/// \param a_codeCommand the custom code of this command
///
/// \param a_data the main data of the command 
///
////////////////////////////////////////////////////////////
void NetworkObject::SendCommand(int a_codeCommand, NetworkData& a_data)
{
	InternalComm::SendCommand(this, a_codeCommand, a_data);
}


////////////////////////////////////////////////////////////
/// \brief callback when the object receive an update. The user
/// can override this function if he wants
///
////////////////////////////////////////////////////////////
void NetworkObject::WasUpdated()
{

}

////////////////////////////////////////////////////////////
/// \brief get the network id of this object
///
/// \return the network id
///
////////////////////////////////////////////////////////////
int NetworkObject::GetId() const
{
	return m_networkId;
}


////////////////////////////////////////////////////////////
/// \brief [Server side] Get a unique id to attribute to a new object
///
/// From client side, the returned value and the unique id will always
/// be override by the server
///
/// \return a unique id
///
////////////////////////////////////////////////////////////
int NetworkObject::GetUniqueId()
{
	s_currentId++; // for now, simple way to give ids

	return s_currentId;
}

}