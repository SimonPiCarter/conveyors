extends Node3D

@onready var line = $LineGodot

# Called when the node enters the scene tree for the first time.
func _ready():
	line.set_up_line(100, 80)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$CanvasLayer/Label.text = String.num(1./delta, 2)
	line.add_to_line(0)
