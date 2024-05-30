#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<vector>
#include<memory>
#include<algorithm>
#include<bitset>
#include<array>
using namespace std;
class Component;
class Entity;
using ComponentID=std::size_t;
inline ComponentID getComponentTypeID()
{
    static ComponentID lastID=0;
    return lastID++;
}
template<typename T>inline ComponentID getComponentTypeID() noexcept
{
    static ComponentID typeID=getComponentTypeID();
    return typeID();
}
constexpr std::size_t maxComponents=32;
using ComponentBitSet=std::bitset<maxComponents>;
using ComponentArray=std::array<component*,maxComponents>;
class Component
{   
public:
    Entity *entity;
    virtual void init()
    {

    }
    virtual void update()
    {

    }
    virtual void draw()
    {

    }
    virtual ~Component()
    {

    }
    virtual void init()
    {

    }

};
class Entity
{
    bool active=true;
    vector<unique_ptr<Component>>components;
    ComponentArray componentArray;
    ComponentBitSet componentBitSet;
public:
    void update()
    {
        for(auto& c:components)c->update();
    }
    void draw()
    {
        for(auto& c:components)c->draw();
    }
    bool isActive() const {return active; }
    void destroy() {active=false;}
    template <typename T>bool hasComponent() const
    {
        return componentBitSet[getComponentID<T>];
    }
    template <typename T,typename... TArgs>
    T& addComponent(TArgs&&...mArgs)
    {
        T* c(new T(forward<TArgs>(mArgs)...));
        c->entity=this;
        unique_ptr<Component>uPtr{c};
        components.emplace_back(move(uptr));
        componentsArray[getComponentTypeID<T>()]=c;
        componentBitSet[getComponentTypeID<T>()]=true;
        c->init();
        return *c;
    }
    template<typename T>T& getComponent()const
    {
        auto ptr(componentArray[getComponentTypeID<T>()]);
        return *static_cast<T*>(ptr);
    }
};
class Manager
{
    vector<unique_ptr<Entity>>entities;
public:
    void update()
    {
        for(auto &e:entities)e->update();
    }
    void draw()
    {
        for(auto& e:entities)e->draw();
    }
    void refresh()
    {
        entities.erase(remove_if(begin(entities,end(entities),
                [](const unique_ptr<Entity> &mEntity)
                        {
                            return !mEntity->isActive();
                        }),
                        end(entities));
    }
    Entity& addEntity()
    {
        Entity *e=new Entity();
        unique_ptr<Entity>uptr{e};
        entities.emplace_back(move(uptr));
        return *e;
    }
};
class PositionComponent:public component
{
    int xpos,ypos;
public:
    PositionComponent()
{
    xpos=0;
    ypos=0;
}
    PositionComponent(int x,int y)
{
    xpos=x;
    ypos=y;
}
    int x(){return xpos;}
    int y(){return ypos;}
    void init()override
    {
        xpos=0;
        ypos=0;
    }
    void update()override
    {
        xpos++;
        ypos++;
    }
    void setPos(int x,int y)
    {
        xpos=x;
        ypos=y;
    }
};
class SpriteComponent:public Component
{
    PositionComponent *position;
    SDL_texture *texture;
    SDL_Rect srcRect,destRect;
public:
    SpriteComponent()=default;
    SpriteComponent(const char* path)
    {
        setTex(path);
    }
    void setTex(const char* path)
    {
        texture=TextureManager::LoadTexture(path);
    }
    void init() override
    {
        position=&entity->getComponent<PositionComponent>();
        srcRect.x=srcRect.y=0;
        srcRect.w=srcRect.h=32;
        destRect.w=destRect.h=64;
    }
    void update() override
    {
        destRect.x=position->x();
        destRect.y=position->y();
    }
    void draw() override
    {
        TextureManager::Draw(texture,srcRect,destRect)
    }
}
Manager manager;
auto& Player(manager.addEntity());
class Game
{
    bool isRunning=false;
    SDL_Window *window;
public:
    Game();
    ~Game();
    void init(const char *title,int xpos,int ypos,int width,int height,bool fullscreen);
    void update();
    void render();
    void clean();
    void handleEvents();
    bool running()
    {
        return isRunning;
    }
    static SDL_Renderer *renderer;
};
SDL_Renderer* Game::renderer=nullptr;
class TextureManager
{
public:
    static SDL_Texture *LoadTexture(const char *texture);
    static void Draw(SDL_Texture *tex,SDL_Rect src,SDL_Rect dest);
};
void TextureManager::Draw(SDL_Texture *tex,SDL_Rect src,SDL_Rect dest)
{
    SDL_RenderCopy(Game::renderer,tex,&src,&dest);
}
SDL_Texture* TextureManager::LoadTexture(const char *texture)
{
    SDL_Surface *tempSurface=IMG_Load(texture);
    SDL_Texture *tex=SDL_CreateTextureFromSurface(Game::renderer,tempSurface);
    SDL_FreeSurface(tempSurface);
    return tex;
}
class Map
{
    SDL_Rect src,dest;
    SDL_Texture *dirt;
    SDL_Texture *grass;
    SDL_Texture *water;
    int map[20][25];
public:
    Map();
    ~Map();
    void LoadMap(int arr[20][25]);
    void DrawMap();
};
int lvl1[20][25]={
    {2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2},

};
Map::Map()
{
    dirt=TextureManager::LoadTexture("assets/dirt.png");
    grass=TextureManager::LoadTexture("assets/grass.png");
    water=TextureManager::LoadTexture("assets/water.png");
    LoadMap(lvl1);
    src.x=src.y=0;
    src.w=dest.w=32;
    src.h=dest.h=32;
    dest.x=dest.y=0;
}
void Map::LoadMap(int arr[20][25])
{
    for(int row=0;row<20;row++)
    {
        for(int col=0;col<25;col++)
            {
                map[row][col]=arr[row][col];
            }
    }
}
void Map::DrawMap()
{
    int type=0;
    for(int row=0;row<20;row++)
    {
        for(int col=0;col<25;col++)
            {
                type=map[row][col];
                dest.x=col*32;
                dest.y=row*32;
                switch(type)
                {
                    case 0:
                        TextureManager::Draw(water,src,dest);
                        break;
                    case 1:
                        TextureManager::Draw(grass,src,dest);
                        break;
                    case 2:
                        TextureManager::Draw(dirt,src,dest);
                        break;
                    default:
                        break;
                }
            }
    }
}
Map *map;
class GameObject
{
    int xpos;
    int ypos;
    SDL_Texture *objTexture;
    SDL_Rect srcRect,destRect;
public:
    GameObject(const char *texturesheet,int x,int y);
    ~GameObject();
    void Update();
    void Render();
};
GameObject *player;
GameObject *enemy;
GameObject::GameObject(const char *texturesheet,int x,int y)
{
    objTexture=TextureManager::LoadTexture(texturesheet);
    xpos=x;
    ypos=y;
}
void GameObject::Update()
{
    xpos++;
    ypos++;
    srcRect.w=96;
    srcRect.h=64;
    srcRect.x=0;
    srcRect.y=0;
    destRect.x=xpos;
    destRect.y=ypos;
    destRect.w=srcRect.w;
    destRect.h=srcRect.h;
}
void GameObject::Render()
{
    SDL_RenderCopy(Game::renderer,objTexture,&srcRect,&destRect);
}
Game::Game()
{}
Game::~Game()
{}
void Game::init(const char *title,int xpos,int ypos,int width,int height,bool fullscreen)
{
    int flags=0;
    if(fullscreen)
    {
        flags=SDL_WINDOW_FULLSCREEN;
    }
    if(SDL_Init(SDL_INIT_EVERYTHING)==0)
    {
    window=SDL_CreateWindow(title,xpos,ypos,width,height,flags);
    renderer=SDL_CreateRenderer(window,-1,0);
    if(renderer)
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
    }
    isRunning=true;
    }
    //SDL_Surface *tmpSurface=IMG_Load("assets/player.png");
    //playerTex=SDL_CreateTextureFromSurface(renderer,tmpSurface);
    //SDL_FreeSurface(tmpSurface);
    //player=new GameObject("assets/player.png",50,50);
    //enemy=new GameObject("assets/enemy.png",0,0);
    map=new Map();
    newplayer.addComponent<PositionComponent>(100,500);
    //newpalyer.getComponent<PositionComponent>().SetPos(100,500);
    newplayer.addComponent<SpriteComponent>("assets/player.png");
    
}
void Game::handleEvents()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type)
    {
    case SDL_QUIT:
        isRunning=false;
        break;
    default:
        break;
    }
}
void Game::update()
{
    manager.refresh();
    manager.update();
    //map->LoadMap();
    if(player.getComponent<>().x()>100)
    {
        player.getComponent<SpriteComponent>().setTex("assets/enemy.png");
    }
}
void Game::render()
{
    SDL_RenderClear(renderer);
    //here we add stuff to render
    //SDL_RenderCopy(renderer,playerTex,NULL,&destR);
    map->DrawMap();
    manager.draw();
    SDL_RenderPresent(renderer);
}
void Game::clean()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    cout<<"game cleaned!.."<<endl;
}
Game *game=nullptr;
int main(int argc, char* argv[])
{
    const int FPS=60;
    const int framedelay=1000/FPS;
    Uint32 frameStart;
    int frameTime;
    game=new Game();
    game->init("sdl2",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,768,640,false);
    while(game->running())
    {
        frameStart=SDL_GetTicks();
        game->handleEvents();
        game->update();
        game->render();
        frameTime=SDL_GetTicks()-frameStart;
        if(framedelay>frameTime)
        {
            SDL_Delay(framedelay-frameTime);
        }
    }
    game->clean();
    return 0;
}
