#ifndef SEAMOCK_H_
#define SEAMOCK_H_

#include <boost/hana/fwd/intersection.hpp>
#include <boost/hana/fwd/tuple.hpp>
#include <boost/hana/string.hpp>
#include <seahorn/seahorn.h>

#include <algorithm>
#include <array>
#include <boost/hana.hpp>
#include <boost/hana/assert.hpp>
#include <boost/preprocessor.hpp>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

namespace hana = boost::hana;
using namespace hana::literals;

extern "C" {
extern void sea_printf(const char *format, ...);
extern bool nd_bool(void);
}

#define CREATE_PARAM(r, data, idx, type) (type BOOST_PP_CAT(arg, idx))
#define CREATE_ARG(r, data, idx, type) (BOOST_PP_CAT(arg, idx))
#define HANA_STRINGIZE_OP(s, _data, elem)                                      \
  (BOOST_HANA_STRING(BOOST_PP_STRINGIZE(elem)))

// Expectation map keys
#define CALL_FN_NAME BOOST_HANA_STRING("call_fn_name")
#define RETURN_FN BOOST_HANA_STRING("return_fn")
#define CAPTURE_ARGS_MAPS BOOST_HANA_STRING("capture_map")
#define TIMES_FN BOOST_HANA_STRING("cardinality_fn")
#define AFTER BOOST_HANA_STRING("predecessors")
#define INVOKE_FN BOOST_HANA_STRING("invoke_fn_on_args")
#define SEQ_COUNTER_MAXVAL 10
static size_t g_sequence_counter;

// Define the check to run on a given function
// NOTE: Current impl does not support querying the return function since
// that is set local to the function.
#define POST_CHECK(r, data, fn_name)                                           \
  do {                                                                         \
    auto cardinality_fn_optional =                                             \
        hana::find(BOOST_PP_CAT(expectations_map_w_name_, fn_name), TIMES_FN); \
    hana::eval_if(                                                             \
        hana::is_nothing(cardinality_fn_optional), [] {},                      \
        [&](auto _) {                                                          \
          sassert(_(cardinality_fn_optional)                                   \
                      .value()(BOOST_PP_CAT(timesCounter_, fn_name)));         \
        });                                                                    \
  } while (0);

// Define the checks to conducts on the given tuple of function names
// names_tuple: a tuple of function names on which to run post (condition)
// checks
#define SETUP_POST_CHECKS(names_tuple)                                         \
  void postchecks_ok() {                                                       \
    BOOST_PP_LIST_FOR_EACH(POST_CHECK, _, BOOST_PP_TUPLE_TO_LIST(names_tuple)) \
  }

static constexpr auto DefaultExpectationsMap =
    hana::make_map(hana::make_pair(CALL_FN_NAME, -1_c),
                   // hana::make_pair(TIMES_FN, -1_c),
                   hana::make_pair(RETURN_FN, -1_c),
                   hana::make_pair(CAPTURE_ARGS_MAPS, hana::make_map()),
                   hana::make_pair(AFTER, hana::make_tuple()));

BOOST_HANA_CONSTEXPR_LAMBDA
auto Times = [](auto times_fn_val, auto expectations_map) {
  static_assert(
      hana::sfinae(times_fn_val)(0_c) != hana::nothing,
      "Use Eq(<int_const>), Lt(<int_const>), Gt(<int_const>) in Times action!");
  auto tmp = hana::erase_key(expectations_map, TIMES_FN);
  return hana::insert(tmp, hana::make_pair(TIMES_FN, times_fn_val));
};

namespace seamock {
template <int val>
constexpr auto Eq() {
    return hana::equal.to(hana::int_c<val>);
}


template <int val>
constexpr auto Lt() {
    return hana::less.than(hana::int_c<val>);
}

template <int val>
constexpr auto Gt() {
    return hana::greater.than(hana::int_c<val>);
}

} // namespace seamock

// TODO: untested
auto And = [](auto vals...) { return hana::demux(hana::and_, vals); };

BOOST_HANA_CONSTEXPR_LAMBDA auto ReturnFn = [](auto ret_fn_val,
                                               auto expectations_map) {
  auto tmp = hana::erase_key(expectations_map, RETURN_FN);
  return hana::insert(tmp, hana::make_pair(RETURN_FN, ret_fn_val));
};

// TODO: Allow one-by-one capture rather than force all-at-once.
BOOST_HANA_CONSTEXPR_LAMBDA auto Capture = [](auto capture_map,
                                              auto expectations_map) {
  auto tmp = hana::erase_key(expectations_map, CAPTURE_ARGS_MAPS);
  return hana::insert(tmp, hana::make_pair(CAPTURE_ARGS_MAPS, capture_map));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto InvokeFn = [](auto invokeFn,
                                               auto expectations_map) {
  auto tmp = hana::erase_key(expectations_map, INVOKE_FN);
  return hana::insert(tmp, hana::make_pair(INVOKE_FN, invokeFn));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto Expect = [](auto func, auto arg0) {
  return hana::partial(func, arg0);
};

BOOST_HANA_CONSTEXPR_LAMBDA auto MakeExpectation = [](auto func) {
  return hana::apply(func, DefaultExpectationsMap);
};

BOOST_HANA_CONSTEXPR_LAMBDA auto AND =
    hana::infix([](auto fn_x, auto fn_y) { return hana::compose(fn_x, fn_y); });


#define UNPACK_TRANSFORM_TUPLE(func, tuple)                                    \
  BOOST_PP_TUPLE_ENUM(BOOST_PP_SEQ_TO_TUPLE(                                   \
      BOOST_PP_SEQ_FOR_EACH_I(func, DONT_CARE, BOOST_PP_TUPLE_TO_SEQ(tuple))))

// set is actually a sequence but we don't account for order so
// tell the user they are making a set
#define MAKE_PRED_FN_SET(fn_names...)                                          \
  BOOST_PP_VARIADIC_TO_SEQ(fn_names)

#define CREATE_ND_FUNC_NAME(name, prefix)                                      \
  BOOST_PP_CAT(nd_, BOOST_PP_CAT(name, BOOST_PP_CAT(prefix, _fn)))

#define LAZY_MOCK_FUNCTION(name, ret_type, args_tuple)                         \
  MOCK_FUNCTION(name, DefaultExpectationsMap, ret_type, args_tuple)

// assumes return type is int
// function can return [MIN_INT, 0]
#define SUC_MOCK_FUNCTION(name, args_tuple)                                    \
  extern int CREATE_ND_FUNC_NAME(name, _ret)(void);                            \
  BOOST_HANA_CONSTEXPR_LAMBDA auto name##_ret_fn = []() {                      \
    int ret = CREATE_ND_FUNC_NAME(name, _ret)();                               \
    assume(ret == 0);                                                          \
    return ret;                                                                \
  };                                                                           \
  constexpr auto name##_suc_map =                                              \
      ReturnFn(name##_ret_fn, DefaultExpectationsMap);                         \
  MOCK_FUNCTION(name, name##_suc_map, int, args_tuple)

// assumes return type is int
// function can return [MIN_INT, 0]
#define ERR_SUC_MOCK_FUNCTION(name, args_tuple)                                \
  extern int CREATE_ND_FUNC_NAME(name, _ret)(void);                            \
  BOOST_HANA_CONSTEXPR_LAMBDA auto name##_ret_fn = []() {                      \
    int ret = CREATE_ND_FUNC_NAME(name, _ret)();                               \
    assume(ret <= 0);                                                          \
    return ret;                                                                \
  };                                                                           \
  constexpr auto name##_err_suc_map =                                          \
      ReturnFn(name##_ret_fn, DefaultExpectationsMap);                         \
  MOCK_FUNCTION(name, name##_err_suc_map, int, args_tuple)

#define MOCK_FUNCTION(name, expectations_map, ret_type, args_tuple)            \
  MOCK_FUNCTION_W_ORDER(name, expectations_map, BOOST_PP_SEQ_NIL /* order seq */, ret_type, args_tuple)

#define ASSERT_EXECUTED_EACH(_r, _data, elem) do { sassert(BOOST_PP_CAT(g_execute_prophecy_, elem) == true); } while(0)


#define MOCK_FUNCTION_W_ORDER(name, expectations_map, order_seq, ret_type, args_tuple)  \
  static int timesCounter_##name = 0;                                          \
  static bool BOOST_PP_CAT(g_execute_prophecy_,name) = false;                  \
  constexpr auto expectations_map_w_name_##name =                              \
      hana::insert(hana::erase_key(expectations_map, CALL_FN_NAME),            \
                   hana::make_pair(CALL_FN_NAME, BOOST_HANA_STRING(#name)));   \
  static_assert(hana::at_key(expectations_map_w_name_##name, CALL_FN_NAME) !=  \
                -1_c);                                                         \
  ret_type name(UNPACK_TRANSFORM_TUPLE(CREATE_PARAM, args_tuple)) {            \
    BOOST_PP_CAT(g_execute_prophecy_,name) = true;                             \
    /* NOTE: check that after constraint is maintained */                      \
    BOOST_PP_SEQ_FOR_EACH(ASSERT_EXECUTED_EACH, /* data */ , order_seq);       \
    auto cardinality_fn_optional = hana::find(expectations_map, TIMES_FN);     \
    hana::eval_if(                                                             \
        hana::is_nothing(cardinality_fn_optional), [] {},                      \
        [&](auto _) {                                                          \
          timesCounter_##name = timesCounter_##name + 1;                       \
          _(cardinality_fn_optional).value()(timesCounter_##name);             \
        });                                                                    \
    return hana::eval_if(                                                      \
        hana::at_key(expectations_map, RETURN_FN) ==                           \
            -1_c /*&&  hana::at_key(expectations_map, INVOKE_FN) == -1_c*/,    \
        [&]() {                                                                \
          extern ret_type CREATE_ND_FUNC_NAME(name, _ret)(void);               \
          BOOST_HANA_CONSTEXPR_LAMBDA auto name##_ret_fn = []() {              \
            return CREATE_ND_FUNC_NAME(name, _ret)();                          \
          };                                                                   \
          constexpr auto tmp =                                                 \
              hana::erase_key(expectations_map_w_name_##name, RETURN_FN);      \
          constexpr auto new_map =                                             \
              hana::insert(tmp, hana::make_pair(RETURN_FN, name##_ret_fn));    \
          /*auto partfn = hana::partial(skeletal, new_map); */                 \
          return hana::apply(skeletal, new_map,                                \
                             hana::make_tuple(UNPACK_TRANSFORM_TUPLE(          \
                                 CREATE_ARG, args_tuple)));                    \
        },                                                                     \
        [&]() {                                                                \
          return hana::apply(skeletal, expectations_map_w_name_##name,         \
                             hana::make_tuple(UNPACK_TRANSFORM_TUPLE(          \
                                 CREATE_ARG, args_tuple)));                    \
        });                                                                    \
  }

// Define a function to check if it's valid to call F with Args...
template <typename F, typename Tuple>
auto is_callable_with_args(F&& f, Tuple&& t) {
    auto checker = hana::is_valid([&](auto&&... args) -> decltype(f(args...)) {});

    // Unpack the tuple and apply to the checker
    return hana::unpack(std::forward<Tuple>(t), checker);
}

#define MOCK_UTIL_WRAP_VAL(x) []() -> decltype(x) { return x; }

// ---------------------------------------------
// Generic mock fn
// ---------------------------------------------

// NOTE: We want to use lazy eval when choosing whether to call return_fn or
// invoke_fn. This means we cannot make skeletal constexpr since each branch
// of hana::eval_if will be evaluated leading to compile-time errors. The
// compile-time errors occur because invoke_fn key may not be present in the
// map. We only want to get the key-value when the key is present, hence
// need lazy eval.
//
// NOTE: If the fact that skeletal is non constexpr is adding runtime cost
// then we need to think of a more complicated approach that does not
// require laziness in eval_if
static auto skeletal = [](auto &&expectations_map, auto &&args_tuple) {
  // auto cardinality_fn = hana::at_key(expectations_map, TIMES_FN);
  auto fnName = hana::at_key(expectations_map, CALL_FN_NAME);
  static_assert(fnName != -1_c);
  // NOTE: record call in Sequence
  // seamock::util::SeqArray[g_sequence_counter] = fnName.c_str();
  // seamock::util::SetTupleAtIdx(seamock::util::SeqTuple,
  // g_sequence_counter,
  //                              fnName);
  // NOTE: update global sequence counter
  g_sequence_counter++;
  auto invoke_fn_optional = hana::find(expectations_map, INVOKE_FN);
  return hana::eval_if(
      hana::is_nothing(invoke_fn_optional),
      [&] {
        // NOTE: check that after constraint is maintained
        // auto pred_tup = hana::at_key(expectations_map, AFTER);
        // auto pred_found_tup = hana::transform(pred_tup, [&](auto elem) {
        //   // TODO: search till g_sequence_counter - 1 instead of std::end
        //   auto it = std::find(std::begin(seamock::util::SeqArray),
        //                       std::end(seamock::util::SeqArray), elem.c_str());
        //   return it != std::end(seamock::util::SeqArray) &&
        //          std::distance(std::begin(seamock::util::SeqArray), it) <
        //              g_sequence_counter - 1;
        // });
        // if (!hana::fold(pred_found_tup, true, [&](auto acc, bool element) {
        //       return acc && element;
        //     })) {
        //   sea_printf("Predecessor (After) match failed!\n");
        //   sassert(0);
        // };
        auto ret_fn = hana::at_key(expectations_map, RETURN_FN);
        auto capture_map = hana::at_key(expectations_map, CAPTURE_ARGS_MAPS);
        // NOTE: INVARIANT: return fn should be callable
        BOOST_HANA_CONSTANT_ASSERT_MSG(hana::is_valid(ret_fn)(),
                                       "Return function is not callable!");
        // NOTE: (arg0, arg1, ..._N) -> (0, 1, ..._N)
        auto args_range =
            hana::make_range(hana::size_c<0>, hana::size(args_tuple));
        // BOOST_HANA_CONSTANT_ASSERT(hana::size(args_tuple) ==
        //                            hana::size_c<2>);
        // BOOST_HANA_CONSTANT_ASSERT(hana::size(args_range) ==
        //                            hana::size_c<2>);
        // NOTE: (0, 1, ..._N), (arg0, arg1, ..._N) --> ((0, arg0), (1,
        // arg1),
        // ..._N)
        auto indexed_args_pairs =
            hana::zip(hana::to_tuple(args_range), args_tuple);
        // NOTE: e.g., ((1, P1), (3, P3)) --> (1, 3)
        auto capture_params_indices = hana::keys(capture_map);
        // NOTE: If assertions fails, a capture map parameter index is out
        // of bounds!
        hana::for_each(capture_params_indices, [&](auto elem) {
          BOOST_HANA_CONSTANT_ASSERT(elem < (hana::size(args_tuple)));
        });
        // NOTE:  _ --> ((1, arg1), (3, arg3))
        auto filtered_args = hana::filter(indexed_args_pairs, [&](auto pair) {
          auto idx = hana::at(pair, hana::size_c<0>);
          return hana::contains(capture_params_indices, idx);
        });
        // BOOST_HANA_CONSTANT_ASSERT(hana::size(filtered_args) ==
        //                            hana::size_c<1>);
        // NOTE: ((1, arg1), (3, arg3)) --> (arg1, arg3)
        auto args = hana::transform(filtered_args, [&](auto pair) {
          return hana::at(pair, hana::size_c<1>);
        });
        // NOTE: ((1, P1), (3, P3)) --> (P1, P3)
        auto capture_params_values = hana::values(capture_map);
        // NOTE: (P1, P3), (arg1, arg3)  --> ((P1, arg1), (P3, arg3))
        auto param_arg_pairs = hana::zip(capture_params_values, args);
        // BOOST_HANA_CONSTANT_ASSERT(hana::size(param_arg_pairs) ==
        //                            hana::size_c<1>);
        // NOTE: call functions P1(arg1), P3(arg3) for side-effects
        hana::for_each(param_arg_pairs, [&](auto pair) {
          auto param = hana::at(pair, hana::size_c<0>);
          auto arg = hana::at(pair, hana::size_c<1>);

          hana::apply(param, arg);
        });
        return ret_fn();
      },
      [&](auto _) {
        auto invoke_fn = _(invoke_fn_optional).value();
        BOOST_HANA_CONSTANT_ASSERT_MSG(is_callable_with_args(invoke_fn, args_tuple),
                                       "Unexpected invoke function signature!");
        return hana::unpack(args_tuple, invoke_fn);
      });
};

namespace seamock {

template <typename MapType=decltype(DefaultExpectationsMap)>
class ExpectationBuilder {
private:
    MapType expectationsMap;

public:
    // Constructor to initialize with an existing map
    constexpr ExpectationBuilder(const MapType& map) : expectationsMap(map) {}

    // Default constructor
    constexpr ExpectationBuilder() : expectationsMap(DefaultExpectationsMap) {}

    // ... other methods ...
    // Method to add/update ComponentA
    template<typename InvokeType>
    constexpr auto invokeFn(InvokeType i) const {
      auto updatedMap = InvokeFn(i, expectationsMap);
      return ExpectationBuilder<decltype(updatedMap)>(updatedMap);
    }

    template<typename TimesFnType>
    constexpr auto times(TimesFnType f) const {
      // constexpr auto cardConst = hana::int_c<Cardinality>;
      // constexpr auto fn = f(cardConst);
      auto updatedMap = Times(f, expectationsMap);
      return ExpectationBuilder<decltype(updatedMap)>(updatedMap);
    }

    template<typename ReturnFnType>
    constexpr auto returnFn(ReturnFnType i) const {
      auto updatedMap = ReturnFn(i, expectationsMap);
      return ExpectationBuilder<decltype(updatedMap)>(updatedMap);
    }

    template<size_t ArgNum, typename CaptureFnType>
    constexpr auto captureArgAndInvoke(CaptureFnType fn) const {
      constexpr auto argNumber = hana::size_c<ArgNum>;
      constexpr auto capture_map =
          hana::make_map(hana::make_pair(argNumber, fn));
      auto updatedMap = Capture(capture_map, expectationsMap);
      return ExpectationBuilder<decltype(updatedMap)>(updatedMap);
    }

    // Finalize build
    constexpr auto build() const {
        return expectationsMap;
    }
};

} // namespace seamock
#endif // SEAMOCK_H_
