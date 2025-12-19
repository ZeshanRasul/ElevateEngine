# Elevate Engine - Real-Time Physics and Simulation Engine (C++)

Elevate Engine is a real-time physics and simulation engine written in C++. It is designed with a goal of being a game-ready, high-performance and physically accurate simulation engine mirroring production-level systems. Having spent significant time refreshing and building a strong foundation in Newtonian Mechanics with Principles of Physics by Halliday and Resnick, along with The Feynman Lectures, Elevate Engine was built as a practical application of the elegant Newtonian theory which underpins our universe as well as the exciting and immersive real-time applications such as games which are at the forefront of technical achievements of the interactive media industry.

## Overview

Elevate Engine provides a comprehensive suite of both foundational and advanced physics simulation features such as rigid body dynamics, collision detection and resolution, and constraints within an engine architecture with integrated user interface and tooling allowing for a wide range of customization.

Elevate is built with modularity and extensibility in mind, using a modern C++ approach which allows for easy integration of new force types, collision shapes and scene objects. Elevate served as both a learning exercise, significantly deepening my understanding and intuition of the constraints, challenges and opportunities that exist within real-time physics simulations of games, while developing my grounding in the underlying theoretical concepts.

Beyond foundational systems such as rigid body dynamics and collision detection, Elevate Engine also implements a number of advanced systems used directly in the AAA games of today such as vehicle physics (including car and aeroplane systems), ragdoll simulations and dynamic fracture destruction systems. Ian Millington's Game Physics Engine Development served as a key reference in the design and grounding of the project while opening up many opportunities for extension and enhancement.

## Project Goals

Not only did Elevate allow me to demonstrate and showcase my abilities in systems development, low-level programming and the integration of complex mathematics and physics theory, it was a core experience in enhancing my knowledge of real-world engine and simulation considerations to further my qualities as a developer in production level engines. 

The core goals of Elevate Engine were:

- Implement a robust, stable and efficient real-time physics engine in C++ utilizing modern programming practices and a low-level graphics API in OpenGL 4.6
- Gain a deeper understanding of Newtonian Mechanics and the principles of physics that underpin real-time simulations
- Further my intuition of these core Newtonian principles and real world constraints through practical, production-level implementation and integration of a wide range of complex systems
- Develop a modular and extensible architecture that allows for easy addition of new features, forces and object types
- Create a suite of tools and debugging visualizations to aid in development and testing of physics simulations

## Core Physics Systems

### Rigid Body Dynamics

- Rigid body representation with properties such as mass, inertia tensor, position, orientation, velocity and angular velocity
- Force and torque accumulation
- World and local space transformations
- Sleep and activation states for improved performance and stability


### Integration and Time Stepping

- Semi-implicit Euler integration for stable motion updates
- Fixed time step simulation loop for smooth rendering and physics updates
- Use of last frame accelerations for improved accuracy

### Collision Detection

- Support for multiple primitive collision shapes: spheres, boxes and planes with future implementation for capsules, convex hulls and meshes
- Contact point generation used for adding forces at points and realistic and accurate fracture simulations
- Penetration depth calculation for accurate collision resolution
- Broad phase and narrow phase collision detection as a work in progress with spherical BVH implementation underway
- Visual debugging of collision shapes and contact points

### Contact Resolution and Constraints

- Impulse-based collision resolution for realistic responses
- Friction and restitution handling
- Constraint solvers for joints used in ragdoll and particle spring systems

## Advanced Simulation Features

Given the ambitious nature of Elevate Engine, the project implements a number of advanced physics simulation features such as:
- Ragdoll Physics: Articulated rigid body systems with joint constraints for realistic character simulations
- Dynamic Fracture: Early stage yet fully functional dynamic fracture system using contact point to generate realistic breakage of objects
- Particle Systems: Mass-aggregate particle systems with spring constraints for pendulum and projectile simulations
- Runtime Tooling: Interactive live user interface for real-time parameter tweaking and scene manipulation (both spawning new objects and manipulating existing objects) using ImGui
- Vehicle Physics (Work in progress): Car and aeroplane simulation systems with car suspension, vehicle steering and aerodynamics including lift and thrust

## Engine Architecture

Central to the design of Elevate Engine is a modular and extensible architecture that allows for easy addition of new features, forces and object types. Key architectural components include:

- Separation of 3D rigid body and mass-aggregate particle system engines to reduce clutter
- Physics world class handling possession of physics objects, force generators and integration of physics objects
- Scene manager allowing for loading and switching between various demonstration scenes and scenarios
- Factory class used for creation of new physics objects as well as simulation specific objects such as brick walls, crate stacks and destroyable blocks
- Custom math library used for math functions and objects such as vectors, matrices and quaternions
- Game object OOP architecture allowing for easy introduction of new game object types
- Separated rendering code to allow easy implementation of new rendering APIs as well as clear separation of concerns between game logic and rendering logic

## Performance and Profiling

As Elevate Engine is a compute intensive project and requires fast and efficient processing to successfully simulate the physics within it, a number of performance considerations needed to be taken into account.
For example, given the player can shoot projectiles within a scene which can cause more computations particularly within collision detection areas of the engine, limitations needed to be placed to ensure there were only ever a certain number of projectiles in the scene at any one time. This mirrors considerations made in games throughout the years, both present leading titles and titles of the past.

Other performance considerations include:

- Ensurin collisions only register for objects that have a realistic chance of colliding
- Work underway to extend collision detection efficiency using bounding volume hierarchy broad phase and narrow phase stages to the collision detection pipeline
- Sleep and awake states for physics objects when their velocity is below a given sleep epsilon variable which is exposed to the runtime UI
- Profiling of key engine systems to identify bottlenecks and areas for optimization (to be extended to use Tracy)
- Treat physics as a priority system within the engine by reducing complexity of rendering and other computationally expensive systems
- Live update of performance metrics within the runtime UI for easy monitoring such as frame time and physics update time allowing the user to experiment with different scenarios and easily measure the impact scenarios have on performance

## Tools, Debugging and Visualization

At Elevate's core is an interactive, runtime graphical user interface, which allows the user to modify physics parameters, spawn and manipulate objects in the scene as well as load a number of different scenarios based on their requirements. 

This UI is essential to achieving the project goals, allowing for fast and efficient iteration and testing of various physics objects, both as they were being implemented and also at each individual project milestone. This GUI was created using ImGui and includes features such as:

- Real-time parameter tweaking for physics properties such as gravity, friction and restitution
- Object spawning and manipulation within the scene
- Loading of different demonstration scenarios
- Performance metrics display such as frame time and physics update time
- Visual debugging of collision shapes and contact points


## Technical Challenges and Solutions

## Example Scenarios and Demos

## Integration and Use Cases

## Build and Run Instructions

Although the Elevate Engine project is currently in the form of a Visual Studio solution, future work will include migrating to CMake for cross-platform compatibility.

To build and run Elevate Engine:

1. Clone the repository from GitHub: `git clone https://www.github.com/zeshanrasul/elevate-engine.git`
2. Open the Visual Studio solution file `ElevateEngine.sln`
3. Build the solution in either Debug or Release mode (Release mode recommended for best performance))
1. Run the executable located in the `bin` directory

## Future Work

## Acknowledgements, References, and Resources

It would be remiss of me not to acknowledge the following resources which were invaluable in the development of Elevate Engine. For anyone looking for physics engine development resources, these books, blogs and videos have been extremely useful throughout the project:

### Books

- "Game Physics Engine Development" by Ian Millington
- "Real-Time Collision Detection" by Christer Ericson
- "Physics for Game Developers" by David M. Bourg
- "Game Physics" by David H. Eberly
- "The Feynman Lectures on Physics" by Richard P. Feynman
- "Principles of Physics" by Halliday and Resnick

### Online Resources

- Pikuma 2D Game Physics series by Gustavo Pezze
- Vehicle Physics blog posts by Various Authors