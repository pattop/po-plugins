#include "biquad.h"
#include "descriptor.h"
#include "ladspa_ids.h"
#include <array>

namespace {

constexpr auto control = 3;
constexpr auto channels = 8;

struct filter {
	LADSPA_Data f0 = 0, gain = 0, Q = 0;
	std::array<std::array<LADSPA_Data *, 2>, channels> io = {};
	unsigned long fs = 0;
	biquad_coefficients bqc;
	std::array<biquad, channels> bq;
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
		p->gain = *d;
		return;
	case 2:
		p->Q = *d;
		return;
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
	p->bqc.peaking_eq(p->f0, p->gain, p->Q, p->fs);
}

void
run(LADSPA_Handle h, unsigned long samples)
{
	filter *p = reinterpret_cast<filter *>(h);
	for (auto i = 0; i < channels; ++i) {
		/* stop on first unconnected port */
		if (!p->io[i][0] || !p->io[i][1])
			return;
		p->bq[i].run(p->bqc, p->io[i][0], p->io[i][1], samples);
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
	"Centre Frequency (Hz)",
	"Gain (dB)",
	"Bandwidth (Q)",
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
				  LADSPA_HINT_DEFAULT_0,
		.LowerBound = -100,
		.UpperBound = 100,
	}, {
		.HintDescriptor = LADSPA_HINT_BOUNDED_BELOW |
				  LADSPA_HINT_BOUNDED_ABOVE |
				  LADSPA_HINT_DEFAULT_1,
		.LowerBound = 0,
		.UpperBound = 100,
	},
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
			d.UniqueID = ladspa_ids::peaking + i;
			d.PortCount = control + (i + 1) * 2;
			register_plugin({d,
			    "peaking_" + std::to_string(i + 1) + "ch",
			    "Peaking (" + std::to_string(i + 1) + " Channel)"});
		}
	}
} init;

} /* namespace */
