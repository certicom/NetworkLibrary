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
#include "UdpHandler.h"

namespace Net
{

bool UdpHandler::s_isThreadRunning; ///< Flag to know if the master thread is running on this handler

std::thread UdpHandler::s_thread; ///< The master thread (only one per machine)

	
////////////////////////////////////////////////////////////
/// \brief The constructor, it will determine if we are master
/// are slave as well as the port to use. If we are master
/// it will start a listener thread
///
////////////////////////////////////////////////////////////
UdpHandler::UdpHandler()
{

	if (m_UdpSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done) // we start by finding an available port TODO : stop picking any random port !
	{
		throw NetworkException("Error : fail to bind udp socket!");
	}

	m_port = m_UdpSocket.getLocalPort();

	std::cout << "Application port : " << m_port << std::endl;

	// We try to 'take control' of the master port
	if (m_UdpSocketForBroadcast.bind(BROADCAST_PORT) != sf::Socket::Done)
	{
		std::cout << "Slave" << std::endl;
		// fail to be master
		m_isMaster = false;

		sf::Packet l_packet; 

		l_packet << "test"; // TODO : still work after removing that (message content do not matter)?

		m_UdpSocket.send(l_packet, sf::IpAddress::LocalHost, MASTER_PORT); // say to the master that i'm a new slave
	}
	else
	{
		std::cout << "Master" << std::endl;

		// if we go the master position, we now reserve the port for listening new slaves
		if (m_UdpSocketForSlaves.bind(MASTER_PORT) != sf::Socket::Done)
		{
			throw NetworkException("Error : fail to bind udp socket!");
		}

		m_isMaster = true;

		m_slavesPorts.push_back(m_port); // Master is also considered as its own slave for less lines

		s_isThreadRunning = true;
		s_thread = std::thread(UdpHandler::UdpThread, this); // run the thread 
	}
}

////////////////////////////////////////////////////////////
/// \brief Get the 'effective' Udp socket. This socket is 
/// supposed to be used like a normal one because everything
/// is done to avoid collision and simulate rerouting of broadcasts
///
/// \return the udp socket to use for communication 
///
////////////////////////////////////////////////////////////
sf::UdpSocket& UdpHandler::GetUdpSocket()
{
	return m_UdpSocket;
}


void UdpHandler::WaitForLock()
{
	m_mutex.lock();
}

void UdpHandler::Unlock()
{
	m_mutex.unlock();
}

////////////////////////////////////////////////////////////
/// \brief Get the port selected among the available ports
/// to make sure we can communicate without collisions
///
/// \return the selected available port used to communicate
/// with this entity
///
////////////////////////////////////////////////////////////
sf::Uint16 UdpHandler::GetUdpPort()
{
	return m_port;
}


////////////////////////////////////////////////////////////
/// \brief Stop the listener thread of the master if it is
/// running on this application
///
////////////////////////////////////////////////////////////
void UdpHandler::StopUdpSharingSystem()
{
	if (s_isThreadRunning)
	{
		s_isThreadRunning = false;
		s_thread.join();
	}
}

////////////////////////////////////////////////////////////
/// \brief The function ran by the listener thread for the master
///
/// \param a_handler is the original handler(master) that 
/// lunched the thread
///
////////////////////////////////////////////////////////////
void UdpHandler::UdpThread(UdpHandler* a_handler)
{
	sf::SocketSelector l_selector;

	l_selector.add(a_handler->m_UdpSocketForBroadcast);
	l_selector.add(a_handler->m_UdpSocketForSlaves);

		
	while (s_isThreadRunning)
	{

		if (l_selector.wait(sf::milliseconds(50))) // 50ms loop to allow the thread to stop quickly
		{
			sf::Packet l_packet;
			sf::IpAddress l_ipAddress;
			unsigned short l_port;

				
			if (l_selector.isReady(a_handler->m_UdpSocketForSlaves))
			{
				a_handler->m_UdpSocketForSlaves.receive(l_packet, l_ipAddress, l_port);

					
				if (l_port != 0) // TODO : why do we receive 0 when connections are down ??
				{
					std::cout << std::endl << "New slave: " << l_port << std::endl;
					a_handler->m_slavesPorts.push_back(l_port); // TODO : no duplicates
				}
			}
				
			if (l_selector.isReady(a_handler->m_UdpSocketForBroadcast))
			{
				a_handler->m_UdpSocketForBroadcast.receive(l_packet, l_ipAddress, l_port);

				// reflect broadcast (broadcasts must contain the original ip and port because of that)
				for (sf::Uint16 port : a_handler->m_slavesPorts)
				{
					a_handler->m_UdpSocketForSlaves.send(l_packet, sf::IpAddress::LocalHost, port);
				}

			}
		}
	}
}

}