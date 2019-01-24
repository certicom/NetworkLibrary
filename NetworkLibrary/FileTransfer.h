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
#include "NetworkEnums.h"


namespace Net
{


	class Client;
	class Server;
	class Connection;

////////////////////////////////////////////////////////////
/// \brief This class is able to handle a file transfert
/// and give to the user a simple way to now the completion
/// of the transfert.
/// This can handle both side of a file transfert
///
////////////////////////////////////////////////////////////
class NET FileTransfer
{
public:

	////////////////////////////////////////////////////////////
	/// \brief Start a new file transfert
	///
	/// \param a_fileName The name of the file to send
	///
	/// \param a_senderEntity The client that send the file
	///
	////////////////////////////////////////////////////////////
	FileTransfer(const std::string& a_fileName, Client* a_senderEntity);

	////////////////////////////////////////////////////////////
	/// \brief Start a new file transfert
	///
	/// \param a_fileName The name of the file to send
	///
	/// \param a_senderEntity The server that send the file
	///
	/// \param a_receiver Optionaly, we can specify a connection
	/// where to send the file. If NULL everyone will receive
	///
	////////////////////////////////////////////////////////////
	FileTransfer(const std::string& a_fileName, Server* a_senderEntity, Connection* a_receiver = NULL);

	////////////////////////////////////////////////////////////
	/// \brief This constructor is for receiving file
	///
	/// \param a_fileName The name of the file to send
	///
	/// \param a_fileSize The expetected size of the file
	///
	////////////////////////////////////////////////////////////
	FileTransfer(const std::string& a_fileName, int a_fileSize);

	////////////////////////////////////////////////////////////
	/// \brief Destructor of the file transfert, it end the thread
	///
	////////////////////////////////////////////////////////////
	~FileTransfer();

	////////////////////////////////////////////////////////////
	/// \brief Stop the file transfert
	///
	////////////////////////////////////////////////////////////
	void StopTransfert();

	////////////////////////////////////////////////////////////
	/// \brief Get if the transfert is complete and was sucessfull
	///
	/// \return if the transfert is complete
	///
	////////////////////////////////////////////////////////////
	bool IsComplete() const;

	////////////////////////////////////////////////////////////
	/// \brief Get if the transfert has failed
	///
	/// \return if the transfert has failed
	///
	////////////////////////////////////////////////////////////
	bool HasFailed() const;


	////////////////////////////////////////////////////////////
	/// \brief Get the current completion of the transfert
	///
	/// \return the pourcentage of transfert completion
	///
	////////////////////////////////////////////////////////////
	float Completion() const;

	////////////////////////////////////////////////////////////
	/// \brief Must be called when this transfert receive a
	/// new packet full of data to add to the file
	///
	/// \param a_packet the packet full of data
	///
	////////////////////////////////////////////////////////////
	void ReceivePacket(sf::Packet a_packet);

	////////////////////////////////////////////////////////////
	/// \brief Get the global path of the .exe
	///
	/// \return the global path of the application
	///
	////////////////////////////////////////////////////////////
	static const std::string GetExecutablePath();

private:

	////////////////////////////////////////////////////////////
	/// \brief The main function of the thread for transfering files
	///
	////////////////////////////////////////////////////////////
	static void TransfertThread(FileTransfer* a_transfert);

	////////////////////////////////////////////////////////////
	/// \brief init commun variables for file transfert
	///
	////////////////////////////////////////////////////////////
	void Init();

	////////////////////////////////////////////////////////////
	/// \brief Ask to the correct entity to send a packet
	///
	/// \param a_packet the packet to send
	///
	/// \param a_receiver The receiver, if NULL send it to everyone
	///
	////////////////////////////////////////////////////////////
	void SendPacket(sf::Packet& a_packet, Connection* a_receiver);


	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////

	bool m_isComplete; ///< More than just m_completion = 100%, it also mean that the thread has ended without error

	bool m_hasFailed; ///< If an error has occured in the transfert

	bool m_isTransfering; ///< If the thread is running

	int m_sendedBits; ///< The currently quantity of data that was sended

	int m_totalBits; ///< The total quantity of data that the transfert must do

	float m_completion; ///< The current pourcentage of completion

	std::thread m_thread; ///< The thread used for transfering as async operation

	const std::string m_fileName; ///< The name of the file to transfer

	Client* m_client; ///< If we use a client to make the file transfert

	Server* m_server; ///< If we use a server to make the file transfert

	Connection* m_receiver; ///< The specific receiver of this transfert, if NULL everyone will receive it

	std::ofstream m_file; ///< If we are in receiving mode, this is the opened file
};

}

#include "Client.h"
#include "Server.h"