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


#include "Command.h"

namespace Net
{


////////////////////////////////////////////////////////////
/// \brief Constructor
///
/// \param a_commandCode the code that the user give to this command
///
/// \param a_data the data of the command
///
////////////////////////////////////////////////////////////
Command::Command(sf::Uint16 a_commandCode, NetworkData& a_data)
{
	m_commandCode = a_commandCode;

	m_data = a_data;

	m_isPending = true;
}

////////////////////////////////////////////////////////////
/// \brief [Server side] Reject the command, so it will not
/// be resent to all client
///
////////////////////////////////////////////////////////////
void Command::Reject()
{
	if (m_isPending)
	{
		m_isPending = false;
		m_isAcceptd = false;
	}
}


////////////////////////////////////////////////////////////
/// \brief [Server side] Accept the command, so it will
/// be resent to all client
///
////////////////////////////////////////////////////////////
void Command::Accept()
{
	if (m_isPending)
	{
		m_isPending = false;
		m_isAcceptd = true;
	}
}



////////////////////////////////////////////////////////////
/// \brief get if a decision was made for this command
///
/// \return the handle flag of the command
///
////////////////////////////////////////////////////////////
bool Command::IsHandled()
{
	return !m_isPending; // if it iss pending then it was not handled
}

////////////////////////////////////////////////////////////
/// \brief Get the command code
///
/// \return the command code
///
////////////////////////////////////////////////////////////
sf::Uint16 Command::GetCommandCode()
{
	return m_commandCode;
}


////////////////////////////////////////////////////////////
/// \brief Get the data as NetworkData
///
/// \return The main data of the command
///
////////////////////////////////////////////////////////////
NetworkData& Command::GetData()
{
	return m_data;
}


////////////////////////////////////////////////////////////
/// \brief get if the command was validated
///
/// \return the validation flag of the command
///
////////////////////////////////////////////////////////////
bool Command::IsValidate()
{
	return (!m_isPending && m_isAcceptd);
}


}
