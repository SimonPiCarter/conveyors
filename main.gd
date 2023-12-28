extends Node3D

var mesh = preload("res://modles/box-small.tres")
@onready var manager_godot = $ManagerGodot
var cur_points : Array[Vector2i] = []
var entry = null
var second_entry = null
@onready var line_build = $line

func add_line(in_points : Array[Vector2i]) -> Node3D:
	var line = manager_godot.add_line(in_points, 160, mesh)
	add_child(line)
	var line_desc = preload("res://scenes/lines/line.tscn").instantiate()
	line_desc.set_up_from_line(prep_line(in_points))
	add_child(line_desc)
	return line

# Called when the node enters the scene tree for the first time.
func _ready():
	var in_points : Array[Vector2i] = [Vector2i(-25,-25), Vector2i(-25,0)]
	entry = add_line(in_points)

	in_points = [Vector2i(-23,-25), Vector2i(-23,0)]
	second_entry = add_line(in_points)

	in_points = [Vector2i(-23,-25), Vector2i(-25,-25)]
	var first = add_line(in_points)

	in_points = [Vector2i(-10,0), Vector2i(-10,-25), Vector2i(-23,-25)]
	var first_b = add_line(in_points)

	manager_godot.add_merger(first_b, first, second_entry)

	in_points = [Vector2i(-40,-20), Vector2i(-40,-25), Vector2i(-25,-25)]
	var second = add_line(in_points)

	in_points = [Vector2i(-30,0), Vector2i(-40,-20)]
	var third = add_line(in_points)

	in_points = [Vector2i(-50,0), Vector2i(-40,-20)]
	var fourth = add_line(in_points)

	manager_godot.add_sorter(entry, first, second, 1)
	manager_godot.add_sorter(second, third, fourth, 2)

	manager_godot.add_score_factory(first_b, 5, 1)
	manager_godot.add_score_factory(third, 5, 2)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$CanvasLayer/Label.text = String.num(1./delta, 2)+"\n"+String.num(manager_godot.get_score())
	var rand = RandomNumberGenerator.new()
	entry.add_to_line(rand.randi_range(1, 3))
	second_entry.add_to_line(rand.randi_range(1, 3))

func _input(event):
	if event is InputEventMouseButton and event.is_pressed():
		if event.button_index == MOUSE_BUTTON_RIGHT:
			cur_points = []
			line_build.set_up_from_line(cur_points)
		elif event.button_index == MOUSE_BUTTON_LEFT:
			var camera3d = $Camera3D
			var from = camera3d.project_ray_origin(event.position)
			var dist = from.y/camera3d.project_ray_normal(event.position).y
			var to = from - camera3d.project_ray_normal(event.position) * dist

			var vec = Vector2i(to.x/1.2, to.z/1.2)
			cur_points.push_front(vec)
			line_build.set_up_from_line(prep_line(cur_points))

	if event is InputEventKey and event.physical_keycode == KEY_SPACE and event.is_pressed():
		add_line(prep_line(cur_points))
		cur_points = []
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
