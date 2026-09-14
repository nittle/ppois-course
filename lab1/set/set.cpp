#include <string>
#include <unordered_set>

class Set {
    private:
    typedef std::string ElementType;
    typedef std::unordered_set<ElementType> SetType;

    // For storing all the elements
    SetType _objects;

    // Constructs a Set with initial elements provided in the parameter
    Set(SetType initial_set) {
        _objects = initial_set;
    }
 
    public:
    Set() {    }
    // TODO: an element of the set may itself be another set;

    // Adds an element to the Set
    void insert(ElementType element) {
        _objects.insert(element);
    }
    // Removes an element from the Set
    void remove(ElementType element) {
        _objects.erase(element);
    }
    // Returns True if the Set is empty
    bool empty() {
        return _objects.empty();
    }
    // Returns the cardinality (size) of the Set
    int cardinality() {
        return _objects.size();
    }
    // Returns True if the element belongs to the Set
    bool contains(ElementType element) {
        return _objects.find(element) != _objects.end();
    }
    // Returns True if the element belongs to the Set
    bool operator[](ElementType element) {
        return contains(element);
    }

    // Returns the union of the Set with other
    Set operator+(Set other) {
        // Temporary set for storing the result
        SetType temp = _objects;

        for(const auto& el : other._objects) {
            temp.insert(el);
        }

        return Set(temp);
    }

    // Unites the Set with other (inplace)
    Set operator+=(Set other) {
        for(const auto& el : other._objects) {
            _objects.insert(el);
        }

        return *this;
    }

    // Returns the intersection of the Set with other
    Set operator*(Set other) {
        SetType temp = _objects;

        for(const auto& el : other._objects) {
            if (!this->contains(el))
                temp.erase(el);
        }

        return Set(temp);
    }

    // Intersects the Set with other (inplace)
    Set operator*=(Set other) {
        for(const auto& el : other._objects) {
            if (!this->contains(el))
                _objects.erase(el);
        }

        return *this;
    }

    // Return the difference of the Set with other
    Set operator-(Set other) {
        SetType temp = _objects;

        for(const auto& el : other._objects) {
            temp.erase(el);
        }

        return Set(temp);
    }

    // Differs the Set with other (inplace)
    Set operator-=(Set other) {
        for(const auto& el : other._objects) {
            _objects.erase(el);
        }

        return *this;
    }

    // TODO: constructing the power set (the set of all subsets) of a given set.



};