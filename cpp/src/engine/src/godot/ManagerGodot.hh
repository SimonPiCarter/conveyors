#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh.hpp>

#include "LineGodot.hh"

#include "line/Splitter.hh"
#include "line/Sorter.hh"
#include "stats/Statistics.hh"
#include "factory/Factory.hh"

class ManagerGodot : public godot::Node3D {
	GDCLASS(ManagerGodot, godot::Node3D)

public:
	~ManagerGodot();

	void _process(double delta_p) override;

	LineGodot* add_line(godot::TypedArray<godot::Vector2i> const &points_p, double speed_p, godot::Ref<godot::Mesh> const &mesh_p);

	void add_splitter(LineGodot * entry_p, LineGodot * first_p, LineGodot * second_p);
	void add_sorter(LineGodot * entry_p, LineGodot * first_p, LineGodot * second_p, int type_p);

	void add_score_factory(LineGodot * entry_p, int duration_p, int type_p);

	double get_score() { return stats.score; }

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	std::vector<LineGodot*> _lines;
	std::vector<Splitter> _splitters;
	std::vector<Sorter> _sorters;
	std::vector<Factory> _factories;

	double _elapsed = 0.;

	Statistics stats;
};

