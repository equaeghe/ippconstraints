// Copyright 2005 Erik Quaeghebeur

#ifndef GUARD_basicconstraints
#define GUARD_basicconstraints

#include <list>
#include <string>

#include "constraints-classes.hpp"

list<std::string> one_normed_constraints(const Set&, const SscSet&);  // arguments are respectively the possibility space and its power set
list<std::string> zero_normed_constraints(const Set&, const SscSet&);  // arguments are respectively the empty set and its power set
list<std::string> positivity_constraints(const SscSet&); // argument is the power set
list<std::string> linearity_constraints(const SscSet&); // argument is the power set
list<std::string> superlinearity_constraints(const SscSet&); // argument is the power set
list<std::string> weak_constraints(const Set&, const SscSet&);  // arguments are respectively the possibility space and its power set

#endif
