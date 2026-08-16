#include "input/input.h"

#include <core/loopy_io.h>

#include <unordered_map>

namespace Input
{

static std::unordered_map<int, PadButton> key_bindings;
static std::unordered_map<int, PadButton> controller_bindings;
static std::unordered_map<int, MouseButton> mouse_bindings;

//Whether the emulated mouse is plugged in. Lives here so it survives System
//re-initialization (reboot, cart load).
static bool mouse_plugged;

void initialize()
{
	//Indicate the gamepad (or mouse, when plugged) is connected
	LoopyIO::set_controller_plugged(true, mouse_plugged);
}

void shutdown()
{
	//nop
}

void set_controller_state(int button, bool pressed)
{
	auto binding = controller_bindings.find(button);
	if (binding == controller_bindings.end())
	{
		return;
	}

	PadButton pad_button = binding->second;
	LoopyIO::update_pad(pad_button, pressed);
}

void set_key_state(int key, bool pressed)
{
	auto binding = key_bindings.find(key);
	if (binding == key_bindings.end())
	{
		return;
	}

	PadButton pad_button = binding->second;
	LoopyIO::update_pad(pad_button, pressed);
}

void set_mouse_button_state(int button, bool pressed)
{
	auto binding = mouse_bindings.find(button);
	if (binding == mouse_bindings.end())
	{
		return;
	}

	MouseButton mouse_button = binding->second;
	LoopyIO::update_mouse_buttons(mouse_button, pressed);
}

void move_mouse(int delta_x, int delta_y)
{
	LoopyIO::update_mouse_position(delta_x, delta_y);
}

void set_mouse_plugged(bool plugged)
{
	mouse_plugged = plugged;
	LoopyIO::set_controller_plugged(true, plugged);
}

bool is_mouse_plugged()
{
	return mouse_plugged;
}

void add_key_binding(int code, PadButton pad_button)
{
	key_bindings.emplace(code, pad_button);
}

void add_controller_binding(int code, PadButton pad_button)
{
	controller_bindings.emplace(code, pad_button);
}

void add_mouse_binding(int code, MouseButton mouse_button)
{
	mouse_bindings.emplace(code, mouse_button);
}

}  // namespace Input