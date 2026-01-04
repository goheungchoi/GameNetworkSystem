#include "gamenet_app/session/play_session.h"

#include <GLFW/glfw3.h>

GameNet::GameSession::GameSession() {}

int GameNet::GameSession::Init() { return 0; }

int GameNet::GameSession::Frame() {
  double frameDt = frame.Tick();

  stepper.AddTime(frameDt);

  // Receive network once per frame
  transport->PumpRecv(*client, server.get());

  while (stepper.Step()) {
    // Sample and buffer inputs with ++tickId.
    // And send batched inputs through the transport.
    client->ProcessInput();

    if (server) {
      // Consume inputs.
      server->ConsumeInputsFrom(*transport);
      server->Tick(stepper.fixedDelta);
      server->MaybeSendSnapshot(*transport, tickId);
    }

    client->ConsumeSnapshotsFrom(*transport);
    client->PredictTick(stepper.fixedDelta);
    client->ReconcileIfNeeded();
  }

  client->Render(stepper.Alpha());
}

void GameNet::GameSession::Shutdown() {}