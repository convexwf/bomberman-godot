#ifndef BOMBERMAN_PLAYER_H
#define BOMBERMAN_PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

namespace godot {

class GridManager;

/**
 * Grid-aligned player. Movement snaps to cell center.
 * Requires a GridManager node (set grid_manager_path) to validate movement.
 */
class Player : public CharacterBody2D {
	GDCLASS(Player, CharacterBody2D)

private:
	int grid_x = 0;
	int grid_y = 0;
	double move_speed = 3.0;  // tiles per second
	int bomb_capacity = 1;
	int active_bombs = 0;
	int flame_range = 1;
	bool is_alive = true;
	NodePath grid_manager_path;
	GridManager *grid_manager = nullptr;

	void _update_world_position();

protected:
	static void _bind_methods();

public:
	Player();
	~Player();

	void _ready() override;
	void _physics_process(double delta) override;

	// Grid position (read/write for GDScript)
	void set_grid_x(int x);
	int get_grid_x() const;
	void set_grid_y(int y);
	int get_grid_y() const;
	void set_grid_position(int x, int y);

	// Movement: direction in grid units (-1,0,1). Returns true if moved.
	bool move_direction(int dx, int dy);
	bool can_move_to(int x, int y) const;

	// Bomb (Phase 1: just decrement/increment count; Bomb node created by GDScript)
	bool can_place_bomb() const;
	void place_bomb();
	void on_bomb_exploded();

	// Phase 2: damage/death
	void die();
	bool take_damage();

	// Properties
	void set_move_speed(double p_speed);
	double get_move_speed() const;
	void set_bomb_capacity(int p_cap);
	int get_bomb_capacity() const;
	void set_flame_range(int p_range);
	int get_flame_range() const;
	void set_grid_manager_path(const NodePath &p_path);
	NodePath get_grid_manager_path() const;
	void set_is_alive(bool p_alive);
	bool get_is_alive() const;

	// Signals: grid_position_changed, died (Phase 2)
	// ADD_SIGNAL in .cpp
};

} // namespace godot

#endif // BOMBERMAN_PLAYER_H
