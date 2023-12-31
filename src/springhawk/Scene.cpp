#include <iostream>
#include <utility>
#include "springhawk/Scene.h"
#include "SDL2/SDL.h"
#include "springhawk/Engine.h"


Scene::Scene(Map &incomingMap, std::vector<GameObject *> &gameObjects, std::vector<UIComponent *>, springhawk::RenderTag renderTag) {
    this->map = &incomingMap;
    this->gameObjects = std::move(gameObjects);
    this->uiComponents = std::move(uiComponents);
    this->renderTag = renderTag;
}

//----------------------------------------------------------------------------------------------------------------------

std::vector<GameObject *> Scene::getGameObjects() {
    return gameObjects;
}

//----------------------------------------------------------------------------------------------------------------------

springhawk::RenderTag Scene::getRenderTag() const {
    return renderTag;
}

//----------------------------------------------------------------------------------------------------------------------

Map & Scene::getMap() {
    return *map;
}

//----------------------------------------------------------------------------------------------------------------------

void Scene::loadTextures(SDL_Renderer& renderer) {
    map->loadTextures(renderer);
    for(GameObject* gameObject : gameObjects){
        springhawk::Engine::swapTexture(gameObject, gameObject->getTexturePath());
//        gameObject->setTexture(renderer, gameObject->getTexturePath());
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Scene::destroyTextures() {
    //TODO: Destory textures in the same places they were created!
}

std::vector<UIComponent *> Scene::getUIComponents() {
    return uiComponents;
}




