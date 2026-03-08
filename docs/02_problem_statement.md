# Problem Statement

## Context

According to the World Health Organization, approximately **2.2 billion people** worldwide have some form of vision impairment, with around **39 million classified as blind**. Navigating unfamiliar or dynamic environments remains one of the most significant challenges for visually impaired individuals. Traditional aids such as white canes and guide dogs, while effective in many scenarios, have notable limitations:

- **White canes** only detect obstacles at ground level and within arm's reach, offering no protection against head-height or mid-body obstacles.
- **Guide dogs** require extensive training, are expensive to maintain, and are not accessible to everyone.
- **GPS-based navigation apps** provide turn-by-turn directions but do not detect immediate physical hazards such as obstacles, stairs, or sudden drops.

There is a clear need for a **low-cost, wearable, real-time assistive device** that can detect obstacles in multiple directions and communicate hazard information to the user through intuitive, non-visual feedback.

## Problem Definition

Visually impaired individuals face the following critical navigation challenges:

1. **Obstacle Collision** — Objects at various heights (e.g., poles, overhanging branches, furniture) are not detected by traditional canes.
2. **Falls** — Sudden trips, slips, or loss of balance can cause serious injury, especially for elderly users.
3. **Stairs and Drop-offs** — Staircases, curbs, and cliffs represent dangerous elevation changes that are difficult to sense without vision.
4. **Delayed Reaction** — Without real-time awareness, users cannot react quickly enough to avoid hazards.

## Proposed Solution

The **Haptic Navigation Vest** is a wearable embedded system that addresses these challenges by:

- Using a **LiDAR sensor** to detect obstacles in the user's forward path with centimetre-level precision
- Using an **IMU (accelerometer + gyroscope)** to detect falls, tilts, and stair/cliff scenarios
- Providing **haptic feedback** (vibration motors on the vest) that conveys the direction and proximity of hazards
- Providing **audio alerts** (buzzer/speaker) for critical hazard warnings (e.g., imminent fall, cliff edge)
- Processing all data in **real-time on a Raspberry Pi** with low response latency (<100 ms target)

## Target Users

| User Group                            | Needs                                                   |
|---------------------------------------|---------------------------------------------------------|
| Visually impaired individuals         | Real-time obstacle and hazard detection while walking    |
| Elderly with vision deterioration     | Fall detection and prevention alerts                     |
| Rehabilitation patients               | Confidence and safety during mobility exercises          |
| Caregivers / support workers          | Peace of mind that the user has hazard awareness         |

## Success Criteria

The project will be considered successful if the Haptic Navigation Vest:

1. Detects obstacles within 0.3–4 m range with ≥90% reliability
2. Detects fall events with ≤500 ms response time
3. Provides haptic feedback that is perceptible and directionally intuitive
4. Operates continuously for ≥2 hours on portable power
5. Achieves end-to-end (sense-to-feedback) latency of <100 ms
6. Is comfortable and lightweight enough for daily wearable use

## Scope

### In Scope
- Obstacle detection (forward-facing LiDAR)
- Fall detection (IMU)
- Stair/cliff detection (downward IMU pitch or secondary sensor)
- Haptic motor feedback (3 motors, directional)
- Audio alerts (buzzer)
- Raspberry Pi-based processing
- CMake build system with unit tests

### Out of Scope (Future Work)
- Full 360° obstacle detection (multiple LiDAR sensors)
- GPS-based turn-by-turn navigation
- Smartphone app integration
- Machine learning-based object recognition
- Commercial product certification

## References

<!-- TODO: Add your actual references here -->
- World Health Organization — Vision Impairment Fact Sheet
- Prior work on assistive wearable devices for the visually impaired
- Raspberry Pi Foundation documentation
- Sensor datasheets (LiDAR, IMU)
