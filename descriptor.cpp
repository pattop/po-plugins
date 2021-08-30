#include "descriptor.h"

#include <vector>

namespace {

std::vector<descriptor> *descriptors;

}

/*
 * descriptor
 */
descriptor::descriptor(const LADSPA_Descriptor &d, std::string &&label,
		       std::string &&name)
: d_{d}, label_{std::move(label)}, name_{std::move(name)}
{
	d_.Label = label_.c_str();
	d_.Name = name_.c_str();
}

descriptor::descriptor(descriptor &&o)
: d_{o.d_}, label_{std::move(o.label_)}, name_{std::move(o.name_)}
{
	d_.Label = label_.c_str();
	d_.Name = name_.c_str();
}

const LADSPA_Descriptor *
descriptor::get()
{
	return &d_;
}

/*
 * register_plugin
 */
void
register_plugin(descriptor &&d)
{
	/* this is slightly ugly but necessary as the initialisation order of
	 * global objects is undefined between translation units */
	static std::vector<descriptor> desc;
	desc.emplace_back(std::move(d));
	descriptors = &desc;
}

/*
 * ladspa_descriptor
 */
__attribute__((externally_visible))
const LADSPA_Descriptor *
ladspa_descriptor(unsigned long i)
{
	if (descriptors && i >= size(*descriptors))
		return nullptr;
	return (*descriptors)[i].get();
}
