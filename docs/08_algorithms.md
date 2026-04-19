1. Time-Series Change Detection Algorithm (Fall Detection)
The system implements a time-based change detection algorithm to identify sudden falls using LiDAR sensor data. This algorithm compares the current distance reading with a previously stored value over a short time interval.
If a rapid decrease in distance is detected within a defined time window, the system interprets this as a fall event. The algorithm also incorporates a cooldown period to prevent repeated triggers from the same incident.
This approach ensures that only significant and abrupt changes in sensor readings are treated as critical events, improving reliability and reducing false positives.
How It Works
The algorithm:
Stores previous distance and timestamp
Reads current distance and timestamp
Calculates:
Time difference
Distance difference
Checks conditions:
Previous distance was high (user upright)
Current distance is very low (user close to ground)
Change occurred quickly (within ~700 ms)
If all conditions are met → fall is detected.
3. Threshold-Based Classification Algorithm
A rule-based classification algorithm is used to convert continuous distance values into discrete alert levels. The system defines multiple thresholds to categorise proximity into levels such as safe, near, and critical.
Each sensor reading is evaluated against these thresholds, and the corresponding alert level is assigned. This simplifies decision-making and allows the system to respond appropriately based on the severity of the detected obstacle.
| Distance Range | Output Level |
| > 2.0 m        | Safe         |
| 1.0–2.0 m      | Far          |
| 0.5–1.0 m      | Near         |
| < 0.5 m        | Critical     |

5. Finite State Machine (FSM)
The project uses a finite state machine to manage system behaviour under different conditions. The system operates in distinct states such as normal operation, emergency mode, and sensor error.
Transitions between these states occur based on incoming events, such as fall detection or invalid sensor readings. For example, when a fall is detected, the system transitions into an emergency state, overriding normal operations.
This structured approach ensures predictable and controlled system behaviour.
The system operates in states such as:
Normal operation
Emergency mode
Error state
Transitions occur when events happen (e.g., fall detected).
7. Priority-Based Scheduling Algorithm
A priority scheduling algorithm is used to manage multiple events and determine which action should take precedence. Events such as fall detection are assigned higher priority compared to routine navigation updates.
When multiple events occur, the system evaluates their priority levels and ensures that critical events override less important ones. This guarantees that emergency situations are handled immediately.
Each event has a priority:
High → fall detection
Medium → obstacle warning
Low → normal updates
The system processes higher-priority events first.
9. Event-Driven Algorithm (Callback Mechanism)
The system follows an event-driven architecture where components communicate through callbacks. When a sensor detects a condition, it generates an event that is passed to other modules for processing.
This design allows the system to respond dynamically to real-time inputs rather than relying on continuous polling. It also improves modularity by decoupling different parts of the system.
How It Works
Sensors generate events
Events are passed via callbacks
Other modules respond accordingly
11. Data Mapping Algorithm
A mapping algorithm is used to translate sensor inputs into meaningful directional or navigational outputs. For example, sensor directions are converted into corresponding system actions.
This ensures that raw hardware data can be effectively interpreted and utilised by the navigation logic.
12. Debouncing (Cooldown) Algorithm
To improve stability, the system implements a debouncing algorithm that introduces a cooldown period after detecting a fall event. During this period, additional triggers are ignored.
This prevents multiple alerts from being generated due to the same physical event or sensor noise.
13. Sensor Validation Algorithm
The project includes a validation algorithm to ensure that sensor readings fall within acceptable ranges. Any values outside predefined limits are treated as invalid and ignored.
This helps maintain system accuracy by filtering out erroneous or noisy data.
