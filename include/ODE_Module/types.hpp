#ifndef ODE_TYPES_HPP
#define ODE_TYPES_HPP

/**
 * @file types.hpp
 * @brief Core type definitions and data structures for ODE solving
 * 
 * This module defines the fundamental types and data structures used throughout
 * the ODE solving framework, including:
 * - Function types for both scalar and vector ODEs
 * - Solution structures
 * - Test case definitions
 * - Mathematical operation overloads
 */

#include <functional>
#include <variant>
#include <Eigen/Dense>
#include <optional>
#include <vector>
#include <string>

/**
 * @namespace ScientificToolbox::ODE
 * @brief Namespace containing utilities for Ordinary Differential Equations (ODE) handling
*/
namespace ScientificToolbox::ODE {

// Type aliases for various function and vector types used in ODE solving
using vec_d = Eigen::VectorXd;
using vec_s = std::vector<std::string>;
using var_vec = std::variant<double, vec_d>;
using var_vecs = std::vector<var_vec>;
using var_expr = std::variant<std::string, vec_s>;
using scalar_func = std::function<double(double, double)>;
using vec_func = std::function<vec_d(double, const vec_d&)>;
using var_func = std::variant<scalar_func, vec_func>;

/**
 * @struct ODESolution
 * @brief Stores the solution of an ODE system
 * @param size The dimension of the system (= y_values.size())
 * @param t_values Time points vector
 * @param y_values Solution values at each time point
 * @param steps Number of steps to print (only for debugging)
 */
struct ODESolution {
    var_expr expr;
    int size;
    vec_d t_values;
    var_vecs y_values;
    int steps_to_print = 10;

    var_vecs get_solution() const { return y_values; }
    var_vec get_result() const { return y_values.back(); }
    vec_d get_times() const { return t_values; }
    var_expr get_expr() const { return expr; }
    int get_size() const {return size; }
    var_vec get_initial_conditions() const { return y_values.front(); }
    double get_final_time() const { return t_values(t_values.size() - 1); }
    double get_step_size() const { return (t_values(t_values.size() - 1) - t_values(0)) / static_cast<double>(t_values.size()); }
};

/**
 * @struct ODETestCase
 * @brief Defines a test case for ODE solvers
 * @param expr Mathematical expression(s) defining the ODE
 * @param y0 Initial condition
 * @param t0 Initial time
 * @param tf Final time
 * @param h Step size
 * @param expected_solution Expected solution at final time
 * @param expected_derivative Expected derivative value
 */
struct ODETestCase {
    ODETestCase() = default; // Add this line
    ODETestCase(var_expr expr, var_vec y0, double t0, double tf, double h, std::optional<var_vec> expected_solution = std::nullopt, std::optional<var_vec> expected_derivative = std::nullopt)
        : expr(expr), y0(y0), t0(t0), tf(tf), h(h), expected_solution(expected_solution), expected_derivative(expected_derivative) {}
    
    var_expr expr;
    var_vec y0;
    double t0;
    double tf;
    double h;
    std::optional<var_vec> expected_solution;
    std::optional<var_vec> expected_derivative;

        bool has_expected_solution() const { return expected_solution.has_value(); }
    std::optional<var_vec> get_expected_solution() const { return expected_solution; }
    var_expr get_expr() const { return expr; }
};

/**
 * @struct Func
 * @brief Wrapper for ODE function handling both scalar and vector cases
 * 
 * This struct provides a unified interface for handling both scalar and vector ODE functions overriding the function call operator.
 */
struct Func {

    var_func func;
    var_expr expr;

    //Func(const var_func& f) : func(std::move(f)) {}
    Func(const var_func& f, var_expr expr) : func(std::move(f)), expr(std::move(expr)) {}

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

// Overload for printing custom types to output streams
std::ostream& operator<<(std::ostream& os, const var_vec& vec);
std::ostream& operator<<(std::ostream& os, const var_expr& expr);
std::ostream& operator<<(std::ostream& os, const ODESolution& solution);
std::ostream& operator<<(std::ostream& os, ODETestCase test);

// Overload for arithmetic operations
var_vec operator*(double h, const var_vec& v);
var_vec operator+(const var_vec& v1, const var_vec& v2);
var_vec operator-(const var_vec& v1, const var_vec& v2);
var_vec operator/(const var_vec& v1, const double v2);
var_vec operator/(const var_vec& v1, const var_vec& v2);

// Solver types
inline const vec_s solver_types = {"ForwardEulerSolver", "RK4Solver", "ExplicitMidpointSolver"};

inline const vec_s get_solver_types() { return solver_types; }

/** ### print_variant
 * @brief Print a variant type to an output stream
 * @tparam Variant Type of the variant
 * @param os Output stream
 * @param var Variant type to print
 * @return std::ostream& 
 */
template <typename Variant>
std::ostream& print_variant(std::ostream& os, const Variant& var) {
    std::visit([&os](const auto& value) {
        using ValueType = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<ValueType, Eigen::VectorXd>) {
            os << "[";
            for (int i = 0; i < value.size(); ++i) {
                os << value(i);
                if (i < value.size() - 1) {
                    os << ", ";
                }
            }
            os << "]";
        } else if constexpr (std::is_same_v<ValueType, std::vector<std::string>>) {
            if (value.empty()) {
                os << "<empty vector>";
            } else {
                os << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    os << value[i];
                    if (i < value.size() - 1) {
                        os << ", ";
                    }
                }
                os << "]";
            }
        } else {
            os << value;
        }
    }, var);
    return os;
}

/** ### apply_binary_operation
 * @brief Apply a binary operation to two variant types
 * @tparam Func Type of the binary operation function
 * @param v1 First variant type
 * @param v2 Second variant type
 * @param op Binary operation function
 * @return var_vec Result of the binary operation
 */
template <typename Func>
var_vec apply_binary_operation(const var_vec& v1, const var_vec& v2, Func op) {
    if (std::holds_alternative<double>(v1)) {
        return op(std::get<double>(v1), std::get<double>(v2));
    } else {
        return op(std::get<Eigen::VectorXd>(v1), std::get<Eigen::VectorXd>(v2));
    }
}

/** ### apply_unary_operation
 * @brief Apply a unary operation to a variant type
 * @tparam Func Type of the unary operation function
 * @param v Variant type
 * @param scalar Scalar value
 * @param op Unary operation function
 * @return var_vec Result of the unary operation
 */
template <typename Func>
var_vec apply_unary_operation(const var_vec& v, double scalar, Func op) {
    if (std::holds_alternative<double>(v)) {
        return op(std::get<double>(v), scalar);
    } else {
        return op(std::get<Eigen::VectorXd>(v), scalar);
    }
}

} // namespace ScientificToolbox::ODE

#endif // ODE_TYPES_HPP