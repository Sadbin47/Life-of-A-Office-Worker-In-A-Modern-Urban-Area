# Life of an Office Worker in a Modern Urban Area

A 2D animated computer graphics project built with C++ and OpenGL/GLUT. The project follows a full office-day routine across nine connected scenes, starting from home in the morning, moving through city traffic and office activities, and ending with the return home at night.

## Features
* **Nine Connected Scenes:** A complete day-in-the-life animation from home departure to night return.
* **Urban Environment:** Roads, traffic systems, office exteriors, indoor workspaces, break areas, and home scenes.
* **Weather System:** Toggleable rain that affects outdoor scenes.
* **Day and Night Presentation:** Morning, daytime, evening, and night scenes with different sky and lighting setups.
* **Animated Vehicles and Characters:** Moving cars, office workers, parking motion, traffic flow, and interior activity.
* **Home Module Integration:** Scene 1 and Scene 9 include enhanced home visuals such as animated sky motion, improved night ambience, stars, moon, fireflies, garage interaction, and house lighting.

## Scene Flow
* **Scene 1:** Morning departure from home
* **Scene 2:** Daytime city traffic
* **Scene 3:** Arrival near the office / entry sequence
* **Scene 4:** Office working environment
* **Scene 5:** Break room / coffee break
* **Scene 6:** Presentation or meeting scene
* **Scene 7:** Evening office exit sequence
* **Scene 8:** Night traffic return
* **Scene 9:** Night arrival at home and parking in the garage

## Controls
* **[Spacebar]** - Pause / Resume the animation
* **[ N ]** - Reset the project and return to Scene 1
* **[ R ]** - Turn Rain ON
* **[ V ]** - Turn Rain OFF
* **[ Left Arrow ]** - Go to the previous scene
* **[ Right Arrow ]** - Go to the next scene
* **[ Esc ]** - Exit the application

## Technical Highlights
* Built using immediate-mode OpenGL with GLUT
* Uses reusable helpers for shapes, vehicles, buildings, trees, office props, and lighting
* Includes animated rain, cloud movement, garage-door motion, traffic movement, and night ambience effects
* Scene 2 and Scene 8 include multiple traffic vehicle silhouettes for more varied road visuals

## Project Files
* `main.cpp` - Main source file containing all scenes, rendering helpers, animation logic, and input handling
* `Life-of-A-Office-Worker-In-A-Modern-Urban-Area.cbp` - Code::Blocks project file
* `bin/` - Built executables
* `obj/` - Object files generated during builds

## How to Run This Project (Code::Blocks)

This project requires **OpenGL** and **FreeGLUT** to run.

1. Clone or copy the project to your local machine.
2. Open `Life-of-A-Office-Worker-In-A-Modern-Urban-Area.cbp` in **Code::Blocks**.
3. Make sure **FreeGLUT** is installed and configured in your compiler settings.
4. If you need to link libraries manually, go to `Settings -> Compiler -> Linker Settings` and add:
   * `opengl32`
   * `glu32`
   * `freeglut`
5. Build and run the project.

## Terminal Build Example

On Linux:

```bash
g++ main.cpp -o app -lGL -lGLU -lglut
./app
```

On macOS:

```bash
g++ main.cpp -o app -framework OpenGL -framework GLUT
./app
```

## Notes
* The project window is configured for `1280 x 720` rendering.
* Rain primarily affects outdoor scenes and is intended as a visual environment toggle.
* Scene 1 and Scene 9 are the most visually detailed home scenes in the current version.
