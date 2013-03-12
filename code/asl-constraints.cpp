// Copyright 2005 Erik Quaeghebeur

#include <list>
  using std::list;
#include <algorithm>
  using std::set_union;
  using std::set_intersection;
#include <map>
  using std::map;
#include <string>
  using std::string;

#include "gsl/gsl_vector.h"
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_linalg.h"

#include "asl-constraints.hpp"
#include "constraints-classes.hpp"
#include "powerset.hpp"
  using namespace Powerset;

list<string> asl_constraints(const Set& empty_set, const Set& possibility_space, const SscSet& power_set) {
  list<string> constraints;
  list<Constraint<int> > constraints_for_positive;
  int cardinality(possibility_space.size());

  // remove the empty set and possibility space from the power set to create an almost power set
  SSet almost_power_set;
  for (SscSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i)
    if (i->size() > 0 && i->size() < cardinality)
      almost_power_set.insert(*i);

  // basic asl constraint, (more or less) prerequisites for Walley (1991), Theorem A3 (i)

  // bound the probabilities from above
  // by setting P_(empty set) <= 0
  {
    Constraint<int> c(power_set, 0);
    c.subsets[empty_set] = -1; constraints.push_back(Constraint2string(c));
  }
  // by setting P_(A) <= 1, where A is different from the empty set
  // go over all the subsets
  for (SSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i)
    // check for nonemptyness
    if (i->size() != 0) {
      Constraint<int> c(power_set, 0);
      c.constant = +1; c.subsets[*i] = -1; constraints.push_back(Constraint2string(c));
    }

  // constraints ensuring asl: Walley (1991), Theorem A3 (i), suppose all P_(A) >= 0

  // we need a translation table for the subsets of the possibility space
  TranslationTable table(possibility_space, power_set);

  // generate all tuples of the possibility_space (without the trivially irrelevant)
  SSSet tuple_set;
  for (int m = 2; m <= cardinality; ++m) {
    SSSet t(tupleset(almost_power_set, m));
    tuple_set.insert(t.begin(), t.end());
  }

  // remove all tuples that are not "space filling"
  SSSet filltuple_set;
  for (SSSet::const_iterator i = tuple_set.begin(); i != tuple_set.end(); ++i) {
    Set tuple_union(*(i->begin()));
    for (SSet::const_iterator j = ++(i->begin()); j != i->end(); ++j) {
      Set temp;
      std::insert_iterator<Set> temp_inserter(temp, temp.begin());
      set_union(tuple_union.begin(), tuple_union.end(), j->begin(), j->end(), temp_inserter);
      tuple_union = temp;
    }
    if (tuple_union.size() == cardinality)
      filltuple_set.insert(*i);
  }

  // go over all the remaining tuples
  for (SSSet::const_iterator i = filltuple_set.begin(); i != filltuple_set.end(); ++i) {
    int tuplicity(i->size());
    // create a linear system Ax = b for the tuples under scrutiny
    // create A, and initialize the values using the translation table
    gsl_matrix* A(gsl_matrix_alloc(cardinality, tuplicity));
    int column(0);
    for (SSet::const_iterator j = i->begin(); j != i->end(); ++j, ++column) {
      list<int> indicator(table.translate(*j));
      int row(0);
      for (list<int>::const_iterator k = indicator.begin(); k != indicator.end(); ++k, ++row)
        gsl_matrix_set(A, row, column, *k);
    }

    // create b, and initialize the values corresponding to the subset under scrutiny using the translation table
    gsl_vector* b(gsl_vector_alloc(cardinality));
    int row(0);
    list<int> indicator(table.translate(possibility_space));
    for (list<int>::const_iterator j = indicator.begin(); j != indicator.end(); ++j, ++row)
      gsl_vector_set(b, row, *j);
    // create x, the unknowns (left uninitialized)
    gsl_vector* x(gsl_vector_alloc(tuplicity));

    // perform a singular value decomposition of the tuple to later check it is nonsingular
    // first create the necessary containers
    gsl_matrix* U(gsl_matrix_alloc(cardinality, tuplicity));
    gsl_matrix_memcpy(U, A);
    gsl_matrix* V(gsl_matrix_alloc(tuplicity, tuplicity));
    gsl_vector* S(gsl_vector_alloc(tuplicity));
    gsl_vector* work(gsl_vector_alloc(tuplicity));
    // do the decomposition (result saved in U, V, S)
    gsl_linalg_SV_decomp(U, V, S, work);
    // remove unnecessary container
    gsl_vector_free(work);

    gsl_vector* SS(gsl_vector_alloc(tuplicity + 1));
    gsl_vector_set_all(SS, 0);
    if (tuplicity < cardinality) {
      gsl_vector_set_all(SS, 1);
      // perform a singular value decomposition of the tuple + the objective to later check that it is singular
      // first create the necessary containers
      gsl_matrix* UU(gsl_matrix_alloc(cardinality, tuplicity + 1));
      gsl_matrix_set_col(UU, tuplicity, b);
      for (int l = 0; l != tuplicity; ++l) {
        gsl_vector* temp(gsl_vector_alloc(cardinality));
        gsl_matrix_get_col(temp, A, l);
        gsl_matrix_set_col(UU, l, temp);
        gsl_vector_free(temp);
      }
      gsl_matrix* VV(gsl_matrix_alloc(tuplicity + 1, tuplicity + 1));
      gsl_vector* workwork(gsl_vector_alloc(tuplicity + 1));
      // do the decomposition (result saved in U, V, S)
      gsl_linalg_SV_decomp(UU, VV, SS, workwork);
      // remove unnecessary container
      gsl_vector_free(workwork);
    }

    if (gsl_vector_get(S, tuplicity - 1) > tol && gsl_vector_get(SS, tuplicity) < tol) { // verify if A is singular by seeing if the smallest singular value (last element of S by construction) is strictly positive and that Ab is singular by seeing if the smallest value is zero
      // solve Ax = b for x using the singular value decomposition
      gsl_linalg_SV_solve(U, V, S, b, x);

      if (gsl_vector_min(x) > tol) {  // check if all the components of the solution are strictly positive
        // create the constraint corresponding to the tuple and solution that satisfy all conditions
        Constraint<double> c(power_set, 0);
        c.constant = 1;
        int row(0);
        for (SSet::const_iterator j = i->begin(); j != i->end(); ++j, ++row)
          c.subsets[*j] = - gsl_vector_get(x, row);
        // add this constraint to our list
        constraints_for_positive.push_back(double2int(power_set, c));
      }
    }
    // remove the containers
    gsl_matrix_free(A); gsl_vector_free(S); gsl_matrix_free(V); gsl_vector_free(x); gsl_vector_free(b);
  }

  // now also allow P_(A) <= 0, i.e., modify the constraints for positive P_(A) depending on the possible number of negavtive P_(A)
  // go over all these constraints
  for (list<Constraint<int> >::const_iterator i = constraints_for_positive.begin(); i != constraints_for_positive.end(); ++i) {
    // for which subsets A did we suppose P_(A) >= 0?  Gather these.
    SSet included_subsets;
    for (map<Set, int>::const_iterator j = i->subsets.begin(); j != i->subsets.end(); ++j)
      if (j->second != 0)
        included_subsets.insert(j->first);
    // calculate all the possible combinations of included subsets for which we can now assume P_(A) <= 0
    SSSet some_included_subsets(powerset(included_subsets));

    // for each of these combinations, modify the original constraint by setting the corresponding coefficients to 0 and add the resulting constraint (creates a large number of redundant constraints)
    some_included_subsets.erase(included_subsets); // remove the set of included subsets itself, gives an identity 1 >= 0;
    for (SSSet::const_iterator j = some_included_subsets.begin(); j != some_included_subsets.end(); ++j) {
      if (j->size() < included_subsets.size() - 1) { // avoid redundand constraints, which we've given as prerequisites
        Constraint<int> c(*i);
        for (SSet::const_iterator k = j->begin(); k != j->end(); ++k)
          c.subsets[*k] = 0;
        constraints.push_back(Constraint2string(c));
      }
    }
  }

  return constraints;
}
