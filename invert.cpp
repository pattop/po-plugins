#include "biquad.h"
#include "descriptor.h"
#include "ladspa_ids.h"
#include <array>
#include <cmath>

namespace {

constexpr auto channels = 8;

struct filter {
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
			out[j] = -in[j];
	}
}

void
cleanup(LADSPA_Handle h)
{
	filter *p = reinterpret_cast<filter *>(h);
	delete p;
}

constexpr std::array<LADSPA_PortDescriptor, 2 * channels> ports = {
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
constexpr std::array<LADSPA_PortRangeHint, size(ports)> port_hints = {};

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
			d.UniqueID = ladspa_ids::invert + i;
			d.PortCount = (i + 1) * 2;
			register_plugin({d,
			    "invert_" + std::to_string(i + 1) + "ch",
			    "Invert (" + std::to_string(i + 1) + " Channel)"});
		}
	}
} init;

} /* namespace */
