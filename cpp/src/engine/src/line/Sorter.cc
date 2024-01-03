#include "Sorter.hh"

void load(Sorter & sorter_p)
{
	if(sorter_p.in[0] && can_consume(*sorter_p.in[0])
	&& can_add(sorter_p.innerLine))
	{
		add_to_start(sorter_p.innerLine, consume(*sorter_p.in[0]));
	}
}

void unload(Sorter & sorter_p)
{
	size_t idx_l = can_consume(sorter_p.innerLine);
	if(idx_l)
	{
		if(idx_l == sorter_p.type)
		{
			if(sorter_p.out[0] && can_add(*sorter_p.out[0]))
			{
				consume(sorter_p.innerLine);
				add_to_start(*sorter_p.out[0], idx_l);
			}
		}
		else if(sorter_p.out[1] && can_add(*sorter_p.out[1]))
		{
			consume(sorter_p.innerLine);
			add_to_start(*sorter_p.out[1], idx_l);
		}
	}
}
