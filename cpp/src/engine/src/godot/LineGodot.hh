#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/classes/mesh.hpp>

#include "line/Line.hh"
#include <vector>

struct LineSegment
{
	godot::Vector3 origin;
	godot::Vector3 destination;
	unsigned long start;
	unsigned long end;
};

class LineGodot : public godot::MultiMeshInstance3D {
	GDCLASS(LineGodot, godot::MultiMeshInstance3D)

public:
	void _ready() override;
	void _process(double delta_p) override;

	void set_mesh(const godot::Ref<godot::Mesh>& mesh_p) { _mesh = mesh_p; }
	godot::Ref<godot::Mesh> get_mesh() const { return _mesh; }

	void set_up_line(int capacity_p, int speed_p);
	void add_to_line(int item);
	int consume_in_line();
	void add_segment_to_line(godot::Vector3 const &origin_p, godot::Vector3 const &destination_p, int end_p);

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

	Line * getLine() { return _line; }
	Line const * getLine() const { return _line; }

	void update();

private:
	godot::Ref<godot::Mesh> _mesh;

	Line *_line = nullptr;

	std::vector<LineSegment> _segments;

	/// @brief old position of all instances for lerp
	std::vector<double> old_pos;

	double _elapsed = 0.;
};

