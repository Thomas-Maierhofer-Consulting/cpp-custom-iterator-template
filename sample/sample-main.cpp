// Copyright Thomas Maierhofer Consulting, Bad Waldsee, Germany
// Licensed under MIT 

#include<iostream>

#include <tmc/foundation/custom-iterator-template-helper.hpp>

struct SampleElement{
    int member_;
};

class SampleContainer{

    SampleContainer(SampleElement * elements, size_t elementCount): elements_{elements}, elementCount_{elementCount} {}

    private:
    SampleElement * elements_; 
    size_t elementCount_;

            template<bool is_const>
            struct iterator_state {
                typedef std::random_access_iterator_tag iterator_category;
                typedef typename std::conditional<is_const, const SampleContainer, SampleContainer>::type        container_type;
                typedef typename std::conditional<is_const, const SampleElement, SampleElement>::type            value_type;

                container_type * container_;
                ptrdiff_t current_{-1};

                // Default Construction without container connection (ALL Iterators)
                inline iterator_state(): container_(nullptr) {}

                // Construction with connected container; (ALL Iterators)
                inline iterator_state(container_type * container): container_(container) {}


                // Copy Construction from the changeble and const variants - allows changeble to const and vice versa assignment (ALL Iterators)
                inline iterator_state(const iterator_state<true> & source): container_(source.container_), current_(source.current_) {}
                inline iterator_state(const iterator_state<false> & source): container_(source.container_), current_(source.current_) {}

                // Destruction (ALL Iterators)
                inline ~iterator_state() {current_ = -1; container_ = nullptr; }

                // Start and End Positions (ALL Iterators)
                inline void begin() {current_ = 0; }
                inline void end() {current_ = container_->elementCount_; }

                // Availability and Equality (ALL Iterators)
                inline bool is_connected() const { return container_ != nullptr; }
                inline bool is_equal(const iterator_state<true> & other) const { return current_ == other.current_; }
                inline bool is_equal(const iterator_state<false> & other) const { return current_ == other.current_; }


                // Move Next (ALL Iterators)
                inline void next() { ++current_; }


                // Element Access (ALL Iterators)
                template<typename T = value_type>
                inline typename std::enable_if<! is_const, T>::type & get() {return container_->elements_[current_]; }

                template<typename T = value_type>
                inline typename std::enable_if<is_const, T>::type & get() const {return container_->elements_[current_]; }


                // Move Previous (Bidirectional, Random Access Iterators)
                inline void prev() { --current_; }
            };

    public:
            SETUP_ITERATORS(iterator_state);
            SETUP_REVERSE_ITERATORS(iterator_state);
};


auto main(int argc, char **argv) -> int{
    std::cout << "C++ Custom Iterator Template Sample" << std::endl;
}