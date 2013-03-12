// Copyright 2005 Erik Quaeghebeur

#include <list>
  using std::list;
#include <algorithm>
  using std::set_union;
  using std::set_intersection;
#include <string>
  using std::string;

#include "gsl/gsl_vector.h"
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_linalg.h"

#include "coherence-constraints.hpp"
#include "constraints-classes.hpp"
#include "powerset.hpp"
  using namespace Powerset;

list<string> coherence_constraints(const Set& empty_set, const Set& possibility_space, const SscSet& power_set) {
  list<string> constraints;
  int cardinality(possibility_space.size());

  // remove the empty set and possibility space from the power set to create an almost power set
  SSet almost_power_set;
  for (SscSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i)
    if (i->size() > 0 && i->size() < cardinality)
      almost_power_set.insert(*i);

  // basic coherence constraints, prerequisites for Walley (1991), Theorem A3

  // bound the probabilities to [0,1] by setting P_(empty set) = 0 and P_(possibility space) = 1
  {
    // P_(empty set) = 0
    Constraint<int> c(power_set, 0);
    c.subsets[empty_set] = +1; constraints.push_back(Constraint2string(c));  // P_(empty set) >= 0
    c.subsets[empty_set] = -1; constraints.push_back(Constraint2string(c));  // P_(empty set) <= 0

    // P_(possibility space) = 1
    c.subsets[empty_set] = 0;
    c.constant = -1; c.subsets[possibility_space] = +1; constraints.push_back(Constraint2string(c));  // P_(possibility space) >= 1
    c.constant = +1; c.subsets[possibility_space] = -1; constraints.push_back(Constraint2string(c));  // P_(possibility space) <= 1
  }

  // ensure betting rate, i.e., for each nontrivial subset A: 0 <= P_(A) <= 1
  // go over all the subsets
  for (SSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i)
    // check for nontriviality (i.e., non-empty and non-possibility space)
    if (i->size() != 0 && i->size() != possibility_space.size()) {
      Constraint<int> c(power_set, 0);
      // add >= 0 constraint
      c.subsets[*i] = +1; constraints.push_back(Constraint2string(c));
      // add <= 1 constraint
      c.constant = +1; c.subsets[*i] = -1; constraints.push_back(Constraint2string(c));
    }

  // constraints ensuring coherency: Walley (1991), Theorem A3 (i) (avoiding sure loss), and (iii)

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
    // create a linear system Ax = b for the tuples under scutiny
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

      // how many components of the solution are negative?
      // are all components non-zero?
      int negative(0);
      bool nonzero(true);
      for(int l = 0; l != tuplicity; ++l)
        if (gsl_vector_get(x, l) < tol) {
          if (gsl_vector_get(x, l) < -tol)
            ++negative;
          else
            nonzero = false;
        }

      if (nonzero && negative <= 1) {  // check if all components are strictly positive A3(i), or only one is strictly negative A3(iii)
        // create the constraint corresponding to the tuple and solution that satisfy all conditions
        Constraint<double> c(power_set, 0);
        c.constant = 1;
        int row(0);
        for (SSet::const_iterator j = i->begin(); j != i->end(); ++j, ++row)
          c.subsets[*j] = - gsl_vector_get(x, row);
        // add this constraint to our list
        constraints.push_back(Constraint2string(double2int(power_set, c)));
      }
    }
    // remove the containers
    gsl_matrix_free(A); gsl_vector_free(S); gsl_matrix_free(V); gsl_vector_free(x); gsl_vector_free(b);
  }

  // constraints ensuring coherency: Walley (1991), Theorem A3 (ii)
  for (SSet::const_iterator subset_iter = almost_power_set.begin(); subset_iter != almost_power_set.end(); ++subset_iter)
    if (subset_iter->size() > 1) {
      // some useful initializations
      int subset_cardinality(subset_iter->size());
      SscSet subset_power_set(ordered_powerset<SizeComp>(*subset_iter));
      SSet subset_almost_power_set;
      for (SscSet::const_iterator i = subset_power_set.begin(); i != subset_power_set.end(); ++i)
        if (i->size() > 0 && i->size() < cardinality)
          subset_almost_power_set.insert(*i);

      // we need a translation table for the subsets of the subset under scrutiny
      TranslationTable table(*subset_iter, subset_power_set);

      // generate all tupels of the set under scrutiny *subset_iter (without the trivially irrelevant)
      SSSet subset_tuple_set;
      for (int m = 2; m <= subset_cardinality; ++m) {
        SSSet t(tupleset(subset_almost_power_set, m));
        subset_tuple_set.insert(t.begin(), t.end());
      }

      // remove all tuples that are not "space filling"
      SSSet subset_filltuple_set;
      for (SSSet::const_iterator i = subset_tuple_set.begin(); i != subset_tuple_set.end(); ++i) {
        Set tuple_union(*(i->begin()));
        for (SSet::const_iterator j = ++(i->begin()); j != i->end(); ++j) {
          Set temp;
          std::insert_iterator<Set> temp_inserter(temp, temp.begin());
          set_union(tuple_union.begin(), tuple_union.end(), j->begin(), j->end(), temp_inserter);
          tuple_union = temp;
        }
        if (tuple_union.size() == subset_cardinality)
          subset_filltuple_set.insert(*i);
      }

      // go over all the remaining tuples
      for (SSSet::const_iterator i = subset_filltuple_set.begin(); i != subset_filltuple_set.end(); ++i) {
        int tuplicity(i->size());
        // create a linear system Ax = b for the tuples under scutiny
        // create A, and initialize the values using the translation table
        gsl_matrix* A(gsl_matrix_alloc(subset_cardinality, tuplicity));
        int column(0);
        for (SSet::const_iterator j = i->begin(); j != i->end(); ++j, ++column) {
          list<int> indicator(table.translate(*j));
          int row(0);
          for (list<int>::const_iterator k = indicator.begin(); k != indicator.end(); ++k, ++row)
            gsl_matrix_set(A, row, column, *k);
        }
        // create b, and initialize the values corresponding to the subset under scrutiny using the translation table
        gsl_vector* b(gsl_vector_alloc(subset_cardinality));
        list<int> indicator(table.translate(*subset_iter));
        int row(0);
        for (list<int>::const_iterator j = indicator.begin(); j != indicator.end(); ++j, ++row)
          gsl_vector_set(b, row, *j);
        // create x, the unknowns (left uninitialized)
        gsl_vector* x(gsl_vector_alloc(tuplicity));

        // perform a singular value decomposition of the tuple to later check it is nonsingular
        // first create the necessary containers
        gsl_matrix* U(gsl_matrix_alloc(subset_cardinality, tuplicity));
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
        if (tuplicity < subset_cardinality) {
          gsl_vector_set_all(SS, 1);
          // perform a singular value decomposition of the tuple + the objective to later check that it is singular
          // first create the necessary containers
          gsl_matrix* UU(gsl_matrix_alloc(subset_cardinality, tuplicity + 1));
          gsl_matrix_set_col(UU, tuplicity, b);
          for (int l = 0; l != tuplicity; ++l) {
            gsl_vector* temp(gsl_vector_alloc(subset_cardinality));
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
            c.subsets[*subset_iter] = 1;
            int row(0);
            for (SSet::const_iterator j = i->begin(); j != i->end(); ++j, ++row)
              c.subsets[*j] = - gsl_vector_get(x, row);
            // add this constraint to our list
            constraints.push_back(Constraint2string(double2int(power_set, c)));
          }
        }
        // remove the containers
        gsl_matrix_free(A); gsl_vector_free(S); gsl_matrix_free(V); gsl_vector_free(x); gsl_vector_free(b);
      }
    }

  return constraints;
}
