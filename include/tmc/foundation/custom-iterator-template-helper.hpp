// Copyright Thomas Maierhofer Consulting, Bad Waldsee, Germany
// Licensed under MIT 

#ifndef _tmc_foundation_custom_iterator_template_helper_hpp_
#define _tmc_foundation_custom_iterator_template_helper_hpp_
#include "custom-iterator-template.hpp"
       
// Use this define to declare both:
// - `iterator`
// - `const_iterator`:
#define SETUP_ITERATORS(STATE_STRUCT) \
  SETUP_MUTABLE_ITERATOR(STATE_STRUCT) \
  SETUP_CONST_ITERATOR(STATE_STRUCT)

// Use this define to declare only `iterator`
#define SETUP_MUTABLE_ITERATOR(STATE_STRUCT) \
  typedef tmc::foundation::custom_iterator_template< STATE_STRUCT , false> iterator; \
  iterator begin() { return iterator::begin(this); } \
  iterator end()   { return iterator::end(this);   }

// Use this define to declare only `const_iterator`
#define SETUP_CONST_ITERATOR(STATE_STRUCT) \
  typedef tmc::foundation::custom_iterator_template<STATE_STRUCT, true> const_iterator; \
  const_iterator cbegin() const { return const_iterator::begin(this); } \
  const_iterator cend()   const { return const_iterator::end(this);   }


// Use this define to declare both:
// - `reverse_iterator`
// - `const_reverse_iterator`:
#define SETUP_REVERSE_ITERATORS(STATE_STRUCT) \
  SETUP_MUTABLE_RITERATOR(STATE_STRUCT) \
  SETUP_CONST_RITERATOR(STATE_STRUCT)

// Use this define to declare only `reverse_iterator`
// `begin()` and `end()` must be defined
#define SETUP_MUTABLE_RITERATOR(STATE_STRUCT) \
  typedef std::reverse_iterator<tmc::foundation::custom_iterator_template<STATE_STRUCT, false>> reverse_iterator; \
  reverse_iterator rbegin() { return reverse_iterator(end()); } \
  reverse_iterator rend()   { return reverse_iterator(begin()); } 

// Use this define to declare only `const_reverse_iterator`
// `cbegin()` and `cend()` must be defined
#define SETUP_CONST_RITERATOR(STATE_STRUCT) \
  typedef std::reverse_iterator<tmc::foundation::custom_iterator_template<STATE_STRUCT, true>> const_reverse_iterator; \
  const_reverse_iterator rbegin() const {return const_reverse_iterator(cend()); } \
  const_reverse_iterator rend() const { return const_reverse_iterator(cbegin()); }

namespace tmc {
namespace foundation {
        
} // namespace foundation
}  // namespace tmc
#endif // _tmc_foundation_custom_iterator_template_hpp_