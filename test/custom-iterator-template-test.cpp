// Copyright Thomas Maierhofer Consulting, Bad Waldsee, Germany
// Licensed under MIT 

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include <tmc/foundation/custom-iterator-template.hpp>

using namespace std;
using namespace testing;
using namespace tmc::foundation;

struct CustomElement {
    CustomElement() = default;
    CustomElement(int data): data_(data) {}

    inline bool operator==(const CustomElement &other) const { return data_ == other.data_; }

    friend ::std::ostream &operator<<(::std::ostream &stream, const CustomElement &item)
    {
        return stream << "{" << item.data_ << "}";
    }
    inline void SetValue(int data) { data_=data; }
    inline int GetValue() const { return data_; }

private:
    int data_;
};

struct CustomContainerBase {

    CustomContainerBase() = default;
    CustomContainerBase(std::initializer_list<int> values){
        for( const auto &value: values) {
            InternalData.push_back(CustomElement(value));
        }
    }

    std::vector<CustomElement> InternalData;
    mutable unsigned int IteratorConnectCount=0;
    mutable unsigned int IteratorDisconnectCount=0;

    typedef std::ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef CustomElement value_type;
    typedef CustomElement* pointer;
    typedef const CustomElement* const_pointer;
    typedef CustomElement& reference;
    typedef const CustomElement& const_reference;

    friend ::std::ostream &operator<<(::std::ostream &stream, const CustomContainerBase &item)
    {
        stream << "CustomContainer{";
        bool successor = false;
        for(auto const & elem: item.InternalData){
            if( successor ){
                stream << ", ";
            } else {
                successor = true;
            }

            stream << elem;
        }
        stream << "}";
        return stream;
    }
};

// Skeleton for Input Iterators
// https://en.cppreference.com/w/cpp/named_req/InputIterator
struct CustomContainerWithInputIterator: public CustomContainerBase {
    CustomContainerWithInputIterator() = default;
    CustomContainerWithInputIterator(std::initializer_list<int> values): CustomContainerBase(values) {}

    template<bool is_const>
    struct iterator_state {

        typedef std::input_iterator_tag iterator_category;
        typedef typename std::conditional<is_const, const CustomContainerWithInputIterator, CustomContainerWithInputIterator>::type         container_type;
        typedef typename std::conditional<is_const, const CustomElement, CustomElement>::type                                       value_type;
        typedef typename std::conditional<is_const, decltype(InternalData)::const_iterator, decltype(InternalData)::iterator>::type internal_iterator;

        container_type * container_;
        internal_iterator current_;

        // Default Construction without container connection (ALL Iterators)
        inline iterator_state(): container_(nullptr) {}

        // Construction with connected container; (ALL Iterators)
        inline iterator_state(container_type * container): container_(container) {
            ++container_->IteratorConnectCount;
        }


        // Copy Construction from the changeble and const variants - allows changeble to const and vice versa assignment (ALL Iterators)
        inline iterator_state(const iterator_state<true> & source): container_(source.container_), current_(source.current_) {}
        inline iterator_state(const iterator_state<false> & source): container_(source.container_), current_(source.current_) {}

        // Destruction (ALL Iterators)
        inline ~iterator_state() {
            current_ = internal_iterator();
            if( container_ != nullptr)
            { 
                ++container_->IteratorDisconnectCount;
                container_ = nullptr; 
            }
        }

        // Start and End Positions (ALL Iterators)
        inline void begin() {current_ = container_->InternalData.begin(); }
        inline void end() {current_ = container_->InternalData.end(); }

        // Availability and Equality (ALL Iterators)
        inline bool is_connected() const { return container_ != nullptr; }
        inline bool is_equal(const iterator_state<true> & other) const { return current_ == other.current_; }
        inline bool is_equal(const iterator_state<false> & other) const { return current_ == other.current_; }


        // Move Next (ALL Iterators)
        inline void next() { ++current_; }


        // Element Access (ALL Iterators)
        template<typename T = value_type>
        inline typename std::enable_if<! is_const, T>::type & get() {return *current_; }

        template<typename T = value_type>
        inline typename std::enable_if<is_const, T>::type & get() const {return *current_; }
    };

    typedef typename tmc::foundation::custom_iterator_template<iterator_state, false> iterator;
    typedef typename tmc::foundation::custom_iterator_template<iterator_state, true> const_iterator;

    iterator begin() { return iterator::begin(this); }
    iterator end() { return iterator::end(this); }

    const_iterator begin() const { return const_iterator::begin(this); }
    const_iterator end() const { return const_iterator::end(this); }

    const_iterator cbegin() const { return const_iterator::begin(this); }
    const_iterator cend() const { return const_iterator::end(this); }
};

// Skeleton for Forward Iterators
// https://en.cppreference.com/w/cpp/named_req/ForwardIterator
struct CustomContainerWithForwardIterator: public CustomContainerBase {

    CustomContainerWithForwardIterator() = default;
    CustomContainerWithForwardIterator(std::initializer_list<int> values): CustomContainerBase(values) {}


    template<bool is_const>
    struct iterator_state {

        // Specifing the type of the specialized iterator - these typedefs are picked up by the template to define the iterators (ALL Iterators)
        typedef std::forward_iterator_tag iterator_category;
        typedef typename std::conditional<is_const, const CustomContainerWithForwardIterator, CustomContainerWithForwardIterator>::type        container_type;
        typedef typename std::conditional<is_const, const CustomElement, CustomElement>::type            value_type;
        typedef typename std::conditional<is_const, decltype(InternalData)::const_iterator, decltype(InternalData)::iterator>::type internal_iterator;

        container_type * container_;
        internal_iterator current_;

        // Default Construction without container connection (ALL Iterators)
        inline iterator_state(): container_(nullptr) {}

        // Construction with connected container; (ALL Iterators)
        inline iterator_state(container_type * container): container_(container) {
            ++container_->IteratorConnectCount;
        }


        // Copy Construction from the changeble and const variants - allows changeble to const and vice versa assignment (ALL Iterators)
        inline iterator_state(const iterator_state<true> & source): container_(source.container_), current_(source.current_) {}
        inline iterator_state(const iterator_state<false> & source): container_(source.container_), current_(source.current_) {}

        // Destruction (ALL Iterators)
        inline ~iterator_state() {
            current_ = internal_iterator();
            if( container_ != nullptr)
            { 
                ++container_->IteratorDisconnectCount;
                container_ = nullptr; 
            }
        }

        // Start and End Positions (ALL Iterators)
        inline void begin() {current_ = container_->InternalData.begin(); }
        inline void end() {current_ = container_->InternalData.end(); }

        // Availability and Equality (ALL Iterators)
        inline bool is_connected() const { return container_ != nullptr; }
        inline bool is_equal(const iterator_state<true> & other) const { return current_ == other.current_; }
        inline bool is_equal(const iterator_state<false> & other) const { return current_ == other.current_; }


        // Move Next (ALL Iterators)
        inline void next() { ++current_; }


        // Element Access (ALL Iterators)
        template<typename T = value_type>
        inline typename std::enable_if<! is_const, T>::type & get() {return *current_; }

        template<typename T = value_type>
        inline typename std::enable_if<is_const, T>::type & get() const {return *current_; }
    };

    typedef custom_iterator_template<iterator_state, false> iterator;
    typedef custom_iterator_template<iterator_state, true> const_iterator;


    iterator begin() { return iterator::begin(this); }
    iterator end() { return iterator::end(this); }

    const_iterator begin() const { return const_iterator::begin(this); }
    const_iterator end() const { return const_iterator::end(this); }

    const_iterator cbegin() const { return const_iterator::begin(this); }
    const_iterator cend() const { return const_iterator::end(this); }
};

// Skeleton for Bidirectional Iterators
// https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
struct CustomContainerWithBidirectionalIterator: public CustomContainerBase {

    CustomContainerWithBidirectionalIterator() = default;
    CustomContainerWithBidirectionalIterator(std::initializer_list<int> values): CustomContainerBase(values) {}


    template<bool is_const>
    struct iterator_state {

        // Specifing the type of the specialized iterator - these typedefs are picked up by the template to define the iterators (ALL Iterators)
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef typename std::conditional<is_const, const CustomContainerWithBidirectionalIterator, CustomContainerWithBidirectionalIterator>::type        container_type;
        typedef typename std::conditional<is_const, const CustomElement, CustomElement>::type            value_type;
        typedef typename std::conditional<is_const, decltype(InternalData)::const_iterator, decltype(InternalData)::iterator>::type internal_iterator;

        container_type * container_;
        internal_iterator current_;

        // Default Construction without container connection (ALL Iterators)
        inline iterator_state(): container_(nullptr) {}

        // Construction with connected container; (ALL Iterators)
        inline iterator_state(container_type * container): container_(container) {
            ++container_->IteratorConnectCount;
        }


        // Copy Construction from the changeble and const variants - allows changeble to const and vice versa assignment (ALL Iterators)
        inline iterator_state(const iterator_state<true> & source): container_(source.container_), current_(source.current_) {}
        inline iterator_state(const iterator_state<false> & source): container_(source.container_), current_(source.current_) {}

        // Destruction (ALL Iterators)
        inline ~iterator_state() {
            current_ = internal_iterator();
            if( container_ != nullptr)
            { 
                ++container_->IteratorDisconnectCount;
                container_ = nullptr; 
            }
        }

        // Start and End Positions (ALL Iterators)
        inline void begin() {current_ = container_->InternalData.begin(); }
        inline void end() {current_ = container_->InternalData.end(); }

        // Availability and Equality (ALL Iterators)
        inline bool is_connected() const { return container_ != nullptr; }
        inline bool is_equal(const iterator_state<true> & other) const { return current_ == other.current_; }
        inline bool is_equal(const iterator_state<false> & other) const { return current_ == other.current_; }


        // Move Next (ALL Iterators)
        inline void next() { ++current_; }


        // Element Access (ALL Iterators)
        template<typename T = value_type>
        inline typename std::enable_if<! is_const, T>::type & get() {return *current_; }

        template<typename T = value_type>
        inline typename std::enable_if<is_const, T>::type & get() const {return *current_; }


        // Move Previous (Bidirectional, Random Access Iterators)
        inline void prev() { --current_; }
    };

    typedef custom_iterator_template<iterator_state, false> iterator;
    typedef custom_iterator_template<iterator_state, true> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


    iterator begin() { return iterator::begin(this); }
    iterator end() { return iterator::end(this); }

    const_iterator begin() const { return const_iterator::begin(this); }
    const_iterator end() const { return const_iterator::end(this); }

    const_iterator cbegin() const { return const_iterator::begin(this); }
    const_iterator cend() const { return const_iterator::end(this); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
};

// Skeleton for Random Access Iterators
struct CustomContainerWithRandomAccessIterator: public CustomContainerBase {

    CustomContainerWithRandomAccessIterator() = default;
    CustomContainerWithRandomAccessIterator(std::initializer_list<int> values): CustomContainerBase(values) {}


    template<bool is_const>
    struct iterator_state {

        // Specifing the type of the specialized iterator - these typedefs are picked up by the template to define the iterators (ALL Iterators)
        typedef std::random_access_iterator_tag iterator_category;
        typedef typename std::conditional<is_const, const CustomContainerWithRandomAccessIterator, CustomContainerWithRandomAccessIterator>::type        container_type;
        typedef typename std::conditional<is_const, const CustomElement, CustomElement>::type            value_type;
        typedef typename std::conditional<is_const, decltype(InternalData)::const_iterator, decltype(InternalData)::iterator>::type internal_iterator;

        container_type * container_;
        internal_iterator current_;

        // Default Construction without container connection (ALL Iterators)
        inline iterator_state(): container_(nullptr) {}

        // Construction with connected container; (ALL Iterators)
        inline iterator_state(container_type * container): container_(container) {
            ++container_->IteratorConnectCount;
        }


        // Copy Construction from the changeble and const variants - allows changeble to const and vice versa assignment (ALL Iterators)
        inline iterator_state(const iterator_state<true> & source): container_(source.container_), current_(source.current_) {}
        inline iterator_state(const iterator_state<false> & source): container_(source.container_), current_(source.current_) {}

        // Destruction (ALL Iterators)
        inline ~iterator_state() {
            current_ = internal_iterator();
            if( container_ != nullptr)
            { 
                ++container_->IteratorDisconnectCount;
                container_ = nullptr; 
            }
        }

        // Start and End Positions (ALL Iterators)
        inline void begin() {current_ = container_->InternalData.begin(); }
        inline void end() {current_ = container_->InternalData.end(); }

        // Availability and Equality (ALL Iterators)
        inline bool is_connected() const { return container_ != nullptr; }
        inline bool is_equal(const iterator_state<true> & other) const { return current_ == other.current_; }
        inline bool is_equal(const iterator_state<false> & other) const { return current_ == other.current_; }


        // Move Next (ALL Iterators)
        inline void next() { ++current_; }


        // Element Access (ALL Iterators)
        template<typename T = value_type>
        inline typename std::enable_if<! is_const, T>::type & get() {return *current_; }

        template<typename T = value_type>
        inline typename std::enable_if<is_const, T>::type & get() const {return *current_; }


        // Move Previous (Bidirectional, Random Access Iterators)
        inline void prev() { --current_; }

        // Move to position (Random Access Iterators)
        inline void move(std::ptrdiff_t offset) { current_ += offset; }

        // Calculate Distance (Random Access Iterators)
        inline std::ptrdiff_t distance(const iterator_state<true> & rhs) const { return current_ - rhs.current_; }
        inline std::ptrdiff_t distance(const iterator_state<false> & rhs) const { return current_ - rhs.current_; }


        // Element access at position (Random Access Iterators)
        template<typename T = value_type>
        inline typename std::enable_if<! is_const, T>::type & at(std::ptrdiff_t offset) {return current_[offset]; }

        template<typename T = std::ptrdiff_t>
        inline value_type & at(typename std::enable_if<is_const, T>::type offset) const {return current_[offset]; }
    };

    typedef custom_iterator_template<iterator_state, false> iterator;
    typedef custom_iterator_template<iterator_state, true> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


    iterator begin() { return iterator::begin(this); }
    iterator end() { return iterator::end(this); }

    const_iterator begin() const { return const_iterator::begin(this); }
    const_iterator end() const { return const_iterator::end(this); }

    const_iterator cbegin() const { return const_iterator::begin(this); }
    const_iterator cend() const { return const_iterator::end(this); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
};

// ****************************** Iterator Concept Test *********************************************
// https://en.cppreference.com/w/cpp/named_req/Iterator
// https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Readable
// Operations: *i; ++i are tested within the concepts
template<class T>
class IteratorTemplate_IteratorConceptTest: public testing::Test {
    T container{1,2,3,4,5,6,7,8};
protected:
    typename T::iterator GetBeginIterator() {return container.begin(); }
    typename T::iterator GetEndIterator() {return container.begin(); }
    int GetIteratorConnectCount() const { return container.IteratorConnectCount; }
    int GetIteratorDisconnectCount() const { return container.IteratorDisconnectCount; }
};

typedef testing::Types<CustomContainerWithInputIterator, CustomContainerWithForwardIterator, CustomContainerWithBidirectionalIterator, CustomContainerWithRandomAccessIterator> IteratorConceptImplementations;

TYPED_TEST_CASE(IteratorTemplate_IteratorConceptTest, IteratorConceptImplementations);

// https://en.cppreference.com/w/cpp/named_req/MoveConstructible
TYPED_TEST(IteratorTemplate_IteratorConceptTest, MoveConstructible) {
    typename TypeParam::iterator iterator = this->GetBeginIterator();
    ASSERT_EQ(*iterator, CustomElement(1));

    typename TypeParam::iterator moveConstructedIterator = iterator++; // rvalue 
    
    EXPECT_EQ(*moveConstructedIterator, CustomElement(1));
}

// https://en.cppreference.com/w/cpp/named_req/CopyConstructible
TYPED_TEST(IteratorTemplate_IteratorConceptTest, CopyConstructible) {
    typename TypeParam::iterator iterator = this->GetBeginIterator();
    ASSERT_EQ(*iterator, CustomElement(1));

    typename TypeParam::iterator copyConstructedIterator = iterator;
    
    EXPECT_EQ(*copyConstructedIterator, CustomElement(1));
}

// https://en.cppreference.com/w/cpp/named_req/MoveAssignable
TYPED_TEST(IteratorTemplate_IteratorConceptTest, MoveAssignable) {
    typename TypeParam::iterator iterator = this->GetBeginIterator();
    typename TypeParam::iterator moveAssigedIterator; 
    ASSERT_FALSE(moveAssigedIterator.is_connected());

    moveAssigedIterator = iterator++;  // rvalue

    EXPECT_EQ(*moveAssigedIterator, CustomElement(1));
}

// https://en.cppreference.com/w/cpp/named_req/CopyAssignable
TYPED_TEST(IteratorTemplate_IteratorConceptTest, CopyAssignable) {
    typename TypeParam::iterator iterator = this->GetBeginIterator();
    typename TypeParam::iterator copyAssigedIterator; 
    ASSERT_FALSE(copyAssigedIterator.is_connected());

    copyAssigedIterator = iterator;
    EXPECT_EQ(*copyAssigedIterator, CustomElement(1));
}

// https://en.cppreference.com/w/cpp/named_req/Destructible
TYPED_TEST(IteratorTemplate_IteratorConceptTest, Destructible) {
    typename TypeParam::iterator destroyedIterator = this->GetBeginIterator();
    typedef decltype(destroyedIterator) TIter;
    ASSERT_EQ(this->GetIteratorConnectCount(),1);
    ASSERT_EQ(this->GetIteratorDisconnectCount(),0);
    ASSERT_TRUE(destroyedIterator.is_connected());

    destroyedIterator.~TIter();
    
    EXPECT_EQ(this->GetIteratorConnectCount(),1);
    EXPECT_EQ(this->GetIteratorDisconnectCount(),1);
}

// https://en.cppreference.com/w/cpp/named_req/Swappable
TYPED_TEST(IteratorTemplate_IteratorConceptTest, Swappable) {
    typename TypeParam::iterator swappedLhsIterator = this->GetBeginIterator();
    typename TypeParam::iterator swappedRhsIterator = this->GetEndIterator();

    std::swap(swappedLhsIterator, swappedRhsIterator);
    
    EXPECT_EQ(swappedLhsIterator, this->GetEndIterator());
    EXPECT_EQ(swappedRhsIterator, this->GetBeginIterator());
}


// ****************************** Input Iterator Concept Test *********************************************
// https://en.cppreference.com/w/cpp/named_req/InputIterator
// Operations: i==j; i != j; *i; i->m; (void)i++; (void)++i; *(++I), *i++;
template<class T>
class IteratorTemplate_InputIteratorConceptTest: public testing::Test {
    T container{1,2,3,4,5,6,7,8};
protected:
    typename T::iterator GetBeginIterator() {return container.begin(); }
    typename T::iterator GetEndIterator() {return container.begin(); }
};

typedef testing::Types<CustomContainerWithInputIterator, CustomContainerWithForwardIterator, CustomContainerWithBidirectionalIterator, CustomContainerWithRandomAccessIterator> InputIteratorConceptImplementations;

TYPED_TEST_CASE(IteratorTemplate_InputIteratorConceptTest, InputIteratorConceptImplementations);

// Operations: i==j; i != j;
TYPED_TEST(IteratorTemplate_InputIteratorConceptTest, EqualityComparison) {
    typename TypeParam::iterator lhsIterator = this->GetBeginIterator();
    typename TypeParam::iterator sameRhsIterator = this->GetBeginIterator();
    typename TypeParam::iterator differentRhsIterator = ++(this->GetBeginIterator());

    EXPECT_EQ(*lhsIterator, CustomElement(1));
    EXPECT_EQ(*sameRhsIterator, CustomElement(1));
    EXPECT_EQ(*differentRhsIterator, CustomElement(2));
    EXPECT_TRUE(lhsIterator == sameRhsIterator);
    EXPECT_FALSE(lhsIterator != sameRhsIterator);
    EXPECT_FALSE(lhsIterator == differentRhsIterator);
    EXPECT_TRUE(lhsIterator != differentRhsIterator);
}

// Operations: *i; i->m;
TYPED_TEST(IteratorTemplate_InputIteratorConceptTest, MemberAccess) {
    EXPECT_EQ( *(this->GetBeginIterator()), CustomElement(1));
    EXPECT_EQ( (this->GetBeginIterator())->GetValue(), 1);
}

// Operations: (void)i++; (void)++i;
TYPED_TEST(IteratorTemplate_InputIteratorConceptTest, PostAndPreincrementWithoutResult) {
    typename TypeParam::iterator preIncrementIterator = this->GetBeginIterator();
    typename TypeParam::iterator postIncrementIterator = this->GetBeginIterator();
    ASSERT_EQ(*preIncrementIterator, CustomElement(1));
    ASSERT_EQ(*postIncrementIterator, CustomElement(1));

    ++preIncrementIterator;
    postIncrementIterator++;
    
    EXPECT_EQ(*preIncrementIterator, CustomElement(2));
    EXPECT_EQ(*postIncrementIterator, CustomElement(2));
}

// Operations:  *(++I), *i++;
TYPED_TEST(IteratorTemplate_InputIteratorConceptTest, PostAndPreincrementWithResult) {
    typename TypeParam::iterator preIncrementIterator = this->GetBeginIterator();
    typename TypeParam::iterator postIncrementIterator = this->GetBeginIterator();
    ASSERT_EQ(*preIncrementIterator, CustomElement(1));
    ASSERT_EQ(*postIncrementIterator, CustomElement(1));

    typename TypeParam::iterator::value_type preIncrementValue = *(++preIncrementIterator);
    typename TypeParam::iterator::value_type postIncrementValue = *postIncrementIterator++;
    
    EXPECT_EQ(*preIncrementIterator, CustomElement(2));
    EXPECT_EQ(*postIncrementIterator, CustomElement(2));
    EXPECT_EQ(preIncrementValue, CustomElement(2));
    EXPECT_EQ(postIncrementValue, CustomElement(1));
}


// ****************************** Forward Iterator Concept Test *********************************************
// https://en.cppreference.com/w/cpp/named_req/ForwardIterator

template<class T>
class IteratorTemplate_ForwardConceptTest: public testing::Test {
    T container{1,2,3,4,5,6,7,8};
protected:
    typename T::iterator GetBeginIterator() {return container.begin(); }
    typename T::iterator GetEndIterator() {return container.begin(); }
    typename T::const_iterator GetConstBeginIterator() {return container.cbegin(); }

};

typedef testing::Types<CustomContainerWithForwardIterator, CustomContainerWithBidirectionalIterator, CustomContainerWithRandomAccessIterator> ForwardIteratorConceptImplementations;

TYPED_TEST_CASE(IteratorTemplate_ForwardConceptTest, ForwardIteratorConceptImplementations);

// https://en.cppreference.com/w/cpp/named_req/DefaultConstructible
TYPED_TEST(IteratorTemplate_ForwardConceptTest, DefaultConstructible) {
    typename TypeParam::iterator defaultConstructedIterator{};

    EXPECT_FALSE(defaultConstructedIterator.is_connected());
}

TYPED_TEST(IteratorTemplate_ForwardConceptTest, Multipass_IteratorsAreEqualWhenPointingOnSameElementOrNonDereferencablePosition) {
    EXPECT_TRUE(this->GetBeginIterator() == this->GetBeginIterator());
    EXPECT_TRUE(this->GetEndIterator() == this->GetEndIterator());
    EXPECT_TRUE(++(this->GetBeginIterator()) == ++(this->GetBeginIterator()));
}

TYPED_TEST(IteratorTemplate_ForwardConceptTest, Multipass_IncrementIteratorCopyDoesNotChangeIteratorPosition) {
    // Incrementing a iterator copy does not change the iterator position
    typename TypeParam::iterator incrementCopyIterator = this->GetBeginIterator();

    ++(typename TypeParam::iterator(incrementCopyIterator));

    EXPECT_EQ(*incrementCopyIterator, CustomElement(1));
}


TYPED_TEST(IteratorTemplate_ForwardConceptTest, Multipass_AssignmentDoesNotInvalidateTheIterator) {
    typename TypeParam::iterator assigmentIterator = this->GetBeginIterator();
    typename TypeParam::iterator secondIterator = this->GetBeginIterator();

    *assigmentIterator = CustomElement(10);

    EXPECT_EQ(*assigmentIterator, CustomElement(10));
    EXPECT_EQ(*secondIterator, CustomElement(10));
}


TYPED_TEST(IteratorTemplate_ForwardConceptTest, SingularIterators) {
    // changeable iterators
    EXPECT_TRUE(typename TypeParam::iterator() == typename TypeParam::iterator());
    EXPECT_FALSE(typename TypeParam::iterator() != typename TypeParam::iterator());

    EXPECT_FALSE(typename TypeParam::iterator() == this->GetBeginIterator());
    EXPECT_FALSE(this->GetBeginIterator() == typename TypeParam::iterator());

    EXPECT_TRUE(typename TypeParam::iterator() != this->GetBeginIterator());
    EXPECT_TRUE(this->GetBeginIterator() != typename TypeParam::iterator());

    // const iterators
    EXPECT_TRUE(typename TypeParam::const_iterator() == typename TypeParam::const_iterator());
    EXPECT_FALSE(typename TypeParam::const_iterator() != typename TypeParam::const_iterator());

    EXPECT_FALSE(typename TypeParam::const_iterator() == this->GetConstBeginIterator());
    EXPECT_FALSE(this->GetConstBeginIterator() == typename TypeParam::const_iterator());

    EXPECT_TRUE(typename TypeParam::const_iterator() != this->GetConstBeginIterator());
    EXPECT_TRUE(this->GetConstBeginIterator() != typename TypeParam::const_iterator());
}


// ****************************** Bidirectional Iterator Concept Test *********************************************
// https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
// Operations: (void)i--; (void)--i; *(--I), *i--;

template<class T>
class IteratorTemplate_BidirectionalConceptTest: public testing::Test {
    T container{1,2,3,4,5,6,7,8};
protected:
    typename T::iterator GetBeginIterator() {return container.begin(); }
    typename T::iterator GetEndIterator() {return container.begin(); }
};

typedef testing::Types<CustomContainerWithBidirectionalIterator, CustomContainerWithRandomAccessIterator> BidirectionalIteratorConceptImplementations;

TYPED_TEST_CASE(IteratorTemplate_BidirectionalConceptTest, BidirectionalIteratorConceptImplementations);

// Operations: (void)i--; (void)--i;
TYPED_TEST(IteratorTemplate_BidirectionalConceptTest, PostAndPredecrementWithoutResult) {
    typename TypeParam::iterator preDecrementIterator = ++(this->GetBeginIterator());  
    typename TypeParam::iterator postDecrementIterator = ++(this->GetBeginIterator()); 
    ASSERT_EQ(*preDecrementIterator, CustomElement(2));
    ASSERT_EQ(*postDecrementIterator, CustomElement(2));

    --preDecrementIterator;
    postDecrementIterator--;
    
    EXPECT_EQ(*preDecrementIterator, CustomElement(1));
    EXPECT_EQ(*postDecrementIterator, CustomElement(1));
}

// Operations:  *(--I), *i--;
TYPED_TEST(IteratorTemplate_BidirectionalConceptTest, PostAndPredecrementWithResult) {
    typename TypeParam::iterator preDecrementIterator = ++(this->GetBeginIterator());
    typename TypeParam::iterator postDecrementIterator = ++(this->GetBeginIterator());
    ASSERT_EQ(*preDecrementIterator, CustomElement(2));
    ASSERT_EQ(*postDecrementIterator, CustomElement(2));

    typename TypeParam::iterator::value_type preDecrementValue = *(--preDecrementIterator);
    typename TypeParam::iterator::value_type postDecrementValue = *postDecrementIterator--;
    
    EXPECT_EQ(*preDecrementIterator, CustomElement(1));
    EXPECT_EQ(*postDecrementIterator, CustomElement(1));
    EXPECT_EQ(preDecrementValue, CustomElement(1));
    EXPECT_EQ(postDecrementValue, CustomElement(2));
}


TEST(IteratorTemplate, TestRandomAccessIteratorConcept) {
    // https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
    // Operations: r += n; r -= n; a + n; n + a, i-n; b - a; i[n], a < b; a <= b; a >= b; a > b;


    CustomContainerWithRandomAccessIterator container{1,2,3,4,5,6,7,8};
    // Operation: r += n;
    CustomContainerWithRandomAccessIterator::iterator addOffsetIterator = container.begin();
    addOffsetIterator += 5;
    addOffsetIterator += -3;
    EXPECT_EQ(*addOffsetIterator, CustomElement(3));

    // Operation: r -= n;
    CustomContainerWithRandomAccessIterator::iterator subtractOffsetIterator = container.begin();
    subtractOffsetIterator -= -5;
    subtractOffsetIterator -= 3;
    EXPECT_EQ(*subtractOffsetIterator, CustomElement(3));

    // Operations: a + n; i-n;
    EXPECT_EQ(*((container.begin()+5)-3), CustomElement(3));
    EXPECT_EQ(*((container.cbegin()+5)-3), CustomElement(3));

    // Operation: n + a;
    EXPECT_EQ(*(5 + container.begin()), CustomElement(6));
    EXPECT_EQ(*(5 + container.cbegin()), CustomElement(6));


    // Operation: i[];
    CustomContainerWithRandomAccessIterator::iterator indexOperationIterator = container.begin() + 3;
    CustomContainerWithRandomAccessIterator::const_iterator constIndexOperationIterator = container.cbegin() + 3;
    EXPECT_EQ(indexOperationIterator[2], CustomElement(6));
    EXPECT_EQ(constIndexOperationIterator[2], CustomElement(6));
    EXPECT_EQ(indexOperationIterator[-2], CustomElement(2));
    EXPECT_EQ(constIndexOperationIterator[-2], CustomElement(2));

    // Operation: b - a; a < b; a <= b; a >= b; a > b;
    CustomContainerWithRandomAccessIterator::iterator smallerIterator = container.begin() + 2;
    CustomContainerWithRandomAccessIterator::iterator biggerIterator = container.begin() + 5;
    EXPECT_EQ(biggerIterator - smallerIterator, 3);
    EXPECT_EQ(smallerIterator - biggerIterator, -3);
    EXPECT_EQ(smallerIterator - smallerIterator, 0);

    // https://en.cppreference.com/w/cpp/named_req/LessThanComparable
    // + Strict total ordering relation
    EXPECT_TRUE(smallerIterator < biggerIterator);
    EXPECT_FALSE(biggerIterator < smallerIterator);

    EXPECT_TRUE(smallerIterator <= biggerIterator);
    EXPECT_FALSE(biggerIterator <= smallerIterator);

    EXPECT_TRUE(biggerIterator > smallerIterator);
    EXPECT_FALSE(smallerIterator > biggerIterator);

    EXPECT_TRUE(biggerIterator >= smallerIterator);
    EXPECT_FALSE(smallerIterator >= biggerIterator);

    EXPECT_TRUE(smallerIterator >= smallerIterator);
    EXPECT_TRUE(smallerIterator <= smallerIterator);
}

TEST(IteratorTemplate, TestIteratorTypeTraits) {
    CustomContainerWithRandomAccessIterator container{1,2};
    const CustomContainerWithRandomAccessIterator constContainer{1,2};

    EXPECT_EQ(typeid(decltype(container.begin())), typeid(CustomContainerWithRandomAccessIterator::iterator));
    EXPECT_EQ(typeid(decltype(container.end())), typeid(CustomContainerWithRandomAccessIterator::iterator));
    EXPECT_EQ(typeid(decltype(constContainer.begin())), typeid(CustomContainerWithRandomAccessIterator::const_iterator));
    EXPECT_EQ(typeid(decltype(constContainer.end())), typeid(CustomContainerWithRandomAccessIterator::const_iterator));
    EXPECT_EQ(typeid(decltype(container.cbegin())), typeid(CustomContainerWithRandomAccessIterator::const_iterator));
    EXPECT_EQ(typeid(decltype(container.cend())), typeid(CustomContainerWithRandomAccessIterator::const_iterator));


    EXPECT_EQ(typeid(decltype(container.rbegin())), typeid(CustomContainerWithRandomAccessIterator::reverse_iterator));
    EXPECT_EQ(typeid(decltype(container.rend())), typeid(CustomContainerWithRandomAccessIterator::reverse_iterator));
    EXPECT_EQ(typeid(decltype(constContainer.rbegin())), typeid(CustomContainerWithRandomAccessIterator::const_reverse_iterator));
    EXPECT_EQ(typeid(decltype(constContainer.rend())), typeid(CustomContainerWithRandomAccessIterator::const_reverse_iterator));

    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::value_type), typeid(CustomElement));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::difference_type), typeid(ptrdiff_t));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::pointer), typeid(CustomElement *));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::reference), typeid(CustomElement &));

    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::value_type), typeid(CustomElement));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::difference_type), typeid(ptrdiff_t));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::pointer), typeid(CustomElement *));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::reference), typeid(CustomElement &));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::iterator_category), typeid(std::random_access_iterator_tag));

    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.rbegin())>::value_type), typeid(CustomElement));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.rbegin())>::difference_type), typeid(ptrdiff_t));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.rbegin())>::pointer), typeid(CustomElement *));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.rbegin())>::reference), typeid(CustomElement &));
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.rbegin())>::iterator_category), typeid(std::random_access_iterator_tag));
}

TEST(IteratorTemplate, TestIteratorBeginAndEndPositions) {
    CustomContainerWithRandomAccessIterator empty{};
    CustomContainerWithRandomAccessIterator twoElements{1, 2};

    const auto &constEmptyRef = empty;
    const auto &constTwoElementsRef = twoElements;

    EXPECT_EQ(empty.begin(), empty.end());
    EXPECT_EQ(empty.rbegin(), empty.rend());
    EXPECT_EQ(*twoElements.begin(), CustomElement{1});
    EXPECT_EQ(*twoElements.rbegin(), CustomElement{2});

    EXPECT_EQ(constEmptyRef.begin(), constEmptyRef.end());
    EXPECT_EQ(constEmptyRef.rbegin(), constEmptyRef.rend());
    EXPECT_EQ(*constTwoElementsRef.begin(), CustomElement{1});
    EXPECT_EQ(*constTwoElementsRef.rbegin(), CustomElement{2});
}

TEST(IteratorTemplate, TestChangebleToConstCast) {
    CustomContainerWithRandomAccessIterator container{1, 2};

    CustomContainerWithRandomAccessIterator::iterator changeableIterator = container.begin();
    CustomContainerWithRandomAccessIterator::const_iterator constIterator = changeableIterator;

    EXPECT_EQ(*changeableIterator, CustomElement{1});
    EXPECT_EQ(*constIterator, CustomElement{1});
}

TEST(IteratorTemplate, TestChangebleAndConstComparison) {
    CustomContainerWithRandomAccessIterator container{1, 2};

    CustomContainerWithRandomAccessIterator::iterator changeableIterator = container.begin();
    CustomContainerWithRandomAccessIterator::const_iterator constIterator = changeableIterator;

    EXPECT_TRUE(changeableIterator == constIterator );
    EXPECT_TRUE(constIterator == changeableIterator );

    EXPECT_FALSE(changeableIterator != constIterator );
    EXPECT_FALSE(constIterator != changeableIterator );
}

TEST(IteratorTemplate, TestIteratorLoops) {
    CustomContainerWithRandomAccessIterator empty{};
    CustomContainerWithRandomAccessIterator twoElements{1,2};

    const auto &constEmptyRef = empty;
    const auto &constTwoElementsRef = twoElements;

    size_t emptyCount = 0u;
    for (auto i = empty.begin(), end = empty.end(); i != end; ++i) {
        ++emptyCount;
    }

    std::vector<int> twoElementsContent;
    for (auto i = twoElements.begin(), end = twoElements.end(); i != end; ++i) {
        static_assert(! std::is_const< std::remove_reference<decltype(*i)>::type>::value, "Iterators must introduce changeable Elements");
        twoElementsContent.push_back(i->GetValue());

        // call of non const function is possible
        i->SetValue(i->GetValue());
    }

    size_t constEmptyCount = 0u;
    for (auto i = constEmptyRef.begin(), end = constEmptyRef.end(); i != end; ++i) {
        ++constEmptyCount;
    }

    std::vector<int> constTwoElementsContent;
    for (auto i = constTwoElementsRef.begin(), end = constTwoElementsRef.end(); i != end; ++i) {
        static_assert(std::is_const< std::remove_reference<decltype(*i)>::type>::value, "Const iterators must introduce const Elements");

        constTwoElementsContent.push_back(i->GetValue());
    }


    size_t constFromChangeableEmptyCount = 0u;
    for (auto i = empty.cbegin(), end = empty.cend(); i != end; ++i) {
        ++constFromChangeableEmptyCount;
    }

    std::vector<int> constFromChangeableTwoElementsContent;
    for (auto i = twoElements.cbegin(), end = twoElements.cend(); i != end; ++i) {
        static_assert(std::is_const< std::remove_reference<decltype(*i)>::type>::value, "Const iterators must introduce const Elements");

        constFromChangeableTwoElementsContent.push_back(i->GetValue());
    }


    EXPECT_EQ(emptyCount,0u);
    EXPECT_EQ(constEmptyCount,0u);
    EXPECT_EQ(constFromChangeableEmptyCount,0u);

    EXPECT_THAT(twoElementsContent, ::testing::ContainerEq(std::vector<int>({1,2})));
    EXPECT_THAT(constTwoElementsContent, ::testing::ContainerEq(std::vector<int>({1,2})));
    EXPECT_THAT(constFromChangeableTwoElementsContent, ::testing::ContainerEq(std::vector<int>({1,2})));
}

TEST(IteratorTemplate, TestReverseIteratorLoops) {
    CustomContainerWithRandomAccessIterator empty{};
    CustomContainerWithRandomAccessIterator twoElements{1,2};

    const auto &constEmptyRef = empty;
    const auto &constTwoElementsRef = twoElements;

    size_t emptyCount = 0u;
    for (auto i = empty.rbegin(), end = empty.rend(); i != end; ++i) {
        ++emptyCount;
    }

    std::vector<int> twoElementsContent;
    for (auto i = twoElements.rbegin(), end = twoElements.rend(); i != end; ++i) {
        static_assert(! std::is_const< std::remove_reference<decltype(*i)>::type>::value, "Iterators must introduce changeable Elements");
        twoElementsContent.push_back(i->GetValue());

        // call of non const function is possible
        i->SetValue(i->GetValue());
    }

    size_t constEmptyCount = 0u;
    for (auto i = constEmptyRef.rbegin(), end = constEmptyRef.rend(); i != end; ++i) {
        ++constEmptyCount;
    }

    std::vector<int> constTwoElementsContent;
    for (auto i = constTwoElementsRef.rbegin(), end = constTwoElementsRef.rend(); i != end; ++i) {
        static_assert(std::is_const< std::remove_reference<decltype(*i)>::type>::value, "Const iterators must introduce const Elements");

        constTwoElementsContent.push_back(i->GetValue());
    }

    EXPECT_EQ(emptyCount,0u);
    EXPECT_EQ(constEmptyCount,0u);

    EXPECT_THAT(twoElementsContent, ::testing::ContainerEq(std::vector<int>({2,1})));
    EXPECT_THAT(constTwoElementsContent, ::testing::ContainerEq(std::vector<int>({2,1})));
}

TEST(IteratorTemplate, TestRangeBasedLoops) {
    CustomContainerWithRandomAccessIterator empty {};
    CustomContainerWithRandomAccessIterator twoElements{1,2};

    const auto & constEmptyRef = empty;
    const auto & constTwoElementsRef = twoElements;

    size_t emptyCount = 0u;
    for(auto & elem: empty)
    {
        ++emptyCount;
    }

    std::vector<int> twoElementsContent;
    for(auto & elem: twoElements)
    {
        static_assert(! std::is_const< std::remove_reference<decltype(elem)>::type>::value, "Iterators must introduce changeable Elements");
        twoElementsContent.push_back(elem.GetValue());

        // call of non const function is possible
        elem.SetValue(elem.GetValue());
    }

    size_t constEmptyCount = 0u;
    for(auto & elem: constEmptyRef)
    {
        ++constEmptyCount;

    }

    std::vector<int> constTwoElementsContent;
    for(auto & elem: constTwoElementsRef)
    {
        static_assert(std::is_const< std::remove_reference<decltype(elem)>::type>::value, "Const iterators must introduce const Elements");
        constTwoElementsContent.push_back(elem.GetValue());
    }

    EXPECT_EQ(emptyCount,0u);
    EXPECT_EQ(constEmptyCount,0u);

    EXPECT_THAT(twoElementsContent, ::testing::ContainerEq(std::vector<int>({1,2})));
    EXPECT_THAT(constTwoElementsContent, ::testing::ContainerEq(std::vector<int>({1,2})));
}

TEST(IteratorTemplate, TestPartialStateForInputIterator) {
    CustomContainerWithInputIterator container{1,2};
    const CustomContainerWithInputIterator constContainer{1,2};
    EXPECT_EQ(typeid(std::iterator_traits<decltype(container.begin())>::iterator_category), typeid(std::input_iterator_tag));
}