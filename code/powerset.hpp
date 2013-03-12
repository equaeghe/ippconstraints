// Copyright 2005 Erik Quaeghebeur

#ifndef GUARD_powerset
#define GUARD_powerset

#include <set>

namespace Powerset {

template<class Element>
std::set<std::set<Element> > powerset(const std::set<Element>&);

template<class Compare, class Element>
std::set<std::set<Element>, Compare> ordered_powerset(const std::set<Element>&);

template<class Element>
std::set<std::set<Element> > tupleset(const std::set<Element>&, const int&);

template<class Element>
std::set<std::set<Element> > tupleset_upto(const std::set<Element>&, const int&);

}

#include "powerset.cpp" // necessary for templates until 'export' is supported by compilers

#endif
