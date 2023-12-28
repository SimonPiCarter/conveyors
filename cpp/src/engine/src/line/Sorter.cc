#include "Sorter.hh"

void load(Sorter & sorter_p)
{
	if(sorter_p.entry && can_consume(*sorter_p.entry)
	&& can_add(sorter_p.innerLine))
	{
		add_to_start(sorter_p.innerLine, consume(*sorter_p.entry));
	}
}

void unload(Sorter & sorter_p)
{
	size_t idx_l = can_consume(sorter_p.innerLine);
	if(idx_l)
	{
		if(idx_l == sorter_p.type)
		{
			if(sorter_p.first && can_add(*sorter_p.first))
			{
				consume(sorter_p.innerLine);
				add_to_start(*sorter_p.first, idx_l);
			}
		}
		else if(sorter_p.second && can_add(*sorter_p.second))
		{
			consume(sorter_p.innerLine);
			add_to_start(*sorter_p.second, idx_l);
		}
	}
}
