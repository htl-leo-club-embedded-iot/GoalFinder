#pragma once

#include <algorithm>
#include <cctype>
#include <string>

class String {
  public:
    String() = default;
    String(const char *value) : value_(value != nullptr ? value : "") {}
    String(const std::string &value) : value_(value) {}
    String(const String &) = default;
    String &operator=(const String &) = default;

    String &operator=(const char *value) {
        value_ = value != nullptr ? value : "";
        return *this;
    }

    const char *c_str() const { return value_.c_str(); }
    size_t length() const { return value_.length(); }
    bool isEmpty() const { return value_.empty(); }

    void trim() {
        auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
        auto beginIt = std::find_if(value_.begin(), value_.end(), notSpace);
        auto endIt = std::find_if(value_.rbegin(), value_.rend(), notSpace).base();

        if (beginIt >= endIt) {
            value_.clear();
            return;
        }

        value_ = std::string(beginIt, endIt);
    }

    bool operator==(const String &other) const { return value_ == other.value_; }
    bool operator==(const char *other) const {
        return value_ == std::string(other != nullptr ? other : "");
    }

  private:
    std::string value_;
};
