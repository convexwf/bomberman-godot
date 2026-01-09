#include "bomb.h"
#include <godot_cpp/core/class_db.hpp>

namespace godot {

void Bomb::_bind_methods() {
	ClassDB::bind_method(D_METHOD("explode"), &Bomb::explode);
	ClassDB::bind_method(D_METHOD("set_grid_x", "x"), &Bomb::set_grid_x);
	ClassDB::bind_method(D_METHOD("get_grid_x"), &Bomb::get_grid_x);
	ClassDB::bind_method(D_METHOD("set_grid_y", "y"), &Bomb::set_grid_y);
	ClassDB::bind_method(D_METHOD("get_grid_y"), &Bomb::get_grid_y);
	ClassDB::bind_method(D_METHOD("set_explosion_time", "time"), &Bomb::set_explosion_time);
	ClassDB::bind_method(D_METHOD("get_explosion_time"), &Bomb::get_explosion_time);
	ClassDB::bind_method(D_METHOD("set_flame_range", "range"), &Bomb::set_flame_range);
	ClassDB::bind_method(D_METHOD("get_flame_range"), &Bomb::get_flame_range);
	ClassDB::bind_method(D_METHOD("set_owner_path", "path"), &Bomb::set_owner_path);
	ClassDB::bind_method(D_METHOD("get_owner_path"), &Bomb::get_owner_path);
	ClassDB::bind_method(D_METHOD("get_has_exploded"), &Bomb::get_has_exploded);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_x"), "set_grid_x", "get_grid_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_y"), "set_grid_y", "get_grid_y");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "explosion_time"), "set_explosion_time", "get_explosion_time");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "flame_range"), "set_flame_range", "get_flame_range");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "owner_path"), "set_owner_path", "get_owner_path");

	ADD_SIGNAL(MethodInfo("exploded", PropertyInfo(Variant::INT, "grid_x"), PropertyInfo(Variant::INT, "grid_y")));
}

Bomb::Bomb() {}

Bomb::~Bomb() {}

void Bomb::_ready() {}

void Bomb::_process(double delta) {
	if (has_exploded) return;
	timer += delta;
	if (timer >= explosion_time) {
		explode();
	}
}

void Bomb::explode() {
	if (has_exploded) return;
	has_exploded = true;
	emit_signal("exploded", grid_x, grid_y);
}

void Bomb::set_grid_x(int x) { grid_x = x; }
int Bomb::get_grid_x() const { return grid_x; }
void Bomb::set_grid_y(int y) { grid_y = y; }
int Bomb::get_grid_y() const { return grid_y; }
void Bomb::set_explosion_time(double p_time) { explosion_time = p_time; }
double Bomb::get_explosion_time() const { return explosion_time; }
void Bomb::set_flame_range(int p_range) { flame_range = p_range; }
int Bomb::get_flame_range() const { return flame_range; }
void Bomb::set_owner_path(const NodePath &p_path) { owner_path = p_path; }
NodePath Bomb::get_owner_path() const { return owner_path; }
bool Bomb::get_has_exploded() const { return has_exploded; }

} // namespace godot
