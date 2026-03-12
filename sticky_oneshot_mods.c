#include "sticky_oneshot_mods.h"

// These store the state of the oneshot mods while an ignored key is being
// pressed.

static uint8_t previous_oneshot_mods = 0;

// Temporarily remove the oneshot mod state if an ignored key is pressed and
// restore it again after the ignored key is released.

bool process_record_sticky_oneshot_mods(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    switch (keycode) {
      case QK_ONE_SHOT_MOD ... QK_ONE_SHOT_MOD_MAX:
        previous_oneshot_mods = get_oneshot_mods();
        clear_oneshot_mods();
        clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
        break;
      default:
        if (is_sticky_oneshot_mods_ignored_key(keycode)) {
          previous_oneshot_mods = get_oneshot_mods();
          clear_oneshot_mods();
        }
    }
  }
  return true;
}

void post_process_record_sticky_oneshot_mods(uint16_t keycode, keyrecord_t *record) {
  if (! record->event.pressed) {
    switch (keycode) {
      case QK_ONE_SHOT_MOD ... QK_ONE_SHOT_MOD_MAX:
        add_oneshot_mods(previous_oneshot_mods);
        previous_oneshot_mods = 0;
        break;
      default:
        if (is_sticky_oneshot_mods_ignored_key(keycode))
          add_oneshot_mods(previous_oneshot_mods);
    }
  }
}

// User overridable function that returns true if oneshot mods should not affect
// the given key and instead remain active for the next key. By default, cursor
// movement keys are ignored.

__attribute__((weak)) bool is_sticky_oneshot_mods_ignored_key(uint16_t keycode) {
  switch (keycode) {

    // Ignore cursor movement keys.

    case KC_C:
    case KC_LEFT:
    case KC_DOWN:
    case KC_UP:
    case KC_RIGHT:
    case KC_HOME:
    case KC_PGDN:
    case KC_PGUP:
    case KC_END:
      return true;
  }

  // Otherwise keep the oneshot mods in place and let them affect this key.

  return false;
}

// User overridable function that returns true if all oneshot mods and the
// static oneshot mod state should be reset. By default the esc key performs a
// reset.

__attribute__((weak)) bool is_sticky_oneshot_mods_reset_key(uint16_t keycode) {

  // Reset the oneshot mods state if a reset key is pressed.

  switch (keycode) {
    case KC_ESC:
      return true;
  }

  // Otherwise keep the oneshot mods in place and let them affect this key.

  return false;
}
