#include "biquad.h"
#include "descriptor.h"
#include "ladspa_ids.h"
#include <array>
#include <cmath>

namespace {

constexpr auto control = 2;
constexpr auto channels = 8;

struct filter {
	LADSPA_Data f0 = 0;
	unsigned order = 0;
	std::array<std::array<LADSPA_Data *, 2>, channels> io = {};
	unsigned long fs = 0;
	biquad_coefficients bqc1;
	biquad_coefficients bqc2;
	std::array<std::array<biquad, 2>, channels> bq;
};

LADSPA_Handle
instantiate(const LADSPA_Descriptor *d, unsigned long fs)
{
	auto p = new filter;
	p->fs = fs;
	return p;
}

void
connect_port(LADSPA_Handle h, unsigned long port, LADSPA_Data *d)
{
	filter *p = reinterpret_cast<filter *>(h);

	switch (port) {
	case 0:
		p->f0 = *d;
		return;
	case 1:
		p->order = *d;
		if (p->order > 4) {
			fprintf(stderr, "WARNING: Maximum supported Butterworth filter order is 4. Clamping.\n");
			p->order = 4;
		}
		if (p->order < 1) {
			fprintf(stderr, "WARNING: Butterworth filter minimum order is 1. Clamping.\n");
			p->order = 1;
		}
	}
	port -= control;
	if (port >= 2 * channels)
		return;
	p->io[port / 2][port % 2] = d;
}

void
activate(LADSPA_Handle h)
{
	filter *p = reinterpret_cast<filter *>(h);

	/* See https://www.earlevel.com/main/2016/09/29/cascading-filters */
	switch (p->order) {
	case 1:
		p->bqc1.lpf1(p->f0, p->fs);
		break;
	case 2:
		p->bqc1.lpf(p->f0, 1.0 / (2.0 * std::cos(std::numbers::pi / 4.0)), p->fs);
		break;
	case 3:
		p->bqc1.lpf1(p->f0, p->fs);
		p->bqc2.lpf(p->f0, 1.0 / (2.0 * std::cos(std::numbers::pi / 3.0)), p->fs);
		break;
	case 4:
		p->bqc1.lpf(p->f0, 1.0 / (2.0 * std::cos(std::numbers::pi / 8.0)), p->fs);
		p->bqc2.lpf(p->f0, 1.0 / (2.0 * std::cos(3.0 * std::numbers::pi / 8.0)), p->fs);
		break;
	}
}

void
run(LADSPA_Handle h, unsigned long samples)
{
	filter *p = reinterpret_cast<filter *>(h);
	for (auto i = 0; i < channels; ++i) {
		/* stop on first unconnected port */
		if (!p->io[i][0] || !p->io[i][1])
			return;

		/* first & second order */
		p->bq[i][0].run(p->bqc1, p->io[i][0], p->io[i][1], samples);

		/* third & fourth order */
		if (p->order > 2)
			p->bq[i][1].run(p->bqc2, p->io[i][1], p->io[i][1], samples);
	}
}

void
cleanup(LADSPA_Handle h)
{
	filter *p = reinterpret_cast<filter *>(h);
	delete p;
}

constexpr std::array<LADSPA_PortDescriptor, control + 2 * channels> ports = {
	LADSPA_PORT_CONTROL | LADSPA_PORT_INPUT,
	LADSPA_PORT_CONTROL | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
};
constexpr std::array<const char *, size(ports)> port_names = {
	"Cutoff Frequency (Hz)",
	"Order (1, 2, 3 or 4)",
	"Channel 1 Input",
	"Channel 1 Output",
	"Channel 2 Input",
	"Channel 2 Output",
	"Channel 3 Input",
	"Channel 3 Output",
	"Channel 4 Input",
	"Channel 4 Output",
	"Channel 5 Input",
	"Channel 5 Output",
	"Channel 6 Input",
	"Channel 6 Output",
	"Channel 7 Input",
	"Channel 7 Output",
	"Channel 8 Input",
	"Channel 8 Output",
};
constexpr std::array<LADSPA_PortRangeHint, size(ports)> port_hints = { { {
		.HintDescriptor = LADSPA_HINT_BOUNDED_BELOW |
				  LADSPA_HINT_BOUNDED_ABOVE |
				  LADSPA_HINT_SAMPLE_RATE |
				  LADSPA_HINT_DEFAULT_MIDDLE,
		.LowerBound = 0,
		.UpperBound = 0.45,
	}, {
		.HintDescriptor = LADSPA_HINT_BOUNDED_BELOW |
				  LADSPA_HINT_BOUNDED_ABOVE |
				  LADSPA_HINT_DEFAULT_1,
		.LowerBound = 1,
		.UpperBound = 4,
	}
} };

/*
 * register plugin instances
 */
struct init {
	init()
	{
		LADSPA_Descriptor d = {
			.UniqueID = 0,
			.Label = nullptr,
			.Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE,
			.Name = nullptr,
			.Maker = "Patrick Oppenlander <patrick.oppenlander@gmail.com>",
			.Copyright = "Patrick Oppenlander, 2021",
			.PortCount = 0,
			.PortDescriptors = data(ports),
			.PortNames = data(port_names),
			.PortRangeHints = data(port_hints),
			.ImplementationData = nullptr,
			.instantiate = instantiate,
			.connect_port = connect_port,
			.activate = activate,
			.run = run,
			.run_adding = nullptr,
			.set_run_adding_gain = nullptr,
			.deactivate = nullptr,
			.cleanup = cleanup,
		};

		for (int i = 0; i < channels; ++i) {
			d.UniqueID = ladspa_ids::butterworth_lowpass + i;
			d.PortCount = control + (i + 1) * 2;
			register_plugin({d,
			    "butterworth_lowpass_" + std::to_string(i + 1) + "ch",
			    "Butterworth Lowpass (" + std::to_string(i + 1) + " Channel)"});
		}
	}
} init;

} /* namespace */
