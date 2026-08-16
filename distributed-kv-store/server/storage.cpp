#include "storage.h"

// Store a key-value pair
std::string Storage::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    store[key] = value;
    return "OK";
}

// Retrieve value for a key
std::string Storage::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = store.find(key);
    if (it != store.end()) {
        return "VALUE " + it->second;
    }
    return "NOT_FOUND";
}

// Delete a key
std::string Storage::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    if (store.erase(key)) {
        return "OK";
    }
    return "NOT_FOUND";
}