// Copyright 2007 Erik Quaeghebeur

#include <list>
  using std::list;
#include <string>
  using std::string;
#include <algorithm>
  using std::set_difference;

#include "basic-constraints.hpp"
#include "constraints-classes.hpp"
#include "powerset.hpp"
  using namespace Powerset;


list<string> one_normed_constraints(const Set& possibility_space, const SscSet& power_set) {
  list<string> constraints;

  // create the constraints
  Constraint<int> c(power_set, 0); Constraint<int> d(power_set, 0);
  c.constant = +1; c.subsets[possibility_space] = -1;
  d.constant = -1; d.subsets[possibility_space] = +1;

  constraints.push_back(Constraint2string(c)); constraints.push_back(Constraint2string(d));

  return constraints;
}

list<string> zero_normed_constraints(const Set& empty_set, const SscSet& power_set) {
  list<string> constraints;

  // create the constraints
  Constraint<int> c(power_set, 0); Constraint<int> d(power_set, 0);
  c.subsets[empty_set] = -1;
  d.subsets[empty_set] = +1;

  constraints.push_back(Constraint2string(c)); constraints.push_back(Constraint2string(d));

  return constraints;
}

list<string> positivity_constraints(const SscSet& power_set) {
  list<string> constraints;

  // go over all the sets A in the powerset and create a constraint expressing P_(A) >= 0
  for (SscSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i) {
    Constraint<int> c(power_set, 0);
    c.subsets[*i] = +1;
    constraints.push_back(Constraint2string(c));
  }

  return constraints;
}

list<string> linearity_constraints(const SscSet& power_set) {
  list<string> constraints;

  // the sum of the probability of the singletons of a subset must be equal to the probability in that subset
  // go over all the subsets
  for (SSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i)
    // select the nontrivial ones
    if (i->size() != 1) {
      // get all the singletons of the selected subset
      SSet singleton_set(tupleset(*i,1));

      // create the constraints
      Constraint<int> c(power_set, 0); Constraint<int> d(power_set, 0);
      c.subsets[*i] = +1; d.subsets[*i] = -1;
      for (SSet::const_iterator j = singleton_set.begin(); j != singleton_set.end(); ++j) {
        c.subsets[*j] = -1; d.subsets[*j] = +1;
      }
    constraints.push_back(Constraint2string(c)); constraints.push_back(Constraint2string(d));
    }

  return constraints;
}

list<string> superlinearity_constraints(const SscSet& power_set) {
  list<string> constraints;

  // the lower probability of a subset must be greater than sum of the lower probabilities in the elements of any partition of the subset
  // only binary partitions need to be considered, the other follow by transitivity of linear constraints
  // go over all the subsets
  for (SSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i)
    // select the nontrivial ones
    if (i->size() != 1) {
      // get all the binary partitions of the selected subset
      // first generate the powerset of this subset
      SSet power_subset(powerset(*i));
      // go over these subsets
      for (SSet::const_iterator j = power_subset.begin(); j != power_subset.end(); ++j)
        // select the nontrivial ones, limit to bottom half, others will appear as complements (still some redundancy for even-sized possibility spaces)
        if (0 < j->size() && j->size() <= i->size()/2) {
          // generate the complements
          Set complement;
          std::insert_iterator<Set> complement_inserter(complement, complement.begin());
          set_difference(i->begin(),i->end(),j->begin(),j->end(),complement_inserter);

          // create the constraints
          Constraint<int> c(power_set, 0);
          c.subsets[*i] = +1;
          c.subsets[*j] = -1; c.subsets[complement] = -1;
          constraints.push_back(Constraint2string(c));
        }
    }

  return constraints;
}

list<string> weak_constraints(const Set& possibility_space, const SscSet& power_set) {  // P_ after permutation - P_ >= 0
  list<string> constraints;

  // all permutations transform sets of a certain cardinality in all sets of the same cardinality
  // we create tuples of all the sets with the same cardinalities, then form pairs of those sets
  // for each of these pairs we write a constraint (due to transitivity, we don't have to form all pairs)
  for (int m = 1; m < possibility_space.size(); ++m) { // we know in advance we don't need to look at the empty set and the possibility space, therefore <, not <=
    SSet tuple_set(tupleset(possibility_space, m));
    // we'll write constraints for all pairs of the form (*(tuple_set.begin()), *i), where i starts from the second element of the tuple_set
    for (SSet::const_iterator i = ++(tuple_set.begin()); i != tuple_set.end(); i++) {
      Constraint<int> c(power_set, 0), d(power_set, 0);
      c.subsets[*(tuple_set.begin())] = +1; c.subsets[*i] = -1;
      d.subsets[*(tuple_set.begin())] = -1; d.subsets[*i] = +1;
      constraints.push_back(Constraint2string(c));
      constraints.push_back(Constraint2string(d));
    }
  }

  return constraints;
}
