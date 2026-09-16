#include<iostream>
#include "set/set.h"


int main() {
    Set set;

    set.insert("1");
    set.insert("2");
    set.insert("3");

    std::cout << set;
}