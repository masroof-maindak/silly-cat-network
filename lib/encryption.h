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

void checkStringForEncryptables (std::string& vertexProperties, bool isDehash) {
    std::istringstream iss(vertexProperties);
    std::string token, key, value;
    while (std::getline(iss, token, '~')) {
        if (std::istringstream(token) >> key >> token >> value && token == ":") {
            if (!value.empty() && key[0] == '!') {
                if (!isDehash)
                    badHasher(value);
                else
                    reverseBadHashing(value);
                size_t pos = vertexProperties.find(key + ":" + value);
                if (pos != std::string::npos)
                    vertexProperties.replace(pos + key.length() + 1, value.length(), value);
            }
        }
    }
}