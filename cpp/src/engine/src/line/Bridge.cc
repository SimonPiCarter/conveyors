#include "Bridge.hh"

void load(Bridge & bridge_p)
{
	if(bridge_p.input && can_consume(*bridge_p.input)
	&& can_add(bridge_p.innerLine))
	{
		add_to_start(bridge_p.innerLine, consume(*bridge_p.input));
	}
}

void unload(Bridge & bridge_p)
{
	if(bridge_p.output && can_consume(bridge_p.innerLine) && can_add(*bridge_p.output))
	{
		add_to_start(*bridge_p.output, consume(bridge_p.innerLine));
	}
}
