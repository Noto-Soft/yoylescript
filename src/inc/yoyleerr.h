#pragma once

__attribute__((noreturn))
void fatal(const char *msg);
__attribute__((noreturn))
void fatalf(const char *fmt, ...);