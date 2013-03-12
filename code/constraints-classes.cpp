// Copyright 2005 Erik Quaeghebeur

#include <map>
  using std::map;
#include <list>
  using std::list;
#include <cmath>
  using std::floor;
  using std::abs;
#include <string>
  using std::string;
#include <sstream>
  using std::stringstream;

#include "constraints-classes.hpp"

bool SizeComp::operator()(const Set& x, const Set& y) const {
  if (x.size() != y.size())
    return x.size() < y.size();
  else
    return x < y;
}

Constraint<int> double2int(const SscSet& power_set, Constraint<double>& c) {
  // initialize are output-to-be
  Constraint<int> d(power_set, 0);

  // create integer multiple of the constraint by iteratively dividing by the smallest non-integer part
  double smallest;
  do {
    smallest = 1;
    // split the constraint in an integer and non-integer part and find the smallest non-zero non-integer part
    // first do this for the constant
    {
      int trunc(int(c.constant));
      d.constant = trunc;
      c.constant -= double(trunc);
      double temp(abs(c.constant));
      if (temp < tol)
        c.constant = 0;
      else
        if (temp < smallest)
          smallest = temp;
    }
    // then for the coefficients
    for (SscSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i) {
      int trunc(int(c.subsets[*i]));
      d.subsets[*i] = trunc;
      c.subsets[*i] -= double(trunc);
      double temp(abs(c.subsets[*i]));
      if (temp < tol)
        c.subsets[*i] = 0;
      else
        if (temp < smallest)
          smallest = temp;
    }
    // if the noninteger part is not nonzero everywhere, merge the two parts together again and divide by the smallest non-integer part
    if (smallest > tol) {
      // first do this for the constant
      {
        c.constant += double(d.constant);
        c.constant /= smallest;
      }
      // then for the coefficients
      for (SscSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i) {
        c.subsets[*i] += double(d.subsets[*i]);
        c.subsets[*i] /= smallest;
      }
    }
  } while (smallest != 1.0);  // as long as the non-integer part is non-zero

  return d;
}


string Constraint2string(const Constraint<int>& constraint) {
  string out;
  stringstream temp;

  temp << constraint.constant;
  temp >> out;
  temp.clear();
  for (map<Set, int>::const_iterator i = constraint.subsets.begin(); i != constraint.subsets.end(); ++i) {
    string fragment;
    temp << i->second;
    temp >> fragment;
    temp.clear();
    out += ' ' + fragment;
  }

  return out;
}


TranslationTable::TranslationTable(const Set& s, const SscSet& subsets) { // subsets must only contain subsets of s, otherwise the indicator functions are relative to s, but don't necessarily describe the subset completely

  // add every Set in s to the table
  for (SscSet::const_iterator i = subsets.begin(); i != subsets.end(); ++i)
    table[*i];
  // add the translation for every set
  for (map<Set, list<int> >::iterator i = table.begin(); i != table.end(); ++i)
  // see if an element is in the set or not and add 0 or 1 accordingly
  for (Set::const_iterator j = s.begin(); j != s.end(); ++j)
    i->second.push_back(((i->first).find(*j) == i->first.end())? 0 : 1);
}

list<int> TranslationTable::translate(const Set& s) {
  // s must be in the table, otherwise return an empty list
  if (table.find(s) != table.end())
    return table.find(s)->second;
  else {
    list<int> empty;
    return empty;
  }
}
