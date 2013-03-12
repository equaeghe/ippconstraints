// Copyright 2005 Erik Quaeghebeur

#ifndef GUARD_kmonotonicityconstraints
#define GUARD_kmonotonicityconstraints

#include <list>
#include <string>

#include "constraints-classes.hpp"

std::list<std::string> k_monotonicity_constraints(const Set&, const Set&, const SscSet&, const int&); // arguments are respectively the empty set, the possibility space, its power set and the monotonicity level

Set Set_intersection(const SSet&);

Set Set_union(const SSet&);

#endif
