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
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include <queue>
#include <type_traits>
#include <unordered_set>

// TODO : to remove
#include <array>
#include <Windows.h>


////////////////////////////////////////////////////////////
// Define portable import / export macros
////////////////////////////////////////////////////////////
#if defined(NETWORKLIBRARY_EXPORTS)

#define NET __declspec(dllexport)

#else

#define NET __declspec(dllimport)

#endif
