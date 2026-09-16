#include "set.h"

Set::Set() {}
Set::Set(SetType initial_set) : _objects(initial_set) {}

void Set::insert(ElementType element) {
    _objects.insert(element);
}

void Set::remove(ElementType element) {
    _objects.erase(element);
}

bool Set::empty() {
    return _objects.empty();
}

int Set::cardinality() {
    return _objects.size();
}

bool Set::contains(ElementType element) {
    return _objects.find(element) != _objects.end();
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
    SetType temp = _objects;
    for (const auto& el : other._objects) {
        if (!this->contains(el))
            temp.erase(el);
    }
    return Set(temp);
}

Set Set::operator*=(Set other) {
    for (const auto& el : other._objects) {
        if (!this->contains(el))
            _objects.erase(el);
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

std::ostream& operator<<(std::ostream& os, const Set& set) {
    for (auto& element : set) {
        os << element << ' ';
    }
    return os;
}