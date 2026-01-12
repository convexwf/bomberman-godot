#include "player.h"
#include "grid_manager.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace godot {

void Player::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_grid_x", "x"), &Player::set_grid_x);
	ClassDB::bind_method(D_METHOD("get_grid_x"), &Player::get_grid_x);
	ClassDB::bind_method(D_METHOD("set_grid_y", "y"), &Player::set_grid_y);
	ClassDB::bind_method(D_METHOD("get_grid_y"), &Player::get_grid_y);
	ClassDB::bind_method(D_METHOD("set_grid_position", "x", "y"), &Player::set_grid_position);
	ClassDB::bind_method(D_METHOD("move_direction", "dx", "dy"), &Player::move_direction);
	ClassDB::bind_method(D_METHOD("can_move_to", "x", "y"), &Player::can_move_to);
	ClassDB::bind_method(D_METHOD("can_place_bomb"), &Player::can_place_bomb);
	ClassDB::bind_method(D_METHOD("place_bomb"), &Player::place_bomb);
	ClassDB::bind_method(D_METHOD("on_bomb_exploded"), &Player::on_bomb_exploded);
	ClassDB::bind_method(D_METHOD("die"), &Player::die);
	ClassDB::bind_method(D_METHOD("take_damage"), &Player::take_damage);
	ClassDB::bind_method(D_METHOD("set_move_speed", "speed"), &Player::set_move_speed);
	ClassDB::bind_method(D_METHOD("get_move_speed"), &Player::get_move_speed);
	ClassDB::bind_method(D_METHOD("set_bomb_capacity", "cap"), &Player::set_bomb_capacity);
	ClassDB::bind_method(D_METHOD("get_bomb_capacity"), &Player::get_bomb_capacity);
	ClassDB::bind_method(D_METHOD("get_active_bombs"), &Player::get_active_bombs);
	ClassDB::bind_method(D_METHOD("set_flame_range", "range"), &Player::set_flame_range);
	ClassDB::bind_method(D_METHOD("get_flame_range"), &Player::get_flame_range);
	ClassDB::bind_method(D_METHOD("set_grid_manager_path", "path"), &Player::set_grid_manager_path);
	ClassDB::bind_method(D_METHOD("get_grid_manager_path"), &Player::get_grid_manager_path);
	ClassDB::bind_method(D_METHOD("set_is_alive", "alive"), &Player::set_is_alive);
	ClassDB::bind_method(D_METHOD("get_is_alive"), &Player::get_is_alive);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_x"), "set_grid_x", "get_grid_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_y"), "set_grid_y", "get_grid_y");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "move_speed"), "set_move_speed", "get_move_speed");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bomb_capacity"), "set_bomb_capacity", "get_bomb_capacity");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "flame_range"), "set_flame_range", "get_flame_range");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "grid_manager_path"), "set_grid_manager_path", "get_grid_manager_path");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_alive"), "set_is_alive", "get_is_alive");

	ADD_SIGNAL(MethodInfo("grid_position_changed", PropertyInfo(Variant::VECTOR2I, "grid_pos")));
	ADD_SIGNAL(MethodInfo("died"));
}

Player::Player() {}

Player::~Player() {}

void Player::_ready() {
	if (!grid_manager_path.is_empty()) {
		grid_manager = get_node<GridManager>(grid_manager_path);
	}
	_update_world_position();
}

void Player::_physics_process(double delta) {
	// Phase 1: movement is driven by GDScript calling move_direction each frame
	// Optional: could add automatic interpolation here later
}

void Player::_update_world_position() {
	if (grid_manager) {
		Vector2 world = grid_manager->grid_to_world(grid_x, grid_y);
		set_position(world);
	}
}

void Player::set_grid_x(int x) { grid_x = x; }
int Player::get_grid_x() const { return grid_x; }
void Player::set_grid_y(int y) { grid_y = y; }
int Player::get_grid_y() const { return grid_y; }

void Player::set_grid_position(int x, int y) {
	grid_x = x;
	grid_y = y;
	_update_world_position();
	emit_signal("grid_position_changed", Vector2i(grid_x, grid_y));
}

bool Player::move_direction(int dx, int dy) {
	if (!grid_manager || !is_alive) return false;
	int nx = grid_x + dx;
	int ny = grid_y + dy;
	if (!can_move_to(nx, ny)) return false;
	grid_x = nx;
	grid_y = ny;
	_update_world_position();
	emit_signal("grid_position_changed", Vector2i(grid_x, grid_y));
	return true;
}

bool Player::can_move_to(int x, int y) const {
	if (!grid_manager) return false;
	return grid_manager->is_tile_walkable(x, y);
}

bool Player::can_place_bomb() const {
	return is_alive && active_bombs < bomb_capacity;
}

void Player::place_bomb() {
	if (!can_place_bomb()) return;
	active_bombs++;
}

void Player::on_bomb_exploded() {
	if (active_bombs > 0) active_bombs--;
}

void Player::die() {
	if (!is_alive) return;
	is_alive = false;
	emit_signal("died");
}

bool Player::take_damage() {
	if (!is_alive) return false;
	die();
	return true;
}

void Player::set_move_speed(double p_speed) { move_speed = p_speed; }
double Player::get_move_speed() const { return move_speed; }
void Player::set_bomb_capacity(int p_cap) { bomb_capacity = p_cap; }
int Player::get_bomb_capacity() const { return bomb_capacity; }
int Player::get_active_bombs() const { return active_bombs; }
void Player::set_flame_range(int p_range) { flame_range = p_range; }
int Player::get_flame_range() const { return flame_range; }
void Player::set_grid_manager_path(const NodePath &p_path) { grid_manager_path = p_path; }
NodePath Player::get_grid_manager_path() const { return grid_manager_path; }
void Player::set_is_alive(bool p_alive) { is_alive = p_alive; }
bool Player::get_is_alive() const { return is_alive; }

} // namespace godot
