// Copyright 2005 Erik Quaeghebeur

#ifndef GUARD_printconstraints
#define GUARD_printconstraints

#include <list>
#include <string>

#include "constraints-classes.hpp"

void print_constraints_table(const Set&, const SscSet&, const std::list<std::string>&, const Set&, const int&);
void print_header(const Set&, const SscSet&, const std::list<std::string>::size_type&, const Set&, const int&);
void print_constraints(const std::list<std::string>&);
void print_footer();

#endif
