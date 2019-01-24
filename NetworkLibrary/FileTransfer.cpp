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
#include "FileTransfer.h"

namespace Net
{

////////////////////////////////////////////////////////////
/// \brief Start a new file transfert
///
/// \param a_fileName The name of the file to send
///
/// \param a_senderEntity The client that send the file
///
////////////////////////////////////////////////////////////
FileTransfer::FileTransfer(const std::string& a_fileName, Client* a_senderEntity) : m_fileName(a_fileName)
{
	m_client = a_senderEntity;
	m_server = NULL;
	m_receiver = NULL;

	Init();

	m_thread = std::thread(TransfertThread, this);
}

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
FileTransfer::FileTransfer(const std::string& a_fileName, Server* a_senderEntity, Connection* a_receiver) : m_fileName(a_fileName)
{
	m_client = NULL;
	m_server = a_senderEntity;
	m_receiver = a_receiver;

	Init();

	m_thread = std::thread(TransfertThread, this);
}


////////////////////////////////////////////////////////////
/// \brief This constructor is for receiving file
///
/// \param a_fileName The name of the file to send
///
/// \param a_fileSize The expetected size of the file
///
////////////////////////////////////////////////////////////
FileTransfer::FileTransfer(const std::string& a_fileName, int a_fileSize) : m_fileName(a_fileName)
{
	m_client = NULL;
	m_server = NULL;
	m_receiver = NULL;

	Init();

	m_totalBits = a_fileSize;

	m_file = std::ofstream(m_fileName, std::ios::out | std::ios::binary);

	if (!m_file)
		m_hasFailed = true;
}

////////////////////////////////////////////////////////////
/// \brief init commun variables for file transfert
///
////////////////////////////////////////////////////////////
void FileTransfer::Init()
{
	m_completion = 0.0f;
	m_hasFailed = false;
	m_isComplete = false;
	m_sendedBits = 0;
	m_totalBits = 0;
}

////////////////////////////////////////////////////////////
/// \brief Destructor of the file transfert, it end the thread
///
////////////////////////////////////////////////////////////
FileTransfer::~FileTransfer()
{
	StopTransfert();
}



////////////////////////////////////////////////////////////
/// \brief Must be called when this transfert receive a
/// new packet full of data to add to the file
///
/// \param a_packet the packet full of data
///
////////////////////////////////////////////////////////////
void FileTransfer::ReceivePacket(sf::Packet a_packet)
{
	if (!m_hasFailed)
	{
		bool l_hasFailed;

		a_packet >> l_hasFailed;

		if (l_hasFailed)
		{
			m_hasFailed = true;
			m_file.close();
		}

		int l_count = 0;
		sf::Int8 l_char;
		while (a_packet >> l_char)
		{
			l_count++;
			if (l_count % 4 == 0) // TODO : understand why we receive 32bits data instead of 8
			{
				m_file.put(l_char);
			}
		}

		m_sendedBits = m_file.tellp();

		m_completion = ((float)m_sendedBits / m_totalBits) * 100.0f;

		if (m_sendedBits == m_totalBits)
		{
			m_isComplete = true;
			m_file.close();
		}
	}
}

////////////////////////////////////////////////////////////
/// \brief Ask to the correct entity to send a packet
///
/// \param a_packet the packet to send
///
/// \param a_receiver The receiver, if NULL send it to everyone
///
////////////////////////////////////////////////////////////
void FileTransfer::SendPacket(sf::Packet& a_packet, Connection* a_receiver)
{
	if (m_server != NULL)
	{
		m_server->SendPartialFile(a_packet, a_receiver);
	}
	else if(m_client != NULL) // or just else ?
	{
		m_client->SendPartialFile(a_packet);
	}
}

////////////////////////////////////////////////////////////
/// \brief The main function of the thread for transfering files
///
////////////////////////////////////////////////////////////
void FileTransfer::TransfertThread(FileTransfer* a_transfert)
{
	a_transfert->m_isTransfering = true;

	std::ifstream l_file(a_transfert->m_fileName, std::ios::in | std::ios::binary);

	l_file.seekg(0, std::ios::end);  // go to the end
	sf::Uint32 l_fileSize = l_file.tellg(); // get the current position
	l_file.seekg(0, std::ios::beg);  // go back to the begining

	if (l_file) // open the file
	{
		char l_char;

		sf::Packet l_packet;

		int l_currentPacketSize = 0;

		// send a fisrt packet with just the name of the file
		l_packet << (sf::Uint16)CT_File << true << a_transfert->m_fileName << l_fileSize << GetExecutablePath();
		a_transfert->SendPacket(l_packet, a_transfert->m_receiver);

		while (l_file.get(l_char) && a_transfert->m_isTransfering) // while there is still a character in the file
		{
			if (l_currentPacketSize == 0) // init the packet
			{
				l_packet = sf::Packet();

				l_packet << (sf::Uint16)CT_File << false << a_transfert->m_fileName << false;
			}
			l_packet << l_char; // add the char in the packet

			l_currentPacketSize++; // imcremente the size

			if (l_currentPacketSize >= 8192) // if we have reach the maximum size TODO : what is the best size ?? 2^13 seems nice
			{
				a_transfert->SendPacket(l_packet, a_transfert->m_receiver); // send the packet

				a_transfert->m_completion = ((float)l_file.tellg() / l_fileSize) * 100.0f;

				l_currentPacketSize = 0; // reset the packet
			}
		}

		if (l_currentPacketSize != 0 && a_transfert->m_isTransfering) // handle a non complete packet
		{
			a_transfert->SendPacket(l_packet, a_transfert->m_receiver);
			a_transfert->m_completion = 100.0f;
		}
			

		l_file.close();

		a_transfert->m_isComplete = true;
	}

	else
	{
		a_transfert->m_hasFailed = true;
	}

	if (a_transfert->m_hasFailed) // transfert can also failed if the transfert was manually stoped
	{
		// we send a packet that will cause the reading to fail and allow the transfert to be canceled at the other side
		sf::Packet l_packet;
		l_packet = sf::Packet();
		l_packet << (sf::Uint16)CT_File << false << a_transfert->m_fileName << true;
	}

	a_transfert->m_isTransfering = false;
}


////////////////////////////////////////////////////////////
/// \brief Stop the file transfert
///
////////////////////////////////////////////////////////////
void FileTransfer::StopTransfert()
{
	if (!m_isComplete)
	{
		m_hasFailed = true;

		if (m_isTransfering)
		{
			m_isTransfering = false;

			m_thread.join();
		}
	}

	if (m_file)
		m_file.close();

}


////////////////////////////////////////////////////////////
/// \brief Get the global path of the .exe
///
/// \return the global path of the application
///
////////////////////////////////////////////////////////////
const std::string FileTransfer::GetExecutablePath()
{
	char  szPath[MAX_PATH];

	// Retrieve the full path for the current module.
	if (GetModuleFileName(NULL, szPath, sizeof szPath) == 0)
	{
		return "";
	}

	std::string str(szPath);

	str.resize(str.find_last_of('\\'));

	return str;
}





////////////////////////////////////////////////////////////
/// \brief Get if the transfert is complete and was sucessfull
///
/// \return if the transfert is complete
///
////////////////////////////////////////////////////////////
bool FileTransfer::IsComplete() const
{
	return m_isComplete;
}

////////////////////////////////////////////////////////////
/// \brief Get if the transfert has failed
///
/// \return if the transfert has failed
///
////////////////////////////////////////////////////////////
bool FileTransfer::HasFailed() const
{
	return m_hasFailed;
}


////////////////////////////////////////////////////////////
/// \brief Get the current completion of the transfert
///
/// \return the pourcentage of transfert completion
///
////////////////////////////////////////////////////////////
float FileTransfer::Completion() const
{
	return m_completion;
}

}