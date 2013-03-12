// Copyright 2005 Erik Quaeghebeur

#ifndef GUARD_constraintsclasses
#define GUARD_constraintsclasses

#include <map>
#include <set>
#include <list>
#include <string>

const double tol(1.0E-9); // tolerance for use with doubles

// simplify some notation
typedef std::set<char> Set;

// prepare a class that is used to sort set<char>'s by size, then lexicographically (instead of just lexicographically)
class SizeComp {
  public:
    bool operator()(const Set&, const Set&) const;
};

// simplify some more notation
typedef std::set<Set> SSet;
typedef std::set<Set, SizeComp> SscSet;
typedef std::set<SSet> SSSet;

// prepare a class that will hold a constraint
template<class Coefficient>
class Constraint {
  public:
    // member variables
    std::map<Set, Coefficient, SizeComp> subsets;  // stores the coefficients of P_(A), for all subsets A
    Coefficient constant; // stores the coefficient of the constant term
    // constructor
    Constraint(const SscSet&, const Coefficient&); // creates a constraint for the subsets given in the set in the first argument with all-zero coefficients and a constant coefficient with value given in the second argument
};

// we must include the functions from template classes immediately (as long as 'export' is not implemented)
template<class Coefficient>
Constraint<Coefficient>::Constraint(const SscSet& power_set, const Coefficient& c): constant(c) {
  for (SscSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i)
    subsets[*i];
}

Constraint<int> double2int(const SscSet&, Constraint<double>&);

std::string Constraint2string(const Constraint<int>&);

// prepare a class that will hold a translation (between sets and indicator functions) table
class TranslationTable {
  private:
    // member variable
    std::map<Set, std::list<int> > table;
  public:
    // constructor
    TranslationTable(const Set&, const SscSet&); // creates a table of indicator functions for all subsets given in the second argument of the possibility space (first argument)
    // "get" function
    std::list<int> translate(const Set&);  // returns the indicator function of the set (as a subset of the previously given possibility space)
};

#endif
