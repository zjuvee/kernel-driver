#pragma once

// mandar mensajes desde el kernel, usar debugview o windbg
#define DebugMessage(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)