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

#include "Data.h"

namespace Net
{


////////////////////////////////////////////////////////////
/// \brief This basically represent a serialized object that
/// can be sent on the network
///
/// InternalComm is the class the handle the de/serialization,
/// but the user must also used it, since this is also used in
/// command communication
///
////////////////////////////////////////////////////////////
class NetworkData
{

public:

	////////////////////////////////////////////////////////////
	/// \brief constructor
	///
	////////////////////////////////////////////////////////////
	NetworkData();

	////////////////////////////////////////////////////////////
	/// \brief Get the list of all the variables in this package
	///
	/// \return the list of all the data
	///
	////////////////////////////////////////////////////////////
	const std::vector<Data>& GetData() const;

	////////////////////////////////////////////////////////////
	/// \brief Get the list of all the variables in this package
	/// those data are alterable, only use when necessary
	///
	/// \return the non constant list of all the data
	///
	////////////////////////////////////////////////////////////
	std::vector<Data>& GetAlterableData();

	////////////////////////////////////////////////////////////
	/// \brief get the id of the network object targeted by this NetworkData
	///
	/// \return The id stored in this NetworkData
	///
	////////////////////////////////////////////////////////////
	sf::Uint16 GetId() const;

	////////////////////////////////////////////////////////////
	/// \brief set the id of the network object targeted by this NetworkData
	///
	/// \param a_id The id of the concerned NetworkObject
	///
	////////////////////////////////////////////////////////////
	void SetId(sf::Uint16 a_id);

	////////////////////////////////////////////////////////////
	/// \brief equivalent to an add function, it add a data in this package
	///
	/// \param a_data the data to add
	///
	////////////////////////////////////////////////////////////
	void operator << (Data& a_data);

	////////////////////////////////////////////////////////////
	/// \brief This is a function for end user uses. it give 
	/// back the data with the request id
	///
	/// \param a_id the id of the variable you want to get
	///
	/// \tparam T type of the data you want to get
	///
	////////////////////////////////////////////////////////////
	template<class T>
	T GetDataFromId(sf::Uint8 a_id) const
	{
		// TODO : this is too costly, find an other way (map ?)
		for (const Data& data : m_data)
		{
			if (data.m_id == a_id)
			{
				return data.GetTypedData<T>();
			}
		}
		throw NetworkException("Id in network data not fund !");
	}

private:

	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////

	sf::Uint16 m_id; ///< Store the id of the concerned network object by this package

	// TODO : use a map with data id as key
	std::vector<Data> m_data; ///< The list of all syncronizable variables
};

}