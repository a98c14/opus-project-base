#include "input.h"

internal bool32
input_key_pressed_raw(Window* window, uint16 key)
{
    return glfwGetKey(window->glfw_window, key) == GLFW_PRESS;
}

internal bool32
input_mouse_pressed_raw(Window* window, uint16 key)
{
    return glfwGetMouseButton(window->glfw_window, key) == GLFW_PRESS;
}

internal void
input_manager_init(Arena* arena, Window* window)
{
    g_input_manager         = arena_push_struct_zero(arena, InputManager);
    g_input_manager->window = window;
}

internal void
input_manager_register_action(String action_name, uint64 action_id, uint16 key)
{
    g_input_manager->key_count                 = max(g_input_manager->key_count, action_id + 1);
    g_input_manager->key_states[action_id].key = key;
    g_input_manager->key_names[action_id]      = action_name;
}

internal void
input_manager_update(EngineTime time)
{
    /** TODO(selim): use polling instead of looping every available action every frame */
    for (uint32 i = 0; i < g_input_manager->key_count; i++)
    {
        InputState* state = &g_input_manager->key_states[i];
        if (state->key == 0)
            continue;

        InputKeyState new_state = input_key_pressed_raw(g_input_manager->window, state->key);
        input_set_key_state(time, i, new_state);
    }
}

internal void
input_set_key_state(EngineTime time, uint64 action_id, InputKeyState key_state)
{
    InputState* state  = &g_input_manager->key_states[action_id];
    bool32      is_new = (state->key_state & key_state) == 0;
    state->key_state   = key_state;
    state->key_state   = is_new ? (state->key_state | InputKeyStateNew) : (state->key_state & ~InputKeyStateNew);

    if (key_state == InputKeyStatePressed && is_new)
    {
        state->t_press = time.current_frame_time;
    }
    else if (key_state == InputKeyStateReleased && is_new)
    {
        state->t_release = time.current_frame_time;
    }
}

internal bool32
input_action_pressed(uint64 action_id)
{
    return flag_is_set(g_input_manager->key_states[action_id].key_state, InputKeyStatePressedNew);
}

internal Vec2
input_mouse_world_position(Vec2 raw_mouse_pos, Camera camera)
{
    Vec2 mouse_world = vec2(-raw_mouse_pos.x / camera.window_width, raw_mouse_pos.y / camera.window_height);
    mouse_world      = sub_vec2(vec2(0, 1), mouse_world);
    mouse_world      = add_vec2_f32(mouse_world, -0.5);
    mouse_world.x *= camera.world_width;
    mouse_world.y *= camera.world_height;

    mouse_world = mul_mat4_vec4(camera.inverse_view, vec4(mouse_world.x, mouse_world.y, 0, 1)).xy;
    return mouse_world;
}

internal Vec2
mouse_screen_position(Vec2 raw_mouse_pos, Camera camera)
{
    Vec2 mouse_world = vec2(-raw_mouse_pos.x / camera.window_width, raw_mouse_pos.y / camera.window_height);
    mouse_world      = sub_vec2(vec2(0, 1), mouse_world);
    mouse_world      = add_vec2_f32(mouse_world, -0.5);
    mouse_world.x *= camera.world_width;
    mouse_world.y *= camera.world_height;
    return mouse_world;
}

internal InputMouse
input_mouse_get(Window* window, Camera camera, InputMouse prev_state)
{
    InputMouse result = {0};
    glfwGetCursorPos(window->glfw_window, &result.raw_x, &result.raw_y);
    result.prev_button_state = prev_state.button_state;
    result.button_state += input_mouse_pressed_raw(window, GLFW_MOUSE_BUTTON_LEFT) << 0;
    result.button_state += input_mouse_pressed_raw(window, GLFW_MOUSE_BUTTON_RIGHT) << 1;
    result.button_state += input_mouse_pressed_raw(window, GLFW_MOUSE_BUTTON_MIDDLE) << 2;

    Vec2 mouse_raw = vec2(result.raw_x, result.raw_y);
    result.world   = input_mouse_world_position(mouse_raw, camera);
    result.screen  = mouse_screen_position(mouse_raw, camera);
    return result;
}

internal bool32
input_mouse_pressed(InputMouse mouse, MouseButtonState state)
{
    return (mouse.button_state & state) > 0 && (mouse.prev_button_state & state) == 0;
}

internal bool32
input_mouse_released(InputMouse mouse, MouseButtonState state)
{
    return (mouse.prev_button_state & state) > 0 && (mouse.button_state & state) == 0;
}

internal bool32
input_mouse_held(InputMouse mouse, MouseButtonState state)
{
    return (mouse.button_state & state) > 0;
}
