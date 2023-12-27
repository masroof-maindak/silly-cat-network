#include <sstream>
#include <vector>

void badHasher (std::string& key) {
    std::vector<int> adder = {3, 0, 0, 3, 2, 0, 0, 4};
    for (int i = 0; i < key.size(); i++)
        key[i] += adder[i % 8];
}

void reverseBadHashing (std::string& key) {
    std::vector<int> adder = {3, 0, 0, 3, 2, 0, 0, 4};
    for (int i = 0; i < key.size(); i++)
        key[i] -= adder[i % 8];
}

void replaceEncryptedValue(std::string& original, const std::string& fromSubstr, const std::string& toSubstr) {
    size_t start_pos = original.find(fromSubstr);
    if (start_pos == std::string::npos)
        return;
    original.replace(start_pos, fromSubstr.length(), toSubstr);
}

void checkStringForEncryptables (std::string& vertexProperties, bool isDehash) {
    //put string into stream
    std::istringstream iss(vertexProperties);
    std::string keyValuePair;

    //iterate through stream, delimiting by ~
    while (std::getline(iss, keyValuePair, '~')) {

        //separate keyValuePairs into key and value strings
        std::istringstream iss2(keyValuePair);
        std::string key, value;
        std::getline(iss2, key, ':');
        std::getline(iss2, value, ':');

        //if first character of key is a !, then encrypt/decrypt corresponding value
        if (key[0] == '!')
            if (isDehash)
                reverseBadHashing(value);
            else
                badHasher(value);
        
        //replace keyValuePair with new value
        replaceEncryptedValue(vertexProperties, keyValuePair, key + ":" + value);
    }
}