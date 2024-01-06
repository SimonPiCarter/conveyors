#include "Merger.hh"

void load(Merger & merger_p)
{
	if(can_add(merger_p.innerLine))
	{
		if(merger_p.in[merger_p.priority] && can_consume(*merger_p.in[merger_p.priority]))
		{
			add_to_start(merger_p.innerLine, consume(*merger_p.in[merger_p.priority]));
			std::swap(merger_p.priority, merger_p.non_priority);
		}
		else if(merger_p.in[merger_p.non_priority] && can_consume(*merger_p.in[merger_p.non_priority]))
		{
			add_to_start(merger_p.innerLine, consume(*merger_p.in[merger_p.non_priority]));
		}
	}
}

void unload(Merger & merger_p)
{
	if(can_consume(merger_p.innerLine)
	&& merger_p.out[0] && can_add(*merger_p.out[0]))
	{
		add_to_start(*merger_p.out[0], consume(merger_p.innerLine));
	}
}
