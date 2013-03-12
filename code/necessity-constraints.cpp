// Copyright 2005 Erik Quaeghebeur

#include <list>
  using std::list;
#include <algorithm>
  using std::set_difference;
#include <string>
  using std::string;

#include "necessity-constraints.hpp"
#include "constraints-classes.hpp"
#include "powerset.hpp"
  using namespace Powerset;

list<string> necessity_constraints(const Set& empty_set, const Set& possibility_space, const SscSet& power_set) {
  list<string> constraints;

  // we order the lower probabilities for the one-below-pspace sets according to the element that is missing
  // the lowest one is 0 (equality constraint), the probability of the possibility space is larger than all
  // create the set we're interested in
  Set previous(possibility_space);
  previous.erase(*(possibility_space.begin()));
  {
    // create the constraint
    Constraint<int> c(power_set, 0), d(power_set, 0);

    // add the coefficient for the set under scrutiny
    c.subsets[previous] = +1;
    d.subsets[previous] = -1;

    // add the generated constraint to our list
    constraints.push_back(Constraint2string(c));
    constraints.push_back(Constraint2string(d));
  }
  for (Set::const_iterator i = ++possibility_space.begin(); i != possibility_space.end(); i++) {
    Set next(possibility_space);
    next.erase(*i);

    // create the constraint
    Constraint<int> c(power_set, 0);

    // add the coefficient for the set under scrutiny
    c.subsets[next] = +1;
    c.subsets[previous] = -1;

    // add the generated constraint to our list
    constraints.push_back(Constraint2string(c));

    // make ready for next iteration
    previous = next;
  }
  // constraint for the possibility space
  {
    // create the constraint
    Constraint<int> c(power_set, 0);

    // add the coefficient for the possibility space
    c.subsets[possibility_space] = +1;

    // add the coefficient for the 'largest' one-below-pspace set
    c.subsets[previous] = -1;

    // add the generated constraint to our list
    constraints.push_back(Constraint2string(c));
  }

  // go over all the subsets except for the possibility space and add constraints for those that are not one-below-pspace
  SscSet almost_power_set(power_set);
  almost_power_set.erase(possibility_space);
  for (SscSet::const_iterator i = almost_power_set.begin(); i != almost_power_set.end(); ++i) {
    // we need to add equality constraints for all of the ones that are not one-below-pspace
    if (i->size() != possibility_space.size()-1) {
      // create the constraint
      Constraint<int> c(power_set, 0), d(power_set, 0);

      // add the coefficient for the set under scrutiny
      c.subsets[*i] = +1;
      d.subsets[*i] = -1;

      // add the coefficient of the lowest corresponding one-below-pspace set
      Set temp, one_below_pspace_set(possibility_space);
      std::insert_iterator<Set> temp_inserter(temp, temp.begin());
      set_difference(possibility_space.begin(), possibility_space.end(), i->begin(), i->end(), temp_inserter);
      one_below_pspace_set.erase(*(temp.begin()));
      c.subsets[one_below_pspace_set] = -1;
      d.subsets[one_below_pspace_set] = 1;

      // add the generated constraint to our list
      constraints.push_back(Constraint2string(c));
      constraints.push_back(Constraint2string(d));
    }
  }

  return constraints;
}
