extends Node2D
## Phase 1+2+3: Input, bomb, explosion, tile destruction, player death, power-up; TileMap, HUD, Game Over.

@onready var grid_manager: GridManager = $GridManager
@onready var map_tile_map: TileMap = $MapTileMap
@onready var player: Player = $Player
@onready var bombs_container: Node2D = $BombsContainer
@onready var bombs_label: Label = $HUD/BombsLabel
@onready var flame_label: Label = $HUD/FlameLabel
@onready var game_over_layer: CanvasLayer = $GameOver
@onready var restart_button: Button = $GameOver/Panel/RestartButton

var bomb_scene: PackedScene
var power_ups_container: Node2D

const MAP_LAYER := 0
const TILE_SOURCE_ID := 0

func _ready() -> void:
	if not grid_manager or not player:
		push_error("GridManager or Player not found")
		return
	player.grid_manager_path = grid_manager.get_path()
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
	_setup_map_tileset()
	_refresh_map_from_grid()
	map_tile_map.position = grid_manager.position
	player.set_grid_position(1, 1)
	player.grid_position_changed.connect(_on_player_grid_position_changed)
	player.died.connect(_on_player_died)
	grid_manager.tile_destroyed.connect(_on_tile_destroyed)
	if has_node("PowerUpsContainer"):
		power_ups_container = $PowerUpsContainer
	else:
		power_ups_container = bombs_container
	if ResourceLoader.exists("res://scenes/bomb.tscn"):
		bomb_scene = load("res://scenes/bomb.tscn") as PackedScene
	game_over_layer.visible = false
	restart_button.pressed.connect(_on_restart_pressed)
	_update_hud()

func _process(_delta: float) -> void:
	_update_hud()

func _setup_map_tileset() -> void:
	var img := Image.create(96, 32, false, Image.FORMAT_RGBA8)
	img.fill_rect(Rect2i(0, 0, 32, 32), Color(0.7, 0.7, 0.7))
	img.fill_rect(Rect2i(32, 0, 32, 32), Color(0.25, 0.25, 0.25))
	img.fill_rect(Rect2i(64, 0, 32, 32), Color(0.55, 0.27, 0.07))
	var tex := ImageTexture.create_from_image(img)
	var atlas := TileSetAtlasSource.new()
	atlas.texture = tex
	atlas.texture_region_size = Vector2i(32, 32)
	atlas.create_tile(Vector2i(0, 0))
	atlas.create_tile(Vector2i(1, 0))
	atlas.create_tile(Vector2i(2, 0))
	var ts := TileSet.new()
	ts.add_source(atlas, TILE_SOURCE_ID)
	map_tile_map.tile_set = ts

func _refresh_map_from_grid() -> void:
	map_tile_map.clear()
	var w := grid_manager.get_grid_width()
	var h := grid_manager.get_grid_height()
	for y in h:
		for x in w:
			var t := grid_manager.get_tile(x, y)
			map_tile_map.set_cell(MAP_LAYER, Vector2i(x, y), TILE_SOURCE_ID, Vector2i(t, 0))

func _set_tile_visual(x: int, y: int, tile_type: int) -> void:
	map_tile_map.set_cell(MAP_LAYER, Vector2i(x, y), TILE_SOURCE_ID, Vector2i(tile_type, 0))

func _update_hud() -> void:
	if bombs_label:
		bombs_label.text = "Bombs: %d/%d" % [player.get_bomb_capacity() - player.get_active_bombs(), player.get_bomb_capacity()]
	if flame_label:
		flame_label.text = "Flame: %d" % player.get_flame_range()

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
		var gx := player.get_grid_x()
		var gy := player.get_grid_y()
		bomb.set_grid_position(gx, gy)
		bomb.position = grid_manager.grid_to_world(gx, gy)
		bomb.set_flame_range(player.get_flame_range())
		bomb.set_owner_path(player.get_path())
		bomb.set_grid_manager_path(grid_manager.get_path())
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

func _on_bomb_exploded(_gx: int, _gy: int, tiles: Array, bomb: Bomb) -> void:
	var player_cell := Vector2i(player.get_grid_x(), player.get_grid_y())
	for i in range(tiles.size()):
		if tiles[i] == player_cell and player.get_is_alive():
			player.die()
			break
	player.on_bomb_exploded()
	if bomb and is_instance_valid(bomb):
		bomb.queue_free()

func _on_player_died() -> void:
	print("[Phase 2] player died")
	game_over_layer.visible = true

func _on_tile_destroyed(x: int, y: int) -> void:
	_set_tile_visual(x, y, GridManager.TILE_FLOOR)
	if not power_ups_container:
		return
	if randf() > 0.4:
		return
	var pu := PowerUp.new()
	pu.set_grid_position(x, y)
	pu.power_up_type = randi() % 3
	var world := grid_manager.grid_to_world(x, y)
	pu.position = world
	var shape := CircleShape2D.new()
	shape.radius = 10.0
	var col := CollisionShape2D.new()
	col.shape = shape
	pu.add_child(col)
	pu.collected.connect(_on_power_up_collected)
	power_ups_container.add_child(pu)

func _on_power_up_collected(p: Player) -> void:
	if p == player:
		if player.get_flame_range() < 5:
			player.set_flame_range(player.get_flame_range() + 1)
		elif player.get_bomb_capacity() < 5:
			player.set_bomb_capacity(player.get_bomb_capacity() + 1)

func _on_restart_pressed() -> void:
	get_tree().reload_current_scene()
