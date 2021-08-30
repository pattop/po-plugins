#pragma once

#include <cstddef>

class biquad_coefficients;

/*
 * biquad - simple biquad filter
 *
 * For details on how this works see the Audio EQ Cookbook.
 */
class biquad {
public:
	void run(const biquad_coefficients &,
		 const float *input, float *output, size_t samples);

private:
	double x1 = 0, x2 = 0, y1 = 0, y2 = 0;
};

class biquad_coefficients {
public:
	void peaking_eq(double f0, double gain, double Q, double fs);
	void lpf1(double f0, double fs);
	void lpf(double f0, double Q, double fs);
	void hpf1(double f0, double fs);
	void hpf(double f0, double Q, double fs);
	void low_shelf(double f0, double gain, double Q, double fs);
	void high_shelf(double f0, double gain, double Q, double fs);

private:
	double b0 = 0, b1 = 0, b2 = 0, a1 = 0, a2 = 0;

	friend class biquad;
};

