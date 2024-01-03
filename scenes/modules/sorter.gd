extends Node3D

var idx = -1

signal clicked(idx)
signal wheeled(idx, up)
@onready var animation_player = $AnimationPlayer

func _on_static_body_3d_input_event(_camera, event, _position, _normal, _shape_idx):
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.is_pressed():
		clicked.emit(idx)
		animation_player.play("pop")
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_WHEEL_DOWN and event.is_pressed():
		wheeled.emit(idx, false)
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_WHEEL_UP and event.is_pressed():
		wheeled.emit(idx, true)
