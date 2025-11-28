#include "safe_fields.h"
#include <sstream>
#include <stdexcept>
#include <shared_mutex>

SafeFields::SafeFields(size_t m)
        : fields(m, 0), mutexes(m) {}

int SafeFields::get(size_t idx) const {
    if (idx >= fields.size())
        throw std::out_of_range("get: invalid index");

    std::shared_lock lock(mutexes[idx]);  // shared read
    return fields[idx];
}

void SafeFields::set(size_t idx, int value) {
    if (idx >= fields.size())
        throw std::out_of_range("set: invalid index");

    std::unique_lock lock(mutexes[idx]);  // exclusive write
    fields[idx] = value;
}

SafeFields::operator std::string() const {
    std::ostringstream oss;
    oss << "[";

    // shared_lock кожного поля в строгому порядку, щоб уникнути deadlock
    for (size_t i = 0; i < fields.size(); ++i) {
        std::shared_lock lock(mutexes[i]);
        oss << fields[i];
        if (i + 1 < fields.size()) oss << ", ";
    }

    oss << "]";
    return oss.str();
}
