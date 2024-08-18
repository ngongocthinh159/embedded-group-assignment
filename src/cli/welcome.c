#include "util/tty.h"

char text[] =
    "oooooooooooo oooooooooooo oooooooooooo ooooooooooooo   .oooo.         .o  "
    "  .ooooo.     .oooo.\r\n"
    "`888'     `8 `888'     `8 `888'     `8 8'   888   `8 .dP\"\""
    "Y88b      .d88   888' `Y88.  d8P'`Y8b\r\n"
    " 888          888          888              888            ]8P'   .d'888  "
    " 888    888 888    888\r\n"
    " 888oooo8     888oooo8     888oooo8         888          .d8P'  .d'  888  "
    "  `Vbood888 888    888\r\n"
    " 888    \"     888    \"     888    \"         888          .dP'   "
    "88ooo888oo       888' 888    888\r\n"
    " 888       o  888       o  888       o      888      .oP     .o      888  "
    "     .88P'  `88b  d88'\r\n"
    "o888ooooood8 o888ooooood8 o888ooooood8     o888o     8888888888     o888o "
    "   .oP'      `Y8bd8P' \r\n"
    "\r\n\r\n"
    "oooooooooo.        .o.       ooooooooo.   oooooooooooo      .oooooo.    "
    ".oooooo..o\r\n"
    "`888'   `Y8b      .888.      `888   `Y88. `888'     `8     d8P'  `Y8b  "
    "d8P'    `Y8\r\n"
    " 888     888     .8\"888.      888   .d88'  888            888      888 "
    "Y88bo.\r\n"
    " 888oooo888'    .8' `888.     888ooo88P'   888oooo8       888      888  "
    "`\"Y8888o.\r\n"
    " 888    `88b   .88ooo8888.    888`88b.     888    \"       888      888   "
    "   `\"Y88b\r\n"
    " 888    .88P  .8'     `888.   888  `88b.   888       o    `88b    d88' oo "
    "    .d8P\r\n"
    "o888bood8P'  o88o     o8888o o888o  o888o o888ooooood8     `Y8bood8P'  8"
    "88888P'\r\n";

int welcome() {
  println(text);
  return 0;
}
