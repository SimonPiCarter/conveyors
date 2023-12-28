#include "Merger.hh"

void load(Merger & merger_p)
{
	if(can_add(merger_p.innerLine))
	{
		if(merger_p.first && can_consume(*merger_p.first))
		{
			add_to_start(merger_p.innerLine, consume(*merger_p.first));
			std::swap(merger_p.first, merger_p.second);
		}
		else if(merger_p.second && can_consume(*merger_p.second))
		{
			add_to_start(merger_p.innerLine, consume(*merger_p.second));
		}
	}
}

void unload(Merger & merger_p)
{
	if(can_consume(merger_p.innerLine)
	&& merger_p.output && can_add(*merger_p.output))
	{
		add_to_start(*merger_p.output, consume(merger_p.innerLine));
	}
}
