# Building
```bash
$ mkdir build
$ cd build
$ cmake ..
```

# Dependencies
- SDL (SDL_GPU)
- box2d
- stb_image.h

# Classes
## Game
- handles game logic
```cpp
Game();
~Game();
void Run();
```

## Platform
- handles platform specific code
```cpp
Platform();
~Platform();
SDL_Window* GetWindowHandle() const;
void HandleEvents();
bool WindowShouldClose() const;
void GetMousePosition(float* pX, float* pY, bool* clickIsPressed);
void GetWindowSize(int* pWidth, int* pHeight);
```

## Renderer
- handles batch rendering of triangles
```cpp
void Initialize(SDL_Window* pWindow, const std::span<const std::string>& texturePaths);
void Release();
static Renderer& GetInstance();
void RenderScene();
void PushTriangle(const RendererTriangle& triangle);
```

## Physics
- handles 2d physics of all entities, being a thin wrapper around box2d
```cpp
Physics();
void Update(float timestep);
PhysicsRigidBox CreateBox(b2Vec2 position, b2Vec2 size, bool dynamic = false);
PhysicsRigidCircle CreateCircle(b2Vec2 position, float radius, bool dynamic = false);
PhysicsSoftBody CreateSoftBody(
    b2Vec2 position,
    const std::span<const b2Vec2>& vertices,
    const std::span<const PhysicsSoftBodyJointConn>& jointConns);
```

## Entity
```cpp
Entity(Platform& platform, Physics& physics, unsigned int texIdx);
virtual void Render() = 0;
virtual void Update() = 0;
```
- base class for all entities
    - `Player`
    - `Wall`
    - `Enemy`
    - `Bullet`

# Exceptions
## RendererException
- rendering related errors
## FilesystemException
- file-not-found errors

# Templates
## SmartPtr<T>
```cpp
SmartPtr(T* pPtr);
~SmartPtr();
T* operator->();
T* GetRawPtr();
```
- frees the memory automatically when it gets out of scope

# Design patterns
## Singleton
- The `Renderer` class
- can be cleanly accessed from anywhere after initialization
## Factory
- The `EntityFactory` class
```cpp
EntityFactory(Platform& platform, Physics& physics);
Entity* MakePlayer();
Entity* MakeEnemy(b2Vec2 pos);
Entity* MakeWall(b2Vec2 pos, b2Vec2 size);
Entity* MakeBullet(b2Vec2 pos, b2Vec2 dir);
```
## Object pool
```cpp
std::vector<SmartPtr<Entity>> objects;
```
- allows easy memory management

# Operators
```cpp
b2Vec2 operator+(b2Vec2 left, b2Vec2 right);
```

# Resources
- https://wiki.libsdl.org/SDL3/CategoryGPU
- https://box2d.org/
- https://stackoverflow.com/

