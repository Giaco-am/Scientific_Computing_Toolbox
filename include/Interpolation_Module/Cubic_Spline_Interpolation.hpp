#ifndef CUBIB_SPLINE_INTERPOLATION_HPP
#define CUBIB_SPLINE_INTERPOLATION_HPP

#include "Interpolation.hpp"
#include "Cubic_Spline_Interpolation.hpp"
#include "utilities_interpolation.hpp"
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
#include <vector>
#include <iostream>
#include <stdexcept>

/** @namespace ScientificToolbox::Interpolation
 * @brief Interpolation module providing basic interpolation functions
 * 
 * This namespace contains template classes for common interpolation methods:
 * - Linear interpolation
 * - Polynomial interpolation
 * - Spline interpolation
 */

namespace ScientificToolbox::Interpolation {


    /** @class Spline Interpolation class
     * @brief Spline interpolation class
     * 
     * This class provides a spline interpolation method using the GNU Scientific Library (GSL).
     * 
     * @tparam T Data type
     * 
     */
    template <typename T>
    class SplineInterpolation : public Interpolation<T> {
    public:
        /** @Constructor
         * @brief Constructor for the SplineInterpolation class
         * 
         * This constructor initializes the spline interpolation method using the data provided.
         * 
         * @param data Set of points to interpolate
         */
        explicit SplineInterpolation(const std::set<point<T>>& data) : Interpolation<T>(data) {
            // Create the spline
            acc = gsl_interp_accel_alloc();
            spline = gsl_spline_alloc(gsl_interp_cspline, this->x_.size());
        }

        /** @Destructor
         * @brief Destructor for the SplineInterpolation class
         * 
         * This destructor frees the memory allocated for the spline interpolation method.
         * 
         * @param data Set of points to interpolate
         * 
         */
        ~SplineInterpolation() {
            gsl_spline_free(spline);
            gsl_interp_accel_free(acc);
        }

        /** @method Interpolation function
         * @brief Interpolation function for the SplineInterpolation class
         * 
         * This function interpolates a value at a given point using the spline interpolation method.
         * 
         * @param x_query Point to interpolate
         * @return Interpolated value at the given point
         */
        T interpolate(T x_query) const override {
            gsl_spline_init(spline, this->x_.data(), this->y_.data(), this->x_.size());
            return gsl_spline_eval(spline, x_query, acc);
        }

    protected:
        gsl_interp_accel *acc;
        gsl_spline *spline;
    };
}

#endif