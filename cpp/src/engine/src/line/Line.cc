#include "Line.hh"

Line::Line(size_t capacity_p) :
	first(capacity_p), dist_start(capacity_p*100), dist_end(0), full_dist(capacity_p*100)
{
	items.resize(capacity_p);
	for(size_t i = 0 ; i < capacity_p; ++i)
	{
		free_idx.push_back(i);
	}
}

void step(Line &line_p)
{
	if(get_content_size(line_p) == 0)
	{
		return;
	}

	unsigned long remaining_movement = 0;
	if (line_p.dist_end < line_p.speed)
	{
		remaining_movement = line_p.speed - line_p.dist_end;
		line_p.dist_end = 0;
	}
	else
	{
		line_p.dist_end -= line_p.speed;
	}

	size_t last = line_p.first;
	size_t idx = line_p.first;
	while(remaining_movement > 0 && idx < line_p.items.size())
	{
		ItemOnLine &item_l = line_p.items[idx];
		if(remaining_movement > item_l.dist_to_next)
		{
			remaining_movement = remaining_movement - item_l.dist_to_next;
			item_l.dist_to_next = 0;
		}
		else
		{
			item_l.dist_to_next -= remaining_movement;
			remaining_movement = 0;
		}
		last = idx;
		idx = item_l.next;
	}

	unsigned long performed_movement = line_p.speed - remaining_movement;
	line_p.dist_start += performed_movement;
}
bool can_add(Line const &line_p)
{
	if(line_p.dist_start < 100 || line_p.free_idx.size() == 0)
	{
		return false;
	}
	return true;
}

size_t can_consume(Line const &line_p)
{
	if(line_p.dist_end > 0 || is_empty(line_p))
	{
		return 0;
	}
	ItemOnLine const & item_l = line_p.items[line_p.first];
	return item_l.idx;
}

bool add_to_start(Line &line_p, size_t item_p)
{
	if(!can_add(line_p))
	{
		return false;
	}

	size_t idx_l = line_p.free_idx.front();
	line_p.free_idx.pop_front();

	if(is_empty(line_p))
	{
		line_p.first = idx_l;
		line_p.dist_end = line_p.full_dist - 100;
	}

	// update chaining
	line_p.items[line_p.last].next = idx_l;
	line_p.items[line_p.last].dist_to_next = line_p.dist_start - 100;
	line_p.last = idx_l;

	// init item
	line_p.items[idx_l].idx = item_p;
	line_p.items[idx_l].next = line_p.items.size();
	line_p.items[idx_l].dist_to_next = 0;

	// update distance
	line_p.dist_start = 0;

	return true;
}

size_t consume(Line &line_p)
{
	if(!can_consume(line_p))
	{
		return 0;
	}

	ItemOnLine const & item_l = line_p.items[line_p.first];
	line_p.free_idx.push_back(line_p.first);
	line_p.first = item_l.next;
	line_p.dist_end = item_l.dist_to_next + 100;

	return item_l.idx;
}

size_t get_content_size(Line const &line_p)
{
	return line_p.items.size() - line_p.free_idx.size();
}

bool is_empty(Line const &line_p)
{
	return line_p.first >= line_p.items.size();
}
