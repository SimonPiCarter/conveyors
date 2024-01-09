#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh.hpp>

#include "LineGodot.hh"

#include "line/connector/Connector.hh"
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

	void init_grid(int width_p, int height_p);
	bool check_connector_pos(godot::Vector2i const &pos_p, godot::Vector2i const &pos_handler_p, bool input);
	int add_connector(godot::Vector2i const &pos_p, godot::Vector2i const &pos_handler_p, bool input);
	void del_connector(int index_p);

	int add_line(godot::TypedArray<godot::Vector2i> const &points_p, double speed_p, godot::Ref<godot::Mesh> const &mesh_p);
	LineGodot * get_line(int idx_p);
	void remove_line(int idx_p);

	bool check_line(godot::TypedArray<godot::Vector2i> const &points_p);
	bool check_point(godot::Vector2i const &point_p);
	int get_point_index(godot::Vector2i const &point_p);
	int get_point_type(godot::Vector2i const &point_p);
	int get_line_from_point(godot::Vector2i const &point_p);

	void del_line_handler(line_handler &handler_p);

	int add_splitter(godot::Vector2i const &pos_p);
	void del_splitter(int index_p);
	godot::Vector2i const &get_splitter_pos(int splitter_p) const;

	int add_sorter(godot::Vector2i const &pos_p, int type_p);
	void del_sorter(int index_p);
	godot::Vector2i const &get_sorter_pos(int sorter_p) const;
	void set_sorter_type(int sorter_p, int type_p);
	int get_sorter_type(int sorter_p) const;

	int add_merger(godot::Vector2i const &pos_p);
	void del_merger(int index_p);
	godot::Vector2i const &get_merger_pos(int merger_p) const;

	void add_bridge(LineGodot * entry_p, LineGodot * output_p, int length_p);

	int add_score_factory(godot::Vector2i const &pos_p, int duration_p, int type_p);
	void del_factory(int index_p);

	double get_score() { return stats.score; }

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

	line_handler & get_handler(CaseType type_p, size_t idx_p);
	line_handler const & get_handler(CaseType type_p, size_t idx_p) const;

private:
	smart_ptr_list<LineGodot> _lines;
	// need to be pointers to be kept in lines as pointers
	smart_ptr_list<Connector> _connectors;
	smart_list_togglable<Splitter> _splitters;
	smart_list_togglable<Sorter> _sorters;
	smart_list_togglable<Merger> _mergers;
	std::vector<Bridge> _bridges;
	smart_list_togglable<Factory> _factories;

	double _elapsed = 0.;

	Statistics stats;
	Grid grid;
	std::vector<std::vector<Connector *> > connectorGrid;
};

