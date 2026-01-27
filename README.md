# Rollback Platform Fighter Study

A rollback-based 2.5D platform fighter built in Unreal Engine, designed to replicate the responsive, expressive “Smash Bros.” gameplay feel while aggressively enforcing determinism for competitive online play. The project focuses on reconciling Unreal’s non-deterministic systems with the strict requirements of rollback netcode.

## Single-threaded simulation

All gameplay logic, physics, and collision run on a single deterministic update loop. No async tasks, no parallel physics, no frame-dependent ordering.

- Fixed 60Hz tick rate
- Simulation is fully decoupled from rendering. Unreal is being used for capturing input, data storage, UI, and gameplay visuals.

## Fixed-Point Math

All gameplay math uses a custom 64-bit fixed-point library.

- Trigonometry via LUT-based sine/cosine

- No IEEE 754 floats in simulation code to guarantees identical results across CPU types ie (x86 / x64 / ARM).

- Determinstic Physics
  - Calculations for Position, velocity, acceleration

- Determinstic Collision
  - Hitboxes, Hurtboxes, and Shields exclusively use Capsule vs Capsule collision.
  - Character (ECB) and environmental shapes use Polygon collisions via SAT method.

## Rollback Netcode (GGPO)

Uses GGPO-style input prediction and rollback, more technical info can be found at ggpo.net.
- Yet to be implemented

## Tech Stack
Language: C++, C#
Engine: Unreal Engine
Plugins: GGPO
