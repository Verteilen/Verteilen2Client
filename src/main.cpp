#pragma once
#include <system/system.h>

using namespace Aktion::System;

int main(int argc, char* argv[]) {
  AktionSystem win = AktionSystem();
  ApplyArgument(win, argc, argv);
  InitMainSystem(win);
  RunMainSystem(win);
}
