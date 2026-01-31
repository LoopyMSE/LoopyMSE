#pragma once

#include <core/config.h>

#include <cstdint>

namespace Printer
{

void initialize(Config::SystemInfo& config);
void shutdown();

bool motor_move_hook(uint32_t addr);
bool printer_hook(uint32_t addr);

}  // namespace Printer