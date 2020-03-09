#ifndef _STL_EXT_H
#define _STL_EXT_H

#include <ostream>
#include <set>
#include <boost/container/flat_set.hpp>

namespace std
{
    template <typename T>
    ostream& operator<<(ostream &out, const set<T> & stl_set)
    {
        for(const auto& elem : stl_set)
            out << elem << " ";
        out << endl;
        return out;
    }

    
}

namespace boost
{
namespace container 
{
    template <typename T>
    std::ostream& operator<<(std::ostream &out, const boost::container::flat_set<T> & fset)
    {
        for(const auto& elem : fset)
            out << elem << " ";
        out << std::endl;
        return out;
    }
}
}

#endif
