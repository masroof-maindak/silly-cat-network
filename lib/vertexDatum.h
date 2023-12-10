#ifndef VERTEXDATUM_H 
#define VERTEXDATUM_H

#include <string>

struct vertexDatum {
    std::string uniqueKey;
    int cumulativeID;

    //equality operator
    bool operator==(const vertexDatum& other) const {
        return (uniqueKey == other.uniqueKey);
    }

    //constructor
    vertexDatum(std::string _uniqueKey, int _cumulativeID) : uniqueKey(_uniqueKey), cumulativeID(_cumulativeID) {}
};

#endif