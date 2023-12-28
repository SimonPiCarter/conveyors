#include "Splitter.hh"

void load(Splitter & splitter_p)
{
	if(splitter_p.entry && can_consume(*splitter_p.entry)
	&& can_add(splitter_p.innerLine))
	{
		add_to_start(splitter_p.innerLine, consume(*splitter_p.entry));
	}
}

void unload(Splitter & splitter_p)
{
	if(can_consume(splitter_p.innerLine))
	{
		if(splitter_p.first && can_add(*splitter_p.first))
		{
			add_to_start(*splitter_p.first, consume(splitter_p.innerLine));
			// swap first and second to check other lane next
			std::swap(splitter_p.first, splitter_p.second);
		}
		else if(splitter_p.second && can_add(*splitter_p.second))
		{
			add_to_start(*splitter_p.second, consume(splitter_p.innerLine));
		}
	}
}
