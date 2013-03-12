// Copyright 2005 Erik Quaeghebeur

#ifndef GUARD_coherenceconstraints
#define GUARD_coherenceconstraints

#include <list>
#include <string>

#include "constraints-classes.hpp"

std::list<std::string> coherence_constraints(const Set&, const Set&, const SscSet&); // arguments are respectively the empty set, the possibility space, and its power set


#endif
