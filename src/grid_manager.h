#ifndef BOMBERMAN_GRID_MANAGER_H
#define BOMBERMAN_GRID_MANAGER_H

#include <godot_cpp/classes/node2d.hpp>
#include <vector>

namespace godot {

/**
 * Manages grid-based map state and coordinate conversion.
 * Uses center-aligned cells: grid_to_world returns the center of each cell.
 */
class GridManager : public Node2D {
	GDCLASS(GridManager, Node2D)

public:
	enum TileType {
		TILE_FLOOR = 0,
		TILE_WALL = 1,
		TILE_DESTRUCTIBLE = 2,
	};

private:
	int grid_width = 15;
	int grid_height = 13;
	int tile_size = 32;
	Vector2 map_offset;
	std::vector<int> tiles;

	int _tile_index(int x, int y) const;

protected:
	static void _bind_methods();

public:
	GridManager();
	~GridManager();

	// Grid dimensions and conversion (center-aligned)
	void set_grid_width(int p_width);
	int get_grid_width() const;
	void set_grid_height(int p_height);
	int get_grid_height() const;
	void set_tile_size(int p_size);
	int get_tile_size() const;
	void set_map_offset(const Vector2 &p_offset);
	Vector2 get_map_offset() const;

	/** Returns world position of the center of cell (x, y). */
	Vector2 grid_to_world(int x, int y) const;
	/** Returns grid cell containing world_pos (cell center convention). */
	Vector2i world_to_grid(const Vector2 &world_pos) const;

	// Tile access
	bool is_tile_walkable(int x, int y) const;
	bool is_tile_destructible(int x, int y) const;
	void set_tile(int x, int y, int p_type);
	int get_tile(int x, int y) const;
	void destroy_tile(int x, int y);

	/** Load map from string: . = floor, # = wall, x = destructible. Lines are rows. */
	void load_map_from_string(const String &p_map_data);

	// Signal: emitted when a destructible tile is destroyed (for GDScript to update TileMap / spawn power-up)
	// ADD_SIGNAL in .cpp
};

} // namespace godot

#endif // BOMBERMAN_GRID_MANAGER_H
