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
		multi_mesh_l->set_instance_count(10000);
		multi_mesh_l->set_visible_instance_count(10000);

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

void LineGodot::_process(double delta_p)
{
	static double elapsed = 0.;
	static double speed = 0.5;
	elapsed += delta_p;

	if(elapsed >= 0.1 && _line)
	{
		if(old_pos.size() != get_content_size(*_line))
		{
			old_pos.resize(get_content_size(*_line), Vector3());
		}
		size_t idx_l = _line->first;
		unsigned long pos_l = _line->dist_end;
		for(size_t i = 0 ; idx_l < _line->items.size() ; ++i)
		{
			ItemOnLine const & item = _line->items[idx_l];

			old_pos[i] = Vector3(pos_l , 0, 0);

			pos_l += item.dist_to_next + 100;
			idx_l = item.next;
		}

		step(*_line);
		get_multimesh()->set_visible_instance_count(get_content_size(*_line));
		elapsed = 0;
	}

	if(get_multimesh().is_valid() && _line)
	{
		double partial_step = std::min(1., elapsed / 0.1);
		size_t idx_l = _line->first;
		unsigned long pos_l = _line->dist_end;
		for(size_t i = 0 ; i < get_multimesh()->get_visible_instance_count() && idx_l < _line->items.size() ; ++i)
		{
			ItemOnLine const & item = _line->items[idx_l];
			Vector3 world_pos_l(pos_l, 0, 0);
			Vector3 dir_l = world_pos_l - old_pos[i];
			world_pos_l = old_pos[i] + dir_l * partial_step;

			get_multimesh()->set_instance_transform(i, Transform3D(Basis(), world_pos_l / 166.));

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

	get_multimesh()->set_instance_count(capacity_p);
	get_multimesh()->set_visible_instance_count(0);
}

void LineGodot::add_to_line(int item)
{
	if(_line)
	{
		add_to_start(*_line, size_t(item));
	}
}

void LineGodot::_bind_methods()
{
		ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &LineGodot::set_mesh);
		ClassDB::bind_method(D_METHOD("get_mesh"), &LineGodot::get_mesh);

		ClassDB::bind_method(D_METHOD("set_up_line", "capacity", "speed"), &LineGodot::set_up_line);
		ClassDB::bind_method(D_METHOD("add_to_line", "item"), &LineGodot::add_to_line);

		ADD_GROUP("LineGodot", "LineGodot_");
		ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_mesh", "get_mesh");

}
