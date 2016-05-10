/* Copyright 2016 Karácsony Máté */
#ifndef TEMPLATE_CONFIG_HPP_INCLUDED
#define TEMPLATE_CONFIG_HPP_INCLUDED

#include <ios>
#include <initializer_list>
#include <type_traits>

#include <boost/mpl/and.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/count_if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/greater_equal.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/long.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/reverse_fold.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_pointer.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/remove_reference.hpp>

// Namespace alias
namespace mpl = boost::mpl;


// Library configuration section
//----------------------------------------------------------------------------------------------------------------------


#ifndef ALLOCATE_FUNCTION
    #define ALLOCATE_FUNCTION malloc
    #include <cstdlib> // for definition of malloc
#endif // ALLOCATE_FUNCTION

#ifndef DEALLOCATE_FUNCTION
    #define DEALLOCATE_FUNCTION free
    #include <cstdlib> // for definition of free
#endif // DEALLOCATE_FUNCTION


// Interface & implementation section
//----------------------------------------------------------------------------------------------------------------------


namespace cfg { // Namespace for configuration library


namespace impl { // Hide internal implementation details


// Array type constructor metafunction (helper)
template<typename BaseType, typename Size>
struct add_array
{
    typedef BaseType type[Size::value];
};

// Add constant type qualifier conditionally (helper)
template<bool addConst, typename T>
struct add_const_if : mpl::if_<mpl::int_<addConst>, typename boost::add_const<T>::type, T> {};


// Configuration tokens
typedef struct {} T_SCALAR;
typedef struct {} T_PTR;
typedef struct {} T_ARRAY;


} // End namespace cfg::impl


// Token sequence generators
struct SCALAR : mpl::vector<impl::T_SCALAR> {};

template<typename Tokens>
struct PTR : mpl::push_front<typename Tokens::type, impl::T_PTR> {};

template<typename Tokens>
struct ARRAY : mpl::push_front<typename Tokens::type, impl::T_ARRAY> {};


// Token matching
template<typename Token> struct is_scalar_token                 : mpl::false_ {};
template<>               struct is_scalar_token<impl::T_SCALAR> : mpl::true_  {};

template<typename Token> struct is_ptr_token              : mpl::false_ {};
template<>               struct is_ptr_token<impl::T_PTR> : mpl::true_  {};

template<typename Token> struct is_array_token                : mpl::false_ {};
template<>               struct is_array_token<impl::T_ARRAY> : mpl::true_  {};


// Item configuration
template<typename Config>
struct item_config
{
    typedef typename mpl::fold<
        typename Config::type,
        mpl::pair<mpl::vector<>, mpl::false_>,
        mpl::pair<
            mpl::if_<
                mpl::second<mpl::_1>,
                mpl::push_back<mpl::first<mpl::_1>, mpl::_2>,
                mpl::first<mpl::_1>
            >,
            mpl::if_<is_array_token<mpl::_2>, mpl::true_, mpl::second<mpl::_1>>
        >
    >::type::first type;
};


// Item configuration pointer checking
template<typename Config>
struct has_ptr_item
{
    typedef typename mpl::fold<
        typename Config::type,
        mpl::pair<mpl::false_, mpl::false_>,
        mpl::pair<
            mpl::if_<
                is_ptr_token<mpl::_2>,
                mpl::if_<mpl::second<mpl::_1>, mpl::true_, mpl::first<mpl::_1>>,
                mpl::first<mpl::_1>
            >,
            mpl::if_<is_array_token<mpl::_2>, mpl::true_, mpl::second<mpl::_1>>
        >
    >::type::first type;
};


// Array rank (the number of dimensions) of a configuration
template<typename Config>
struct array_rank : mpl::count_if<typename Config::type, is_array_token<mpl::_>> {};


// Value configuration (dereferenced)
template<typename Config>
struct value_config
{
    typedef typename mpl::fold<
        typename Config::type,
        mpl::pair<mpl::vector<>, mpl::false_>,
        mpl::pair<
            mpl::if_<
                mpl::or_<mpl::or_<is_array_token<mpl::_2>, is_scalar_token<mpl::_2>>,mpl::second<mpl::_1>>,
                mpl::push_back<mpl::first<mpl::_1>, mpl::_2>,
                mpl::first<mpl::_1>
            >,
            mpl::if_<mpl::or_<is_array_token<mpl::_2>, is_scalar_token<mpl::_2>>, mpl::true_, mpl::second<mpl::_1>>
        >
    >::type::first type;
};


namespace impl { // Hide internal implementation details


// Derive a type from configuration
template<typename BaseType, typename Config, typename AsPointer = mpl::false_, typename Dimensions = mpl::vector_c<int>>
struct configured_type_base
{
    typedef typename array_rank<Config>::type count_dimensions;
    typedef typename mpl::size<Dimensions>::type supplied_dimensions;

    BOOST_MPL_ASSERT_MSG(
        (mpl::or_<mpl::not_<AsPointer>, mpl::greater_equal<mpl::long_<0>, supplied_dimensions>>::value),
        Please_do_not_specify_the_size_of_array_dimensions_for_configured_pointer_only_type,
        (supplied_dimensions)
    );
    BOOST_MPL_ASSERT_MSG(
        (mpl::or_<AsPointer, mpl::less_equal<count_dimensions, supplied_dimensions>>::value),
        Please_specify_the_size_of_all_array_dimensions_for_configured_type,
        (count_dimensions, supplied_dimensions)
    );
    BOOST_MPL_ASSERT_MSG(
        (mpl::or_<AsPointer, mpl::greater_equal<count_dimensions, supplied_dimensions>>::value),
        Too_many_dimension_sizes_specified_for_configured_type,
        (count_dimensions, supplied_dimensions)
    );

    typedef typename mpl::if_<AsPointer, mpl::vector_c<int, 0>, Dimensions>::type dimension_vector;

    typedef typename mpl::reverse_fold<
        typename Config::type,
        mpl::pair<BaseType, typename mpl::prior<count_dimensions>::type>,
        mpl::pair<
            mpl::if_<
                is_array_token<mpl::_2>,
                mpl::if_< // T_ARRAY
                    AsPointer,
                    boost::add_pointer<mpl::first<mpl::_1>>,
                    impl::add_array<mpl::first<mpl::_1>, mpl::at<dimension_vector, mpl::second<mpl::_1>>>
                >,
                mpl::if_<
                    is_ptr_token<mpl::_2>,
                    boost::add_pointer<mpl::first<mpl::_1>>, // T_PTR
                    mpl::first<mpl::_1> // T_SCALAR
                >
            >,
            mpl::if_<
                mpl::and_<is_array_token<mpl::_2>, mpl::not_<AsPointer>>,
                mpl::prior<mpl::second<mpl::_1>>,
                mpl::second<mpl::_1>
            >
        >
    >::type::first type;
};


} // End namespace cfg::impl


template<typename BaseType, typename Config, typename Dimensions = mpl::vector_c<int>>
struct configured_type : impl::configured_type_base<BaseType, Config, mpl::false_, Dimensions> {};

template<typename BaseType, typename Config>
struct configured_return_type : impl::configured_type_base<BaseType, Config, mpl::true_> {};


// Allocation
template<typename BaseType, typename Config, typename Dimensions = mpl::vector_c<int>>
struct allocate
{
public:
    typedef typename configured_type<BaseType, Config, Dimensions>::type real_type;

    static void run(real_type& value)
    {
        typename mpl::front<typename Config::type>::type token;
        run_token<Config, Dimensions>(token, value);
    };

private:
    template<typename T, bool is_trivial>
    struct explicit_constructor
    {
        static void execute(T * value)
        {
            new (value) T;
        };
    };

    template<typename T>
    struct explicit_constructor<T, true>
    {
        static void execute(T * value) {};
    };

    template<typename CurrentConfig, typename CurrentDimensions>
    static void run_token(impl::T_SCALAR token, BaseType& value) {};

    template<typename CurrentConfig, typename CurrentDimensions>
    static void run_token(
        impl::T_PTR token,
        typename configured_type<BaseType, CurrentConfig, CurrentDimensions>::type& value
    ) {
        typedef typename mpl::pop_front<typename CurrentConfig::type>::type remaining_config;
        typename mpl::front<typename remaining_config::type>::type new_token;

        typedef typename boost::remove_reference<decltype(*value)>::type ctor_type;
        typedef typename boost::has_trivial_constructor<ctor_type>::type is_trivial;

        value = static_cast<ctor_type*>(ALLOCATE_FUNCTION(sizeof(ctor_type)));

        // explicitly run the constructor of the object on the newly allocated memory area
        // only the non-trivial constructors are needed to be executed
        explicit_constructor<ctor_type, is_trivial::value>::execute(value);

        run_token<remaining_config, CurrentDimensions>(new_token, *value);
    };

    template<typename CurrentConfig, typename CurrentDimensions>
    static void run_token(
        impl::T_ARRAY token,
        typename configured_type<BaseType, CurrentConfig, CurrentDimensions>::type& value
    ) {
        typedef typename mpl::pop_front<typename CurrentConfig::type>::type remaining_config;
        typedef typename mpl::pop_front<typename CurrentDimensions::type>::type remaining_dimensions;
        typedef typename mpl::front<typename CurrentDimensions::type>::type size;

        typename mpl::front<typename remaining_config::type>::type new_token;

        for(int i = 0; i < size::value; ++i)
        {
            run_token<remaining_config, remaining_dimensions>(new_token, value[i]);
        }
    };
};


// Deallocation
template<typename BaseType, typename Config, typename Dimensions = mpl::vector_c<int>>
struct deallocate
{
public:
    typedef typename configured_type<BaseType, Config, Dimensions>::type real_type;

    static void run(real_type& value)
    {
        typename mpl::front<typename Config::type>::type token;
        run_token<Config, Dimensions>(token, value);
    };

private:
    template<typename T, bool is_trivial>
    struct explicit_destructor
    {
        static void execute(T * value)
        {
            value->~T();
        };
    };

    template<typename T>
    struct explicit_destructor<T, true>
    {
        static void execute(T * value) {};
    };

    template<typename CurrentConfig, typename CurrentDimensions>
    static void run_token(impl::T_SCALAR token, BaseType& value) {};

    template<typename CurrentConfig, typename CurrentDimensions>
    static void run_token(
        impl::T_PTR token,
        typename configured_type<BaseType, CurrentConfig, CurrentDimensions>::type& value
    ) {
        typedef typename mpl::pop_front<typename CurrentConfig::type>::type remaining_config;
        typename mpl::front<typename remaining_config::type>::type new_token;

        run_token<remaining_config, CurrentDimensions>(new_token, *value);

        // explicitly run the destructor of the object (see allocation)
        // only the non-trivial destructors are needed to be executed
        typedef typename boost::remove_reference<decltype(*value)>::type dtor_type;
        typedef typename boost::has_trivial_destructor<dtor_type>::type is_trivial;
        explicit_destructor<dtor_type, is_trivial::value>::execute(value);
        DEALLOCATE_FUNCTION(value);
    };

    template<typename CurrentConfig, typename CurrentDimensions>
    static void run_token(
        impl::T_ARRAY token,
        typename configured_type<BaseType, CurrentConfig, CurrentDimensions>::type& value
    ) {
        typedef typename mpl::pop_front<typename CurrentConfig::type>::type remaining_config;
        typedef typename mpl::pop_front<typename CurrentDimensions::type>::type remaining_dimensions;
        typedef typename mpl::front<typename CurrentDimensions::type>::type size;
        typename mpl::front<typename remaining_config::type>::type new_token;

        for(int i = 0; i < size::value; ++i)
        {
            run_token<remaining_config, remaining_dimensions>(new_token, value[i]);
        }
    };
};


// Value of
template<typename BaseType, typename Config, typename Dimensions = mpl::vector_c<int>, bool isConst = false>
struct value_of
{
    typedef typename impl::add_const_if<
        isConst,
        typename configured_type<BaseType, Config, Dimensions>::type
    >::type real_type;

    typedef typename impl::add_const_if<
        isConst,
        typename configured_type<BaseType, value_config<Config>, Dimensions>::type
    >::type type;

    static type& calculate(real_type& value)
    {
        typename mpl::front<typename Config::type>::type token;

        return calculate_token<Config>(token, value);
    };

private:
    template<typename CurrentConfig>
    static type& calculate_token(impl::T_SCALAR token, type& value)
    {
        return value;
    };

    template<typename CurrentConfig>
    static type& calculate_token(
        impl::T_PTR token,
        typename impl::add_const_if<
            isConst,
            typename configured_type<BaseType, CurrentConfig, Dimensions>::type
        >::type& value
    ) {
        typedef typename mpl::pop_front<typename CurrentConfig::type>::type remaining_config;
        typename mpl::front<remaining_config>::type new_token;

        return calculate_token<remaining_config>(new_token, *value);
    };

    template<typename CurrentConfig>
    static type& calculate_token(impl::T_ARRAY token, type& value)
    {
        return value;
    };
};


// Value at a specified array index
template<typename BaseType, typename Config, typename Dimensions = mpl::vector_c<int>, bool isConst = false>
struct value_at
{
    typedef typename mpl::pop_front<typename Dimensions::type>::type inner_dimensions;

    typedef typename impl::add_const_if<
        isConst,
        typename configured_type<BaseType, Config, Dimensions>::type
    >::type real_type;

    typedef typename impl::add_const_if<
        isConst,
        typename value_of<BaseType, item_config<Config>, inner_dimensions>::type
    >::type type;

    static type& calculate(real_type& value, const int index)
    {
        return value_of<BaseType, item_config<Config>, inner_dimensions, isConst>::calculate(
            value_of<BaseType, Config, Dimensions, isConst>::calculate(value)[index]
        );
    };
};


namespace impl { // internal implementation details


template<typename BaseType, typename Config, bool AutoAlloc, bool isReference, typename Dimensions>
struct configured_array_base;

template<typename BaseType, typename Config, typename Dimensions, typename ReturnType, bool isConst = false>
struct value_at_
{
    typedef typename impl::add_const_if<
        isConst,
        typename configured_type<BaseType, Config, Dimensions>::type
    >::type real_type;

    static typename impl::add_const_if<isConst, ReturnType>::type calculate(real_type& value, int index);
};

template<typename BaseType, typename Config, typename Dimensions, bool isConst>
struct value_at_<BaseType, Config, Dimensions, typename value_at<BaseType, Config, Dimensions>::type&, isConst>
{
    typedef typename impl::add_const_if<
        isConst,
        typename configured_type<BaseType, Config, Dimensions>::type
    >::type real_type;

    typedef typename impl::add_const_if<
        isConst,
        typename value_at<BaseType, Config, Dimensions>::type
    >::type type;

    static type& calculate(real_type& value, int index)
    {
        return value_at<BaseType, Config, Dimensions, isConst>::calculate(value, index);
    };
};

template<typename BaseType, typename Config, typename Dimensions, bool isConst>
struct value_at_<BaseType, Config, Dimensions, configured_array_base<
    BaseType,
    typename value_config<typename item_config<Config>::type>::type,
    false,
    true,
    typename value_at<BaseType, Config, Dimensions>::inner_dimensions
>, isConst>
{
    typedef typename impl::add_const_if<
        isConst,
        typename configured_type<BaseType, Config, Dimensions>::type
    >::type real_type;


    typedef typename impl::add_const_if<
        isConst,
        configured_array_base<
            BaseType,
            typename value_config<typename item_config<Config>::type>::type,
            false,
            true,
            typename value_at<BaseType, Config, Dimensions>::inner_dimensions
        >
    >::type type;

    static type calculate(real_type& value, int index)
    {
        return type(value_at<BaseType, Config, Dimensions, isConst>::calculate(value, index));
    };
};


// Proxy implementation
template<typename BaseType, typename Config, bool AutoAlloc, bool isReference, typename Dimensions>
struct configured_base
{
    typedef BaseType base_type;
    typedef Config config;
    typedef typename mpl::and_<mpl::int_<AutoAlloc>, mpl::not_<mpl::int_<isReference>>>::type do_alloc;

    typedef typename Dimensions::type dimensions;
    typedef typename configured_type<BaseType, Config, dimensions>::type real_type_no_ref;
    typedef typename mpl::if_<mpl::int_<isReference>, real_type_no_ref&, real_type_no_ref>::type real_type;
    typedef typename value_of<BaseType, Config, dimensions>::type value_of_type;


    // Construction and initialization
    template<bool B = !isReference, typename = typename std::enable_if<B, void>::type>
    configured_base() : _value()
    {
        if(do_alloc::value) allocate();
    };

    template<bool B = isReference, typename = typename std::enable_if<B, void>::type>
    explicit configured_base(real_type& value) : _value(value) {};

    // Copy constructor
    configured_base(const configured_base& other) = delete;

    // Assignment operator
    configured_base& operator=(const configured_base& other) = delete;

    // Destruction
    ~configured_base()
    {
        if(do_alloc::value) deallocate();
    };

    // Allocation
    void allocate()
    {
        ::cfg::allocate<base_type, config, dimensions>::run(_value);
    };

    // Deallocation
    void deallocate()
    {
        ::cfg::deallocate<base_type, config, dimensions>::run(_value);
    };

    // Value of
    value_of_type& operator()()
    {
        return ::cfg::value_of<base_type, config, dimensions>::calculate(_value);
    };

    const value_of_type& operator()() const
    {
        return ::cfg::value_of<base_type, config, dimensions, true>::calculate(_value);
    };

    operator value_of_type&()
    {
        return operator()();
    };

    operator const value_of_type&() const
    {
        return operator()();
    };

protected:
    real_type _value;
};

template<typename BaseType, typename Config, bool AutoAlloc, bool isReference, typename Dimensions>
struct configured_array_base : public configured_base<BaseType, Config, AutoAlloc, isReference, Dimensions>
{
    using typename configured_base<BaseType, Config, AutoAlloc, isReference, Dimensions>::dimensions;
    using typename configured_base<BaseType, Config, AutoAlloc, isReference, Dimensions>::base_type;
    using typename configured_base<BaseType, Config, AutoAlloc, isReference, Dimensions>::config;

    using typename configured_base<BaseType, Config, AutoAlloc, isReference, Dimensions>::real_type;
    using typename configured_base<BaseType, Config, AutoAlloc, isReference, Dimensions>::value_of_type;

    typedef typename value_config<typename item_config<Config>::type>::type item_config;
    typedef typename value_at<base_type, config, dimensions>::inner_dimensions inner_dimensions;
    typedef typename configured_type<base_type, item_config, inner_dimensions>::type item_value_of_type;


    configured_array_base() {};

    explicit configured_array_base(real_type& value)
        : configured_base<BaseType, Config, AutoAlloc, isReference, Dimensions>::configured_base(value) {};

    // Copy constructor
    configured_array_base(const configured_array_base& other)
        : configured_base<BaseType, Config, AutoAlloc, isReference, Dimensions>::configured_base(other._value) {};

    // Value converter constructor
    configured_array_base(const std::initializer_list<item_value_of_type>& items)
    {
        int i = 0;
        for(item_value_of_type item : items)
        {
            if(i >= size())
            {
                break;
            }

            (*this)[i++] = item;
        }
    };

    // Assignment operator
    configured_array_base& operator=(const configured_array_base& other)
    {
        if(this != &other) // Protect against self-assignment
        {
            for(int i = 0; i < size(); ++i)
            {
                (*this)[i] = other[i];
            }
        }
        return *this;
    };

    // Initializer list assignment operator
    configured_array_base& operator=(const std::initializer_list<item_value_of_type>& items)
    {
        int i = 0;
        for(item_value_of_type item : items)
        {
            if(i >= size())
            {
                break;
            }

            (*this)[i++] = item;
        }
        return *this;
    };

    typedef typename mpl::if_<
        typename mpl::size<typename value_at<base_type, config, dimensions>::inner_dimensions>::type,
        configured_array_base<
            base_type,
            item_config,
            false,
            true,
            inner_dimensions
        >,
        typename value_at<base_type, config, dimensions>::type&
    >::type value_at_type;

    // Array rank (number of dimensions)
    const int rank() const
    {
        return ::cfg::array_rank<config>::type::value;
    };

    // Length of the first dimension
    const int size() const
    {
        return mpl::front<dimensions>::type::value;
    };

    // Value at a specified array index
    value_at_type operator[](const int index)
    {
        return value_at_<base_type, config, dimensions, value_at_type>::calculate(this->_value, index);
    };

    // The return type 'const value_at_type' handles the reference incorrectly
    typename mpl::if_<
        typename mpl::size<typename value_at<base_type, config, dimensions>::inner_dimensions>::type,
        const configured_array_base<
            base_type,
            item_config,
            false,
            true,
            inner_dimensions
        >,
        const typename value_at<base_type, config, dimensions>::type&
    >::type operator[](const int index) const
    {
        return value_at_<base_type, config, dimensions, value_at_type, true>::calculate(this->_value, index);
    };
};


template<typename BaseType, typename Config, bool AutoAlloc>
struct configured_base_assign_and_member
    : public configured_base<BaseType, Config, AutoAlloc, false, mpl::vector_c<int>>
{
    using typename configured_base<BaseType, Config, AutoAlloc, false, mpl::vector_c<int>>::value_of_type;
    using typename configured_base<BaseType, Config, AutoAlloc, false, mpl::vector_c<int>>::operator();

    // Assignment operator
    configured_base_assign_and_member& operator=(const configured_base_assign_and_member& other)
    {
        if(this != &other) // Protect against self-assignment
        {
            this->operator()() = other.operator()();
        }
        return *this;
    };

    // Structure member access
    typename boost::add_pointer<value_of_type>::type operator->()
    {
        return &(this->operator()());
    };

    typename boost::add_pointer<const value_of_type>::type operator->() const
    {
        return &(this->operator()());
    };
};


} // End namespace cfg::impl


template<typename BaseType, typename Config, bool AutoAlloc = true>
struct configured
    : public impl::configured_base_assign_and_member<BaseType, Config, AutoAlloc> {};

template<typename BaseType, typename Config>
struct configured<BaseType, Config, true>
    : public impl::configured_base_assign_and_member<BaseType, Config, true>
{
    using typename impl::configured_base_assign_and_member<BaseType, Config, true>::value_of_type;
    using typename impl::configured_base_assign_and_member<BaseType, Config, true>::operator();

    configured() {};

    // Copy constructor
    configured(const configured& other)
    {
        this->operator()() = other.operator()();
    };

    // Value converter constructor
    configured(const value_of_type& value)
    {
        this->operator()() = value;
    };
};

template<typename BaseType, typename Config, bool AutoAlloc, int ...Dimensions>
struct configured_array
    : public impl::configured_array_base<BaseType, Config, AutoAlloc, false, mpl::vector_c<int,Dimensions...>> {};

template<typename BaseType, typename Config, int ...Dimensions>
struct configured_array<BaseType, Config, true, Dimensions...>
    : public impl::configured_array_base<BaseType, Config, true, false, mpl::vector_c<int,Dimensions...>>
{
    using typename impl::configured_array_base<
        BaseType,
        Config,
        true,
        false,
        mpl::vector_c<int, Dimensions...>
    >::item_value_of_type;

    configured_array() {};

    // Copy constructor
    configured_array(const configured_array& other)
    {
        for(int i = 0; i < this->size(); ++i)
        {
            (*this)[i] = other[i];
        }
    };

    // Value converter constructor (forwarded)
    configured_array(const std::initializer_list<item_value_of_type>& items)
        : impl::configured_array_base<BaseType, Config, true, false, mpl::vector_c<int, Dimensions...>>
            :: configured_array_base(items) {};
};


} // End namespace cfg


#undef ALLOCATE_FUNCTION
#undef DEALLOCATE_FUNCTION


#endif // TEMPLATE_CONFIG_HPP_INCLUDED
