// Copyright 2005 Erik Quaeghebeur

#include <list>
  using std::list;
#include <algorithm>
  using std::set_intersection;
  using std::set_union;
#include <string>
  using std::string;

#include "k-monotonicity-constraints.hpp"
#include "constraints-classes.hpp"
#include "powerset.hpp"
  using namespace Powerset;

list<string> k_monotonicity_constraints(const Set& empty_set, const Set& possibility_space, const SscSet& power_set, const int& k) {
  list<string> constraints;

  // go over all subsets of the powerset, except the empty set
  SscSet almost_power_set(power_set);
  almost_power_set.erase(empty_set);
  for (SscSet::const_iterator i = almost_power_set.begin(); i != almost_power_set.end(); ++i) {
    // create the powerset of the 'generating set' and remove *i itself and the empty set
    SSet generating_power_set(powerset(*i));
    generating_power_set.erase(*i);

    // create the <=k-tuplesets of this set
    SSSet tuple_set(tupleset_upto(generating_power_set, k));

    // generate a constraint for each of the sets in this tupleset
    for (SSSet::const_iterator j = tuple_set.begin(); j != tuple_set.end(); ++j) {
      // sometimes, we know in advance that the constraint will be redundant
      // we explicitly keep: case |*i|=1 (plain monotonicity) [NOTE: not used as efficiently as described in paper!]
      // then we drop: case Set_intersection(*j) is an element of *j and case Set_union(*j) is not equal to *i
      if (j->size() == 1 || (Set_union(*j) == *i && j->find(Set_intersection(*j)) == j->end())) {
        // create the constraint
        Constraint<int> c(power_set, 0);

        // add the term for the 'generating set'
        c.subsets[*i] = +1;

        // create all the tuples of *j
        SSSet subtuple_set(tupleset_upto(*j, j->size()));

        // go over all these tuples
        for (SSSet::const_iterator jj = subtuple_set.begin(); jj != subtuple_set.end(); ++jj) {
          // calculate the intersection of the sets in *jj
          Set tuple_intersection(Set_intersection(*jj));

          // add the corresponding coefficient
          c.subsets[tuple_intersection] += (jj->size() == 2 * (jj->size() / 2))? +1 : -1;  // sign depends on the evenness of *jj
        }

        // add the generated constraint to our list
        constraints.push_back(Constraint2string(c));
      }
    }
  }

  return constraints;
}

Set Set_intersection(const SSet& setofsets) {
  Set s(*(setofsets.begin()));
  for (SSet::const_iterator i = ++(setofsets.begin()); i != setofsets.end(); ++i) {
    Set temp;
    std::insert_iterator<Set> temp_inserter(temp, temp.begin());
    set_intersection(s.begin(), s.end(), i->begin(), i->end(), temp_inserter);
    s = temp;
  }
  return s;
}

Set Set_union(const SSet& setofsets) {
  Set s(*(setofsets.begin()));
  for (SSet::const_iterator i = ++(setofsets.begin()); i != setofsets.end(); ++i) {
    Set temp;
    std::insert_iterator<Set> temp_inserter(temp, temp.begin());
    set_union(s.begin(), s.end(), i->begin(), i->end(), temp_inserter);
    s = temp;
  }
  return s;
}
