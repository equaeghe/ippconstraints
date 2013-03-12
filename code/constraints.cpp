// Copyright 2005 Erik Quaeghebeur

#include <set>
  using std::set;
#include <map>
  using std::map;
#include <string>
  using std::string;
#include <list>
  using std::list;
#include <cstdlib>
  using std::atoi;
  
#include "constraints-classes.hpp"
#include "constraints-text.hpp"
#include "powerset.hpp"
  using namespace Powerset;
#include "basic-constraints.hpp"  // includes monotonicity -, normed -, positivity -, linearity -, superlinearity -, and weak permutation invariance constraints
#include "k-monotonicity-constraints.hpp"
#include "print-constraints.hpp"
#include "asl-constraints.hpp"
#include "coherence-constraints.hpp"
#include "necessity-constraints.hpp"

int main(int argc, char** argv) {
  // check for correct number of arguments
  if (argc == 1) {  // if no argument is given, give the instructions
    help_text();
    return 1;
  }
  if (argc <= 2) { // if too few arguments are given, give error
    too_few_arguments_text();
    return 1;
  }

  // if the number of arguments is ok, continue with the program
  // read in the cardinality of the possibility space and check its sanity
  const int cardinality(atoi(argv[1]));
  if (cardinality < 1) {
    wrong_cardinality_text();
    return 1;
  }

  // generate the possibility space, its powerset and the empty set
  Set possibility_space;
  for (char c = 'a'; c != 'a' + cardinality; ++c)
    possibility_space.insert(c);
  const SscSet power_set(ordered_powerset<SizeComp>(possibility_space));
  const Set empty_set;

  // create a set containing all valid properties (except k-monotonicity)
  set<char> property_set; {
    property_set.insert('o'); // one-normed
    property_set.insert('z'); // zero-normed
    property_set.insert('p'); // positivity
    property_set.insert('a'); // avoiding sure loss
    property_set.insert('c'); // coherence
    property_set.insert('l'); // additivity
    property_set.insert('s'); // superadditivity
    property_set.insert('w'); // weak permutation invariance
    property_set.insert('n'); // necessity
  }
  // read in the requested properties
  set<char> requested_properties; // store the requested properties here (except k-monotonicity)
  int k_monotonicity(0);  // if k-monotonicity is requested, we'll store k in here
  for(int i = 2; i < argc; ++i){
    string temp_string(argv[i]);
    if (temp_string.size() == 1  && property_set.find(argv[i][0]) != property_set.end())
      requested_properties.insert(argv[i][0]);  // add the property (as a char, all valid properties are char* of length 1)
    else  // check for k-monotonicity
      if (k_monotonicity == 0 && atoi(argv[i]) > 0) // only one valid k-monotonicity request
        k_monotonicity = atoi(argv[i]);
      else {  // there is some error in the list of arguments
        wrong_argument_text(argv[i]);
        return 1;
      }
  }

  // create an empty list of constraints
  list<string> constraints;

  // generate all the constraints for the properties in requested_properties, and possibly k-monotonicicty,
  // the order is chosen with a pass of a redundant-constraint remover in mind
  if (requested_properties.find('o') != requested_properties.end()) {
    list<string> o_constraints(one_normed_constraints(possibility_space, power_set));
    constraints.insert(constraints.end(), o_constraints.begin(), o_constraints.end());
  }
  if (requested_properties.find('z') != requested_properties.end()) {
    list<string> z_constraints(zero_normed_constraints(empty_set, power_set));
    constraints.insert(constraints.end(), z_constraints.begin(), z_constraints.end());
  }
  if (requested_properties.find('p') != requested_properties.end()) {
    list<string> p_constraints(positivity_constraints(power_set));
    constraints.insert(constraints.end(), p_constraints.begin(), p_constraints.end());
  }
  if (requested_properties.find('l') != requested_properties.end()) {
    list<string> l_constraints(linearity_constraints(power_set));
    constraints.insert(constraints.end(), l_constraints.begin(), l_constraints.end());
  }
  if (requested_properties.find('s') != requested_properties.end()) {
    list<string> s_constraints(superlinearity_constraints(power_set));
    constraints.insert(constraints.end(), s_constraints.begin(), s_constraints.end());
  }
  if (requested_properties.find('w') != requested_properties.end()) {
    list<string> w_constraints(weak_constraints(possibility_space, power_set));
    constraints.insert(constraints.end(), w_constraints.begin(), w_constraints.end());
  }
  if (k_monotonicity > 0) {
    list<string> k_m_constraints(k_monotonicity_constraints(empty_set, possibility_space, power_set, k_monotonicity));
    constraints.insert(constraints.end(), k_m_constraints.begin(), k_m_constraints.end());
  }
  if (requested_properties.find('a') != requested_properties.end()) {
    list<string> a_constraints(asl_constraints(empty_set, possibility_space, power_set));
    constraints.insert(constraints.end(), a_constraints.begin(), a_constraints.end());
  }
  if (requested_properties.find('c') != requested_properties.end()) {
    list<string> c_constraints(coherence_constraints(empty_set, possibility_space, power_set));
    constraints.insert(constraints.end(), c_constraints.begin(), c_constraints.end());
  }
  if (requested_properties.find('n') != requested_properties.end()) {
    list<string> n_constraints(necessity_constraints(empty_set, possibility_space, power_set));
    constraints.insert(constraints.end(), n_constraints.begin(), n_constraints.end());
  }

  // print out the constraints in a format usable by cddlib and lrslib
  print_constraints_table(possibility_space, power_set, constraints, requested_properties, k_monotonicity);

  return 0;
}
