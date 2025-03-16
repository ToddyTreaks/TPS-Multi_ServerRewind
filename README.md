# Unreal Engine Multiplayer TPS Project

## Overview
This project is a **third-person shooter (TPS) multiplayer game** developed in Unreal Engine, where I implemented fundamental mechanics such as:

- **Weapon Pickup**: Players can grab and switch weapons.
- **Shooting & Aiming**: Weapons can be fired with aiming mechanics.
- **Replication**: All core gameplay features are synchronized across **server and clients**.
- **Server Rewind System**: Implements **frame package-based lag compensation**, ensuring fair hit detection in high-latency scenarios.

I built this project to **explore multiplayer programming in Unreal Engine** and to deepen my understanding of:

- **How lag affects multiplayer gameplay and player experience.**
- **Lag compensation techniques**, including **client-side prediction and server-side rewind**.
- **How to create a fair online competitive game** by balancing latency and accuracy.
- **Optimizing network performance** to ensure smooth multiplayer interactions.

The project follows the teachings from the course:
> *"Create a fast-paced, competitive multiplayer shooter in Unreal Engine 5 using C++!"* by **Stephen Ulibarri**
---
## Features Implemented

### **Multiplayer Mechanics**
- **Online Matchmaking & Session Management**: Players can create and join online sessions.
- **Replication of Player Actions**: Movement, animations, shooting, and aiming are fully synchronized across the network.
- **Network-Optimized Shooting Mechanics**: Ensuring accurate and fair hit registration with minimal bandwidth usage.
- **Real-time Movement Synchronization**: Implemented smooth player movement with network prediction to reduce perceived lag.

### **Lag Compensation & Optimization**
- **Client-Side Prediction**: Ensures smooth movement for players with high latency.
- **Server-Side Rewind System**: Uses **frame packages** to verify hit accuracy based on past player positions.
- **Optimized Network Performance**: Reducing unnecessary bandwidth consumption while maintaining responsiveness.

### **Game Systems & UI**
- **Custom Game Modes & Match States**
    - **Warmup Phase**: Players can move freely before the match starts.
    - **Main Game Phase**: Competitive elimination-based gameplay.
    - **Cooldown Phase**: Displays match results and winners.
- **HUD Elements**
    - Health bar
    - Game timer
    - Ammo count

### **Online Subsystem Integration**
- Utilized Unreal Engine’s **Online Subsystem** for multiplayer connectivity.
- Custom-built **plugin** to streamline multiplayer integration into other projects.
- Compatible with online services like **Steam** for session hosting.
---
## What I Learned
Through this project, I gained valuable experience in:

- **Unreal Engine Multiplayer Networking** – Implementing replication, Remote Procedure Calls (RPCs), and server authority logic.  
- **Network Optimization Techniques** – Minimizing bandwidth usage while ensuring smooth gameplay.  
- **Server-Side Rewind & Lag Compensation** – Creating a system that fairly validates hits in high-latency conditions.  
- **Multiplayer Game Class Structure** – Properly using **GameMode, GameState, PlayerState, Character, PlayerController, and GameInstance** for a networked game.  
- **Fairness in Competitive Multiplayer Games** – Balancing the trade-off between responsiveness and accuracy.
---
## Course Reference
This project follows core lessons from Stephen Ulibarri's course, including:
- **Matchmaking & session management**
- **Multiplayer movement & combat systems**
- **Replication & networking fundamentals**
- **Client-side prediction & server-side rewind**
- **Performance optimization for competitive shooters**


---

This project has been a **stepping stone in my journey into advanced multiplayer development**, giving me hands-on experience with **Unreal Engine's networking architecture** while ensuring smooth and fair gameplay for all players. I now have a much deeper understanding of what makes a multiplayer shooter both **technically robust and enjoyable to play**!

