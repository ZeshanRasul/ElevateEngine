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
- Runtime Tooling: In-engine user interface for real-time parameter tweaking and scene manipulation (both spawning new objects and manipulating existing objects) using ImGui
- Vehicle Physics (Work in progress): Car and aeroplane simulation systems with car suspension, vehicle steering and aerodynamics including lift and thrust

## Engine Architecture

## Validation and Physical Correctness

## Performance and Profiling

## Tools, Debugging and Visualization

## Technical Challenges and Solutions

## Example Scenarios and Demos

## Integration and Use Cases

## Build and Run Instructions

## Future Work

## Acknowledgements, References, and Resources