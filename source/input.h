// input.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"

namespace Themepark {

class Input final {
  DISABLE_COPY_AND_MOVE(Input);
public:
  Input();
  ~Input() = default;
  
  void startup(); // TODO: Allow custom key mappings.
  void shutdown();

  bool update(void* window, u32 width, u32 height);

  // TODO: Replace with generic key_pressed/key_was_pressed functions.
  bool space_key_pressed();
  bool space_key_was_pressed();

  bool s_key_pressed();
  bool s_key_was_pressed();

  bool w_key_pressed();
  bool w_key_was_pressed();

  bool move_forward();
  bool move_back();
  bool move_left();
  bool move_right();
  bool move_up();
  bool move_down();
  bool mouse_primary_pressed();
  bool mouse_secondary_pressed();
  bool mouse_tertiary_pressed();
  f32 mouse_delta_x();
  f32 mouse_delta_y();

private:
  bool keyboard_keys_new[512];
  bool keyboard_keys_old[512];
  bool mouse_buttons[24];

  i32 kb_move_forward;
  i32 kb_move_back;
  i32 kb_move_left;
  i32 kb_move_right;
  i32 kb_move_up;
  i32 kb_move_down;

  i32 mouse_primary;
  i32 mouse_secondary;
  i32 mouse_tertiary;

  f32 delta_x;
  f32 delta_y;

  f32 mouse_sensitivity;
};

} // namespace Themepark
