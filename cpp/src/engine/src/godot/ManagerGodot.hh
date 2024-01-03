#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh.hpp>

#include "LineGodot.hh"

#include "line/Bridge.hh"
#include "line/Merger.hh"
#include "line/Sorter.hh"
#include "line/Splitter.hh"
#include "stats/Statistics.hh"
#include "factory/Factory.hh"
#include "grid/Grid.hh"

#include "utils/smart_list.hh"

class ManagerGodot : public godot::Node3D {
	GDCLASS(ManagerGodot, godot::Node3D)

public:
	~ManagerGodot();

	void _process(double delta_p) override;

	void init_grid(size_t width_p, size_t height_p);

	int add_line(godot::TypedArray<godot::Vector2i> const &points_p, double speed_p, godot::Ref<godot::Mesh> const &mesh_p);
	LineGodot * get_line(int idx_p);
	void remove_line(int idx_p);

	bool check_line(godot::TypedArray<godot::Vector2i> const &points_p);
	bool check_point(godot::Vector2i const &point_p);
	int get_point_index(godot::Vector2i const &point_p);
	int get_point_type(godot::Vector2i const &point_p);
	int get_line_from_point(godot::Vector2i const &point_p);

	int add_splitter(godot::Vector2i const &pos_p, LineGodot * entry_p, LineGodot * first_p, LineGodot * second_p);
	int add_splitter_from_line(int line_p);
	bool connect_line_to_splitter_output(int line_p, int splitter_p);
	bool connect_line_to_splitter_input(int line_p, int splitter_p);
	godot::Vector2i const &get_splitter_pos(int splitter_p) const;

	int add_sorter(godot::Vector2i const &pos_p, LineGodot * entry_p, LineGodot * first_p, LineGodot * second_p, int type_p);
	int add_sorter_from_line(int line_p);
	bool connect_line_to_sorter_output(int line_p, int sorter_p);
	bool connect_line_to_sorter_input(int line_p, int sorter_p);
	godot::Vector2i const &get_sorter_pos(int sorter_p) const;
	void set_sorter_type(int sorter_p, int type_p);
	int get_sorter_type(int sorter_p) const;

	void add_merger(godot::Vector2i const &pos_p, LineGodot * output_p, LineGodot * first_p, LineGodot * second_p);
	void add_bridge(LineGodot * entry_p, LineGodot * output_p, int length_p);

	void add_score_factory(godot::Vector2i const &pos_p, LineGodot * entry_p, int duration_p, int type_p);

	double get_score() { return stats.score; }

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	smart_ptr_list<LineGodot> _lines;
	smart_list_togglable<Splitter> _splitters;
	smart_list_togglable<Sorter> _sorters;
	std::vector<Merger> _mergers;
	std::vector<Bridge> _bridges;
	std::vector<Factory> _factories;

	double _elapsed = 0.;

	Statistics stats;
	Grid grid;
};

