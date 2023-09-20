#ifndef PCH_H
#define PCH_H

#include<iostream>
#include<string>
#include<vector>
#include<deque>
#include<thread>
#include<mutex>
#include<filesystem>

#include<boost/serialization/vector.hpp>
#include<boost/archive/text_oarchive.hpp>
#include<boost/archive/text_iarchive.hpp>
#include<boost/asio.hpp>
#include<boost/asio/ts/buffer.hpp>
#include<boost/asio/ts/internet.hpp>

using namespace boost;

namespace fs = std::filesystem;

#endif // !PCH.h


