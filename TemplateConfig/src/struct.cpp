#include <cstdio>
#include <cstdbool>
#include <cstring>
#include "template_config.hpp"

using namespace cfg;


#define CONFIG_MINIMAL // configuration switch

#ifdef CONFIG_MINIMAL // this is the "minimal" working configuration for this code
    typedef SCALAR CFG_t0;
    typedef SCALAR CFG_tx;
    typedef ARRAY<SCALAR> CFG_ty;
#else
    typedef PTR<SCALAR> CFG_t0;
    typedef PTR<PTR<SCALAR>> CFG_tx;
    typedef PTR<ARRAY<SCALAR>> CFG_ty;
#endif


BOOST_MPL_ASSERT_MSG( // configuration constraint determined by the algorithm
    (mpl::not_<has_ptr_item<CFG_ty>>::type::value),
    The_inner_element_configuration_of_member__test_t__y__must_be_SCALAR_as_constrained_by__strcpy,
    (CFG_ty)
);


typedef struct test_t
{
    int id;
    configured<bool, CFG_tx> x;
    configured_array<char, CFG_ty, true, 8> y;
} test_t;

int main()
{
    configured<test_t, CFG_t0> t0;
    test_t t1 = { 1, true };

    t0->id = 0;
    t0->x  = true;
    strcpy(t0->y, "test");
    printf("t0: { %d, %d, \"%s\" }\n", t0->id, t0->x(), t0->y());

    strcpy(t1.y, "test");
    printf("t1: { %d, %d, \"%s\" }\n", t1.id, t1.x(), t1.y());

    return 0;
}
