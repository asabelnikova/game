#define CATCH_CONFIG_MAIN

#include "GameController.h"
#include "IGameEventListener.h"

#include "fakeit.hpp"

using namespace fakeit;

TEST_CASE("That game event listener got notified well", "[GameController]") {
  Mock<tsg::move::MoveController> moveControllerMock;
  Mock<tsg::map::MapController> mapControllerMock;
  tsg::game::GameController* controller =
      tsg::game::GameController::getInstance();
  controller->injectControllers(&moveControllerMock.get(),
                                &mapControllerMock.get());
  When(Method(mapControllerMock, loadMap)).Return();
  When(Method(mapControllerMock, IGameEventListener::onInit)).Return();
  When(Method(mapControllerMock, IGameEventListener::onUpdate)).Return();
  Verify(Method(mapControllerMock, IGameEventListener::onInit)).Exactly(0);
  controller->init();
  Verify(Method(mapControllerMock, IGameEventListener::onInit)).Exactly(1);
}