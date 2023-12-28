extends Node3D

var lines = []
var mesh = preload("res://modles/box-small.tres")

func set_up_line(list_points, speed):
	var old_point = null
	var length = 0
	for point in list_points:
		if old_point != null:
			if old_point.x != point.x and old_point.y != point.y:
				push_error("Cannot create line with diagonals")
				return
			length += abs(old_point.x - point.x) + abs(old_point.y - point.y)
		old_point = point

	var new_line = LineGodot.new()
	new_line.set_mesh(mesh)
	print(2*length, " ",speed)
	new_line.set_up_line(2*length, speed)

	old_point = null
	var cur_length = 0
	for point in list_points:
		if old_point != null:
			length = abs(old_point.x - point.x) + abs(old_point.y - point.y)
			cur_length += 2 * length * 100.
			new_line.add_segment_to_line(
				Vector3(old_point.x*1.2,0,old_point.y*1.2),
				Vector3(point.x*1.2,0,point.y*1.2),
				cur_length)
			print(Vector3(old_point.x*1.2,0,old_point.y*1.2), Vector3(point.x*1.2,0,point.y*1.2))
		old_point = point

	add_child(new_line)
	lines.push_back(new_line)


# Called when the node enters the scene tree for the first time.
func _ready():
	var points : Array[Vector2i] = [Vector2i(0,0), Vector2i(25,0), Vector2i(25,-25)]
	set_up_line(points, 160)
	points = [Vector2i(-25,-25), Vector2i(-25,0), Vector2i(0,0), Vector2i(0,-25)]
	set_up_line(points, 160)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$CanvasLayer/Label.text = String.num(1./delta, 2)
	for line_ in lines:
		line_.add_to_line(0)
