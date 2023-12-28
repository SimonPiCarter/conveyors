#include "LineGodot.hh"
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void LineGodot::_ready()
{
	if(_mesh.is_valid())
	{
		MultiMesh * multi_mesh_l = memnew(MultiMesh);

		multi_mesh_l->set_mesh(_mesh);
		multi_mesh_l->set_transform_format(MultiMesh::TransformFormat::TRANSFORM_3D);
		multi_mesh_l->set_instance_count(_line ? _line->items.size() : 10000);
		multi_mesh_l->set_visible_instance_count(_line ? get_content_size(*_line) : 10000);

		set_multimesh(multi_mesh_l);
		for(size_t i = 0 ; i < 10 ; ++i)
		{
			for(size_t j = 0 ; j < multi_mesh_l->get_instance_count()/10 ; ++j)
			{
				multi_mesh_l->set_instance_transform(i+j*10, Transform3D(Basis(), Vector3(i , 0, j)));
			}
		}
	}
}

std::pair<LineSegment, size_t> get_segment(LineSegment segment_p, size_t index_segment_p, std::vector<LineSegment> const &segments_p, unsigned long pos_p)
{
	// look up for segment
	while(index_segment_p+1 < segments_p.size() && segment_p.end < pos_p)
	{
		++index_segment_p;
		segment_p = segments_p[index_segment_p];
	}
	return {segment_p, index_segment_p};
}

void LineGodot::update()
{
	if(_line)
	{
		if(old_pos.size() != get_content_size(*_line))
		{
			old_pos.resize(get_content_size(*_line), 0.);
		}
		size_t idx_l = _line->first;
		unsigned long pos_l = _line->dist_end;
		for(size_t i = 0 ; idx_l < _line->items.size() ; ++i)
		{
			ItemOnLine const & item = _line->items[idx_l];

			old_pos[i] = pos_l;

			pos_l += item.dist_to_next + 100;
			idx_l = item.next;
		}

		step(*_line);
		get_multimesh()->set_visible_instance_count(get_content_size(*_line));
	}
	_elapsed = 0;
}

void LineGodot::_process(double delta_p)
{
	_elapsed += delta_p;

	if(get_multimesh().is_valid() && _line)
	{
		double partial_step = std::min(1., _elapsed / 0.1);
		size_t idx_l = _line->first;
		unsigned long pos_l = _line->dist_end;
		LineSegment segment_l { Vector3(0,0,0), Vector3(_line->items.size()*100/166., 0, 0), 0, _line->items.size()*100 };
		if(!_segments.empty())
		{
			segment_l = _segments[0];
		}
		size_t index_segment_l = 0;
		for(size_t i = 0 ; i < get_multimesh()->get_visible_instance_count() && idx_l < _line->items.size() ; ++i)
		{
			ItemOnLine const & item = _line->items[idx_l];
			double line_pos_l = pos_l;

			// look up for segment
			double diff_l = line_pos_l - old_pos[i];
			line_pos_l = old_pos[i] + diff_l * partial_step;

			std::pair<LineSegment, size_t> segment_pair_l = get_segment(segment_l, index_segment_l, _segments, line_pos_l);
			segment_l = segment_pair_l.first;
			index_segment_l = segment_pair_l.second;

			double seg_pos_l = (line_pos_l - segment_l.start)/(segment_l.end - segment_l.start);

			// convert line pos to world pos
			godot::Vector3 dir3d_l = segment_l.destination - segment_l.origin;
			Vector3 world_pos_l = segment_l.origin + dir3d_l * seg_pos_l;

			get_multimesh()->set_instance_transform(i, Transform3D(Basis(), world_pos_l));

			pos_l += item.dist_to_next + 100;
			idx_l = item.next;
		}
	}
}

void LineGodot::set_up_line(int capacity_p, int speed_p)
{
	delete _line;
	_line = new Line(capacity_p);
	_line->speed = speed_p;
	_segments.clear();

	if(get_multimesh().is_valid())
	{
		get_multimesh()->set_instance_count(capacity_p);
		get_multimesh()->set_visible_instance_count(0);
	}
}

void LineGodot::add_to_line(int item)
{
	if(_line)
	{
		add_to_start(*_line, size_t(item));
	}
}

int LineGodot::consume_in_line()
{
	if(_line)
	{
		return consume(*_line);
	}
	return 0;
}


void LineGodot::add_segment_to_line(godot::Vector3 const &origin_p, godot::Vector3 const &destination_p, int end_p)
{
	if(end_p < 0)
	{
		UtilityFunctions::push_error("Cannot add_segment_to_line with end < 0");
	}
	unsigned long start_l = 0;
	if(!_segments.empty())
	{
		start_l = _segments.back().end;
	}
	_segments.push_back({origin_p, destination_p, start_l, (unsigned long)end_p});
}

void LineGodot::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &LineGodot::set_mesh);
	ClassDB::bind_method(D_METHOD("get_mesh"), &LineGodot::get_mesh);

	ClassDB::bind_method(D_METHOD("set_up_line", "capacity", "speed"), &LineGodot::set_up_line);
	ClassDB::bind_method(D_METHOD("add_to_line", "item"), &LineGodot::add_to_line);
	ClassDB::bind_method(D_METHOD("consume_in_line"), &LineGodot::consume_in_line);
	ClassDB::bind_method(D_METHOD("add_segment_to_line", "origin", "destination", "end"), &LineGodot::add_segment_to_line);


	ADD_GROUP("LineGodot", "LineGodot_");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_mesh", "get_mesh");
}
