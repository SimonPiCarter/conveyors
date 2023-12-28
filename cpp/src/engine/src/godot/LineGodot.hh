#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/classes/mesh.hpp>

#include "line/Line.hh"
#include <vector>

class LineGodot : public godot::MultiMeshInstance3D {
	GDCLASS(LineGodot, godot::MultiMeshInstance3D)

public:
	void _ready() override;
	void _process(double delta_p) override;

	void set_mesh(const godot::Ref<godot::Mesh>& mesh_p) { _mesh = mesh_p; }
	godot::Ref<godot::Mesh> get_mesh() const { return _mesh; }

	void set_up_line(int capacity_p, int speed_p);
	void add_to_line(int item);

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	godot::Ref<godot::Mesh> _mesh;

	Line *_line = nullptr;

	/// @brief old position of all instances for lerp
	std::vector<godot::Vector3> old_pos;
};

