#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

class Storage {
private:
    std::unordered_map<std::string, std::string> store;
    std::mutex mtx;

public:
    std::string put(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    std::string remove(const std::string& key);
};