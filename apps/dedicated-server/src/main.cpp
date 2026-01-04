#include "server_app/dedicated_server_app.h"

int main(int argc, char** argv) {
  GameNet::DedicatedServerApp app;
  return app.Exec(argc, argv);
}
