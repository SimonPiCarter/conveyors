extends Node3D

var mesh = preload("res://modles/box-small.tres")
@onready var manager_godot = $ManagerGodot
var cur_points : Array[Vector2i] = []
var test_cur_points : Array[Vector2i] = []
var entries : Array[int] = []
@onready var line_build = $line

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

var _x = false
var _c = false

func _input(event):

	if event is InputEventMouseMotion and _c and cur_points.size() > 0:
		var camera3d = $Camera3D
		var from = camera3d.project_ray_origin(event.position)
		var dist = (from.y + 0.4)/camera3d.project_ray_normal(event.position).y
		var to = from - camera3d.project_ray_normal(event.position) * dist

		var vec = Vector2i(round(to.x/1.2), round(to.z/1.2))

		test_cur_points = cur_points.duplicate()
		test_cur_points.push_front(vec)
		var prepped_line = prep_line(test_cur_points)
		if not manager_godot.check_line(prepped_line):
			line_build.set_up_from_line(prep_line(cur_points))
			test_cur_points = cur_points.duplicate()
		else:
			line_build.set_up_from_line(prepped_line)

	if event is InputEventMouseButton and event.is_pressed():
		var camera3d = $Camera3D
		var from = camera3d.project_ray_origin(event.position)
		var dist = (from.y + 0.4)/camera3d.project_ray_normal(event.position).y
		var to = from - camera3d.project_ray_normal(event.position) * dist

		var vec = Vector2i(round(to.x/1.2), round(to.z/1.2))
		var is_free = manager_godot.check_point(vec)
		print(vec, " : ", is_free)

		if event.button_index == MOUSE_BUTTON_RIGHT and not _x:
			cur_points = []
			test_cur_points = []
			line_build.set_up_from_line(cur_points)

		if event.button_index == MOUSE_BUTTON_LEFT and is_free and not _x and _c:
			cur_points.push_front(vec)
			var prepped_line = prep_line(cur_points)
			if not manager_godot.check_line(prepped_line):
				cur_points.pop_front()
			else:
				line_build.set_up_from_line(prepped_line)

		if event.button_index == MOUSE_BUTTON_RIGHT and _x:
			var line = manager_godot.get_line_from_point(vec)
			if line >= 0:
				entries.erase(line)
				manager_godot.remove_line(line)

	if event is InputEventKey and event.physical_keycode == KEY_X:
		_x = event.is_pressed()

	if event is InputEventKey and event.physical_keycode == KEY_C:
		_c = event.is_pressed()

	if event is InputEventKey and event.physical_keycode == KEY_SPACE and event.is_pressed():
		entries.push_back(add_line(prep_line(test_cur_points), false))
		cur_points = []
		test_cur_points = []
		line_build.set_up_from_line(cur_points)

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
