/**
 * @file set.h
 * @brief Declaration of the Set class.
 *
 * The Set class models an unordered set whose elements are not repeated.
 * An element of the set may itself be another set; nested sets are stored
 * in a canonical string form and parsed from / serialized to a string
 * representation such as "{a, b, {c, d}}".
 */

#include <iterator>
#include <ostream>
#include <string>
#include <unordered_set>
#include <vector>

/**
 * @brief An unordered set of elements that may contain nested sets.
 *
 * Elements are stored as canonicalized strings in an unordered_set, which
 * guarantees that elements are not repeated and that their order is not
 * significant. A nested set is represented by its canonical string form,
 * e.g. element "{a,b}" is itself a set.
 */
class Set {
    private:
    /** @brief The type used for a single element of the set. */
    typedef std::string ElementType;

    /** @brief The internal container type used to store the elements. */
    typedef std::unordered_set<ElementType> SetType;

    /** @brief The container that holds all the elements of the set. */
    SetType _objects;

    /**
     * @brief Constructs a Set with the given initial elements.
     * @param initial_set The container whose elements will be copied into the set.
     */
    Set(SetType initial_set);

    /**
     * @brief Splits a string by commas that are at the top nesting level.
     * @param s The string to split, normally the inner content of a set.
     * @return The list of top-level comma-separated tokens.
     */
    static std::vector<std::string> split_top_level(const std::string& s);

    /**
     * @brief Removes leading and trailing whitespace from a string.
     * @param s The string to trim.
     * @return The trimmed string, or an empty string if only whitespace was present.
     */
    static std::string trim(const std::string& s);

    /**
     * @brief Canonicalizes a single element before storing it.
     *
     * If the element is wrapped in braces it is treated as a nested set:
     * the element is parsed into a Set and re-serialized in canonical form,
     * so that logically equal sets always share the same representation.
     *
     * @param s The raw element string.
     * @return The canonical string form of the element.
     */
    static std::string canonicalize_element(const std::string& s);

    /**
     * @brief Returns the canonical string form of this set.
     *
     * The result is the elements sorted and joined with commas,
     * surrounded by braces, for example "{a,b,{c}}".
     *
     * @return The canonical string representation of the set.
     */
    std::string to_canonical_string() const;

    public:
    /** @brief Iterator type over the set elements. */
    using iterator = SetType::iterator;

    /** @brief Const iterator type over the set elements. */
    using const_iterator = SetType::const_iterator;

    /**
     * @brief Constructs an empty Set.
     */
    Set();

    /**
     * @brief Constructs a Set from its string representation.
     *
     * The string may describe nested sets, for example "{a, b, {c, d}}".
     * Outer braces are optional.
     *
     * @param str The string representation of the set, e.g. "{a, b, {c, d}}".
     */
    Set(const std::string& str);

    /**
     * @brief Adds an element to the set.
     *
     * If the set already contains the element, nothing changes. Nested sets
     * passed as strings (e.g. "{a, b}") are canonicalized before storing.
     *
     * @param element The element to add.
     */
    void insert(ElementType element);

    /**
     * @brief Removes an element from the set.
     * @param element The element to remove.
     */
    void remove(ElementType element);

    /**
     * @brief Checks whether the set is empty.
     * @return True if the set contains no elements, false otherwise.
     */
    bool empty();

    /**
     * @brief Determines the cardinality (number of elements) of the set.
     * @return The number of elements in the set.
     */
    int cardinality();

    /**
     * @brief Checks whether an element belongs to the set.
     * @param element The element to look for.
     * @return True if the element is present, false otherwise.
     */
    bool contains(ElementType element);

    /**
     * @brief Checks whether an element belongs to the set.
     * @param element The element to look for.
     * @return True if the element is present, false otherwise.
     */
    bool operator[](ElementType element);

    /**
     * @brief Returns the union of this set with another set.
     *
     * The union contains every element that belongs to at least one of the sets.
     *
     * @param other The other set.
     * @return A new set containing all elements of both sets.
     */
    Set operator+(Set other);

    /**
     * @brief Unites this set with another set in place.
     * @param other The other set.
     * @return A reference to this set after the union.
     */
    Set operator+=(Set other);

    /**
     * @brief Returns the intersection of this set with another set.
     *
     * The intersection contains every element that belongs to both sets.
     *
     * @param other The other set.
     * @return A new set containing the common elements.
     */
    Set operator*(Set other);

    /**
     * @brief Intersects this set with another set in place.
     * @param other The other set.
     * @return A reference to this set after the intersection.
     */
    Set operator*=(Set other);

    /**
     * @brief Returns the difference of this set and another set.
     *
     * The difference contains every element that belongs to this set
     * but not to the other set.
     *
     * @param other The other set.
     * @return A new set containing the remaining elements.
     */
    Set operator-(Set other);

    /**
     * @brief Subtracts another set from this set in place.
     * @param other The other set.
     * @return A reference to this set after the difference.
     */
    Set operator-=(Set other);

    /**
     * @brief Returns an iterator to the first element (non-const).
     * @return Iterator to the beginning of the set.
     */
    iterator begin();

    /**
     * @brief Returns an iterator past the last element (non-const).
     * @return Iterator to one past the end of the set.
     */
    iterator end();

    /**
     * @brief Returns a const iterator to the first element.
     * @return Const iterator to the beginning of the set.
     */
    const_iterator begin() const;

    /**
     * @brief Returns a const iterator past the last element.
     * @return Const iterator to one past the end of the set.
     */
    const_iterator end() const;

    /**
     * @brief Returns a const iterator to the first element.
     * @return Const iterator to the beginning of the set.
     */
    const_iterator cbegin() const;

    /**
     * @brief Returns a const iterator past the last element.
     * @return Const iterator to one past the end of the set.
     */
    const_iterator cend() const;

    /**
     * @brief Constructs the power set of this set.
     *
     * The power set contains every subset of this set (2^cardinality elements).
     * Nested subsets are stored as canonical string forms.
     *
     * @return A new set containing all subsets of this set.
     */
    Set power_set() const;
};

/**
 * @brief Outputs the set to a stream.
 *
 * The set is printed in braces with elements separated by ", ".
 * Nested sets are printed recursively, e.g. "{a, {b, c}}".
 *
 * @param os The output stream.
 * @param set The set to print.
 * @return The output stream, for chaining.
 */
std::ostream& operator<<(std::ostream& os, const Set& set);