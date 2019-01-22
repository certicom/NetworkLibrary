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
#include "Data.h"

namespace Net
{

////////////////////////////////////////////////////////////
/// \brief All the constructors for data of any allowed type
///
/// TODO : template constructor ? in all case a type differentiation is required
///
/// \param a_id the id for this data
///
/// \param a_(type) the data
///
/// \param a_copieValue bit tricky, if true it allow this object to behave like
/// a normal variable, but loose its pointer reference
///
////////////////////////////////////////////////////////////
Data::Data(sf::Uint8 a_id, bool& a_bool, bool a_copieValue) : m_id(a_id), m_type(DT_bool), m_data(&a_bool), m_copied(a_copieValue)
{
	if (a_copieValue)
	{
		bool* l_data = new bool();
		*l_data = a_bool;
		m_data = l_data;
	}
}

Data::Data(sf::Uint8 a_id, float& a_float, bool a_copieValue) : m_id(a_id), m_type(DT_float), m_data(&a_float), m_copied(a_copieValue)
{
	if (a_copieValue)
	{
		float* l_data = new float();
		*l_data = a_float;
		m_data = l_data;
	}
}

Data::Data(sf::Uint8 a_id, double& a_double, bool a_copieValue) : m_id(a_id), m_type(DT_double), m_data(&a_double), m_copied(a_copieValue)
{
	if (a_copieValue)
	{
		double* l_data = new double();
		*l_data = a_double;
		m_data = l_data;
	}
}

Data::Data(sf::Uint8 a_id, std::string& a_string, bool a_copieValue) : m_id(a_id), m_type(DT_string), m_data(&a_string), m_copied(a_copieValue)
{
	if (a_copieValue)
	{
		std::string* l_data = new std::string();
		*l_data = a_string;
		m_data = l_data;
	}
}

Data::Data(sf::Uint8 a_id, sf::Int32& a_int32, bool a_copieValue) : m_id(a_id), m_type(DT_Int32), m_data(&a_int32), m_copied(a_copieValue)
{
	if (a_copieValue)
	{
		sf::Int32* l_data = new sf::Int32();
		*l_data = a_int32;
		m_data = l_data;
	}
}

Data::Data(sf::Uint8 a_id, sf::Uint32& a_uint32, bool a_copieValue) : m_id(a_id), m_type(DT_Uint32), m_data(&a_uint32), m_copied(a_copieValue)
{
	if (a_copieValue)
	{
		sf::Uint32* l_data = new sf::Uint32();
		*l_data = a_uint32;
		m_data = l_data;
	}
}

Data::Data(sf::Uint8 a_id, sf::Uint8& a_uint8, bool a_copieValue) : m_id(a_id), m_type(DT_Uint8), m_data(&a_uint8), m_copied(a_copieValue)
{
	if (a_copieValue)
	{
		sf::Uint8* l_data = new sf::Uint8();
		*l_data = a_uint8;
		m_data = l_data;
	}
}

////////////////////////////////////////////////////////////
/// \brief destructor
///
////////////////////////////////////////////////////////////
Data::~Data()
{
	
	if (m_copied)
		delete m_data;
		
}


////////////////////////////////////////////////////////////
/// \brief copy the data into a new data object
///
/// VERY IMPORTANT: the ptr is not copied, only the value
///
/// \return the new data with same value but different pointer
///
////////////////////////////////////////////////////////////
Data Data::CopyValueData() const
{
	switch (m_type)
	{
	case DT_bool:
	{
		bool l_bool = GetTypedData<bool>();
		return Data(m_id, l_bool, true);
	}
		
	case DT_float:
	{
		float l_float = GetTypedData<float>();
		return Data(m_id, l_float, true);
	}

	case DT_double:
	{
		double l_double = GetTypedData<double>();
		return Data(m_id, l_double, true);
	}
		
	case DT_string: 
	{
		std::string l_string = GetTypedData<std::string>();
		return Data(m_id, l_string, true);
	}

	case DT_Int32:
	{
		sf::Int32 l_int32 = GetTypedData<sf::Int32>();
		return Data(m_id, l_int32, true);
	}

	case DT_Uint32:
	{
		sf::Uint32 l_uint32 = GetTypedData<sf::Uint32>();
		return Data(m_id, l_uint32, true);
	}

	case DT_Uint8: 
	{
		sf::Uint8 l_uint8 = GetTypedData<sf::Uint8>();
		return Data(m_id, l_uint8, true);
	}

	default:
		throw NetworkException("Error : Bad type while copying data!");
		break;
	}
}



////////////////////////////////////////////////////////////
/// \brief utility function to allow comparing types
///
/// \param a_type the code of the type of a data
///
/// \return the number that can be compared with typeid
///
////////////////////////////////////////////////////////////
size_t Data::GetTypeCode(DataType a_type)
{
	switch (a_type)
	{
	case DT_bool: return typeid(bool).hash_code();
	case DT_double: return typeid(double).hash_code();
	case DT_float: return typeid(float).hash_code();
	case DT_Int32: return typeid(sf::Int32).hash_code();
	case DT_string: return typeid(std::string).hash_code();
	case DT_Uint32: return typeid(sf::Uint32).hash_code();
	case DT_Uint8: return typeid(sf::Uint8).hash_code();
	default: return 0;
	}
}



////////////////////////////////////////////////////////////
/// \brief change the value of the data using the current pointer
///
/// \param a_data the data with the new value
///
/// \return if the override was successfull
///
////////////////////////////////////////////////////////////
bool Data::OverrideData(const Data& a_data)
{
	if (m_type == a_data.m_type)
	{
		switch (m_type)
		{
		case DT_bool:   *static_cast<bool*>       (m_data) = *static_cast<bool*>       (a_data.m_data); break;
		case DT_double: *static_cast<double*>     (m_data) = *static_cast<double*>     (a_data.m_data); break;
		case DT_float:  *static_cast<float*>      (m_data) = *static_cast<float*>      (a_data.m_data); break;
		case DT_Int32:  *static_cast<sf::Int32*>  (m_data) = *static_cast<sf::Int32*>  (a_data.m_data); break;
		case DT_string: *static_cast<std::string*>(m_data) = *static_cast<std::string*>(a_data.m_data); break;
		case DT_Uint32: *static_cast<sf::Uint32*> (m_data) = *static_cast<sf::Uint32*> (a_data.m_data); break;
		case DT_Uint8:  *static_cast<sf::Uint8*>  (m_data) = *static_cast<sf::Uint8*>  (a_data.m_data); break;
		default: return false;
		}

		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////
/// \brief equal Operator
///
/// TODO : verify why an where this is required
///
////////////////////////////////////////////////////////////
Data& Data::operator=(const Data& other)
{
	throw NetworkException("Error : illegal operation, connection = !");
}


////////////////////////////////////////////////////////////
/// \brief copy constructor
///
/// TODO : verify why an where this is required
///
////////////////////////////////////////////////////////////
Data::Data(const Data& other) : m_data(other.m_data), m_id(other.m_id), m_type(other.m_type), m_copied(other.m_copied)
{
	if (m_copied)
	{
		switch (m_type)
		{
		case DT_bool:   m_data = new bool(); break;
		case DT_double: m_data = new double(); break;
		case DT_float:  m_data = new float(); break;
		case DT_Int32:  m_data = new sf::Int32(); break;
		case DT_string: m_data = new std::string(); break;
		case DT_Uint32: m_data = new sf::Uint32(); break; 
		case DT_Uint8:  m_data = new sf::Uint8(); break;
		}

		OverrideData(other); // since the 'other' may be delete because of local scope, we need to keep an address still alive
	}
}

}