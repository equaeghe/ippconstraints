// Copyright 2005 Erik Quaeghebeur

#include <iostream>

using std::cout;
using std::endl;

int wrong_cardinality_text() {
  cout << endl
       << "Wrong cardinality, make sure it's at least 1;\n" << "run the program without arguments for more information." << endl
       << endl;
}

int help_text() {
  cout << endl
       << "To calculate the constraints for the set of lower probabilities that satisfies some requirements,\n" << "you have to add the following arguments:" << endl
       << "\t * a positive integer, the cardinality of the possibility space," << endl
       << "\t * a code for the required properties, separated by a space:" << endl
       << "\t\t - 'o' for the one-normed constraints, i.e., P_(possibility space)=1," << endl
       << "\t\t - 'z' for the zero-normed constraints, i.e., P_(empty set)=0," << endl
       << "\t\t - 'p' for the positivity constraints, i.e., P_(set)>=0, for all sets" << endl
       << "\t\t - 'a' for avoiding sure loss," << endl
       << "\t\t - 'c' for coherence," << endl
       << "\t\t - 'l' for additivity," << endl
       << "\t\t - 's' for superadditivity," << endl
       << "\t\t - one number k>0 for k-monotonicity (1-monotonicity is plain monotonicity)," << endl
       << "\t\t - 'w' for weak permutation invariance," << endl
       << "\t\t - 'n' for necessity measure constraints (one of the permutations only)." << endl
       << "\t The order of these property codes is unimportant." << endl
       << endl
       << "An example: \"constraints 3 w 2\" returns the constraints for the set of weak permutation invariant 2-monotone lower probabilities on a possibility space of cardinality 3." << endl
       << endl;
}

int too_few_arguments_text() {
    cout << endl
         << "You have to give at least two arguments;\n" << "run the program without arguments for more information." << endl
         << endl;
}

void wrong_argument_text(const char* s) {
    cout << endl
         << "The argument \"" << s << "\" is incorrect;\n" << "run the program without arguments for more information." << endl
         << endl;
}
