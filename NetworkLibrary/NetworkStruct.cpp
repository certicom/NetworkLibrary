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


#include"NetworkStruct.h"

namespace Net
{
	NetworkStruct::~NetworkStruct()
	{

	}


	////////////////////////////////////////////////////////////
	/// \brief The function that internally handle update, it will
	/// automatically change the value of some variables
	///
	/// \param a_data the received data to update the object
	///
	////////////////////////////////////////////////////////////
	void NetworkStruct::ReceiveUpdate(NetworkData& a_data)
	{

	}

	////////////////////////////////////////////////////////////
	/// \brief [Server side] consider this object up to date to avoid
	/// resending the same data
	///
	////////////////////////////////////////////////////////////
	void NetworkStruct::ConsiderUpToDate()
	{

	}

	////////////////////////////////////////////////////////////
	/// \brief This add a struture in the list of syncronizable
	/// structure
	///
	/// \param a structure this is a void*, but it will be cast into
	/// NetworkStruct*
	///
	////////////////////////////////////////////////////////////
	void NetworkStruct::AddStructure(void* a_structure)
	{
		m_structs.push_back(static_cast<NetworkStruct*>(a_structure));
	}


}