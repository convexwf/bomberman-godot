#include "power_up.h"
#include "player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace godot {

void PowerUp::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_type", "type"), &PowerUp::set_type);
	ClassDB::bind_method(D_METHOD("get_type"), &PowerUp::get_type);
	ClassDB::bind_method(D_METHOD("set_grid_x", "x"), &PowerUp::set_grid_x);
	ClassDB::bind_method(D_METHOD("get_grid_x"), &PowerUp::get_grid_x);
	ClassDB::bind_method(D_METHOD("set_grid_y", "y"), &PowerUp::set_grid_y);
	ClassDB::bind_method(D_METHOD("get_grid_y"), &PowerUp::get_grid_y);
	ClassDB::bind_method(D_METHOD("set_grid_position", "x", "y"), &PowerUp::set_grid_position);
	ClassDB::bind_method(D_METHOD("_on_body_entered", "body"), &PowerUp::_on_body_entered);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "power_up_type"), "set_type", "get_type");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_x"), "set_grid_x", "get_grid_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "grid_y"), "set_grid_y", "get_grid_y");

	ADD_SIGNAL(MethodInfo("collected", PropertyInfo(Variant::OBJECT, "player", PROPERTY_HINT_NODE_TYPE, "Player")));

	ClassDB::bind_integer_constant(get_class_static(), "PowerUpType", "TYPE_FLAME_UP", TYPE_FLAME_UP);
	ClassDB::bind_integer_constant(get_class_static(), "PowerUpType", "TYPE_BOMB_UP", TYPE_BOMB_UP);
	ClassDB::bind_integer_constant(get_class_static(), "PowerUpType", "TYPE_SPEED_UP", TYPE_SPEED_UP);
	ClassDB::bind_integer_constant(get_class_static(), "PowerUpType", "TYPE_KICK", TYPE_KICK);
	ClassDB::bind_integer_constant(get_class_static(), "PowerUpType", "TYPE_REMOTE_DETONATOR", TYPE_REMOTE_DETONATOR);
}

PowerUp::PowerUp() {}

PowerUp::~PowerUp() {}

void PowerUp::_ready() {
	connect("body_entered", Callable(this, "_on_body_entered"));
}

void PowerUp::_on_body_entered(const Variant &body_v) {
	if (body_v.get_type() != Variant::OBJECT) return;
	Object *obj = body_v.operator Object *();
	if (!obj) return;
	Player *player = Object::cast_to<Player>(obj);
	if (player && player->get_is_alive()) {
		emit_signal("collected", player);
		queue_free();
	}
}

void PowerUp::set_type(int p_type) { type = p_type; }
int PowerUp::get_type() const { return type; }
void PowerUp::set_grid_x(int x) { grid_x = x; }
int PowerUp::get_grid_x() const { return grid_x; }
void PowerUp::set_grid_y(int y) { grid_y = y; }
int PowerUp::get_grid_y() const { return grid_y; }
void PowerUp::set_grid_position(int x, int y) { grid_x = x; grid_y = y; }

} // namespace godot
