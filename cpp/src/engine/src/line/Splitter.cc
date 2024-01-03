#include "Splitter.hh"

void load(Splitter & splitter_p)
{
	if(splitter_p.in[0] && can_consume(*splitter_p.in[0])
	&& can_add(splitter_p.innerLine))
	{
		add_to_start(splitter_p.innerLine, consume(*splitter_p.in[0]));
	}
}

void unload(Splitter & splitter_p)
{
	if(can_consume(splitter_p.innerLine))
	{
		if(splitter_p.out[splitter_p.priority] && can_add(*splitter_p.out[splitter_p.priority]))
		{
			add_to_start(*splitter_p.out[splitter_p.priority], consume(splitter_p.innerLine));
			// swap out[splitter_p.priority] and out[splitter_p.non_priority] to check other lane next
			std::swap(splitter_p.priority, splitter_p.non_priority);
		}
		else if(splitter_p.out[splitter_p.non_priority] && can_add(*splitter_p.out[splitter_p.non_priority]))
		{
			add_to_start(*splitter_p.out[splitter_p.non_priority], consume(splitter_p.innerLine));
		}
	}
}
