#include <common/wordops.h>
#include <core/loopy_io.h>
#include <log/log.h>

#include <algorithm>
#include <cassert>
#include <cstdio>

namespace LoopyIO
{

struct PadState
{
	bool plugged;
	uint16_t buttons;
};

struct MouseState
{
	bool plugged;
	uint16_t buttons;
	uint16_t counter_x;
	uint16_t counter_y;
	uint16_t noise;
};

struct State
{
	uint16_t latched_sensors;
	uint16_t print_temp;
	PadState pad;
	MouseState mouse;
	bool scan_pad;
	bool scan_mouse;
};

static State state;

void initialize()
{
	state = {};
}

void shutdown()
{
	//nop
}

//Measured on hardware: while a mouse is on the port, the pad matrix bits read
//constantly-changing garbage. A Galois LFSR stands in for it: deterministic, so
//runs are reproducible, but hostile enough that software which trusts the pad
//while a mouse is present misbehaves as it does on the console.
static uint16_t mouse_pad_noise()
{
	if (state.mouse.noise == 0)
	{
		state.mouse.noise = 0xACE1;
	}
	state.mouse.noise = (state.mouse.noise >> 1) ^ (-(state.mouse.noise & 1) & 0xB400);
	return state.mouse.noise;
}

uint8_t reg_read8(uint32_t addr)
{
	READ_HALFWORD(reg, addr);
}

uint16_t reg_read16(uint32_t addr)
{
	addr &= 0xFFE;
	switch (addr)
	{
	case 0x000:
		return state.print_temp;
	case 0x010:
		if (state.scan_pad && state.pad.plugged)
		{
			return ((state.pad.buttons << 4) & 0x0F00) | (state.pad.buttons & 0x000E) | 0x0001;
		}
		else if (state.mouse.plugged)
		{
			//Bit 15 is the mouse presence bit and bits 12-14 mirror the buttons
			//active-low; games check this pattern to detect the mouse. The pad
			//matrix bits read garbage while a mouse is connected.
			uint16_t mb = ((~state.mouse.buttons) & 0x7000) | 0x8000;
			return mb | (mb >> 8) | (mouse_pad_noise() & 0x0F0F);
		}
		return 0;
	case 0x012:
		if (state.scan_pad && state.pad.plugged)
		{
			return (state.pad.buttons >> 8) & 0x000F;
		}
		else if (state.mouse.plugged)
		{
			return mouse_pad_noise() & 0x000F;
		}
		return 0;
	case 0x014:
		return 0;
	case 0x030:
		return state.latched_sensors;
	case 0x050:
		if (state.mouse.plugged)
		{
			//12-bit delta counter, L active-low in bit 12, R active-low in bit 14.
			//Reading clears the counter. Bits 13 and 15 read low on hardware.
			uint16_t mouse_xreg = (state.mouse.counter_x & 0xFFF) | ((~state.mouse.buttons) & 0x5000);
			state.mouse.counter_x = 0;
			return mouse_xreg;
		}
		return 0;
	case 0x052:
		if (state.mouse.plugged)
		{
			//12-bit delta counter, read-to-clear. No buttons mirrored here.
			uint16_t mouse_yreg = state.mouse.counter_y & 0xFFF;
			state.mouse.counter_y = 0;
			return mouse_yreg;
		}
		return 0;
	default:
		Log::warn("[IO] unmapped read16 %08X", addr);
		return 0;
	}
}

uint32_t reg_read32(uint32_t addr)
{
	READ_DOUBLEWORD(reg, addr);
}

void reg_write8(uint32_t addr, uint8_t value)
{
	WRITE_HALFWORD(reg, addr, value);
}

void reg_write16(uint32_t addr, uint16_t value)
{
	addr &= 0xFFE;
	switch (addr)
	{
	case 0x030:
		state.latched_sensors = (state.latched_sensors & ~0x0100) | (value & 0x0100);
		break;
	default:
		Log::warn("[IO] unmapped write16 %08X: %04X", addr, value);
	}
}

void reg_write32(uint32_t addr, uint32_t value)
{
	WRITE_DOUBLEWORD(reg, addr, value);
}

void update_pad(int btn_info, bool pressed)
{
	if (state.pad.plugged)
	{
		if (pressed)
		{
			state.pad.buttons |= btn_info;
		}
		else
		{
			state.pad.buttons &= ~btn_info;
		}
	}
}

void update_mouse_buttons(int btn_info, bool pressed)
{
	if (state.mouse.plugged)
	{
		if (pressed)
		{
			state.mouse.buttons |= btn_info;
		}
		else
		{
			state.mouse.buttons &= ~btn_info;
		}
	}
}

void update_mouse_position(int delta_x, int delta_y)
{
	//Measured on hardware: the delta counters only accumulate under mouse-only
	//scan. With the pad scan bit also set the same motion produces no counts,
	//so software that leaves both scan bits armed sees a frozen cursor.
	if (state.mouse.plugged && state.scan_mouse && !state.scan_pad)
	{
		//The hardware counters are 12-bit and wrap
		state.mouse.counter_x = (state.mouse.counter_x + delta_x) & 0xFFF;
		state.mouse.counter_y = (state.mouse.counter_y + delta_y) & 0xFFF;
	}
}

void set_controller_scan_mode(bool scan_pad, bool scan_mouse)
{
	state.scan_pad = scan_pad;
	state.scan_mouse = scan_mouse;
}

void set_controller_plugged(bool plugged_pad, bool plugged_mouse)
{
	state.pad.plugged = plugged_pad && !plugged_mouse;
	state.mouse.plugged = plugged_mouse;

	if (!plugged_mouse)
	{
		state.mouse.buttons = 0;
		state.mouse.counter_x = 0;
		state.mouse.counter_y = 0;
	}
}

void update_print_temp()
{
	float temp = 22.f;
	int bits_temp = std::clamp((int)(temp * 16), 0, 0x3FF);
	state.print_temp = bits_temp << 6;
}

void update_sensors()
{
	//Stock mainboard is always configured for NTSC
	constexpr int region_jumper = 1;

	//BIOS hooks allow simulated printing of XS-11 type seals
	constexpr bool seal_cartridge_present = true;
	constexpr int seal_cartridge_type = 1;

	//Set sensors for appropriate idle state
	int print_mech_sensors = seal_cartridge_present ? 0b100 : 0b011;

	state.latched_sensors = (state.latched_sensors & 0x0100) | ((seal_cartridge_type & 7) << 4) | ((print_mech_sensors & 7) << 1) | (region_jumper & 1);
}

}  // namespace LoopyIO