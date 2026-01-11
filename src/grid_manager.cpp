#include "grid_manager.h"
#include <godot_cpp/core/class_db.hpp>

namespace godot {

int GridManager::_tile_index(int x, int y) const {
	return y * grid_width + x;
}

void GridManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_grid_width", "width"), &GridManager::set_grid_width);
	ClassDB::bind_method(D_METHOD("get_grid_width"), &GridManager::get_grid_width);
	ClassDB::bind_method(D_METHOD("set_grid_height", "height"), &GridManager::set_grid_height);
	ClassDB::bind_method(D_METHOD("get_grid_height"), &GridManager::get_grid_height);
	ClassDB::bind_method(D_METHOD("set_tile_size", "size"), &GridManager::set_tile_size);
	ClassDB::bind_method(D_METHOD("get_tile_size"), &GridManager::get_tile_size);
	ClassDB::bind_method(D_METHOD("set_map_offset", "offset"), &GridManager::set_map_offset);
	ClassDB::bind_method(D_METHOD("get_map_offset"), &GridManager::get_map_offset);

	ClassDB::bind_method(D_METHOD("grid_to_world", "x", "y"), &GridManager::grid_to_world);
	ClassDB::bind_method(D_METHOD("world_to_grid", "world_pos"), &GridManager::world_to_grid);

	ClassDB::bind_method(D_METHOD("is_tile_walkable", "x", "y"), &GridManager::is_tile_walkable);
	ClassDB::bind_method(D_METHOD("is_tile_destructible", "x", "y"), &GridManager::is_tile_destructible);
	ClassDB::bind_method(D_METHOD("set_tile", "x", "y", "type"), &GridManager::set_tile);
	ClassDB::bind_method(D_METHOD("get_tile", "x", "y"), &GridManager::get_tile);
	ClassDB::bind_method(D_METHOD("destroy_tile", "x", "y"), &GridManager::destroy_tile);
	ClassDB::bind_method(D_METHOD("load_map_from_string", "map_data"), &GridManager::load_map_from_string);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_width"), "set_grid_width", "get_grid_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_height"), "set_grid_height", "get_grid_height");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "tile_size"), "set_tile_size", "get_tile_size");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "map_offset"), "set_map_offset", "get_map_offset");

	ADD_SIGNAL(MethodInfo("tile_destroyed", PropertyInfo(Variant::INT, "x"), PropertyInfo(Variant::INT, "y")));

	// Bind enum as integer constants (godot-cpp has no GetTypeInfo for custom enums)
	ClassDB::bind_integer_constant(get_class_static(), "TileType", "TILE_FLOOR", TILE_FLOOR);
	ClassDB::bind_integer_constant(get_class_static(), "TileType", "TILE_WALL", TILE_WALL);
	ClassDB::bind_integer_constant(get_class_static(), "TileType", "TILE_DESTRUCTIBLE", TILE_DESTRUCTIBLE);
}

GridManager::GridManager() {
	tiles.resize((size_t)(grid_width * grid_height), TILE_FLOOR);
}

GridManager::~GridManager() {}

void GridManager::set_grid_width(int p_width) {
	if (p_width <= 0) return;
	grid_width = p_width;
	tiles.resize((size_t)(grid_width * grid_height), TILE_FLOOR);
}

int GridManager::get_grid_width() const {
	return grid_width;
}

void GridManager::set_grid_height(int p_height) {
	if (p_height <= 0) return;
	grid_height = p_height;
	tiles.resize((size_t)(grid_width * grid_height), TILE_FLOOR);
}

int GridManager::get_grid_height() const {
	return grid_height;
}

void GridManager::set_tile_size(int p_size) {
	if (p_size <= 0) return;
	tile_size = p_size;
}

int GridManager::get_tile_size() const {
	return tile_size;
}

void GridManager::set_map_offset(const Vector2 &p_offset) {
	map_offset = p_offset;
}

Vector2 GridManager::get_map_offset() const {
	return map_offset;
}

Vector2 GridManager::grid_to_world(int x, int y) const {
	// Center of cell: (x + 0.5) * tile_size + offset
	return Vector2(
			(x + 0.5f) * tile_size + map_offset.x,
			(y + 0.5f) * tile_size + map_offset.y
	);
}

Vector2i GridManager::world_to_grid(const Vector2 &world_pos) const {
	Vector2 rel = world_pos - map_offset;
	int gx = (int)floor(rel.x / tile_size);
	int gy = (int)floor(rel.y / tile_size);
	return Vector2i(gx, gy);
}

bool GridManager::is_tile_walkable(int x, int y) const {
	if (x < 0 || x >= grid_width || y < 0 || y >= grid_height) return false;
	return get_tile(x, y) == TILE_FLOOR;
}

bool GridManager::is_tile_destructible(int x, int y) const {
	if (x < 0 || x >= grid_width || y < 0 || y >= grid_height) return false;
	return get_tile(x, y) == TILE_DESTRUCTIBLE;
}

void GridManager::set_tile(int x, int y, int p_type) {
	if (x < 0 || x >= grid_width || y < 0 || y >= grid_height) return;
	tiles[(size_t)_tile_index(x, y)] = p_type;
}

int GridManager::get_tile(int x, int y) const {
	if (x < 0 || x >= grid_width || y < 0 || y >= grid_height) return TILE_WALL;
	return tiles[(size_t)_tile_index(x, y)];
}

void GridManager::destroy_tile(int x, int y) {
	if (is_tile_destructible(x, y)) {
		set_tile(x, y, TILE_FLOOR);
		emit_signal("tile_destroyed", x, y);
	}
}

void GridManager::load_map_from_string(const String &p_map_data) {
	PackedStringArray lines = p_map_data.split("\n", false);
	int row = 0;
	for (int i = 0; i < lines.size(); i++) {
		String line = lines[i].strip_edges();
		if (line.is_empty()) continue;
		if (row >= grid_height) break;
		for (int col = 0; col < line.length() && col < grid_width; col++) {
			char32_t c = line[col];
			if (c == '#') set_tile(col, row, TILE_WALL);
			else if (c == 'x' || c == 'X') set_tile(col, row, TILE_DESTRUCTIBLE);
			else set_tile(col, row, TILE_FLOOR);
		}
		row++;
	}
}

} // namespace godot
