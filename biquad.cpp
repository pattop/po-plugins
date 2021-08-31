#include "biquad.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <numbers>

#define dbg(...)

/*
 * biquad::run - run biquad filter across sample data
 */
void
biquad::run(const biquad_coefficients &c,
	    const float *input, float *output, size_t samples)
{
	/*
	 * Eq 4:    y[n] = (b0/a0)*x[n] + (b1/a0)*x[n-1] + (b2/a0)*x[n-2]
	 *			        - (a1/a0)*y[n-1] - (a2/a0)*y[n-2]
	 *
	 * We normalise the coefficients so don't need to divide by a0.
	 *
	 * Careful, input and output arrays can point to the same place!
	 */
	for (size_t i = 0; i < samples; ++i) {
		auto x0 = input[i];
		auto y0 = c.b0 * x0 + c.b1 * x1 + c.b2 * x2 -
				      c.a1 * y1 - c.a2 * y2;
		x2 = x1;
		x1 = x0;
		y2 = y1;
		y1 = y0;
		output[i] = y0;
	}
}

/*
 * biquad_coefficients::init_peaking_eq
 *
 * See Audio EQ Cookbook peakingEQ.
 */
void
biquad_coefficients::peaking_eq(const double f0, double gain, double Q,
				double fs)
{
	using std::numbers::pi;
	auto w0 = 2.0 * pi * std::clamp(f0, 1.0, fs * 0.49) / fs;
	auto A = std::pow(10.0, gain / 40.0);
	auto alpha = std::sin(w0) / (2.0 * Q);
	auto a0 = 1.0 + alpha / A;
	b0 = (1.0 + alpha * A) / a0;
	b1 = (-2.0 * std::cos(w0)) / a0;
	b2 = (1.0 - alpha * A) / a0;
	a1 = (-2.0 * std::cos(w0)) / a0;
	a2 = (1.0 - alpha / A) / a0;

	dbg("peaking_eq:\n  b0=%.20f\n  b1=%.20f\n  b2=%.20f\n  a1=%.20f\n  a2=%.20f\n",
	    b0, b1, b2, a1, a2);
}

/*
 * biquad_coefficients::lpf1
 *
 * First order lowpass.
 */
void
biquad_coefficients::lpf1(double f0, double fs)
{
	using std::numbers::pi;
	auto x = std::tan(pi * f0 / fs);
	auto a0 = x + 1.0;
	b0 = x / a0;
	b1 = b0;
	b2 = 0;
	a1 = (x - 1.0) / a0;
	a2 = 0;

	dbg("lpf1:\n  b0=%.20f\n  b1=%.20f\n  b2=%.20f\n  a1=%.20f\n  a2=%.20f\n",
	    b0, b1, b2, a1, a2);
}

/*
 * biquad_coefficients::lpf
 *
 * See Audio EQ Cookbook LPF.
 */
void
biquad_coefficients::lpf(double f0, double Q, double fs)
{
	using std::numbers::pi;
	auto w0 = 2.0 * pi * std::clamp(f0, 1.0, fs * 0.49) / fs;
	auto alpha = std::sin(w0) / (2.0 * Q);
	auto a0 = 1.0 + alpha;
	b0 = (1.0 - std::cos(w0)) / 2 / a0;
	b1 = (1.0 - std::cos(w0)) / a0;
	b2 = (1.0 - std::cos(w0)) / 2 / a0;
	a1 = (-2.0 * std::cos(w0)) / a0;
	a2 = (1.0 - alpha) / a0;

	dbg("lpf:\n  b0=%.20f\n  b1=%.20f\n  b2=%.20f\n  a1=%.20f\n  a2=%.20f\n",
	    b0, b1, b2, a1, a2);
}

/*
 * biquad_coefficients::hpf1
 *
 * First order highpass.
 */
void
biquad_coefficients::hpf1(double f0, double fs)
{
	using std::numbers::pi;
	auto x = std::tan(pi * f0 / fs);
	auto a0 = x + 1.0;
	b0 = 1.0 / a0;
	b1 = -b0;
	b2 = 0;
	a1 = (x - 1.0) / a0;
	a2 = 0;

	dbg("hpf1:\n  b0=%.20f\n  b1=%.20f\n  b2=%.20f\n  a1=%.20f\n  a2=%.20f\n",
	    b0, b1, b2, a1, a2);
}

/*
 * biquad_coefficients::hpf
 *
 * See Audio EQ Cookbook HPF.
 */
void
biquad_coefficients::hpf(double f0, double Q, double fs)
{
	using std::numbers::pi;
	auto w0 = 2.0 * pi * std::clamp(f0, 1.0, fs * 0.49) / fs;
	auto alpha = std::sin(w0) / (2.0 * Q);
	auto a0 = 1.0 + alpha;
	b0 = (1.0 + std::cos(w0)) / 2 / a0;
	b1 = -(1.0 + std::cos(w0)) / a0;
	b2 = (1.0 + std::cos(w0)) / 2 / a0;
	a1 = (-2.0 * std::cos(w0)) / a0;
	a2 = (1.0 - alpha) / a0;

	dbg("hpf:\n  b0=%.20f\n  b1=%.20f\n  b2=%.20f\n  a1=%.20f\n  a2=%.20f\n",
	    b0, b1, b2, a1, a2);
}

/*
 * biquad_coefficients::low_shelf
 *
 * See Audio EQ Cookbool lowShelf.
 */
void
biquad_coefficients::low_shelf(double f0, double gain, double Q, double fs)
{
	using std::numbers::pi;
	auto w0 = 2.0 * pi * std::clamp(f0, 1.0, fs * 0.49) / fs;
	auto A = std::pow(10.0, gain / 40.0);
	auto alpha = std::sin(w0) / (2.0 * Q);
	auto a0 = A + 1.0 + (A - 1.0) * std::cos(w0) + 2.0 * std::sqrt(A) * alpha;
	b0 = A * (A + 1.0 - (A - 1.0) * std::cos(w0) + 2.0 * std::sqrt(A) * alpha) / a0;
	b1 = 2.0 * A * (A - 1.0 - (A + 1.0) * std::cos(w0)) / a0;
	b2 = A * (A + 1.0 - (A - 1.0) * std::cos(w0) - 2.0 * std::sqrt(A) * alpha) / a0;
	a1 = -2.0 * (A - 1.0 + (A + 1.0) * std::cos(w0)) / a0;
	a2 = (A + 1.0 + (A - 1.0) * std::cos(w0) - 2.0 * std::sqrt(A) * alpha) / a0;

	dbg("low_shelf:\n  b0=%.20f\n  b1=%.20f\n  b2=%.20f\n  a1=%.20f\n  a2=%.20f\n",
	    b0, b1, b2, a1, a2);
}

/*
 * biquad_coefficients::high_shelf
 *
 * See Audio EQ Cookbool highShelf.
 */
void
biquad_coefficients::high_shelf(double f0, double gain, double Q, double fs)
{
	using std::numbers::pi;
	auto w0 = 2.0 * pi * std::clamp(f0, 1.0, fs * 0.49) / fs;
	auto A = std::pow(10.0, gain / 40.0);
	auto alpha = std::sin(w0) / (2.0 * Q);
	auto a0 = A + 1.0 - (A - 1.0) * std::cos(w0) + 2.0 * std::sqrt(A) * alpha;
	b0 = A * (A + 1.0 + (A - 1.0) * std::cos(w0) + 2.0 * std::sqrt(A) * alpha) / a0;
	b1 = -2.0 * A * (A - 1.0 + (A + 1.0) * std::cos(w0)) / a0;
	b2 = A * (A + 1.0 + (A - 1.0) * std::cos(w0) - 2.0 * std::sqrt(A) * alpha) / a0;
	a1 = 2.0 * (A - 1.0 - (A + 1.0) * std::cos(w0)) / a0;
	a2 = (A + 1.0 - (A - 1.0) * std::cos(w0) - 2.0 * std::sqrt(A) * alpha) / a0;

	dbg("high_shelf:\n  b0=%.20f\n  b1=%.20f\n  b2=%.20f\n  a1=%.20f\n  a2=%.20f\n",
	    b0, b1, b2, a1, a2);
}
