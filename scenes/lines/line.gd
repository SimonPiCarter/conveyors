extends Node3D

@export var mesh : Mesh = preload("res://modles/conveyor.tres")

func set_up_from_line(line : Array[Vector2i]):
	for child in get_children():
		child.queue_free()

	var old_point = null
	for point in line:
		if old_point != null:
			var minimal = min(point.y, old_point.y)
			var maximal = max(point.y, old_point.y)
			var x_mode = false
			if point.y == old_point.y:
				minimal = min(point.x, old_point.x)
				maximal = max(point.x, old_point.x)
				x_mode = true

			for i in range(minimal, maximal+1):
				var instance = MeshInstance3D.new()
				instance.mesh = mesh
				if x_mode:
					instance.position = Vector3(i*1.2, -0.369, point.y*1.2)
				else:
					instance.position = Vector3(point.x*1.2, -0.369, i*1.2)
					instance.rotation_degrees.y = 90
				add_child(instance)

		old_point = point
