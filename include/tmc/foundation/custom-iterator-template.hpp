// Copyright Thomas Maierhofer Consulting, Bad Waldsee, Germany
// Licensed under MIT 

#ifndef _tmc_foundation_custom_iterator_template_hpp_
#define _tmc_foundation_custom_iterator_template_hpp_

#include <iterator>

namespace tmc {
namespace foundation {

template<template<bool> typename TIteratorState, bool is_const>
struct custom_iterator_template {

    static_assert(std::is_base_of<std::input_iterator_tag, typename TIteratorState<is_const>::iterator_category>::value, "Iterator category must be/derive from std::forward_iterator_tag");

    static_assert(std::is_member_function_pointer<decltype(&TIteratorState<is_const>::begin)>::value,"begin() is not defined in iterator state"); 



    typedef typename TIteratorState<is_const>::iterator_category    iterator_category;
    typedef typename TIteratorState<is_const>::container_type       container_type;
    typedef typename TIteratorState<is_const>::value_type           value_type;
    typedef typename TIteratorState<is_const>::value_type &         element_access_type;
    typedef typename std::ptrdiff_t                                 difference_type;
    typedef typename TIteratorState<is_const>::value_type *         pointer;
    typedef typename TIteratorState<is_const>::value_type &         reference;

    // *** Start and End Positions ***
    static custom_iterator_template begin(container_type *ref) {
        custom_iterator_template it(ref);
        it.begin();
        return it;
    }

    static custom_iterator_template end(container_type *ref) {
        custom_iterator_template it(ref);
        it.end();
        return it;
    }


    // *** construction ***
    inline custom_iterator_template() = default;

    inline custom_iterator_template(const custom_iterator_template & source): iteratorState_(source.iteratorState_) {}

    // Implicit Cast changeable -> const
    template<typename T= const custom_iterator_template<TIteratorState,false> &>
    inline custom_iterator_template(typename std::enable_if<is_const, T>::type source): iteratorState_(source.iteratorState_) {}

    // *** Element Access ***
    inline element_access_type operator*() { return this->get(); }
    inline pointer operator->() { return &(operator*()); }
    element_access_type operator[](difference_type offset) { return at(offset); }


    // *** Increment / Decrement ***
    inline custom_iterator_template &operator++() {
        this->next();
        return *this;
    }

    inline custom_iterator_template operator++(int) {
        auto result = *this;
        this->next();
        return result;
    }

    inline custom_iterator_template &operator--() {
        this->prev();
        return *this;
    }

    inline custom_iterator_template operator--(int) {
        auto result = *this;
        this->prev();
        return result;
    }


    // *** Random Access ***
    custom_iterator_template& operator+=(difference_type offset) { move(offset); return *this; }
    custom_iterator_template& operator-=(difference_type offset) { move(-offset); return *this; }

    custom_iterator_template operator+(difference_type offset) {
        custom_iterator_template result = *this;
        result += offset;
        return result;
    }

    friend custom_iterator_template operator + (difference_type offset, const custom_iterator_template &rhs) {
        custom_iterator_template result = rhs;
        result += offset;
        return result;
    }

    custom_iterator_template operator-(difference_type offset) const {
        custom_iterator_template result = *this;
        result -= offset;
        return result;
    }


    difference_type operator-(const custom_iterator_template& rhs) const { return distance(rhs); }


    // *** Comparison with const and changeable iterators ***
    inline bool operator!=(const custom_iterator_template<TIteratorState, true> &other) const {
        return !this->is_equal(other);
    }

    inline bool operator!=(const custom_iterator_template<TIteratorState, false> &other) const {
        return !this->is_equal(other);
    }

    inline bool operator==(const custom_iterator_template<TIteratorState, true> &other) const {
        return this->is_equal(other);
    }

    inline bool operator==(const custom_iterator_template<TIteratorState, false> &other) const {
        return this->is_equal(other);
    }


    // *** Relations with const and changeable iterators ***
    inline bool operator<(const custom_iterator_template<TIteratorState, false> &other) const {
        return this->distance(other) < 0;
    }

    inline bool operator<=(const custom_iterator_template<TIteratorState, false> &other) const {
        return this->distance(other) <= 0;
    }

    inline bool operator>(const custom_iterator_template<TIteratorState, false> &other) const {
        return this->distance(other) > 0;
    }

    inline bool operator>=(const custom_iterator_template<TIteratorState, false> &other) const {
        return this->distance(other) >= 0;
    }

    // Status and Helpers
    inline bool is_connected() { return this->iteratorState_.is_connected(); }

    // Allow access for the corresponding changeable/const implementation
    friend class custom_iterator_template<TIteratorState, !is_const>;

private:
    // Construction with container conenction  - must be implemented in state for all kind of iterators
    custom_iterator_template(container_type *ref) : iteratorState_(ref) {}

    // Begin of collection - must be implemented in state for all kind of iterators
    inline void begin() { this->iteratorState_.begin(); }

    // Behind end of collection - must be implemented in state for all kind of iterators
    inline void end() { this->iteratorState_.end(); }

    // Move to next element - must be implemented in state for all kind of iterators
    inline void next() { this->iteratorState_.next(); }

    // Element access  - must be implemented in state for all kind of iterators
    inline element_access_type get() { return this->iteratorState_.get(); }
    inline const element_access_type get() const { return this->iteratorState_.get(); }

    // Comparison with const iterator - must be implemented in state for all kind of iterators
    inline bool is_equal(const custom_iterator_template<TIteratorState, true> &other) const {
        if(!this->iteratorState_.is_connected()) {
            if( other.iteratorState_.is_connected()) {
                return false;
            }

            return true;
        }

        if(!other.iteratorState_.is_connected() && this->iteratorState_.is_connected()) {
            return false;
        }

        return this->iteratorState_.is_equal(other.iteratorState_); }

    // Comparison with changeable iterator - must be implemented in state for all kind of iterators
    inline bool is_equal(const custom_iterator_template<TIteratorState, false> &other) const {
        if(!this->iteratorState_.is_connected()) {
            if( other.iteratorState_.is_connected()) {
                return false;
            }

            return true;
        }

        if(!other.iteratorState_.is_connected() && this->iteratorState_.is_connected()) {
            return false;
        }

        return this->iteratorState_.is_equal(other.iteratorState_);
    }

    // Move to previous element - must be implemented in state for bidirectional and random access iterators
    inline void prev() { this->iteratorState_.prev(); }

    // Move to arbitrary position - must be implemented in state for random access iterators
    inline void move(std::ptrdiff_t offset) { this->iteratorState_.move(offset); }

    // Element access at offset - must be implemented in state for random access iterators
    inline element_access_type at(std::ptrdiff_t offset) { return this->iteratorState_.at(offset); }
    inline const element_access_type at(std::ptrdiff_t offset) const { return this->iteratorState_.at(offset); }

    // Distance from const iterator - must be implemented in state for random access iterators
    inline std::ptrdiff_t distance(const custom_iterator_template<TIteratorState, true> & rhs) const {
        return this->iteratorState_.distance(rhs.iteratorState_);
    }

    // Distance from changeable iterator - must be implemented in state for random access iterators
    inline std::ptrdiff_t distance(const custom_iterator_template<TIteratorState, false> & rhs) const {
        return this->iteratorState_.distance(rhs.iteratorState_);
    }

    TIteratorState<is_const> iteratorState_;
};

} // namespace foundation
}  // namespace tmc
#endif