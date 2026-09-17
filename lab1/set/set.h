#include <iterator>
#include <ostream>
#include <string>
#include <unordered_set>
#include <vector>

class Set {
    private:
    typedef std::string ElementType;
    typedef std::unordered_set<ElementType> SetType;

    // For storing all the elements
    SetType _objects;

    // Constructs a Set with initial elements provided in the parameter
    Set(SetType initial_set);

    // Splits a string by commas, respecting nested braces
    static std::vector<std::string> split_top_level(const std::string& s);

    // Trims leading and trailing whitespace from a string
    static std::string trim(const std::string& s);

    // Canonicalizes an element: if it looks like a set "{...}",
    // parse it into a Set and re-serialize it in canonical form.
    static std::string canonicalize_element(const std::string& s);

    // Returns the canonical string form of this set, e.g. "{a,b,{c}}"
    std::string to_canonical_string() const;

    public:
    using iterator = SetType::iterator;
    using const_iterator = SetType::const_iterator;

    Set();
    // Constructs a Set from a string representation, e.g. "{a, b, {c, d}}"
    Set(const std::string& str);

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

    // Returns the power set (the set of all subsets) of this set
    Set power_set() const;



};

std::ostream& operator<<(std::ostream& os, const Set& set);