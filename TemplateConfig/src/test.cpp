#include <cstdio>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/string.hpp>
#include "template_config.hpp"

using namespace cfg;


// Test configurations
typedef SCALAR c_s;
typedef PTR<SCALAR> c_ps;
typedef PTR<PTR<SCALAR>> c_pps;
typedef PTR<PTR<PTR<SCALAR>>> c_ppps;
typedef ARRAY<SCALAR> c_as;
typedef PTR<ARRAY<SCALAR>> c_pas;
typedef PTR<PTR<ARRAY<SCALAR>>> c_ppas;
typedef ARRAY<PTR<SCALAR>> c_aps;
typedef ARRAY<PTR<PTR<SCALAR>>> c_apps;
typedef PTR<ARRAY<PTR<SCALAR>>> c_paps;
typedef PTR<ARRAY<PTR<PTR<SCALAR>>>> c_papps;
typedef PTR<PTR<ARRAY<PTR<SCALAR>>>> c_ppaps;
typedef PTR<PTR<ARRAY<PTR<PTR<SCALAR>>>>> c_ppapps;
typedef ARRAY<ARRAY<SCALAR>> c_aas;
typedef PTR<ARRAY<ARRAY<SCALAR>>> c_paas;
typedef ARRAY<PTR<ARRAY<SCALAR>>> c_apas;
typedef ARRAY<ARRAY<PTR<SCALAR>>> c_aaps;
typedef ARRAY<PTR<ARRAY<PTR<SCALAR>>>> c_apaps;
typedef PTR<ARRAY<PTR<ARRAY<SCALAR>>>> c_papas;
typedef PTR<ARRAY<PTR<ARRAY<PTR<SCALAR>>>>> c_papaps;
typedef PTR<PTR<ARRAY<PTR<PTR<ARRAY<PTR<PTR<SCALAR>>>>>>>> c_ppappapps;

typedef mpl::vector<
    c_s, c_ps, c_pps, c_ppps
>::type scalar_configurations;

typedef mpl::vector<
    c_as, c_pas, c_ppas, c_aps, c_apps, c_paps, c_papps, c_ppaps, c_ppapps
>::type rank1_array_configurations;

typedef mpl::vector<
    c_aas, c_paas, c_apas, c_aaps,  c_apaps, c_papas, c_papaps, c_ppappapps
>::type rank2_array_configurations;

typedef mpl::joint_view<rank1_array_configurations, rank2_array_configurations>::type array_configurations;
typedef mpl::joint_view<scalar_configurations, array_configurations>::type all_configurations;

template<typename BT, typename C, bool A = true> struct array_def {};
template<typename BT, bool A> struct array_def<BT, c_as, A>        { typedef configured_array<BT, c_as, A, 2> type; };
template<typename BT, bool A> struct array_def<BT, c_pas, A>       { typedef configured_array<BT, c_pas, A, 4> type; };
template<typename BT, bool A> struct array_def<BT, c_ppas, A>      { typedef configured_array<BT, c_ppas, A, 8> type; };
template<typename BT, bool A> struct array_def<BT, c_aps, A>       { typedef configured_array<BT, c_aps, A, 16> type; };
template<typename BT, bool A> struct array_def<BT, c_apps, A>      { typedef configured_array<BT, c_apps, A, 32> type; };
template<typename BT, bool A> struct array_def<BT, c_paps, A>      { typedef configured_array<BT, c_paps, A, 64> type; };
template<typename BT, bool A> struct array_def<BT, c_papps, A>     { typedef configured_array<BT, c_papps, A, 2> type; };
template<typename BT, bool A> struct array_def<BT, c_ppaps, A>     { typedef configured_array<BT, c_ppaps, A, 4> type; };
template<typename BT, bool A> struct array_def<BT, c_ppapps, A>    { typedef configured_array<BT, c_ppapps, A, 8> type; };
template<typename BT, bool A> struct array_def<BT, c_aas, A>       { typedef configured_array<BT, c_aas, A, 16, 32> type; };
template<typename BT, bool A> struct array_def<BT, c_paas, A>      { typedef configured_array<BT, c_paas, A, 2, 4> type; };
template<typename BT, bool A> struct array_def<BT, c_apas, A>      { typedef configured_array<BT, c_apas, A, 8, 16> type; };
template<typename BT, bool A> struct array_def<BT, c_aaps, A>      { typedef configured_array<BT, c_aaps, A, 32, 2> type; };
template<typename BT, bool A> struct array_def<BT, c_apaps, A>     { typedef configured_array<BT, c_apaps, A, 4, 8> type; };
template<typename BT, bool A> struct array_def<BT, c_papas, A>     { typedef configured_array<BT, c_papas, A, 16, 32> type; };
template<typename BT, bool A> struct array_def<BT, c_papaps, A>    { typedef configured_array<BT, c_papaps, A, 2, 4> type; };
template<typename BT, bool A> struct array_def<BT, c_ppappapps, A> { typedef configured_array<BT, c_ppappapps, A, 8, 16> type; };


// Test helper utilities
template<typename Config>
std::string to_string()
{
    return mpl::c_str<typename mpl::fold<
        typename Config::type,
        mpl::string<>,
        mpl::push_back<mpl::_1, mpl::if_<
            is_array_token<mpl::_2>,
            mpl::char_<'A'>,
            mpl::if_<
                is_ptr_token<mpl::_2>,
                mpl::char_<'P'>,
                mpl::char_<'S'>
            >
        >>
    >::type>::value;
};

static int failure_counter;

template<typename Config> void print_result(const char * test_name, bool result)
{
    printf("%s (%s) [%s]\n", test_name, to_string<Config>().c_str(), result ? "success" : "failure");

    if(!result)
    {
        ++failure_counter;
    }
}


// Item configuration
template<bool ScalarInput>
struct item_config_tests
{
    template<typename Config> void operator()(Config)
    {
        if(ScalarInput)
        {
            print_result<Config>(
                "Scalar item configuration is empty",
                0 == mpl::size<typename item_config<Config>::type>::value
            );
        }
        else
        {
            typedef typename item_config<Config>::type item_config;

            print_result<Config>(
                "Array item configuration is not empty",
                0 < mpl::size<item_config>::value
            );

            print_result<Config>(
                "Rank of array item configuration is correct",
                array_rank<item_config>::value + 1 == array_rank<Config>::value
            );
        }
    };
};

// Item configuration pointer checking
struct has_ptr_item_tests
{
    template<typename Config> void operator()(Config)
    {
        typedef typename has_ptr_item<Config>::type type;
    };
};

// Array rank (the number of dimensions) of a configuration
template<bool ScalarInput>
struct array_rank_tests
{
    template<typename Config> void operator()(Config)
    {
        const int rank = array_rank<Config>::type::value;

        if(ScalarInput) print_result<Config>("Scalar configuration rank is zero", 0 == rank);
        else print_result<Config>("Array configuration rank is greater than zero", 0 < rank);
    };
};

// Value configuration (dereferenced)
struct value_config_tests
{
    template<typename Config> void operator()(Config)
    {
        typedef typename value_config<Config>::type type;

        print_result<Config>("Value configuration is not empty", 0 < mpl::size<type>::value);
    };
};

// Scalar (non-array, non-struct) objects
struct scalar_tests
{
    template<typename Config> void operator()(Config)
    {
        configured<int, Config> a = 42;
        print_result<Config>("Scalar tests: Scalar initialization", 42 == a);

        configured<int, Config> ac(a);
        print_result<Config>("Scalar tests: Copy constructor", a() == ac());

        configured<int, Config> aa;
        aa = a;
        print_result<Config>("Scalar tests: Assignment operator", a() == aa());

        configured<int, Config> b = a();
        print_result<Config>("Scalar tests: Value converter constructor", 42 == b);

        b = 64;
        print_result<Config>("Scalar tests: Assignment from scalar", 64 == b);
        print_result<Config>("Scalar tests: Assignment is independent", 42 == a);

        typename decltype(a)::value_of_type& avr = a;
        avr = 32;
        print_result<Config>("Scalar tests: Value reference assignment", 32 == a);

        decltype(b)& bpr = b;
        bpr = 16;
        print_result<Config>("Scalar tests: Proxy reference assignment", 16 == b);

        configured<int, Config, false> c;
        c.allocate();
        c() = 42;
        print_result<Config>("Scalar tests: Manual allocation", 42 == c);
        c.deallocate();

        configured<int, Config> d = testAsReturnValue<Config>();
        print_result<Config>("Scalar tests: Function return value", 30 == d);

        testAsValueArg<Config>(d);
        print_result<Config>("Scalar tests: Passing as value argument", 30 == d);

        testAsRefArg<Config>(d);
        print_result<Config>("Scalar tests: Passing as reference argument", 10 == d);
    };

    template<typename Config> configured<int, Config> testAsReturnValue() { return 30; };

    template<typename Config> void testAsValueArg(configured<int, Config> arg) { arg = 20; };

    template<typename Config> void testAsRefArg(configured<int, Config>& arg) { arg = 10; };
};

// Array objects (rank 1)
struct rank1_array_tests
{
    template<typename Config> void operator()(Config)
    {
        typename array_def<double, Config, true>::type a = { 42 };
        print_result<Config>("Array (rank 1) tests: Rank is correct", 1 == a.rank());
        print_result<Config>("Array (rank 1) tests: Initialization from list", 42 == a[0]);

        decltype(a) ac(a);
        print_result<Config>("Array (rank 1) tests: Copy constructor", a[0] == ac[0]);

        decltype(a) aa;
        aa = a;
        print_result<Config>("Array (rank 1) tests: Assignment operator", a[0] == aa[0]);

        typename decltype(a)::value_at_type& avr = a[0];
        avr = 32;
        print_result<Config>("Array (rank 1) tests: Value reference assignment", 32 == a[0]);

        a[0] = 16;
        print_result<Config>("Array (rank 1) tests: Index assignment", 16 == a[0]);

        decltype(a)& apr = a;
        apr[0] = 8;
        print_result<Config>("Array (rank 1) tests: Proxy reference assignment", 8 == a[0]);

        typename array_def<double, Config, false>::type b;
        b.allocate();
        b[0] = 42;
        print_result<Config>("Array (rank 1) tests: Manual allocation", 42 == b[0]);
        b.deallocate();

        typename array_def<double, Config, true>::type c = testAsReturnValue<Config>();
        print_result<Config>("Array (rank 1) tests: Function return value", 30 == c[0]);

        testAsValueArg<Config>(c);
        print_result<Config>("Array (rank 1) tests: Passing as value argument", 30 == c[0]);

        testAsRefArg<Config>(c);
        print_result<Config>("Array (rank 1) tests: Passing as reference argument", 10 == c[0]);
    };

    template<typename Config> typename array_def<double, Config, true>::type testAsReturnValue() { return {30}; };

    template<typename Config> void testAsValueArg(typename array_def<double, Config, true>::type arg) { arg = {20}; };

    template<typename Config> void testAsRefArg(typename array_def<double, Config, true>::type& arg) { arg = {10}; };
};

// Array objects (rank 2)
struct rank2_array_tests
{
    template<typename Config> void operator()(Config)
    {
        typename array_def<double, Config, true>::type a;
        print_result<Config>("Array (rank 2) tests: Rank is correct", 2 == a.rank());

        a[0][0] = 42;
        print_result<Config>("Array (rank 2) tests: Multiple index assignment", 42 == a[0][0]);

        typename decltype(a)::value_at_type a0 = a[0];
        a0[0] = 32;
        print_result<Config>("Array (rank 2) tests: Partial reference index assignment", 32 == a[0][0]);

        a0[0] = { 16 };
        print_result<Config>("Array (rank 2) tests: Partial reference index initializer", 16 == a[0][0]);
    };
};

// Structure objects
struct struct_tests
{
    template<typename Config> void operator()(Config)
    {
        struct p
        {
            int u;
            configured<long, Config> v;
        };

        struct q
        {
            char x;
            configured<double, Config> y;
            configured<p, Config> z;
        };

        configured<q, Config> a = (q){ 1, 2.0, (p){ 3, 4 } };
        print_result<Config>(
            "Structure tests: Nested structure initializer",
            1 == a->x && 2.0 == a->y && 3 == a->z->u && 4 == a->z->v
        );

        a->x = 2;
        a->z->u = 6;
        print_result<Config>("Structure tests: Member assignment", 2 == a->x && 6 == a->z->u);

        p bp = { 1, 2 };
        configured<p, Config> b = bp;
        print_result<Config>("Structure tests: Structure assignment initializer", 1 == b->u && 2 == b->v);

        struct r
        {
            configured<int, Config, false> s;
        };

        configured<r, Config> c;
        c->s.allocate();
        c->s() = 42;
        print_result<Config>("Structure tests: Manual field allocation", 42 == c->s);
        c->s.deallocate();
    };
};


int main()
{
    // Item configuration
    mpl::for_each<scalar_configurations>(item_config_tests<true>());
    mpl::for_each<array_configurations>(item_config_tests<false>());

    // Item configuration pointer checking
    mpl::for_each<all_configurations>(has_ptr_item_tests());

    // Array rank (the number of dimensions) of a configuration
    mpl::for_each<scalar_configurations>(array_rank_tests<true>());
    mpl::for_each<array_configurations>(array_rank_tests<false>());

    // Value configuration (dereferenced)
    mpl::for_each<all_configurations>(value_config_tests());

    // Scalar (non-array, non-struct) objects
    mpl::for_each<scalar_configurations>(scalar_tests());

    // Array objects (rank 1)
    mpl::for_each<rank1_array_configurations>(rank1_array_tests());

    // Array objects (rank 2)
    mpl::for_each<rank2_array_configurations>(rank2_array_tests());

    // Structure objects
    mpl::for_each<scalar_configurations>(struct_tests());

    // Test summary
    if(failure_counter > 0)
    {
        printf("\n%d tests failed.\n", failure_counter);
        return -1;
    }
    else
    {
        puts("\nAll tests were successful.");
        return 0;
    }
}
