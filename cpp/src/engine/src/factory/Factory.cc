#include "Factory.hh"

#include "stats/Statistics.hh"

void noop_factory(Factory &, size_t) {}

void score_factory(Statistics &stats_p, size_t good_p, Factory &, size_t idx_p)
{
	if(idx_p == good_p)
	{
		stats_p.score += 50;
	}
	else
	{
		stats_p.score -= 10;
	}
}

void load(Factory & factory_p)
{
	for(size_t i = 0 ; i < factory_p.entries.size() ; ++ i)
	{
		Line * entry_l = factory_p.entries[i];
		if(entry_l && can_consume(*entry_l)
		&& factory_p.buffer.size() < factory_p.max_buffer_size)
		{
			factory_p.buffer.push_back(consume(*entry_l));
		}
	}
}

void process(Factory & factory_p)
{
	if(factory_p.buffer.size() > 0)
	{
		++factory_p.step_count;
		if(factory_p.step_count == factory_p.duration)
		{
			factory_p.output_handler(factory_p, factory_p.buffer.front());
			factory_p.buffer.pop_front();
			factory_p.step_count = 0;
		}
	}
}
