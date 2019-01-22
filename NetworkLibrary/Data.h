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


////////////////////////////////////////////////////////////
/// \brief Like a polymorphic structure capable of storing
/// any transmittable type
///
/// All member are const, so public access is not a problem
/// The internal operation of this class is a bit tricky since
/// the main data is a void pointer that can be copied
///
////////////////////////////////////////////////////////////
class NET Data
{

public:


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
	Data(sf::Uint8 a_id, bool& a_bool, bool a_copieValue = false);
	Data(sf::Uint8 a_id, float& a_float, bool a_copieValue = false);
	Data(sf::Uint8 a_id, double& a_double, bool a_copieValue = false);
	Data(sf::Uint8 a_id, std::string& a_string, bool a_copieValue = false);
	Data(sf::Uint8 a_id, sf::Int32& a_int32, bool a_copieValue = false);
	Data(sf::Uint8 a_id, sf::Uint32& a_uint32, bool a_copieValue = false);
	Data(sf::Uint8 a_id, sf::Uint8& a_uint8, bool a_copieValue = false);


	////////////////////////////////////////////////////////////
	/// \brief copy constructor
	///
	/// TODO : verify why an where this is required
	///
	////////////////////////////////////////////////////////////
	Data(const Data& other);

	////////////////////////////////////////////////////////////
	/// \brief destructor
	///
	////////////////////////////////////////////////////////////
	~Data();


	////////////////////////////////////////////////////////////
	/// \brief equal Operator
	///
	/// TODO : verify why an where this is required
	///
	////////////////////////////////////////////////////////////
	Data& operator=(const Data& other);


	////////////////////////////////////////////////////////////
	/// \brief copy the data into a new data object
	///
	/// VERY IMPORTANT: the ptr is not copied, only the value
	///
	/// \return the new data with same value but different pointer
	///
	////////////////////////////////////////////////////////////
	Data CopyValueData() const;

	////////////////////////////////////////////////////////////
	/// \brief change the value of the data using the current pointer
	///
	/// \param a_data the data with the new value
	///
	/// \return if the override was successfull
	///
	////////////////////////////////////////////////////////////
	bool OverrideData(const Data& a_data);


	////////////////////////////////////////////////////////////
	/// \brief get the value of the data by passing its original type
	///
	/// \tparam T the type expected as result
	///
	/// \return the data casted as T type
	///
	////////////////////////////////////////////////////////////
	template<class T>
	T GetTypedData() const
	{

		if (GetTypeCode(m_type) == typeid(T).hash_code())
		{
			return *static_cast<T*>(m_data); // if we arrive here, this cast is supposed to work
		}

		throw NetworkException("Error : Invalid cast or unexpected type!");
	}
		
private :

	////////////////////////////////////////////////////////////
	/// \brief utility function to allow comparing types
	///
	/// \param a_type the code of the type of a data
	///
	/// \return the number that can be compared with typeid
	///
	////////////////////////////////////////////////////////////
	static size_t GetTypeCode(DataType a_type);


	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////

	const bool m_copied; ///< Flag to know if this data was created from a copie (the m_data ptr will be automatically deleted)

public: // public attributs because of const attributs

	void* m_data; ///< A pointer to the main data, void* to handle multiple basic type

	const DataType m_type; ///< The type of the stored data

	const sf::Uint8 m_id; ///< The id given to this data

};

}