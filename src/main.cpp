#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  // It's possible to limit the number of output processes by adding the second interger argument
  NCursesDisplay::Display(system);
}