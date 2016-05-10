#include <boost/preprocessor.hpp>

// Prevent including malloc.h
#define ALLOCATE_FUNCTION malloc
#define DEALLOCATE_FUNCTION free
#include "macro_config.h"


// Test configurations
#define C_S SCALAR
#define C_PS PTR(SCALAR)
#define C_PPS PTR(PTR(SCALAR))
#define C_PPPS PTR(PTR(PTR(SCALAR)))
#define C_AS ARRAY(SCALAR)
#define C_PAS PTR(ARRAY(SCALAR))
#define C_PPAS PTR(PTR(ARRAY(SCALAR)))
#define C_APS ARRAY(PTR(SCALAR))
#define C_APPS ARRAY(PTR(PTR(SCALAR)))
#define C_PAPS PTR(ARRAY(PTR(SCALAR)))
#define C_PAPPS PTR(ARRAY(PTR(PTR(SCALAR))))
#define C_PPAPS PTR(PTR(ARRAY(PTR(SCALAR))))
#define C_PPAPPS PTR(PTR(ARRAY(PTR(PTR(SCALAR)))))
#define C_AAS ARRAY(ARRAY(SCALAR))
#define C_PAAS PTR(ARRAY(ARRAY(SCALAR)))
#define C_APAS ARRAY(PTR(ARRAY(SCALAR)))
#define C_AAPS ARRAY(ARRAY(PTR(SCALAR)))
#define C_APAPS ARRAY(PTR(ARRAY(PTR(SCALAR))))
#define C_PAPAS PTR(ARRAY(PTR(ARRAY(SCALAR))))
#define C_PAPAPS PTR(ARRAY(PTR(ARRAY(PTR(SCALAR)))))
#define C_PPAPPAPPS PTR(PTR(ARRAY(PTR(PTR(ARRAY(PTR(PTR(SCALAR))))))))

#define SCALAR_CONFIGURATIONS (C_S)(C_PS)(C_PPS)(C_PPPS)

#define ARRAY_CONFIGURATIONS  (C_AS)   (C_PAS)  (C_PPAS)  (C_APS)   (C_APPS)     (C_PAPS) \
                              (C_PAPPS)(C_PPAPS)(C_PPAPPS)(C_AAS)   (C_PAAS)     (C_APAS) \
                              (C_AAPS) (C_APAPS)(C_PAPAS) (C_PAPAPS)(C_PPAPPAPPS)

#define ARRAY_DIMENSIONS      ((2))    ((4))    ((8))     ((16))    ((32))       ((64))   \
                              ((2))    ((4))    ((8))     ((16,32)) ((2,4))      ((8,16)) \
                              ((32,2)) ((4,8))  ((16,32)) ((2,4))   ((8,16))

#define ALL_CONFIGURATIONS    SCALAR_CONFIGURATIONS ARRAY_CONFIGURATIONS


// Convert a test configuration into a label
#define MAKE_LABEL(prefix, config) BOOST_PP_CAT(prefix, BOOST_PP_CAT(__, BOOST_PP_TUPLE_ELEM(0, BOOST_PP_SEQ_FOLD_LEFT(MAKE_LABEL_, (BOOST_PP_EMPTY(), 0), config)))):
#define MAKE_LABEL_(s, state, item) \
    (BOOST_PP_CAT( \
        BOOST_PP_TUPLE_ELEM(0, state), \
        BOOST_PP_IIF( \
            BOOST_PP_GREATER(BOOST_PP_TUPLE_ELEM(1, state), 0), \
            BOOST_PP_CAT(_, item), \
            item \
        ) \
    ), BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(1, state)))


// Configurable function arguments
#define ARGUMENT_WRAPPER(r, data, index, elem) \
/*  MAKE_LABEL(ARGUMENT, elem) */ \
    void BOOST_PP_CAT(f, index)(ARGUMENT( \
        BOOST_PP_TUPLE_ELEM(0, data), \
        BOOST_PP_TUPLE_ELEM(1, data), \
        elem \
    ));
BOOST_PP_SEQ_FOR_EACH_I(ARGUMENT_WRAPPER, (int, x), SCALAR_CONFIGURATIONS)
#undef ARGUMENT_WRAPPER

#define ARRAY_ARGUMENT_WRAPPER(r, data, index, elem) \
/*    MAKE_LABEL(ARRAY_ARGUMENT, elem) */ \
    void BOOST_PP_CAT(f, BOOST_PP_ADD(index, BOOST_PP_SEQ_SIZE(SCALAR_CONFIGURATIONS)))(ARRAY_ARGUMENT( \
        BOOST_PP_TUPLE_ELEM(0, data), \
        BOOST_PP_TUPLE_ELEM(1, data), \
        elem, \
        BOOST_PP_TUPLE_ENUM(BOOST_PP_SEQ_ELEM(index, ARRAY_DIMENSIONS)) \
    ));
BOOST_PP_SEQ_FOR_EACH_I(ARRAY_ARGUMENT_WRAPPER, (int, x), ARRAY_CONFIGURATIONS)
#undef ARRAY_ARGUMENT_WRAPPER


// Configurable function return value
#define DECLARE_FUNCTION_WRAPPER(r, data, index, elem) \
/*    MAKE_LABEL(DECLARE_FUNCTION, elem) */ \
    DECLARE_FUNCTION( \
        BOOST_PP_TUPLE_ELEM(0, data), \
        BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(1, data), index), \
        BOOST_PP_TUPLE_ELEM(2, data), \
        elem \
    );
BOOST_PP_SEQ_FOR_EACH_I(DECLARE_FUNCTION_WRAPPER, (int, g, (void)), ALL_CONFIGURATIONS)
#undef DECLARE_FUNCTION_WRAPPER


void test()
{
// Declaration
    #define DECLARE_WRAPPER(r, data, index, elem) \
        MAKE_LABEL(DECLARE, elem); \
        DECLARE( \
            BOOST_PP_TUPLE_ELEM(0, data), \
            BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(1, data), index), \
            elem \
        );
    BOOST_PP_SEQ_FOR_EACH_I(DECLARE_WRAPPER, (int, a), SCALAR_CONFIGURATIONS)
    #undef DECLARE_WRAPPER

    #define DECLARE_ARRAY_WRAPPER(r, data, index, elem) \
        MAKE_LABEL(DECLARE_ARRAY, elem); \
        DECLARE_ARRAY( \
            BOOST_PP_TUPLE_ELEM(0, data), \
            BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(1, data), BOOST_PP_ADD(index, BOOST_PP_SEQ_SIZE(SCALAR_CONFIGURATIONS))), \
            elem, \
            BOOST_PP_TUPLE_ENUM(BOOST_PP_SEQ_ELEM(index, ARRAY_DIMENSIONS)) \
        );
    BOOST_PP_SEQ_FOR_EACH_I(DECLARE_ARRAY_WRAPPER, (int, a), ARRAY_CONFIGURATIONS)
    #undef DECLARE_ARRAY_WRAPPER


// Allocation
    #define ALLOCATE_WRAPPER(r, data, index, elem) \
        MAKE_LABEL(ALLOCATE, elem) \
        ALLOCATE(BOOST_PP_CAT(data, index), elem);
    BOOST_PP_SEQ_FOR_EACH_I(ALLOCATE_WRAPPER, a, ALL_CONFIGURATIONS)
    #undef ALLOCATE_WRAPPER


// Value of
    #define VALUE_OF_WRAPPER(r, data, index, elem) \
        MAKE_LABEL(VALUE_OF, elem) \
        VALUE_OF(BOOST_PP_CAT(data, index), elem);
    BOOST_PP_SEQ_FOR_EACH_I(VALUE_OF_WRAPPER, a, ALL_CONFIGURATIONS)
    #undef VALUE_OF_WRAPPER


// Value at a specified array index
    #define VALUE_AT_WRAPPER(r, data, index, elem) \
        MAKE_LABEL(VALUE_AT, elem) \
        VALUE_AT( \
            BOOST_PP_CAT(data, BOOST_PP_ADD(index, BOOST_PP_SEQ_SIZE(SCALAR_CONFIGURATIONS))), \
            elem, \
            BOOST_PP_TUPLE_ENUM(BOOST_PP_SEQ_ELEM(index, ARRAY_DIMENSIONS)) \
        );
    BOOST_PP_SEQ_FOR_EACH_I(VALUE_AT_WRAPPER, a, ARRAY_CONFIGURATIONS)
    #undef VALUE_AT_WRAPPER

// Deallocation
    #define DEALLOCATE_WRAPPER(r, data, index, elem) \
        MAKE_LABEL(DEALLOCATE, elem) \
        DEALLOCATE(BOOST_PP_CAT(data, index), elem);
    BOOST_PP_SEQ_FOR_EACH_I(DEALLOCATE_WRAPPER, a, ALL_CONFIGURATIONS)
    #undef DEALLOCATE_WRAPPER
}
