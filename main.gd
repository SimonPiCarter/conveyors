extends Node3D

var mesh = preload("res://modles/box-small.tres")
@onready var manager_godot = $ManagerGodot
var cur_points : Array[Vector2i] = []
var entries = []
@onready var line_build = $line

func add_line(in_points : Array[Vector2i], adjusted) -> Node3D:
	var other : Array[Vector2i] = []
	for vec in in_points:
		if adjusted:
			other.push_back(vec + Vector2i(100,100))
		else:
			other.push_back(vec)
	var line = manager_godot.add_line(other, 160, mesh)
	add_child(line)
	var line_desc = preload("res://scenes/lines/line.tscn").instantiate()
	line_desc.set_up_from_line(prep_line(other))
	add_child(line_desc)
	return line

# Called when the node enters the scene tree for the first time.
func _ready():
	manager_godot.init_grid(110,110)

	var in_points : Array[Vector2i] = [Vector2i(-25,-25), Vector2i(-25,0)]
	entries.push_back(add_line(in_points, true))

	in_points = [Vector2i(-23,-25), Vector2i(-23,0)]
	entries.push_back(add_line(in_points, true))

	in_points = [Vector2i(-23,-25), Vector2i(-25,-25)]
	var first = add_line(in_points, true)

	in_points = [Vector2i(-10,0), Vector2i(-10,-25), Vector2i(-23,-25)]
	var first_b = add_line(in_points, true)

	manager_godot.add_merger(Vector2i(100-23,100-25), first_b, first, entries[1])

	in_points = [Vector2i(-40,-20), Vector2i(-40,-25), Vector2i(-25,-25)]
	var second = add_line(in_points, true)

	in_points = [Vector2i(-36,-20), Vector2i(-40,-20)]
	var third = add_line(in_points, true)

	in_points = [Vector2i(-30,0), Vector2i(-36,-20)]
	var third_b = add_line(in_points, true)

	in_points = [Vector2i(-50,0), Vector2i(-40,-20)]
	var fourth = add_line(in_points, true)

	manager_godot.add_sorter(Vector2i(100-25,100-25), entries[0], first, second, 1)
	manager_godot.add_sorter(Vector2i(100-40,100-20), second, third, fourth, 2)

	manager_godot.add_score_factory(Vector2i(100-10,100), first_b, 5, 1)
	manager_godot.add_score_factory(Vector2i(100-30,100), third_b, 5, 2)
	manager_godot.add_score_factory(Vector2i(100-30,100), fourth, 5, 0)

	# second line
	in_points = [Vector2i(78,88), Vector2i(88,88)]
	entries.push_back(add_line(in_points, false))

	in_points = [Vector2i(71,88), Vector2i(74,88)]
	var next = add_line(in_points, false)

	manager_godot.add_bridge(entries[2], next, 3)

	in_points = [Vector2i(54,83), Vector2i(69,88)]
	var next2 = add_line(in_points, false)

	manager_godot.add_bridge(next, next2, 1)

	in_points = [Vector2i(64,80), Vector2i(64,83), Vector2i(54,83)]
	var third_c = add_line(in_points, false)

	in_points = [Vector2i(52,83), Vector2i(54,83)]
	var fourth_b = add_line(in_points, false)

	manager_godot.add_sorter(Vector2i(100-40,100-20), next2, third_c, fourth_b, 2)
	manager_godot.add_merger(Vector2i(64,80), third_b, third_c, third)

	manager_godot.add_score_factory(Vector2i(52,83), fourth_b, 5, 2)
	manager_godot.add_score_factory(Vector2i(52,83), fourth_b, 5, 2)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$CanvasLayer/Label.text = String.num(1./delta, 2)+"\n"+String.num(manager_godot.get_score())
	var rand = RandomNumberGenerator.new()
	for entry in entries:
		entry.add_to_line(rand.randi_range(1, 3))

func _input(event):
	if event is InputEventMouseButton and event.is_pressed():
		if event.button_index == MOUSE_BUTTON_RIGHT:
			cur_points = []
			line_build.set_up_from_line(cur_points)
		elif event.button_index == MOUSE_BUTTON_LEFT:
			var camera3d = $Camera3D
			var from = camera3d.project_ray_origin(event.position)
			var dist = (from.y + 0.4)/camera3d.project_ray_normal(event.position).y
			var to = from - camera3d.project_ray_normal(event.position) * dist

			var vec = Vector2i(round(to.x/1.2), round(to.z/1.2))
			print(vec, " : ", manager_godot.check_point(vec))
			cur_points.push_front(vec)
			var prepped_line = prep_line(cur_points)
			if not manager_godot.check_line(prepped_line):
				cur_points.pop_front()
				print("removed")
			else:
				line_build.set_up_from_line(prepped_line)

	if event is InputEventKey and event.physical_keycode == KEY_SPACE and event.is_pressed():
		add_line(prep_line(cur_points), false)
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
