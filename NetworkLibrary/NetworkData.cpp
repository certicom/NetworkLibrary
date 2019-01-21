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


#include "NetworkData.h"

namespace Net
{

////////////////////////////////////////////////////////////
/// \brief constructor
///
////////////////////////////////////////////////////////////
NetworkData::NetworkData()
{

}


////////////////////////////////////////////////////////////
/// \brief equivalent to an add function, it add a data in this package
///
/// \param a_data the data to add
///
////////////////////////////////////////////////////////////
void NetworkData::operator << (Data& a_data)
{
	m_data.push_back(a_data);
}


////////////////////////////////////////////////////////////
/// \brief get the id of the network object targeted by this NetworkData
///
/// \return The id stored in this NetworkData
///
////////////////////////////////////////////////////////////
sf::Uint16 NetworkData::GetId() const
{
	return m_id;
}

////////////////////////////////////////////////////////////
/// \brief set the id of the network object targeted by this NetworkData
///
/// \param a_id The id of the concerned NetworkObject
///
////////////////////////////////////////////////////////////
void NetworkData::SetId(sf::Uint16 a_id)
{
	m_id = a_id;
}

////////////////////////////////////////////////////////////
/// \brief Get the list of all the variables in this package
///
/// \return the list of all the data
///
////////////////////////////////////////////////////////////
const std::vector<Data>& NetworkData::GetData() const
{
	return m_data;
}


////////////////////////////////////////////////////////////
/// \brief Get the list of all the variables in this package
/// those data are alterable, only use when necessary
///
/// \return the non constant list of all the data
///
////////////////////////////////////////////////////////////
std::vector<Data>& NetworkData::GetAlterableData()
{
	return m_data;
}

}
