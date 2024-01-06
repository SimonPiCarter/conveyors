extends Node3D

var mesh = preload("res://modles/box-small.tres")
@onready var manager_godot = $ManagerGodot
var cur_points : Array[Vector2i] = []
var test_cur_points : Array[Vector2i] = []
var entries : Array[int] = []
@onready var line_build = $line

var splitters = {}
var sorters = {}
var mergers = {}

func add_line(in_points : Array[Vector2i], adjusted) -> int:
	var other : Array[Vector2i] = []
	for vec in in_points:
		if adjusted:
			other.push_back(vec + Vector2i(100,100))
		else:
			other.push_back(vec)

	var line = manager_godot.add_line(other, 160, mesh)

	# coveyors
	var line_desc = preload("res://scenes/lines/line.tscn").instantiate()
	line_desc.set_up_from_line(prep_line(other))
	manager_godot.get_line(line).add_child(line_desc)
	return line

# Called when the node enters the scene tree for the first time.
func _ready():
	manager_godot.init_grid(110,110)

	var in_points : Array[Vector2i] = [Vector2i(-25,-25), Vector2i(-25,0)]
	entries.push_back(add_line(in_points, true))

	in_points = [Vector2i(-23,-25), Vector2i(-23,0)]
	entries.push_back(add_line(in_points, true))

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$CanvasLayer/Label.text = String.num(1./delta, 2)+"\n"+String.num(manager_godot.get_score())
	var rand = RandomNumberGenerator.new()
	for entry in entries:
		manager_godot.get_line(entry).add_to_line(rand.randi_range(1, 3))

enum build_type {NONE, DELETE, BELT, SPLITTER, SORTER, MERGER}
var cur_built_type = build_type.NONE

var cur_splitter = -1
var cur_sorter = -1
var cur_merger = -1

func is_handler_line():
	return cur_built_type == build_type.SPLITTER or cur_built_type == build_type.SORTER or cur_built_type == build_type.MERGER

func _input(event):

	if event is InputEventMouse:
		var camera3d = $Camera3D
		var from = camera3d.project_ray_origin(event.position)
		var dist = (from.y + 0.4)/camera3d.project_ray_normal(event.position).y
		var to = from - camera3d.project_ray_normal(event.position) * dist

		var vec = Vector2i(round(to.x/1.2), round(to.z/1.2))
		var is_free = manager_godot.check_point(vec)
		var type = manager_godot.get_point_type(vec)
		var case_idx = manager_godot.get_point_index(vec)

		if event is InputEventMouseMotion and cur_built_type == build_type.BELT and cur_points.size() > 0:
			test_cur_points = cur_points.duplicate()
			test_cur_points.push_front(vec)
			var prepped_line = prep_line(test_cur_points)
			if not manager_godot.check_line(prepped_line):
				line_build.set_up_from_line(prep_line(cur_points))
				test_cur_points = cur_points.duplicate()
			else:
				line_build.set_up_from_line(prepped_line)

		if event is InputEventMouseButton and event.is_pressed():

			if event.button_index == MOUSE_BUTTON_RIGHT and cur_built_type != build_type.DELETE:
				cur_points = []
				test_cur_points = []
				line_build.set_up_from_line(cur_points)

			if event.button_index == MOUSE_BUTTON_LEFT and is_free and cur_built_type == build_type.BELT:
				cur_points.push_front(vec)
				var prepped_line = prep_line(cur_points)
				if not manager_godot.check_line(prepped_line):
					cur_points.pop_front()
				else:
					line_build.set_up_from_line(prepped_line)

			if event.button_index == MOUSE_BUTTON_RIGHT and cur_built_type == build_type.DELETE:
				var line = manager_godot.get_line_from_point(vec)
				if line >= 0:
					entries.erase(line)
					manager_godot.remove_line(line)

			if event.button_index == MOUSE_BUTTON_LEFT and is_handler_line():
				if type == 2:
					if cur_built_type == build_type.SPLITTER:
						var idx_splitter = manager_godot.add_splitter_from_line(case_idx)
						if idx_splitter < 0:
							return
						var splitter = preload("res://scenes/modules/splitter.tscn").instantiate()
						splitter.idx = idx_splitter
						splitter.clicked.connect(_on_splitter_clicked)
						var pos_2d = manager_godot.get_splitter_pos(idx_splitter)
						splitter.position = Vector3(pos_2d.x*1.2,0,pos_2d.y*1.2)
						splitters[idx_splitter] = splitter
						add_child(splitter)
						cur_built_type = build_type.NONE
					elif cur_built_type == build_type.SORTER:
						var idx_sorter = manager_godot.add_sorter_from_line(case_idx)
						if idx_sorter < 0:
							return
						var sorter = preload("res://scenes/modules/sorter.tscn").instantiate()
						sorter.idx = idx_sorter
						sorter.clicked.connect(_on_sorter_clicked)
						sorter.wheeled.connect(_on_sorter_wheeled)
						var pos_2d = manager_godot.get_sorter_pos(idx_sorter)
						sorter.position = Vector3(pos_2d.x*1.2,0,pos_2d.y*1.2)
						sorters[idx_sorter] = sorter
						add_child(sorter)
						cur_built_type = build_type.NONE
					elif cur_built_type == build_type.MERGER:
						var idx_merger = manager_godot.add_merger_from_line(case_idx)
						if idx_merger < 0:
							return
						var merger = preload("res://scenes/modules/merger.tscn").instantiate()
						merger.idx = idx_merger
						merger.clicked.connect(_on_merger_clicked)
						var pos_2d = manager_godot.get_merger_pos(idx_merger)
						merger.position = Vector3(pos_2d.x*1.2,0,pos_2d.y*1.2)
						mergers[idx_merger] = merger
						add_child(merger)
						cur_built_type = build_type.NONE

			if event.button_index == MOUSE_BUTTON_LEFT:
				print(vec, " : ", is_free)
				if cur_splitter >= 0 and type == 2:
					manager_godot.connect_line_to_splitter_output(case_idx, cur_splitter)
				if cur_sorter >= 0 and type == 2:
					manager_godot.connect_line_to_sorter_output(case_idx, cur_sorter)
				if cur_merger >= 0 and type == 2:
					manager_godot.connect_line_to_merger_input(case_idx, cur_merger)
				if is_free or type != 3:
					cur_splitter = -1
				if is_free or type != 5:
					cur_sorter = -1
				if is_free or type != 6:
					cur_merger = -1

	if event is InputEventKey and event.physical_keycode == KEY_X and event.is_pressed():
		cur_built_type = build_type.DELETE

	if event is InputEventKey and event.physical_keycode == KEY_V and event.is_pressed():
		cur_built_type = build_type.SPLITTER

	if event is InputEventKey and event.physical_keycode == KEY_C and event.is_pressed():
		cur_built_type = build_type.BELT

	if event is InputEventKey and event.physical_keycode == KEY_B and event.is_pressed():
		cur_built_type = build_type.SORTER

	if event is InputEventKey and event.physical_keycode == KEY_N and event.is_pressed():
		cur_built_type = build_type.MERGER

	if event is InputEventKey and event.physical_keycode == KEY_ESCAPE and event.is_pressed():
		cur_built_type = build_type.NONE

	if event is InputEventKey and event.physical_keycode == KEY_SPACE and event.is_pressed():
		add_line(prep_line(test_cur_points), false)
		cur_points = []
		test_cur_points = []
		line_build.set_up_from_line(cur_points)
		cur_built_type = build_type.NONE

func prep_line(list_points) -> Array[Vector2i]:
	var array : Array[Vector2i] = []
	var old_point = null
	for point in list_points:
		if old_point != null:
			if old_point.x != point.x and old_point.y != point.y:
				array.push_front(Vector2i(old_point.x, point.y))
				array.push_front(Vector2i(point.x, point.y))
			else:
				array.push_front(point)
		else:
			array.push_front(point)
		old_point = point

	return array

func _on_splitter_clicked(idx):
	cur_splitter = idx

func _on_sorter_clicked(idx):
	cur_sorter = idx

func _on_sorter_wheeled(idx, up):
	var type = manager_godot.get_sorter_type(idx)
	if up:
		type += 1
	else:
		type -= 1
	manager_godot.set_sorter_type(idx, type)

func _on_merger_clicked(idx):
	cur_merger = idx
