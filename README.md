Deterministic Rollback Platform Fighter Study
A high-performance platform fighter engine built from the ground up with a focus on frame-perfect determinism and low-latency online play. This project serves as a technical demonstration of custom fixed-point physics, manual memory management for state-saving, and the integration of rollback netcode.

Technical Core Pillars
1. Deterministic Fixed-Point Math
To ensure cross-platform synchronization and prevent desyncs in a rollback environment, the engine eschews standard floating-point arithmetic in favor of a custom Fixed-Point Math Library.

Precision Control: Implements 64-bit fixed-point integers to handle positions, velocities, and acceleration.

Consistency: Guarantees identical simulation results across different CPU architectures (x86 vs. ARM) by avoiding IEEE 754 floating-point non-determinism.

Custom Trigonometry: Includes LUT (Look-Up Table) based sine and cosine functions for deterministic knockback trajectories and angle calculations.

2. Rollback Netcode (GGPO)
The networking layer utilizes the GGPO (Good Game Peace Out) protocol to provide a lag-free experience, hiding latency through predictive input and state resimulation.

State Snapshotting: The engine is architected to separate "View" (rendering) from "Simulation" (logic). This allows the simulation state to be serialized and "rolled back" instantly.

Input Prediction: Implements local input prediction to provide 0ms perceived latency for the player.

Resimulation Loop: When a remote input conflict is detected, the engine can roll back up to 7 frames and resimulate the physics and collision logic in a single frame.

3. Fixed-Point Collision & Physics
A custom collision detection system built specifically to interface with the fixed-point library, optimized for the high-speed movement characteristic of platform fighters.

AABB & Slopes: Support for Axis-Aligned Bounding Boxes with specialized handling for varied terrain and platform passthroughs.

Frame-Perfect Hitboxes: Uses a frame-discrete "Active/Inactive" system for hitboxes and hurtboxes, ensuring that interactions occur exactly when the visual data suggests.

Deterministic Resolvers: Collision resolution logic that avoids "jitter" by using integer-based penetration depth calculations.

Architecture Overview
Simulation/Logic: Completely decoupled from the frame rate. The game logic runs at a locked 60Hz tick rate.

Memory Management: Utilizes pre-allocated memory pools for game states to make "Save/Load" operations for GGPO extremely fast (O(1) or O(n) shallow copies).

Rendering: An interpolation layer sits between the fixed-point simulation and the floating-point renderer, ensuring smooth visuals without sacrificing logic determinism.

Tech Stack
Language: [e.g., C++ / C# / Rust]

Networking: GGPO (Good Game Peace Out)

Math: Custom Fixed-Point Library

Physics: Custom Deterministic Solver
