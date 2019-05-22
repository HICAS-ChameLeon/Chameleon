//
// Created by Heldon on 19-5-21.
//

#ifndef CHAMELEON_CHAMELEON_TYPE_UTILS_HPP
#define CHAMELEON_CHAMELEON_TYPE_UTILS_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include <boost/functional/hash.hpp>

#include <google/protobuf/repeated_field.h>

#include <mesos.pb.h>

#include <stout/hashmap.hpp>
#include <stout/stringify.hpp>
#include <stout/strings.hpp>
#include <stout/uuid.hpp>

namespace chameleon{
    namespace type{
        
        //OfferID
        inline bool operator==(const mesos::OfferID& left, const mesos::OfferID& right)
        {
            return left.value() == right.value();
        }

        inline bool operator==(const mesos::OfferID& left, const std::string& right)
        {
            return left.value() == right;
        }

        inline bool operator<(const mesos::OfferID& left, const mesos::OfferID& right)
        {
            return left.value() < right.value();
        }

        //SlaveID
        inline bool operator==(const mesos::SlaveID& left, const mesos::SlaveID& right)
        {
            return left.value() == right.value();
        }

        inline bool operator==(const mesos::SlaveID& left, const std::string& right)
        {
            return left.value() == right;
        }

        inline bool operator!=(const mesos::SlaveID& left, const mesos::SlaveID& right)
        {
            return left.value() != right.value();
        }

        inline bool operator<(const mesos::SlaveID& left, const mesos::SlaveID& right)
        {
            return left.value() < right.value();
        }

    }// namespace type
}//namespace chameleon

namespace std{

    //OfferID
    template <>
    struct hash<mesos::OfferID>
    {
        typedef size_t result_type;

        typedef mesos::OfferID argument_type;

        result_type operator()(const argument_type& offerId) const
        {
            size_t seed = 0;
            boost::hash_combine(seed, offerId.value());
            return seed;
        }
    };

    //SlaveID
    template <>
    struct hash<mesos::SlaveID>
    {
        typedef size_t result_type;

        typedef mesos::SlaveID argument_type;

        result_type operator()(const argument_type& slaveId) const
        {
            size_t seed = 0;
            boost::hash_combine(seed, slaveId.value());
            return seed;
        }
    };
}
#endif //CHAMELEON_CHAMELEON_TYPE_UTILS_HPP
