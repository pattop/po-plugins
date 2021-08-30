#include "biquad.h"
#include "descriptor.h"
#include "ladspa_ids.h"
#include <array>
#include <cmath>

namespace {

constexpr auto control = 1;
constexpr auto channels = 8;

struct filter {
	LADSPA_Data gain;
	std::array<std::array<LADSPA_Data *, 2>, channels> io = {};
};

LADSPA_Handle
instantiate(const LADSPA_Descriptor *d, unsigned long fs)
{
	return new filter;
}

void
connect_port(LADSPA_Handle h, unsigned long port, LADSPA_Data *d)
{
	filter *p = reinterpret_cast<filter *>(h);

	switch (port) {
	case 0:
		/* db->magnitude */
		p->gain = std::pow(10.0, *d / 20.0);
		return;
	}
	port -= control;
	if (port >= 2 * channels)
		return;
	p->io[port / 2][port % 2] = d;
}

void
run(LADSPA_Handle h, unsigned long samples)
{
	filter *p = reinterpret_cast<filter *>(h);
	for (auto i = 0; i < channels; ++i) {
		auto in = p->io[i][0];
		auto out = p->io[i][1];
		/* stop on first unconnected port */
		if (!in || !out)
			return;
		for (unsigned long j = 0; j < samples; ++j)
			out[j] = in[j] * p->gain;
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
	"Gain (dB)",
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
				  LADSPA_HINT_DEFAULT_0,
		.LowerBound = -100,
		.UpperBound = 100,
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
			.activate = nullptr,
			.run = run,
			.run_adding = nullptr,
			.set_run_adding_gain = nullptr,
			.deactivate = nullptr,
			.cleanup = cleanup,
		};

		for (int i = 0; i < channels; ++i) {
			d.UniqueID = ladspa_ids::gain + i;
			d.PortCount = control + (i + 1) * 2;
			register_plugin({d,
			    "gain_" + std::to_string(i + 1) + "ch",
			    "Gain (" + std::to_string(i + 1) + " Channel)"});
		}
	}
} init;

} /* namespace */
