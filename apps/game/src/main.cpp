#include "game_app/game_app.h"

int main(int argc, char** argv) {
  GameNet::GameApp app;
  return app.Exec(argc, argv);
}
