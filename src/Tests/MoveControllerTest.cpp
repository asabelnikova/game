#include "MoveController.h"
#include "GameController.h"

#include "fakeit.hpp"

using namespace fakeit;
using cocos2d::Vec2;
using cocos2d::TMXTiledMap;
using cocos2d::TMXLayer;
using cocos2d::TMXObjectGroup;
using cocos2d::Size;
using cocos2d::Sprite;
using cocos2d::ValueVector;
using cocos2d::ValueMap;
using cocos2d::Value;

class MoveControllerStub: public tsg::move::MoveController {
 public:
  virtual Vec2 getPlayerSpawn() const;
  virtual Vec2 getPlayerPosition() const;
  virtual void initSprite(Vec2) override;
  virtual void onMapLoad(TMXTiledMap *) override;
  virtual void onInit() override;
  virtual void onUpdate(float) override;
};

class MapControllerStub: public tsg::map::MapController {
  virtual void loadMapFromFile(const std::string &) { }
 public:
  MapControllerStub() : MapController(nullptr) { }
};

class TMXTiledMapStub: public TMXTiledMap {
 public:
  virtual TMXLayer *getLayer(const std::string &layerName) const;
  virtual TMXObjectGroup *getObjectGroup(const std::string &groupName) const;
  virtual const Size &getTileSize() const;
};

class TMXLayerStub: public cocos2d::TMXLayer {

 public:
  virtual Sprite *getTileAt(const Vec2 &);
};

class SpriteStub: public Sprite {

 public:
  virtual const Vec2 &getPosition() const;
};

//=============================8<==================================

void MoveControllerStub::initSprite(Vec2) { /*stubbed*/}
void MoveControllerStub::onMapLoad(TMXTiledMap *map) { tsg::move::MoveController::onMapLoad(map); }
void MoveControllerStub::onInit() { tsg::move::MoveController::onInit(); }
void MoveControllerStub::onUpdate(float d) { tsg::move::MoveController::onUpdate(d); }
Vec2 MoveControllerStub::getPlayerSpawn() const { return playerSpawn; }
Vec2 MoveControllerStub::getPlayerPosition() const { return playerPosition; }

TMXLayer *TMXTiledMapStub::getLayer(const std::string &) const { return nullptr; }
TMXObjectGroup *TMXTiledMapStub::getObjectGroup(const std::string &) const { return nullptr; }
const Size &TMXTiledMapStub::getTileSize() const { return Size::ZERO; }

const Vec2 &SpriteStub::getPosition() const { return Vec2::ZERO; }
Sprite *TMXLayerStub::getTileAt(const Vec2 &) { return nullptr; }

//================================8<=======================================

TMXObjectGroup mkObjectGroup(Vec2 p) {
  TMXObjectGroup group;
  ValueVector v;
  ValueMap vp;
  vp["name"] = "spawn1";
  vp["x"] = Value(p.x);
  vp["y"] = Value(p.y);
  v.push_back(Value(vp));
  group.setObjects(v);
  return group;
}

TEST_CASE("That player spawns well in spawn point and map is scrolled to spawn point correctly",
          "[MoveController]") {
  Mock<TMXLayerStub> layerMock;
  Mock<TMXTiledMapStub> mapMock;
  SpriteStub zeroPointSpriteStub;
  Mock<Sprite> zeroPointSpriteMock(zeroPointSpriteStub);
  SpriteStub spawnSpriteStub;
  Mock<Sprite> spawnSpriteMock(spawnSpriteStub);
  Mock<MapControllerStub> mapControllerMock;
  MoveControllerStub moveControllerStub;
  std::string spawnLayer = "spawn point";
  auto gameController = tsg::game::GameController::getInstance();
  auto zeroPointGrid = Vec2(0, 0);
  auto zeroPointWorld = Vec2(1000.0f, 1000.0f);
  auto spawnPointGrid = Vec2(4, 29);
  auto spawnPointWorld = Vec2(100, 100);
  auto tileSize = Size(64, 32);
  auto group = mkObjectGroup(Vec2(100, 100));

  gameController->injectControllers(&moveControllerStub, &mapControllerMock.get(), nullptr);

  When(ConstOverloadedMethod(mapMock, getLayer,TMXLayer *(const std::string &)).Using("water")).AlwaysReturn(&layerMock.get());
  When(ConstOverloadedMethod(zeroPointSpriteMock, getPosition,const Vec2&())).Return(zeroPointWorld);
  When(ConstOverloadedMethod(spawnSpriteMock, getPosition,const Vec2&())).Return(spawnPointWorld);
  When(Method(mapMock, getObjectGroup).Using(spawnLayer)).AlwaysReturn(&group);
  When(Method(mapMock, getTileSize)).AlwaysReturn(tileSize);
  When(Method(layerMock, getTileAt).Using(zeroPointGrid)).AlwaysReturn(&zeroPointSpriteMock.get());
  When(Method(layerMock, getTileAt).Using(spawnPointGrid)).AlwaysReturn(&spawnSpriteMock.get());
  When(OverloadedMethod(mapControllerMock, lookAt, void(Vec2))).Return();
  moveControllerStub.onMapLoad(&mapMock.get());
  REQUIRE(moveControllerStub.getPlayerSpawn() == spawnPointWorld);
  REQUIRE(moveControllerStub.getPlayerPosition() == moveControllerStub.getPlayerSpawn());
  Verify(OverloadedMethod(mapControllerMock, lookAt, void(Vec2)).Using(spawnPointWorld)).Once();
}
