extends Node2D
## Phase 1: Input handling and bomb placement.
## GridManager and Player are GDExtension C++ nodes.

@onready var grid_manager: GridManager = $GridManager
@onready var player: Player = $Player
@onready var bombs_container: Node2D = $BombsContainer

var bomb_scene: PackedScene

func _ready() -> void:
	if not grid_manager or not player:
		push_error("GridManager or Player not found")
		return
	player.grid_manager_path = grid_manager.get_path()
	# Load default map: walls around, floor inside, some destructible
	var map_data := """
		###################
		#.................#
		#.xxx.xxx.xxx.xxx.#
		#.................#
		#.xxx.xxx.xxx.xxx.#
		#.................#
		#.xxx.xxx.xxx.xxx.#
		#.................#
		###################
		"""
	grid_manager.load_map_from_string(map_data)
	player.set_grid_position(1, 1)
	# Phase 1 verification: confirm C++ signal is emitted
	player.grid_position_changed.connect(_on_player_grid_position_changed)
	# Try to load Bomb scene if exists (optional)
	if ResourceLoader.exists("res://scenes/bomb.tscn"):
		bomb_scene = load("res://scenes/bomb.tscn") as PackedScene

func _input(event: InputEvent) -> void:
	if not player or not player.get_is_alive():
		return
	if event.is_action_pressed("ui_up"):
		player.move_direction(0, -1)
	elif event.is_action_pressed("ui_down"):
		player.move_direction(0, 1)
	elif event.is_action_pressed("ui_left"):
		player.move_direction(-1, 0)
	elif event.is_action_pressed("ui_right"):
		player.move_direction(1, 0)
	elif event.is_action_pressed("ui_accept") or event.is_action_pressed("ui_focus_next"):
		_try_place_bomb()

func _try_place_bomb() -> void:
	if not player.can_place_bomb():
		return
	if bomb_scene and bombs_container:
		var bomb: Bomb = bomb_scene.instantiate() as Bomb
		bomb.set_grid_position(player.get_grid_x(), player.get_grid_y())
		bomb.set_flame_range(player.get_flame_range())
		bomb.set_owner_path(player.get_path())
		bomb.exploded.connect(_on_bomb_exploded.bind(bomb))
		bombs_container.add_child(bomb)
		player.place_bomb()
	else:
		# No bomb scene: still consume capacity for testing
		player.place_bomb()
		# Simulate explosion after 2s (call on_bomb_exploded)
		get_tree().create_timer(2.0).timeout.connect(player.on_bomb_exploded)

func _on_player_grid_position_changed(grid_pos: Vector2i) -> void:
	print("[Phase 1] grid_position_changed: ", grid_pos)

func _on_bomb_exploded(_gx: int, _gy: int, bomb: Bomb) -> void:
	player.on_bomb_exploded()
	if bomb and is_instance_valid(bomb):
		bomb.queue_free()
