#include "application.h"
#define APP_API extern "C" __declspec(dllexport) // TODO(alx): replace with /EXPORT flag in build.bat file, because it's windows specific.

#if defined(_WIN32)
#include "windows_slim.h"
#endif

APP_API APP_UPDATE_AND_RENDER(app_update_and_render){
    /*
    char debug_buf[Kilobytes(1)];
    char *format = 
        "LShoulder: %i\n"
        "RShoulder: %i\n"
        "A: %i\n"
        "B: %i\n"
        "X: %i\n"
        "Y: %i\n"
        "DpadUP: %i\n"
        "DpadDown: %i\n"
        "DpadLeft: %i\n"
        "DpadRight: %i\n"
        "LeftStick: (%f, %f)\nRightStick: (%f, %f)\nTrigger: (%f, %f)\n\n";
    sprintf_s(debug_buf, size_of(debug_buf), format,
              left_shoulder, right_shoulder,
              a_button, b_button, x_button, y_button,
              dpad_up, dpad_down, dpad_left, dpad_right,
              left_stick.x, left_stick.y,
              right_stick.x, right_stick.y,
              trigger.x, trigger.y);
    debug_out(debug_buf);
    */
    
    /*     
        if(input.is_analog){
            // NOTE(alx): use analog movement tuning
        }
        else{
            // NOTE(alx): use digital movement tuning
        }
         */
    
    // the problem with this approach is that we should use a growing array or a linked list etc...
#if 0
    // usage code of event-drive input system.
    for(int32_t event_index = 0; event_index < event_count; event_index += 1){
        switch(events[event_index].type){
            case EventAButton:{
            }break;
            case EventBButton:{
            };
            
        }
        // or
        events[event_index].stick_x;
        events[event_index].a_button_down;
        //...
    }
#endif
    
    glClearColor(0.0f/255.0f, 130.0f/255.0f, 97.0f/255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}