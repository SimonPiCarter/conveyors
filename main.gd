extends Node3D

var mesh = preload("res://modles/box-small.tres")
@onready var manager_godot = $ManagerGodot
var cur_points = []
var entry = null
var first = null
var second = null

# Called when the node enters the scene tree for the first time.
func _ready():
	var in_points = [Vector2i(-25,-25), Vector2i(-25,0)]
	entry = manager_godot.add_line(in_points, 160, mesh)
	add_child(entry)

	in_points = [Vector2i(-10,0), Vector2i(-10,-25), Vector2i(-25,-25)]
	first = manager_godot.add_line(in_points, 160, mesh)
	add_child(first)

	in_points = [Vector2i(-40,0), Vector2i(-40,-25), Vector2i(-25,-25)]
	second = manager_godot.add_line(in_points, 160, mesh)
	add_child(second)

	manager_godot.add_splitter(entry, first, second)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$CanvasLayer/Label.text = String.num(1./delta, 2)
	entry.add_to_line(1)

func _input(event):
	if event is InputEventMouseButton and event.is_pressed():
		var camera3d = $Camera3D
		var from = camera3d.project_ray_origin(event.position)
		var dist = from.y/camera3d.project_ray_normal(event.position).y
		var to = from - camera3d.project_ray_normal(event.position) * dist

		var vec = Vector2i(to.x/1.2, to.z/1.2)
		cur_points.push_front(vec)

	if event is InputEventKey and event.physical_keycode == KEY_SPACE and event.is_pressed():
		var line = manager_godot.add_line(cur_points, 160, mesh)
		add_child(line)
		cur_points = []

	if event is InputEventKey and event.physical_keycode == KEY_Q and event.is_pressed():
		first.consume_in_line()
		second.consume_in_line()
