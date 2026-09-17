#include "set.h"

#include <algorithm>

Set::Set() {}
Set::Set(SetType initial_set) : _objects(initial_set) {}

Set::Set(const std::string& str) {
    std::string inner = trim(str);

    // Strip the enclosing braces, if present
    if (inner.size() >= 2 && inner.front() == '{' && inner.back() == '}') {
        inner = inner.substr(1, inner.size() - 2);
    }

    for (const auto& token : split_top_level(inner)) {
        std::string element = canonicalize_element(token);
        if (!element.empty()) {
            _objects.insert(element);
        }
    }
}

std::string Set::trim(const std::string& s) {
    const char* whitespace = " \t\n\r\f\v";
    size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

std::vector<std::string> Set::split_top_level(const std::string& s) {
    std::vector<std::string> tokens;
    std::string current;
    int depth = 0;

    for (char c : s) {
        if (c == '{') {
            ++depth;
            current += c;
        } else if (c == '}') {
            --depth;
            current += c;
        } else if (c == ',' && depth == 0) {
            tokens.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    tokens.push_back(current);

    return tokens;
}

std::string Set::canonicalize_element(const std::string& s) {
    std::string trimmed = trim(s);

    // An element wrapped in braces is itself a set: parse and re-serialize it
    // so that logically equal sets get the same representation.
    if (trimmed.size() >= 2 && trimmed.front() == '{' && trimmed.back() == '}') {
        Set nested(trimmed);
        return nested.to_canonical_string();
    }

    return trimmed;
}

std::string Set::to_canonical_string() const {
    std::vector<std::string> elements(_objects.begin(), _objects.end());
    std::sort(elements.begin(), elements.end());

    std::string result = "{";
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) {
            result += ',';
        }
        result += elements[i];
    }
    result += '}';

    return result;
}

void Set::insert(ElementType element) {
    _objects.insert(canonicalize_element(element));
}

void Set::remove(ElementType element) {
    _objects.erase(canonicalize_element(element));
}

bool Set::empty() {
    return _objects.empty();
}

int Set::cardinality() {
    return _objects.size();
}

bool Set::contains(ElementType element) {
    return _objects.find(canonicalize_element(element)) != _objects.end();
}

bool Set::operator[](ElementType element) {
    return contains(element);
}

Set Set::operator+(Set other) {
    SetType temp = _objects;
    for (const auto& el : other._objects) {
        temp.insert(el);
    }
    return Set(temp);
}

Set Set::operator+=(Set other) {
    for (const auto& el : other._objects) {
        _objects.insert(el);
    }
    return *this;
}

Set Set::operator*(Set other) {
    SetType temp;
    for (const auto& el : _objects) {
        if (other.contains(el))
            temp.insert(el);
    }
    return Set(temp);
}

Set Set::operator*=(Set other) {
    for (auto it = _objects.begin(); it != _objects.end();) {
        if (!other.contains(*it))
            it = _objects.erase(it);
        else
            ++it;
    }
    return *this;
}

Set Set::operator-(Set other) {
    SetType temp = _objects;
    for (const auto& el : other._objects) {
        temp.erase(el);
    }
    return Set(temp);
}

Set Set::operator-=(Set other) {
    for (const auto& el : other._objects) {
        _objects.erase(el);
    }
    return *this;
}

Set::iterator Set::begin() {
    return _objects.begin();
}

Set::iterator Set::end() {
    return _objects.end();
}

Set::const_iterator Set::begin() const {
    return _objects.begin();
}

Set::const_iterator Set::end() const {
    return _objects.end();
}

Set::const_iterator Set::cbegin() const {
    return _objects.cbegin();
}

Set::const_iterator Set::cend() const {
    return _objects.cend();
}

Set Set::power_set() const {
    std::vector<ElementType> elements(_objects.begin(), _objects.end());
    size_t n = elements.size();

    Set result;
    size_t total = static_cast<size_t>(1) << n;

    for (size_t mask = 0; mask < total; ++mask) {
        Set subset;
        for (size_t i = 0; i < n; ++i) {
            if (mask & (static_cast<size_t>(1) << i)) {
                subset.insert(elements[i]);
            }
        }
        result.insert(subset.to_canonical_string());
    }

    return result;
}

std::ostream& operator<<(std::ostream& os, const Set& set) {
    os << '{';

    bool first = true;
    for (const auto& element : set) {
        if (!first) {
            os << ", ";
        }
        first = false;

        // Nested sets are pretty-printed recursively so that spacing is
        // inserted between their elements as well.
        if (element.size() >= 2 && element.front() == '{' && element.back() == '}') {
            Set nested(element);
            os << nested;
        } else {
            os << element;
        }
    }

    os << '}';
    return os;
}
