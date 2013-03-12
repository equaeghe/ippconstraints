// Copyright 2005 Erik Quaeghebeur

#include <list>
  using std::list;
#include <iostream>
  using std::cout;
  using std::endl;
#include <map>
  using std::map;
#include <string>
  using std::string;

#include "print-constraints.hpp"
#include "constraints-classes.hpp"

void print_constraints_table(const Set& possibility_space, const SscSet& power_set, const list<string>& constraints, const Set& requested_properties, const int& monotonicity) {
  print_header(possibility_space, power_set, constraints.size(), requested_properties, monotonicity);
  print_constraints(constraints);
  print_footer();
}

void print_header(const Set& possibility_space, const SscSet& power_set, const list<string>::size_type& constraints_size, const Set& requested_properties, const int& monotonicity) {
  // the necessary header of the file, with some useful comments (start with *)
  cout << endl
       << "* " << "This output is formatted as follows:" << endl
       << "* \t" << "- First a line with the representation (i.e., Hyperplane)," << endl
       << "* \t" << "  then an opening line with \'begin\' (matching a closing line with 'end')." << endl
       << "* \t" << "- This is followed by a line listing the number of constraints," << endl
       << "* \t" << "  the number of coefficients, and the coefficient type." << endl
       << "* \t" << "- The main table lists the coefficients a_constant, a_set of" << endl
       << "* \t" << "  the constraints (one per line) \"a_constant + sum_sets a_set P_(set) >= 0\"." << endl
       << '*' << endl
       << "* " << "You asked constraints for" << endl
       << "* \t" << "- a possibility space of cardinality " << possibility_space.size() << endl;
  if (requested_properties.find('o') != requested_properties.end())
    cout << "* \t" << "- one-normalization" << endl;
  if (requested_properties.find('z') != requested_properties.end())
    cout << "* \t" << "- zero-normalization" << endl;
  if (requested_properties.find('p') != requested_properties.end())
    cout << "* \t" << "- positivity" << endl;
  if (requested_properties.find('a') != requested_properties.end())
    cout << "* \t" << "- avoiding sure loss" << endl;
  if (requested_properties.find('c') != requested_properties.end())
    cout << "* \t" << "- coherence" << endl;
  if (requested_properties.find('w') != requested_properties.end())
    cout << "* \t" << "- weak permutation invariance" << endl;
  if (requested_properties.find('n') != requested_properties.end())
    cout << "* \t" << "- necessity" << endl 
         << "  \t" << "  (Note that the constraints given are only for a subset," << endl
         << "  \t" << "   defined by the resulting extreme necessity measures." << endl
         << "  \t" << "   The non-convex complete set is obtained by considering" << endl
         << "  \t" << "   the union of all the permutations of this subset.)" << endl;
  if (requested_properties.find('s') != requested_properties.end())
    cout << "* \t" << "- superadditivity" << endl;
  if (requested_properties.find('l') != requested_properties.end())
    cout << "* \t" << "- additivity" << endl;
  if (monotonicity > 0)
    cout << "* \t" << "- " << monotonicity <<"-monotonicity" << endl;
  cout << '*' << endl
       << "* " << "The coefficients come in the following order (given by their indices):" << endl
       << "* " << "constant";
        for (SscSet::const_iterator i = power_set.begin(); i != power_set.end(); ++i) {
          cout << " {";
          for (Set::const_iterator j = i->begin(); j != i->end(); ++j)
            cout << *j;
          cout << "}";
        }
  cout << endl
       << '*' << endl;

  cout << "H-representation" << endl
       << "begin" << endl
       << constraints_size << ' ' << power_set.size()+1 << ' ' << "integer" << endl;
}

void print_constraints(const list<string>& constraints) {
  for (list<string>::const_iterator l = constraints.begin(); l != constraints.end(); ++l) {
    cout << *l << endl;
  }
}

void print_footer() {
  // the necessary footer of the file
  cout << "end" << endl;
}
