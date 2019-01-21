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

#include "NetworkData.h"

namespace Net
{

////////////////////////////////////////////////////////////
/// \brief This is an abstract class, inherit from it to
/// add a syncronizable sub structure to an NetworkObject
///
////////////////////////////////////////////////////////////
class NetworkStruct
{
public:

	~NetworkStruct();


	////////////////////////////////////////////////////////////
	/// \brief The function that internally handle update, it will
	/// automatically change the value of some variables
	///
	/// \param a_data the received data to update the object
	///
	////////////////////////////////////////////////////////////
	void ReceiveUpdate(NetworkData& a_data);

	////////////////////////////////////////////////////////////
	/// \brief [Server side] consider this object up to date to avoid
	/// resending the same data
	///
	////////////////////////////////////////////////////////////
	void ConsiderUpToDate();

protected:


	////////////////////////////////////////////////////////////
	/// \brief constructor, also the end of recursion constructor
	/// from the variadic constructor
	///
	////////////////////////////////////////////////////////////
	NetworkStruct()
	{
		m_lastSyncronizedData = new NetworkData();
	}

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
	NetworkStruct(T* t, Args... rest) : NetworkStruct(rest...)
	{
		if (typeid(T).hash_code() == typeid(NetworkStruct))
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
	// Member data
	////////////////////////////////////////////////////////////

	NetworkData m_syncronizedData; ///< The list of all the data that must be syncronized on this object

	NetworkData* m_lastSyncronizedData; ///< The list that contains the value of the syncronizable variables of the last update

	std::vector<NetworkStruct*> m_structs; ///< The list of all syncronizable sub structure of the object
};

}