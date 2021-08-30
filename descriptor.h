#pragma once

#include <ladspa.h>
#include <string>

/*
 * descriptor - take a copy of a LADSPA_Descriptor and change label and name
 *
 * This is helpful when using the same descriptor with different port counts.
 */
class descriptor {
public:
	descriptor(const LADSPA_Descriptor &, std::string &&label,
		   std::string &&name);
	descriptor(descriptor &&);

	const LADSPA_Descriptor *get();

private:
	LADSPA_Descriptor d_;
	std::string label_;
	std::string name_;
};

/*
 * register_plugin - register a plugin descriptor
 */
void register_plugin(descriptor &&);
