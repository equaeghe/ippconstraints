// Copyright 2005 Erik Quaeghebeur

#include "powerset.hpp"

template<class Element>
std::set<std::set<Element> > Powerset::powerset(const std::set<Element>& in) {
  typedef std::set<Element> Subset;
  typedef typename std::set<Subset>::const_iterator SetIter;
  typedef typename Subset::const_iterator SubsetIter;
  std::set<Subset> out;

  if (in.empty()) { // if 'in' is empty, only add the empty set
    Subset s; // create empty set
    out.insert(s);
    return out;
  }

  else {  // if 'in' is not empty, use recursion
    Subset s(in); // copy 'in' into 's'
    for (SubsetIter i = in.begin(); i != in.end(); ++i) { // go over all the elements of 'in'
      s.erase(s.begin()); // remove the current element '*i' from 's' (N.B.: *i == *s.begin() by construction)

      std::set<Subset> p(powerset(s)); // calculate the powerset of this smaller set (recursion step)

      // add the sets formed after adding the element under scrutiny
      std::set<Subset> q(p);
      for (SetIter j = p.begin(); j != p.end(); ++j) {
        Subset ss(*j);
        ss.insert(*i);
        q.insert(ss);
      }
      out.insert(q.begin(), q.end()); // add all these sets to 'out'
    }
    return out;
  }
}

template<class Compare, class Element>
std::set<std::set<Element>, Compare> Powerset::ordered_powerset(const std::set<Element>& in) {
  typedef std::set<Element> Subset;
  typedef typename std::set<Subset, Compare>::const_iterator SetIter;
  typedef typename Subset::const_iterator SubsetIter;
  std::set<Subset, Compare> out;

  if (in.empty()) { // if 'in' is empty, only add the empty set
    Subset s; // create empty set
    out.insert(s);
    return out;
  }

  else {  // if 'in' is not empty, use recursion
    Subset s(in); // copy 'in' into 's'
    for (SubsetIter i = in.begin(); i != in.end(); ++i) { // go over all the elements of 'in'
      s.erase(s.begin()); // remove the current element '*i' from 's' (N.B.: *i == *s.begin() by construction)

      std::set<Subset, Compare> p(ordered_powerset<Compare>(s)); // calculate the powerset of this smaller set (recursion step)

      // add the sets formed after adding the element under scrutiny
      std::set<Subset, Compare> q(p);
      for (SetIter j = p.begin(); j != p.end(); ++j) {
        Subset ss(*j);
        ss.insert(*i);
        q.insert(ss);
      }
      out.insert(q.begin(), q.end()); // add all these sets to 'out'
    }
    return out;
  }
}

template<class Element>
std::set<std::set<Element> > Powerset::tupleset(const std::set<Element>& in, const int& k) {
  typedef std::set<Element> Subset;
  typedef typename std::set<Subset>::const_iterator SetIter;
  typedef typename Subset::const_iterator SubsetIter;
  std::set<Subset> out;

  if (in.size() < k)  // if 'in' is too small, return an empty tupleset
    return out;

  else
    if (k == 1) { // if 1-tuples are asked, add the atoms of 'in'
      for (SubsetIter i = in.begin(); i != in.end(); ++i) {
        Subset ss;
        ss.insert(*i);
        out.insert(ss);
      }
      return out;
    }

    else {  // if k > 1, use recursion
      Subset s(in); // copy 'in' into 's'
      for (SubsetIter i = in.begin(); i != in.end(); ++i) { // go over all the elements of 'in'
        s.erase(s.begin()); // remove the current element '*i' from 's' (N.B.: *i == *s.begin() by construction)

        if (k-1 <= s.size()) {
          std::set<Subset> p(tupleset(s, k - 1)); // calculate the tupleset of this smaller set (recursion step)

          // generate the sets formed after adding the element under scrutiny
          std::set<Subset> q;
          for (SetIter j = p.begin(); j != p.end(); ++j) {
            Subset ss(*j);
            ss.insert(*i);
            q.insert(ss);
          }
          out.insert(q.begin(), q.end()); // add these sets to 'out'
        }
      }
      return out;
    }
}

template<class Element>
std::set<std::set<Element> > Powerset::tupleset_upto(const std::set<Element>& in, const int& k) {
  typedef std::set<Element> Subset;
  typedef typename std::set<Subset>::const_iterator SetIter;
  typedef typename Subset::const_iterator SubsetIter;
  std::set<Subset> out;

  if (k == 1) { // if 1-tuples are asked, add the atoms of 'in'
    for (SubsetIter i = in.begin(); i != in.end(); ++i) {
      Subset ss;
      ss.insert(*i);
      out.insert(ss);
    }
    return out;
  }

  else {  // if k > 1, use recursion
    Subset s(in); // copy 'in' into 's'
    for (SubsetIter i = in.begin(); i != in.end(); ++i) { // go over all the elements of 'in'
      s.erase(s.begin()); // remove the current element '*i' from 's' (N.B.: *i == *s.begin() by construction)

      if (s.size() >= 1) {
        std::set<Subset> p(tupleset_upto(s, k - 1)); // calculate the tupleset of this smaller set (recursion step)

        // generate the sets formed after adding the element under scrutiny
        std::set<Subset> q;
        for (SetIter j = p.begin(); j != p.end(); ++j) {
          Subset ss(*j);
          ss.insert(*i);
          q.insert(ss);
        }
        // add these sets to 'out'
        out.insert(q.begin(), q.end()); 
      }
      // also add the set containing just the element under scrutiny
      Subset ss;
      ss.insert(*i);
      out.insert(ss);
    }
    return out;
  }
}
