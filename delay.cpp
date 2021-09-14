#include "biquad.h"
#include "descriptor.h"
#include "ladspa_ids.h"
#include <array>
#include <cmath>

namespace {

constexpr auto control = 1;
constexpr auto channels = 8;
constexpr auto max_delay = 1024;	/* in samples, must be power-of-two */

struct filter {
	unsigned long delay = 0;	/* in samples */
	unsigned long pos = 0;
	std::array<std::array<LADSPA_Data *, 2>, channels> io = {};
	std::array<std::array<LADSPA_Data, max_delay>, channels> data = {};
	unsigned long fs = 0;
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
		p->delay = std::round(*d / 1000.0 * p->fs);
		if (p->delay == 0) {
			fprintf(stderr, "WARNING: Minimum delay is %.2fms at %luHz. Clamping.\n",
				1000.0 / p->fs, p->fs);
			p->delay = 1;
		}
		if (p->delay >= max_delay) {
			fprintf(stderr, "WARNING: Maximum delay is %.2fms at %luHz. Clamping.\n",
			        (max_delay - 1.0) / p->fs, p->fs);
			p->delay = max_delay - 1;
		}
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
			break;
		/* careful, input and output can overlap */
		/* REVISIT: this could probably be a bit more optimal.. */
		auto &data = p->data[i];
		for (unsigned long j = 0; j < samples; ++j) {
			data[(p->pos + j) % max_delay] = in[j];
			out[j] = data[(p->pos + j - p->delay) % max_delay];
		}
	}
	p->pos += samples;
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
	"Delay (ms)",
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
				  LADSPA_HINT_DEFAULT_1,
		.LowerBound = 0,
		.UpperBound = 1.0 / max_delay,
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
			d.UniqueID = ladspa_ids::delay + i;
			d.PortCount = control + (i + 1) * 2;
			register_plugin({d,
			    "delay_" + std::to_string(i + 1) + "ch",
			    "Delay (" + std::to_string(i + 1) + " Channel)"});
		}
	}
} init;

} /* namespace */
