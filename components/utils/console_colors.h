/**
 * http://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html#256-colors
 */

#define CONSOLE_COLORS_RESET        "\033[0m"
#define CONSOLE_BACKGROUND_BLACK        "\033[40m"
#define CONSOLE_BACKGROUND_RED          "\033[41m"
#define CONSOLE_BACKGROUND_GREEN        "\033[42m"
#define CONSOLE_BACKGROUND_YELLOW       "\033[43m"
#define CONSOLE_BACKGROUND_BLUE         "\033[44m"
#define CONSOLE_BACKGROUND_MAGENTA      "\033[45m"
#define CONSOLE_BACKGROUND_CYAN         "\033[46m"
#define CONSOLE_BACKGROUND_WHITE        "\033[47m"

#define CONSOLE_BACKGROUND_BLACK_BRIGHT        "\033[40;1m"
#define CONSOLE_BACKGROUND_RED_BRIGHT          "\033[41;1m"
#define CONSOLE_BACKGROUND_GREEN_BRIGHT        "\033[42;1m"
#define CONSOLE_BACKGROUND_YELLOW_BRIGHT       "\033[43;1m"
#define CONSOLE_BACKGROUND_BLUE_BRIGHT         "\033[44;1m"
#define CONSOLE_BACKGROUND_MAGENTA_BRIGHT      "\033[45;1m"
#define CONSOLE_BACKGROUND_CYAN_BRIGHT         "\033[46;1m"
#define CONSOLE_BACKGROUND_WHITE_BRIGHT        "\033[47;1m"

//foreground

#define CONSOLE_TEXT_BLACK        "\033[30m"
#define CONSOLE_TEXT_RED          "\033[31m"
#define CONSOLE_TEXT_GREEN        "\033[32m"
#define CONSOLE_TEXT_YELLOW       "\033[33m"
#define CONSOLE_TEXT_BLUE         "\033[34m"
#define CONSOLE_TEXT_MAGENTA      "\033[35m"
#define CONSOLE_TEXT_CYAN         "\033[36m"
#define CONSOLE_TEXT_WHITE        "\033[37m"

#define CONSOLE_TEXT_BLACK_BRIGHT        "\033[30;1m"
#define CONSOLE_TEXT_RED_BRIGHT          "\033[31;1m"
#define CONSOLE_TEXT_GREEN_BRIGHT        "\033[32;1m"
#define CONSOLE_TEXT_YELLOW_BRIGHT       "\033[33;1m"
#define CONSOLE_TEXT_BLUE_BRIGHT         "\033[34;1m"
#define CONSOLE_TEXT_MAGENTA_BRIGHT      "\033[35;1m"
#define CONSOLE_TEXT_CYAN_BRIGHT         "\033[36;1m"
#define CONSOLE_TEXT_WHITE_BRIGHT        "\033[37;1m"

//decorations
#define CONSOLE_TEXT_BOLD               "\033[1m"
#define CONSOLE_TEXT_UNDERLINE          "\033[4m"
#define CONSOLE_TEXT_REVERSED           "\033[7m"

#define console_set_color(code){printf(code);}





