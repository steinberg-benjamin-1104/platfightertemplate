# Rollback Platform Fighter Study

A deterministic 2.5D platform fighter built in Unreal Engine to support GGPO-style rollback netcode.
This project focuses on eliminating non-determinism at every layer of the simulation: update order, math, physics, and collision.

## Core Determinism Goals

- Frame-perfect rollback compatibility

- Identical simulation results across machines

- Fast state save/load for resimulation

- Deterministic Simulation Model

## Single-threaded simulation

All gameplay logic, physics, and collision run on a single deterministic update loop. No async tasks, no parallel physics, no frame-dependent ordering.

- Fixed 60Hz tick rate
- Simulation is fully decoupled from rendering. Visuals interpolate; logic does not.

## Fixed-Point Math

- All gameplay math uses a custom 64-bit fixed-point library.

- No IEEE 754 floats in simulation code.

## Deterministic replacements for:

- Position, velocity, acceleration

- Knockback and movement curves

- Trigonometry via LUT-based sine/cosine

- Guarantees identical results across x86 / ARM and different compilers.

## Collision & Physics (2D)

- Designed specifically for rollback and fixed-point math.

### Capsule vs Capsule (2D)
- Primary character collision uses 2D capsules for stable movement, smooth sliding, and predictable penetration resolution.

### SAT for Convex Polygons
- Hitboxes and environmental shapes use Separating Axis Theorem with fixed-point math for deterministic overlap testing.

### Frame-discrete hit detection
- Hitboxes/hurtboxes are explicitly active or inactive per frame—no continuous overlap queries.

### Integer-only resolution
- Penetration depth and separation vectors are computed deterministically to avoid jitter and divergence during resimulation.

## Rollback Netcode (GGPO)

Uses GGPO-style input prediction and rollback, more technical info can be found at ggpo.net

## Tech Stack
Language: C++, C#
Engine: Unreal Engine
Plugins: GGPO
