## Initial Progress (week1,2)
### Initial Presentation & System Planning

### Activities
- Conducted first project presentation.
- Introduced the concept of a Haptic Navigation Vest for visually impaired individuals.
- Presented project motivation and real-world relevance.
- Discussed core features:
  - Obstacle detection
  - Fall detection
  - Stair/cliff detection
  - Vibration and audio alerts
- Identified main hardware components:
  - Raspberry Pi
  - LiDAR sensor
  - IMU (accelerometer + gyroscope)
  - Vibration motors
  - Buzzer/speaker
  - Power module
  - Motor driver circuit
- Created GitHub repository for documentation and version control.
- Structured initial documentation folders.

### Outcomes
- Project concept approved.
- Functional requirements clearly defined.
- Team responsibilities discussed and distributed.
- Basic system architecture direction decided.

### Challenges
- Understanding real-time processing requirements.
- Selecting suitable sensors within budget constraints.
- Clarifying hardware–software interaction.


## Week 3 
### Finalised Component Modules & Procurement

### Activities
- Finalised hardware components required for system implementation:
  - Vibration motors (x3)
  - LiDAR sensor (x1)
  - Speaker (x1)
  - Gyroscope + Accelerometer (IMU) (x1)
- Compared available modules based on:
  - Cost
  - Availability
  - Compatibility with Raspberry Pi
- Calculated total budget for core components.
- Ordered all selected modules.
- Updated Sensor Selection and Hardware Design documentation on GitHub.
- Continued improving documentation structure.

### Cost Summary

| Item                     | Cost (1) | Quantity | Total |
|--------------------------|----------|----------|-------|
| Vibration motor          | 1.70     | 3        | 5.10  |
| LiDAR                    | 24.00    | 1        | 24.00 |
| Speaker                  | 5.70     | 1        | 5.70  |
| Vibration motor driver   | 7.70     | 1        | 7.70  |
| Gyroscope + Accelerometer| 11.40    | 1        | 11.40 |
| **Total Cost**           |          |          | **53.90** |

### Outcomes
- All required modules identified and ordered.
- Budget confirmed within acceptable range.
- Hardware list finalized for implementation phase.

### Challenges
- Balancing cost vs sensor accuracy.
- Ensuring module compatibility with Raspberry Pi GPIO and I2C interfaces.


## Week 4 
### Raspberry Pi Setup & Documentation Development

### Activities
- Visited laboratory for initial hardware setup.
- Installed Raspberry Pi OS on microSD card.
- Configured Raspberry Pi environment:
  - Enabled SSH
  - Enabled I2C and SPI interfaces
  - Updated system packages
- Verified Raspberry Pi boot and system stability.
- Continued documentation development:
  - Updated architecture section
  - Improved real-time software design explanation
  - Structured progress log
- Resolved GitHub workflow issues (pull/rebase/push process).

### Outcomes
- Raspberry Pi successfully configured.
- Development environment prepared for sensor integration.
- Documentation repository structured and regularly updated.
