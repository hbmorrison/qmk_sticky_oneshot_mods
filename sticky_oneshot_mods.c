#include "sticky_oneshot_mods.h"

// These store the state of the oneshot mods while an ignored key is being
// pressed.

static uint8_t oneshot_mod_state = 0;

// Temporarily remove the oneshot mod state if an ignored key is pressed and
// restore it again after the ignored key is released.

bool process_record_sticky_oneshot_mods(uint16_t keycode, keyrecord_t *record) {

  // If a reset key is pressed, clear the oneshot mod state and any active
  // oneshot mods, and return false to indicate that the key press has been
  // used up.

  if (is_sticky_oneshot_mods_reset_key(keycode) && oneshot_mod_state) {
    clear_oneshot_mods();
    return false;
  }

  // Work out whether to save or restore the oneshot mod state.

  if (record->event.pressed) {
    switch (keycode) {

      // If a oneshot mod key has been pressed, add its modifiers to the saved
      // oneshot mod state.

      case QK_ONE_SHOT_MOD ... QK_ONE_SHOT_MOD_MAX:
        oneshot_mod_state = get_oneshot_mods() | (keycode & 0x1F);
        break;

      // If a layer key has been pressed, save the oneshot mod state.

      case QK_LAYER_MOD ... QK_LAYER_MOD_MAX:
        oneshot_mod_state = get_oneshot_mods();
        clear_oneshot_mods();
        break;

      // If the key should be ignored, save the oneshot mod state.

      default:
        if (is_sticky_oneshot_mods_ignored_key(keycode)) {
          oneshot_mod_state = get_oneshot_mods();
          clear_oneshot_mods();
        }
    }
  } else {
    switch (keycode) {

      // Restore the full oneshot mod state when a oneshot mod key is released.

      case QK_ONE_SHOT_MOD ... QK_ONE_SHOT_MOD_MAX:
        set_oneshot_mods(oneshot_mod_state);
        break;

      // Restore the full oneshot mod state when a layer key is released.

      case QK_LAYER_MOD ... QK_LAYER_MOD_MAX:
        set_oneshot_mods(oneshot_mod_state);
        break;

      // Restore the full oneshot mod state when an ignored key is released.

      default:
        if (is_sticky_oneshot_mods_ignored_key(keycode))
          set_oneshot_mods(oneshot_mod_state);
    }
  }
  return true;
}

// User overridable function that returns true if oneshot mods should not affect
// the given key and stay active for the next one. By default, cursor movement
// keys are ignored.

__attribute__((weak)) bool is_sticky_oneshot_mods_ignored_key(uint16_t keycode) {
  switch (keycode) {

    // Ignore cursor movement keys.

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
  switch (keycode) {

    // Reset the oneshot mods state if esc is pressed.

    case KC_ESC:
      return true;
  }

  // Otherwise keep the oneshot mods in place and let them affect this key.

  return false;
}
