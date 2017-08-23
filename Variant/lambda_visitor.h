#pragma once
#include <boost/variant.hpp>

namespace variant_utils {

// Based on
// https://stackoverflow.com/questions/7867555/best-way-to-do-variant-visitation-with-lambdas
// https://stackoverflow.com/questions/7870498/using-declaration-in-variadic-template
// https://habrahabr.ru/post/270689/

namespace detail {

template <typename... Args>
struct Visitor : Args... {
  Visitor(Args&&... args)
    : Args{std::forward<Args>(args)}... {}
};

} // namespace detail

template <typename... Lambdas>
auto make_lambda_visitor(Lambdas&&... lambdas) {
  return detail::Visitor<Lambdas...>{std::forward<Lambdas>(lambdas)...};
}

template <typename... VarTypes, typename... Lambdas>
auto visit(const boost::variant<VarTypes...>& v, Lambdas&&... lambdas) {
  return boost::apply_visitor(make_lambda_visitor(std::forward<Lambdas>(lambdas)...), v);
}

} // namespace variant_utils