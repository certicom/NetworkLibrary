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
#include "NetworkData.h"


namespace Net
{


////////////////////////////////////////////////////////////
/// \brief Small class to handle the reception of command. The 
/// game core must accept or reject those commands
///
/// Also allow to simply retreive data from the original command
///
////////////////////////////////////////////////////////////
class NET Command
{
public:

	////////////////////////////////////////////////////////////
	/// \brief Constructor
	///
	/// \param a_commandCode the code that the user give to this command
	///
	/// \param a_data the data of the command
	///
	////////////////////////////////////////////////////////////
	Command(sf::Uint16 a_commandCode, NetworkData& a_data);

	////////////////////////////////////////////////////////////
	/// \brief [Server side] Reject the command, so it will not
	/// be resent to all client
	///
	////////////////////////////////////////////////////////////
	void Reject();

	////////////////////////////////////////////////////////////
	/// \brief [Server side] Accept the command, so it will
	/// be resent to all client
	///
	////////////////////////////////////////////////////////////
	void Accept();

	////////////////////////////////////////////////////////////
	/// \brief Get the command code
	///
	/// \return the command code
	///
	////////////////////////////////////////////////////////////
	sf::Uint16 GetCommandCode();

	////////////////////////////////////////////////////////////
	/// \brief Get the data as NetworkData
	///
	/// \return The main data of the command
	///
	////////////////////////////////////////////////////////////
	NetworkData& GetData();

	////////////////////////////////////////////////////////////
	/// \brief get if the command was validated
	///
	/// \return the validation flag of the command
	///
	////////////////////////////////////////////////////////////
	bool IsValidate();

	////////////////////////////////////////////////////////////
	/// \brief get if a decision was made for this command
	///
	/// \return the handle flag of the command
	///
	////////////////////////////////////////////////////////////
	bool IsHandled();


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
		return m_data.GetDataFromId<T>(a_id);
	}

private:


	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////

	sf::Uint16 m_commandCode; ///< The custom command code send by the user to indicate what he want
	
	NetworkData m_data; ///< The main data of the command

	bool m_isPending; ///< Flag to know if a decision was made for this command

	bool m_isAcceptd; ///< Flag to know if the command was accepted (doesn't make sens while m_isPending is not true)

};

}