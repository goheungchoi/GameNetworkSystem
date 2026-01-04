#include "editor_app/editor_app.h"

int main(int argc, char** argv) {
  GameNet::EditorApp app;
  return app.Exec(argc, argv);
}
