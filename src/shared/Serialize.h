#ifndef SERIALIZE_H
#define SERIALIZE_H

#include"pch.h"

namespace srlz
{
    // Template function to serialize an object to a string
    template <typename T>
    std::string serialize(const T& obj) 
    {
        std::ostringstream archive_stream;
        boost::archive::text_oarchive archive(archive_stream);
        archive << obj;
        return archive_stream.str();
    }

    // Template function to deserialize an object from a string
    template <typename T>
    T deserialize(const std::string& data) {
        T obj;
        std::istringstream archive_stream(data);
        boost::archive::text_iarchive archive(archive_stream);
        archive >> obj;
        return obj;
    }
}

#endif //SERIALIZE_H