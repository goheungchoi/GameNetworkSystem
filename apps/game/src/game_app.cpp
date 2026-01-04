#include "game_app/game_app.h"

#include <GLFW/glfw3.h>

#include "gamenet_app/session/play_session.h"
#include "game_app.h"

GameNet::GameApp::GameApp() : _gameSession() {}

int GameNet::GameApp::OnInit(int argc, char** argv) { 
  return 0; 
}

int GameNet::GameApp::OnTick() { 
  return 0; 
}

void GameNet::GameApp::OnShutdown() {
  
}

void GameNet::GameApp::OnResize(int width, int height) {}

void GameNet::GameApp::OnKey(int key, int scancode, int action, int mods) {}

void GameNet::GameApp::OnMouseButton(int button, int action, int mods) {}

void GameNet::GameApp::OnMouseMove(double xpos, double ypos) {}
