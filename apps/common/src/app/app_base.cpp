#include "gamenet_app/app/app_base.h"

int GameNet::AppBase::Exec(int argc, char** argv) { 
  int res = Init(argc, argv);
  if (res != 0) 
    return res;
  
  res = Run();
  Shutdown();

  return res; 
}

