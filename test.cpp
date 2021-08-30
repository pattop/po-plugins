#include "descriptor.h"
#include "ladspa_ids.h"
#include <array>
#include <cstdio>

/*
 * port: input or output for audio or control data
 *
 * plugin runs over a block at a time
 *
 * control data has a single value at the start of run() or run_adding()
 * - doesn't change during duration
 *
 * plugin can assume ports are connected before it is asked to run
 *
 * 1.0f is 0dB reverence amplitude
 *
 * Use LADSPA_Data (float).
 *
 * Data can be processed in-place (i.e. input buffer == output buffer)
 *
 * LADSPA_PROPERTY_HARD_RT_CAPABLE
 *  - don't use malloc/free/new/delete during run() or run_adding()
 *  - don't call dumb library functions
 *  - don't call any blocking functions
 */

namespace {

  /* This member is a function pointer that instantiates a plugin. A
     handle is returned indicating the new plugin instance. The
     instantiation function accepts a sample rate as a parameter. The
     plugin descriptor from which this instantiate function was found
     must also be passed. This function must return NULL if
     instantiation fails. 

     Note that instance initialisation should generally occur in
     activate() rather than here. */
LADSPA_Handle
instantiate(const LADSPA_Descriptor *Descriptor, unsigned long sample_rate)
{

}

  /* This member is a function pointer that connects a port on an
     instantiated plugin to a memory location at which a block of data
     for the port will be read/written. The data location is expected
     to be an array of LADSPA_Data for audio ports or a single
     LADSPA_Data value for control ports. Memory issues will be
     managed by the host. The plugin must read/write the data at these
     locations every time run() or run_adding() is called and the data
     present at the time of this connection call should not be
     considered meaningful.

     connect_port() may be called more than once for a plugin instance
     to allow the host to change the buffers that the plugin is
     reading or writing. These calls may be made before or after
     activate() or deactivate() calls.

     connect_port() must be called at least once for each port before
     run() or run_adding() is called. When working with blocks of
     LADSPA_Data the plugin should pay careful attention to the block
     size passed to the run function as the block allocated may only
     just be large enough to contain the block of samples.

     Plugin writers should be aware that the host may elect to use the
     same buffer for more than one port and even use the same buffer
     for both input and output (see LADSPA_PROPERTY_INPLACE_BROKEN).
     However, overlapped buffers or use of a single buffer for both
     audio and control data may result in unexpected behaviour. */
void
connect_port(LADSPA_Handle h, unsigned long port, LADSPA_Data *d)
{

}

  /* This member is a function pointer that initialises a plugin
     instance and activates it for use. This is separated from
     instantiate() to aid real-time support and so that hosts can
     reinitialise a plugin instance by calling deactivate() and then
     activate(). In this case the plugin instance must reset all state
     information dependent on the history of the plugin instance
     except for any data locations provided by connect_port() and any
     gain set by set_run_adding_gain(). If there is nothing for
     activate() to do then the plugin writer may provide a NULL rather
     than an empty function.

     When present, hosts must call this function once before run() (or
     run_adding()) is called for the first time. This call should be
     made as close to the run() call as possible and indicates to
     real-time plugins that they are now live. Plugins should not rely
     on a prompt call to run() after activate(). activate() may not be
     called again unless deactivate() is called first. Note that
     connect_port() may be called before or after a call to
     activate(). */
void activate(LADSPA_Handle h)
{

}

  /* This method is a function pointer that runs an instance of a
     plugin for a block. Two parameters are required: the first is a
     handle to the particular instance to be run and the second
     indicates the block size (in samples) for which the plugin
     instance may run.

     Note that if an activate() function exists then it must be called
     before run() or run_adding(). If deactivate() is called for a
     plugin instance then the plugin instance may not be reused until
     activate() has been called again.

     If the plugin has the property LADSPA_PROPERTY_HARD_RT_CAPABLE
     then there are various things that the plugin should not do
     within the run() or run_adding() functions (see above). */
void
run(LADSPA_Handle h, unsigned long samples)
{

}

  /* This method is a function pointer that runs an instance of a
     plugin for a block. This has identical behaviour to run() except
     in the way data is output from the plugin. When run() is used,
     values are written directly to the memory areas associated with
     the output ports. However when run_adding() is called, values
     must be added to the values already present in the memory
     areas. Furthermore, output values written must be scaled by the
     current gain set by set_run_adding_gain() (see below) before
     addition.

     run_adding() is optional. When it is not provided by a plugin,
     this function pointer must be set to NULL. When it is provided,
     the function set_run_adding_gain() must be provided also. */
void
run_adding(LADSPA_Handle h, unsigned long samples)
{

}

  /* This method is a function pointer that sets the output gain for
     use when run_adding() is called (see above). If this function is
     never called the gain is assumed to default to 1. Gain
     information should be retained when activate() or deactivate()
     are called.

     This function should be provided by the plugin if and only if the
     run_adding() function is provided. When it is absent this
     function pointer must be set to NULL. */
void
set_run_adding_gain(LADSPA_Handle h, LADSPA_Data gain)
{

}

  /* This is the counterpart to activate() (see above). If there is
     nothing for deactivate() to do then the plugin writer may provide
     a NULL rather than an empty function.

     Hosts must deactivate all activated units after they have been
     run() (or run_adding()) for the last time. This call should be
     made as close to the last run() call as possible and indicates to
     real-time plugins that they are no longer live. Plugins should
     not rely on prompt deactivation. Note that connect_port() may be
     called before or after a call to deactivate().

     Deactivation is not similar to pausing as the plugin instance
     will be reinitialised when activate() is called to reuse it. */
void
deactivate(LADSPA_Handle h)
{

}

  /* Once an instance of a plugin has been finished with it can be
     deleted using the following function. The instance handle passed
     ceases to be valid after this call.
  
     If activate() was called for a plugin instance then a
     corresponding call to deactivate() must be made before cleanup()
     is called. */
void
cleanup(LADSPA_Handle h)
{

}

constexpr auto channels = 8;
constexpr std::array<LADSPA_PortDescriptor, 1 + 2 * channels> ports = {
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
	"Control: e.g. cutoff frequency",
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
	} },
};

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
			.run_adding = run_adding,
			.set_run_adding_gain = set_run_adding_gain,
			.deactivate = deactivate,
			.cleanup = cleanup,
		};

		for (int i = 0; i < channels; ++i) {
			d.UniqueID = ladspa_ids::test + i;
			d.PortCount = 1 + (i + 1) * 2;
			register_plugin({d,
			    "test_" + std::to_string(i + 1) + "ch",
			    "Test (" + std::to_string(i + 1) + " Channel)"});
		}
	}
} init;

} /* namespace */
