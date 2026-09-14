## Set Theory

The task is to describe a "Set" class. The class must implement the following capabilities:

- an element of the set may itself be another set;
- checking whether the set is empty;
- adding an element;
- removing an element;
- determining the cardinality of the set;
- checking whether an element belongs to the set ([]);
- the union of two sets (+, +=);
- the intersection of two sets (*, *=);
- the difference of two sets (-, -=);
- constructing the power set (the set of all subsets) of a given set.

Describe an "Unordered Cantor Set" class (elements are not repeated and are unordered). The class must additionally support building a set from a string (for example, {a, b, c, {a, b}, {}, {a, {c}}}).