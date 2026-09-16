#include <iterator>
#include <ostream>
#include <string>
#include <unordered_set>

class Set {
    private:
    typedef std::string ElementType;
    typedef std::unordered_set<ElementType> SetType;

    // For storing all the elements
    SetType _objects;

    // Constructs a Set with initial elements provided in the parameter
    Set(SetType initial_set);
    public:
    using iterator = SetType::iterator;
    using const_iterator = SetType::const_iterator;

    Set();
    // TODO: an element of the set may itself be another set;

    // Adds an element to the Set
    void insert(ElementType element);
    // Removes an element from the Set
    void remove(ElementType element);
    // Returns True if the Set is empty
    bool empty();
    // Returns the cardinality (size) of the Set
    int cardinality();
    // Returns True if the element belongs to the Set
    bool contains(ElementType element);
    // Returns True if the element belongs to the Set
    bool operator[](ElementType element);

    // Returns the union of the Set with other
    Set operator+(Set other);

    // Unites the Set with other (inplace)
    Set operator+=(Set other);

    // Returns the intersection of the Set with other
    Set operator*(Set other);

    // Intersects the Set with other (inplace)
    Set operator*=(Set other);

    // Return the difference of the Set with other
    Set operator-(Set other);

    // Differs the Set with other (inplace)
    Set operator-=(Set other);

    // Non-const iterator methods
    iterator begin();
    iterator end();

    // Const iterator methods
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    // TODO: constructing the power set (the set of all subsets) of a given set.



};

std::ostream& operator<<(std::ostream& os, const Set& set);