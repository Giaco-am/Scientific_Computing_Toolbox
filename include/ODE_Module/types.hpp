#ifndef ODE_TYPES_HPP
#define ODE_TYPES_HPP

#include <functional>
#include <variant>
#include <Eigen/Dense>
#include <vector>
#include <string>

namespace ScientificToolbox::ODE {

using scalar_func = std::function<double(double, double)>;
using vec_func = std::function<Eigen::VectorXd(double, const Eigen::VectorXd&)>;
using var_func = std::variant<scalar_func, vec_func>;
using var_vec = std::variant<double, Eigen::VectorXd>;

using vec_d = Eigen::VectorXd;
using vec_s = std::vector<std::string>;
using var_vecs = std::vector<var_vec>;
using var_expr = std::variant<std::string, vec_s>;

struct ScalarODETestCase {
    std::string expr;
    double t0;
    double tf;
    double h;
    double y0;
    double expected_final;
    double expected_derivative;
};

struct VectorODETestCase {
    vec_s exprs;
    double t0;
    double tf;
    double h;
    vec_d y0;
    vec_d expected_final;
    vec_d expected_derivative;
};

struct Func {
    var_func func;
    var_vec operator()(double t, const var_vec& y) const {
        return std::visit([&](auto&& f) -> var_vec {
            using T = std::decay_t<decltype(f)>;
            if constexpr (std::is_same_v<T, scalar_func>) {
                double y_double = std::get<double>(y);
                return var_vec(f(t, y_double));
            } else {
                vec_d y_vec = std::get<vec_d>(y);
                return var_vec(f(t, y_vec));
            }
        }, func);
    }
};

using test_case = std::variant<ScalarODETestCase, VectorODETestCase>;

} // namespace ScientificToolbox::ODE

#endif // ODE_TYPES_HPP