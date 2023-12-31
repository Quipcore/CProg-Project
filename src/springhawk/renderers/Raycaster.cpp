#include <iostream>
#include "springhawk/Input.h"
#include "springhawk/renderers/Raycaster.h"
#include "Math.h"
#include "springhawk/maps/Map.h"
#include "springhawk/Scene.h"
#include "springhawk/Engine.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace springhawk;

//----------------------------------------------------------------------------------------------------------------------

int Raycaster::SCREEN_WIDTH = 0;
int Raycaster::SCREEN_HEIGHT = 0;
int Raycaster::tileMap[mapWidth][mapHeight] = {
        {1,1,1,1,1,1,1,1},
        {3,0,2,0,0,0,0,5},
        {3,0,2,0,0,0,0,5},
        {3,0,2,0,0,0,0,5},
        {3,0,0,0,0,0,0,5},
        {3,0,0,0,0,2,0,5},
        {3,0,0,0,0,0,0,5},
        {3,4,4,4,4,4,4,4}
};

//----------------------------------------------------------------------------------------------------------------------
void Raycaster::render(SDL_Renderer &renderer, std::vector<GameObject *> &gameObjects, Map &map, int screenWidth, int screenHeight) {
    Player* player = Engine::getObjects<Player>()[0];
    render(renderer, gameObjects, *player, map, screenWidth, screenHeight);
}

void Raycaster::render(SDL_Renderer &renderer, std::vector<GameObject *> &gameObjects, Player &player, Map &map, int screenWidth, int screenHeight) {
    SCREEN_WIDTH = screenWidth;
    SCREEN_HEIGHT = screenHeight;

//    //VERY INEFFICIENT. Copies the map to the tileMap array every frame.
//    for(int x = 0; x < map.getWidth(); x++){
//        for(int y = 0; y < map.getHeight(); y++){
//            Raycaster::tileMap[x][y] = map[{x,y}];
//        }
//    }

    if(!Input::bufferContains(TAB)){
        //Draw ceiling
        Color ceilingColor = {125,125,125,255};
        SDL_Rect ceiling = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT/2};
        SDL_SetRenderDrawColor(&renderer, ceilingColor.r,ceilingColor.g,ceilingColor.b,ceilingColor.a);
        SDL_RenderFillRect(&renderer, &ceiling);

        //Draw floor
        Color floorColor = {125,0,125,255};
        SDL_Rect floor = {0,SCREEN_HEIGHT/2,SCREEN_WIDTH,SCREEN_HEIGHT/2};
        SDL_SetRenderDrawColor(&renderer, floorColor.r,floorColor.g,floorColor.b,floorColor.a);
        SDL_RenderFillRect(&renderer, &floor);
    }


    drawPlayer(&renderer, &player);
}

//----------------------------------------------------------------------------------------------------------------------

void Raycaster::drawMap(SDL_Renderer *pRenderer) {
    int w = SCREEN_WIDTH / mapWidth;
    int h = SCREEN_HEIGHT / mapHeight;
    int x, y, xo, yo;
    for (y = 0; y < mapHeight; y++) {
        for (x = 0; x < mapWidth; x++) {
            setRenderDrawColor(pRenderer, tileMap[y][x]);

            xo = x * w;
            yo = y * h;
            SDL_Rect rect = {xo, yo, w - 1, h - 1};
            SDL_RenderFillRect(pRenderer, &rect);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Raycaster::setRenderDrawColor(SDL_Renderer *pRenderer, int wallValue){
    switch(wallValue){
        case 1:
            SDL_SetRenderDrawColor(pRenderer, 0xff, 0xff, 0xff, 0xff);
            break;
        case 2:
            SDL_SetRenderDrawColor(pRenderer, 0x00, 0xff, 0x00, 0xff);
            break;
        case 3:
            SDL_SetRenderDrawColor(pRenderer,0x00,0x00,0xff,0xff);
            break;
        case 4:
            SDL_SetRenderDrawColor(pRenderer,0xff,0xff,0x00,0xff);
            break;
        case 5:
            SDL_SetRenderDrawColor(pRenderer,0xff,0x00,0xff,0xff);
            break;
        default:
            SDL_SetRenderDrawColor(pRenderer, 0x00, 0x00, 0x00, 0xff);
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Raycaster::drawObjects(SDL_Renderer *pRenderer, std::vector<GameObject *> &vector) {
    for (auto &gameObject: vector) {
        //Not yet implemented
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Raycaster::drawPlayer(SDL_Renderer *pRenderer, Player *const &player) {
    drawRays(pRenderer, player);
}

//----------------------------------------------------------------------------------------------------------------------

void Raycaster::drawRays(SDL_Renderer *pRenderer, Player *pPlayer) {
    const int radius = 200;
    const int lineCount = pPlayer->getLineCount();
    const double sliceWidth = SCREEN_WIDTH / lineCount;
    const double fov = M_PI/2;
    const double playerAngle = pPlayer->getAngle();
    const double wallScale = SCREEN_HEIGHT/16;
    const Color rayColor = {0xff, 0, 0, 0xff};

    Vector2 playerPosition = pPlayer->getPosition();
    int playerX = (int) playerPosition.getX();
    int playerY = (int) playerPosition.getY();

    SDL_SetRenderDrawColor(pRenderer, rayColor.r, rayColor.g, rayColor.b, rayColor.a);
    for(int i = -lineCount/2; i < lineCount/2; i++){

        double angle = (fov * (i)/ lineCount);
        Vector2 direction = {cos(angle + playerAngle), sin(angle + playerAngle)};
        Vector2 endPosition = findEndPosition(playerPosition, direction, radius);

        Vector2 mapPos = findMapPoint(endPosition);
        int mapPointX = (int) mapPos.getX();
        int mapPointY = (int) mapPos.getY();
        setRenderDrawColor(pRenderer, tileMap[mapPointY][mapPointX]);

        double rayMag = (pPlayer->getPosition() - endPosition).magnitude();
        double lineDistance = (rayMag)* cos((angle));
        double wallHeight = SCREEN_HEIGHT * wallScale/ lineDistance;
        if(wallHeight > SCREEN_HEIGHT){
            wallHeight = SCREEN_HEIGHT;
        }

        bool render2D = Input::bufferContains(Keycode::TAB);
        if(render2D){
            drawMap(pRenderer);
            SDL_SetRenderDrawColor(pRenderer, rayColor.r, rayColor.g, rayColor.b, rayColor.a);
            SDL_RenderDrawLine(pRenderer, playerX, playerY, (int) endPosition.getX(), (int) endPosition.getY());
        }else{
            int rectX = (lineCount-(i+lineCount/2)) * sliceWidth;
            int rectY = (SCREEN_HEIGHT - wallHeight) / 2;
            SDL_Rect rect = {rectX,rectY,(int)sliceWidth,(int)wallHeight};
            SDL_RenderFillRect(pRenderer, &rect);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

/*
 * 1. Shoot a ray from the player to the end of the screen
 * 2. Check if the ray intersects with a wall
 * 3. return the point of intersection
 */
Vector2 Raycaster::findEndPosition(Vector2 &position, Vector2 &direction, double maxDistance) {
    double stepSize = 1;
    Vector2 endPosition = position;
    while(isPositionValid(endPosition)){
        endPosition += direction * stepSize;
    }
    return endPosition;
}

//----------------------------------------------------------------------------------------------------------------------

Vector2 Raycaster::getIntersectionPoint(Vector2 &position, double angle) {
    return position;
}

//----------------------------------------------------------------------------------------------------------------------

bool Raycaster::isPositionValid(Vector2 vector2) {
    int x = (int) (vector2.getX() * mapWidth / SCREEN_WIDTH);
    int y = (int) (vector2.getY() * mapHeight / SCREEN_HEIGHT);

    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) {
        return false;
    }
    if (tileMap[y][x] != 0) {
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------

Vector2 Raycaster::findMapPoint(Vector2 vector2) {
    double setX = (vector2.getX() * mapWidth / SCREEN_WIDTH);
    double setY = (vector2.getY() * mapHeight / SCREEN_HEIGHT);

    return {setX, setY};
}