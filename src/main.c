#include "opus/base/base_inc.h"
#include "opus/base/base_inc.c"

#include "opus/gfx/gfx_inc.h"
#include "opus/gfx/gfx_inc.c"

#include "opus/os/os_inc.h"
#include "opus/os/os_inc.c"

#include "opus/text/text_inc.h"
#include "opus/text/text_inc.c"

#include "opus/draw/draw_inc.h"
#include "opus/draw/draw_inc.c"

#include "opus/ui/ui_inc.h"
#include "opus/ui/ui_inc.c"

#include "opus/input/input_inc.h"
#include "opus/input/input_inc.c"

int32
main(void)
{
    ThreadContext tctx;
    tctx_init_and_equip(&tctx);
    Arena* persistent_arena = arena_new();
    Arena* frame_arena      = arena_new();

    GFX_Configuration config = {0};
    config.clear_color       = ColorSlate800;

    input_init(persistent_arena);
    OS_Handle window = os_window_create(1920, 1080, string("Memory Tag"), input_key_callback);
    gfx_init(config);
    font_cache_init(persistent_arena);
    ui_init(persistent_arena);
    d_context_init(persistent_arena, frame_arena, string("..\\assets"));

    /*********************/
    /** APPLICATION CODE */
    mtag_init();
    mtag_register_arena(persistent_arena, string("persistent_arena"));
    mtag_register_arena(frame_arena, string("frame_arena"));

    /** setup inputs */
    String action_quit = string("quit");
    input_register_key(ui_input_select, OS_KeyCode_MouseLeft);
    input_register_key(action_quit, OS_KeyCode_BracketRight);

    /** ui state */
    float32 slider_value = 0;
    bool32  should_quit  = false;

    float32 dt = 0.05f; // TODO(selim): calculate this
    for (;;)
    {
        input_update(dt);

        Input_MouseInfo mouse = input_mouse_info();
        if (input_is_pressed(action_quit))
        {
            should_quit = true;
        }

        d_string_at(vec2(-100, -100), string_pushf(frame_arena, "Cursor: %f, %f", mouse.screen.x, mouse.screen.y), 32, ColorBlack, AlignmentLeft);

        Input_Key key = _input_context->keys[OS_KeyCode_MouseLeft];
        d_string_at(vec2(-100, -130), string_pushf(frame_arena, "Key State: %d, Event Start: %llu, Event End: %llu", key.state, _input_context->input_event_buffer_start, _input_context->input_event_buffer_end), 32, ColorWhite100, AlignmentLeft);
        d_circle(mouse.screen, 24, 1, ColorBlack);
        arena_reset(frame_arena);

        if (should_quit || os_window_should_close(window))
        {
            log_info("Exiting...");
            break;
        }

        ui_create_fixed(screen_rect())
        {
            ui_create(CutSideTop, 64)
            {
                ui_create(CutSideLeft, 512)
                {
                    ui_fill(ColorSlate900);
                    ui_pad(8);
                    ui_slider(string("Text Size:"), 6, 64, &slider_value);
                }
            }
        }

        MTAG_ArenaNode* registered_arena;
        const float32   line_height = 24;
        float32         y           = 0;
        for_each(registered_arena, _mtag_context->registered_arenas.first)
        {
            String text = string_pushf(frame_arena, "Pos: %d, Cap: %d, Commit: %d", registered_arena->value.cached_pos, registered_arena->value.cached_cap, registered_arena->value.cached_commit_pos);
            d_string_at(vec2(0, y), text, slider_value, ColorWhite, AlignmentCenter);
            y -= line_height - 2;
        }

        ui_update(dt);
        gfx_render(dt);
        os_window_update(window);
        mtag_refresh();
    }

    return 0;
}