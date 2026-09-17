#include "UnitTest++/UnitTest++.h"
#include <UnitTest++/CheckMacros.h>
#include <UnitTest++/TestMacros.h>
#include <UnitTest++/TestRunner.h>
#include <iostream>
#include <sstream>
#include "../set.h"

SUITE(SetClassTest) {
    TEST(Sanity) {
        CHECK_EQUAL(0, 0);
    }

    // Inserts some elements and then calls to contains() method
    TEST(Insert) {
        Set set;

        set.insert("1");
        set.insert("2");

        CHECK(set.contains("1"));
        CHECK(set.contains("2"));

        set.insert("3");
        CHECK(set.contains("3"));
    }

    TEST(InsertThenBracketsContains) {
        Set set;

        set.insert("1");
        set.insert("2");

        CHECK(set["1"]);
        CHECK(set["2"]);

        set.insert("3");
        CHECK(set["3"]);
    }

    // Inserts some elements, then removes then one by one calling contains() method to check
    TEST(InsertThenRemove) {
        Set set;

        set.insert("1");
        set.insert("2");

        set.remove("1");
        CHECK(!set.contains("1"));

        set.remove("2");
        CHECK(!set.contains("2"));

        set.insert("3");
        set.remove("3");
        CHECK(!set.contains("3"));
    }

    // Initializes an empty set, inserts some elements, removes them and checks
    TEST(Empty) {
        Set set;
        CHECK(set.empty());

        set.insert("1");
        set.insert("2");
        set.insert("3");

        CHECK(!set.empty());

        set.remove("1");
        set.remove("2");
        set.remove("3");

        CHECK(set.empty());
    }

    TEST(Cardinality) {
        Set set;

        set.insert("1");
        set.insert("2");
        set.insert("3");
        set.insert("4");

        CHECK(set.cardinality() == 4);

        set.remove("1");
        set.remove("2");

        CHECK(set.cardinality() == 2);

        set.remove("3");

        CHECK(set.cardinality() == 1);

        set.remove("4");

        CHECK(set.cardinality() == 0);
    }

    // Inserting the same element more than once keeps the set free of repeats
    TEST(InsertDoesNotRepeatElements) {
        Set set;

        set.insert("a");
        set.insert("a");
        set.insert("a");

        CHECK(set.cardinality() == 1);
        CHECK(set.contains("a"));
    }

    // contains() and operator[] return false for an element that is absent
    TEST(ContainsAbsentElement) {
        Set set("{a, b}");

        CHECK(!set.contains("c"));
        CHECK(!set["c"]);

        CHECK(set.contains("a"));
        CHECK(set["a"]);
    }

    // Union of two plain sets (+, +=)
    TEST(Union) {
        Set a("{a, b}");
        Set b("{b, c}");

        Set sum = a + b;
        CHECK(sum.cardinality() == 3);
        CHECK(sum.contains("a"));
        CHECK(sum.contains("b"));
        CHECK(sum.contains("c"));

        a += b;
        CHECK(a.cardinality() == 3);
        CHECK(a.contains("c"));
        CHECK(a.contains("a"));
    }

    // Union with the empty set does not change the set
    TEST(UnionWithEmptySet) {
        Set a("{a, b}");
        Set empty;

        Set sum = a + empty;
        CHECK(sum.cardinality() == 2);
        CHECK(sum.contains("a"));
        CHECK(sum.contains("b"));

        empty += a;
        CHECK(empty.cardinality() == 2);
    }

    // Intersection of two plain sets (*, *=)
    TEST(Intersection) {
        Set a("{a, b, c}");
        Set b("{b, c, d}");

        Set common = a * b;
        CHECK(common.cardinality() == 2);
        CHECK(common.contains("b"));
        CHECK(common.contains("c"));
        CHECK(!common.contains("a"));
        CHECK(!common.contains("d"));

        a *= b;
        CHECK(a.cardinality() == 2);
        CHECK(a.contains("b"));
        CHECK(!a.contains("a"));
    }

    // Intersection of disjoint sets is empty
    TEST(IntersectionDisjointIsEmpty) {
        Set a("{a, b}");
        Set b("{c, d}");

        CHECK((a * b).empty());

        a *= b;
        CHECK(a.empty());
    }

    // Difference of two plain sets (-, -=)
    TEST(Difference) {
        Set a("{a, b, c}");
        Set b("{b}");

        Set diff = a - b;
        CHECK(diff.cardinality() == 2);
        CHECK(diff.contains("a"));
        CHECK(diff.contains("c"));
        CHECK(!diff.contains("b"));

        a -= b;
        CHECK(a.cardinality() == 2);
        CHECK(!a.contains("b"));
        CHECK(a.contains("a"));
    }

    // Removing every element leaves the set empty
    TEST(DifferenceToEmpty) {
        Set a("{a, b}");
        Set b("{a, b}");

        CHECK((a - b).empty());

        a -= b;
        CHECK(a.empty());
    }

    // Building the set from "{}" yields an empty set
    TEST(ConstructEmptyFromString) {
        Set set("{}");

        CHECK(set.empty());
        CHECK(set.cardinality() == 0);
    }

    // Builds a set that contains nested sets from its string representation
    TEST(ConstructFromStringWithNestedSets) {
        Set set("{a, b, {c, d}, {}, {a, {c}}}");

        CHECK(set.contains("a"));
        CHECK(set.contains("b"));
        CHECK(set.contains("{c,d}"));
        CHECK(set.contains("{}"));
        CHECK(set.contains("{a,{c}}"));
        CHECK(set.cardinality() == 5);
    }

    // Nested sets with the same elements (in a different order) are equal
    TEST(NestedSetCanonicalEquality) {
        Set set;

        set.insert("{a, b}");
        CHECK(set.contains("{b,a}"));
        CHECK(set.contains("{ b , a }"));

        set.insert("{a, {c, d}}");
        CHECK(set.contains("{{d,c},a}"));
    }

    // Union, intersection and difference work with nested set elements
    TEST(NestedSetOperations) {
        Set a("{a, {b, c}}");
        Set b("{{b,c}, d}");

        Set unionSet = a + b;
        CHECK(unionSet.cardinality() == 3);
        CHECK(unionSet.contains("a"));
        CHECK(unionSet.contains("d"));
        CHECK(unionSet.contains("{b,c}"));

        Set intersection = a * b;
        CHECK(intersection.cardinality() == 1);
        CHECK(intersection.contains("{b,c}"));

        Set difference = a - b;
        CHECK(difference.cardinality() == 1);
        CHECK(difference.contains("a"));
    }

    // The power set of a 3-element set has 2^3 = 8 subsets
    TEST(PowerSet) {
        Set set("{a, b, c}");
        Set powerset = set.power_set();

        CHECK(powerset.cardinality() == 8);
        CHECK(powerset.contains("{}"));
        CHECK(powerset.contains("{a}"));
        CHECK(powerset.contains("{a,b}"));
        CHECK(powerset.contains("{a,b,c}"));
    }

    // Prints nested sets in braces with comma separators
    TEST(PrintNestedSet) {
        Set set("{a, {c, d}}");

        std::ostringstream os;
        os << set;
        std::string out = os.str();

        CHECK(out.front() == '{');
        CHECK(out.back() == '}');
        CHECK(out.find("{c, d}") != std::string::npos ||
              out.find("{d, c}") != std::string::npos);
    }

    // The power set of the empty set has exactly one subset: the empty set
    TEST(PowerSetOfEmptySet) {
        Set set;
        Set powerset = set.power_set();

        CHECK(powerset.cardinality() == 1);
        CHECK(powerset.contains("{}"));
    }

    // Non-const iterators visit every element exactly once
    TEST(NonConstIterators) {
        Set set("{a, b, c}");

        int count = 0;
        int totalLength = 0;
        for (Set::iterator it = set.begin(); it != set.end(); ++it) {
            ++count;
            totalLength += it->size();
        }

        CHECK(count == 3);
        CHECK(totalLength == 3);
    }

    // Const begin()/end() visit every element of a const set
    TEST(ConstIterators) {
        const Set set("{a, b, c}");

        int count = 0;
        for (Set::const_iterator it = set.begin(); it != set.end(); ++it) {
            ++count;
        }

        CHECK(count == 3);
    }

    // cbegin()/cend() visit every element
    TEST(ConstBeginEndIterators) {
        Set set("{a, b, c}");

        int count = 0;
        for (Set::const_iterator it = set.cbegin(); it != set.cend(); ++it) {
            ++count;
        }

        CHECK(count == 3);
    }

    // Iterating an empty set performs no iterations
    TEST(IteratorsOnEmptySet) {
        Set set;

        CHECK(set.begin() == set.end());
        CHECK(set.cbegin() == set.cend());
    }
}

int main() {
    std::cout << "Running tests!" << std::endl;
    return UnitTest::RunAllTests();
}