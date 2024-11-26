// input.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "input.h"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>

namespace Themepark {

Input::Input()
: kb_move_forward(SDL_SCANCODE_UP)
, kb_move_back(SDL_SCANCODE_DOWN)
, kb_move_left(SDL_SCANCODE_LEFT)
, kb_move_right(SDL_SCANCODE_RIGHT)
, kb_move_up(SDL_SCANCODE_PAGEUP)
, kb_move_down(SDL_SCANCODE_PAGEDOWN)
, mouse_primary(0)
, mouse_secondary(1)
, mouse_tertiary(2)
, delta_x(0.0F)
, delta_y(0.0F)
, mouse_sensitivity(0.5F) {
    
  memset(keyboard_keys, 0, sizeof(keyboard_keys));
  memset(mouse_buttons, 0, sizeof(mouse_buttons));
}

void Input::startup() {
  //TODO:
}

void Input::shutdown() {
  //TODO:
}

bool Input::update(void* window, u32 width, u32 height) {
  i32 key_count = 0;
  const auto key_state = SDL_GetKeyboardState(&key_count);

  if (key_state[SDL_SCANCODE_ESCAPE]) { // TODO:
    return true; // User quit.
  }

  for (int k = 0; k < key_count; ++k) {
    keyboard_keys[k] = (key_state[k] != 0) ? true : false;
  }

  f32 x, y;
  const auto button_state = SDL_GetRelativeMouseState(&x, &y);

  if (button_state & SDL_BUTTON_MASK(1)) {
    mouse_buttons[0] = true;
  } else {
    mouse_buttons[0] = false;
  }

  if (button_state & SDL_BUTTON_MASK(2)) {
    mouse_buttons[1] = true;
  } else {
    mouse_buttons[1] = false;
  }

  if (button_state & SDL_BUTTON_MASK(3)) {
    mouse_buttons[2] = true;
  } else {
    mouse_buttons[2] = false;
  }

  if (mouse_buttons[mouse_secondary]) {
    delta_x = x;
    delta_y = y;

    SDL_WarpMouseInWindow((SDL_Window*)window, width >> 1, height >> 1);
    SDL_HideCursor();
  } else {
    delta_x = 0.0F;
    delta_y = 0.0F;
    SDL_ShowCursor();
  }

  return false;
}

bool Input::move_forward() {
  return keyboard_keys[kb_move_forward];
}

bool Input::move_back() {
  return keyboard_keys[kb_move_back];
}

bool Input::move_left() {
  return keyboard_keys[kb_move_left];
}

bool Input::move_right() {
  return keyboard_keys[kb_move_right];
}

bool Input::move_up() {
  return keyboard_keys[kb_move_up];
}

bool Input::move_down() {
  return keyboard_keys[kb_move_down];
}

bool Input::mouse_primary_pressed() {
  return mouse_buttons[mouse_primary];
}

bool Input::mouse_secondary_pressed() {
  return mouse_buttons[mouse_secondary];
}

bool Input::mouse_tertiary_pressed() {
  return mouse_buttons[mouse_tertiary];
}

f32 Input::mouse_delta_x() {
  return delta_x * mouse_sensitivity;
}

f32 Input::mouse_delta_y() {
  return delta_y * mouse_sensitivity;
}

} // namespace Themepark
