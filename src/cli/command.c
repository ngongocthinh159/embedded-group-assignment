#include "cli/command.h"

#include "cli/cli.h"
#include "lib/framebf.h"
#include "util/tty.h"

// This list has been sorted based on "name" attribute
Command all_commands[] = {
    {.name = CMD_BAUDRATE_PREFIX,
     .length = 8,
     .help = "Change board baud rate (available in: CLI mode)",
     .full_help = "Change board baud rate including RPI3 and RPI4, available "
                  "baudrate list:\n"
                  "+ 600\n"
                  "+ 1200\n"
                  "+ 2400\n"
                  "+ 4800\n"
                  "+ 9600\n"
                  "+ 14400\n"
                  "+ 19200\n"
                  "+ 38400\n"
                  "+ 57600\n"
                  "+ 115200\n"
                  "+ 230400\n"
                  "+ 460800\n"
                  "+ 921600\n"
                  "Example: baudrate 9600\n"
                  "Command only available in CLI mode",
     .fn = _handle_baudrate_config},
    {.name = CMD_CLEAR,
     .length = 5,
     .help = "Clear current shell (available in: CLI and Video modes)",
     .full_help = "Clear the command line to make the cmd look clean\n"
                  "Example: clear\n"
                  "Command only available in CLI and Video Play modes",
     .fn = clrscr},
    {
        .name = CMD_CURRENT_MODE,
        .length = 12,
        .help = "Show the current program's mode (available in: CLI and Video "
                "modes)",
        .full_help = "Show current program's mode: one of CLI, VIDEO or GAME\n"
                     "Example: currentmode\n"
                     "Command only available in CLI and Video Play modes",
    },
    {.name = CMD_SHOW_IMAGE,
     .length = 12,
     .help =
         "Display an image with team member's names (available in: CLI mode)",
     .full_help =
         "Display an image with team member's names (about the authors)\n"
         "Example: displayimage\n"
         "Command only available in CLI mode",
     .fn = displayWelcomeImage},
    {.name = CMD_EXIT,
     .length = 4,
     .help = "Exit the current program's mode (available in: Video modes)",
     .full_help = "Program flow control command to exit from Video Play mode "
                  "to CLI mode (we can not exit CLI or Game mode with this "
                  "command as we can not exit CLI and have different mechanism "
                  "to exit from Game mode)\n"
                  "Example: exit"},
    {.name = CMD_HELP,
     .length = 4,
     .help = "Show help message (available in: CLI and Video modes)",
     .full_help =
         "Show full information of a specific command: help <command>\n"
         "Example: help\n"
         "Command only available in CLI and Video Play modes",
     .fn = _print_help},
    {.name = CMD_VIDEO_PAUSE,
     .length = 5,
     .help = "Stop video player (available in: Video mode)",
     .full_help = "Stop playing video\n"
                  "Example: pause\n"
                  "Command only available in Video Play mode"},
    {.name = CMD_SWITCH_TO_GAME_MODE,
     .length = 8,
     .help = "Switch to game play mode (available in: CLI and Video modes)",
     .full_help =
         "Program flow control command to switch to game play mode, this "
         "command only available when the program is either in CLI or Video "
         "Play mode\n"
         "Example: playgame"},
    {.name = CMD_SWITCH_TO_VIDEO_PLAYER_MODE,
     .length = 9,
     .help = "Switch to video play mode (available in: CLI mode)",
     .full_help =
         "Program flow control command to switch to video play mode, this "
         "command only available when the program is in CLI mode\n"
         "Example: playvideo"},
    {.name = CMD_VIDEO_RESUME,
     .length = 6,
     .help = "Resume video player (available in: Video mode)",
     .full_help = "Resume from where the video is paused\n"
                  "Example: resume\n"
                  "Command only available in Video Play mode"},
    {.name = CMD_SHOW_INFO,
     .length = 8,
     .help = "Show hardware information (available in: CLI and Video modes)",
     .full_help = "Show hardware information including:\n"
                  "+ Board Revision\n"
                  "+ MAC Address\n"
                  "+ Model\n"
                  "+ Processor\n"
                  "+ Manufracturer\n"
                  "+ Memory Size\n"
                  "+ Revision Code Style\n"
                  "+ Warranty\n"
                  "+ OTP Information\n"
                  "+ Overvoltage Information\n"
                  "Example: showinfo\n"
                  "Command only available in CLI and Video Play modes",
     .fn = showinfo},
    {.name = CMD_STOPBIT_PREFIX,
     .length = 7,
     .help = "Change board stopbit (available in: CLI mode)",
     .full_help = "Change board number of stop bits including RPI3 and RPI4, "
                  "available number of stop bits configuration:\n"
                  "+ For UART1: 1 stop bit only\n"
                  "+ For UART0: 1 or 2 stop bits\n"
                  "Example: stopbit 2\n"
                  "Command only available in CLI mode",
     .fn = _handle_stopbit_config},
};

int all_commands_size = 12;





/* LEGACY */

// Command all_commands[] = {
//     // flow control
//     {.name = CMD_SWITCH_TO_VIDEO_PLAYER_MODE,
//      .length = 9,
//      .help = "Switch to video play mode (available in: CLI mode)",
//      .full_help =
//          "Program flow control command to switch to video play mode, this "
//          "command only available when the program is in CLI mode\n"
//          "Example: playvideo"},
//     {.name = CMD_SWITCH_TO_GAME_MODE,
//      .length = 8,
//      .help = "Switch to game play mode (available in: CLI and Video modes)",
//      .full_help =
//          "Program flow control command to switch to game play mode, this "
//          "command only available when the program is either in CLI or Video "
//          "Play mode\n"
//          "Example: playgame"},
//     {.name = CMD_EXIT,
//      .length = 4,
//      .help = "Exit the current program's mode (available in: Video modes)",
//      .full_help = "Program flow control command to exit from Video Play mode
//      "
//                   "to CLI mode (we can not exit CLI or Game mode with this "
//                   "command as we can not exit CLI and have different
//                   mechanism " "to exit from Game mode)\n" "Example: exit"},

//     // global command: work for CLI mode and video player mode
//     {.name = CMD_HELP,
//      .length = 4,
//      .help = "Show help message (available in: CLI and Video modes)",
//      .full_help =
//          "Show full information of a specific command: help <command>\n"
//          "Example: help\n"
//          "Command only available in CLI and Video Play modes",
//      .fn = _print_help},
//     {.name = CMD_CLEAR,
//      .length = 5,
//      .help = "Clear current shell (available in: CLI and Video modes)",
//      .full_help = "Clear the command line to make the cmd look clean\n"
//                   "Example: clear\n"
//                   "Command only available in CLI and Video Play modes",
//      .fn = clrscr},
//     {.name = CMD_SHOW_INFO,
//      .length = 8,
//      .help = "Show hardware information (available in: CLI and Video modes)",
//      .full_help = "Show hardware information including:\n"
//                   "+ Board Revision\n"
//                   "+ MAC Address\n"
//                   "+ Model\n"
//                   "+ Processor\n"
//                   "+ Manufracturer\n"
//                   "+ Memory Size\n"
//                   "+ Revision Code Style\n"
//                   "+ Warranty\n"
//                   "+ OTP Information\n"
//                   "+ Overvoltage Information\n"
//                   "Example: showinfo\n"
//                   "Command only available in CLI and Video Play modes",
//      .fn = showinfo},
//     {
//         .name = CMD_CURRENT_MODE,
//         .length = 12,
//         .help = "Show the current program's mode (available in: CLI and Video
//         "
//                 "modes)",
//         .full_help = "Show current program's mode: one of CLI, VIDEO or
//         GAME\n"
//                      "Example: currentmode\n"
//                      "Command only available in CLI and Video Play modes",
//     },

//     // only work for video player mode
//     {.name = CMD_VIDEO_RESUME,
//      .length = 6,
//      .help = "Resume video player (available in: Video mode)",
//      .full_help = "Resume from where the video is paused\n"
//                   "Example: resume\n"
//                   "Command only available in Video Play mode"},
//     {.name = CMD_VIDEO_PAUSE,
//      .length = 5,
//      .help = "Stop video player (available in: Video mode)",
//      .full_help = "Stop playing video\n"
//                   "Example: pause\n"
//                   "Command only available in Video Play mode"},

//     // only work for cli mode
//     {.name = CMD_SHOW_IMAGE,
//      .length = 12,
//      .help =
//          "Display an image with team member's names (available in: CLI
//          mode)",
//      .full_help =
//          "Display an image with team member's names (about the authors)\n"
//          "Example: displayimage\n"
//          "Command only available in CLI mode",
//      .fn = displayWelcomeImage},
//     {.name = CMD_BAUDRATE_PREFIX,
//      .length = 8,
//      .help = "Change board baud rate (available in: CLI mode)",
//      .full_help = "Change board baud rate including RPI3 and RPI4, available
//      "
//                   "baudrate list:\n"
//                   "+ 600\n"
//                   "+ 1200\n"
//                   "+ 2400\n"
//                   "+ 4800\n"
//                   "+ 9600\n"
//                   "+ 14400\n"
//                   "+ 19200\n"
//                   "+ 38400\n"
//                   "+ 57600\n"
//                   "+ 115200\n"
//                   "+ 230400\n"
//                   "+ 460800\n"
//                   "+ 921600\n"
//                   "Example: baudrate 9600\n"
//                   "Command only available in CLI mode",
//      .fn = _handle_baudrate_config},
//     {.name = CMD_STOPBIT_PREFIX,
//      .length = 7,
//      .help = "Change board stopbit (available in: CLI mode)",
//      .full_help = "Change board number of stop bits including RPI3 and RPI4,
//      "
//                   "available number of stop bits configuration:\n"
//                   "+ For RPI3: 1 stop bit only\n"
//                   "+ For RPI4: 1 or 2 stop bits\n"
//                   "Example: stopbit 2\n"
//                   "Command only available in CLI mode",
//      .fn = _handle_stopbit_config},
// };

// int all_commands_size = 12;
