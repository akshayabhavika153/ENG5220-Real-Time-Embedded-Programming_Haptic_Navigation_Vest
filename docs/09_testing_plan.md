1. Introduction to Testing
Briefly explain:
Why testing is important for your system
What you are trying to verify
Example:
The system was tested to ensure accurate fall detection, reliable obstacle classification, and correct prioritisation of events under real-time conditions.
2. Testing Objectives
State clearly what you want to prove:
Correct detection of falls
Accurate distance classification
Proper handling of invalid sensor data
Correct priority handling (emergency overrides)
No repeated false triggers (cooldown works)
3. Types of Testing Used

a) Unit Testing
Testing individual components separately.
Examples:
Fall detection function
Distance classification function
Sensor validation

b) Integration Testing
Testing how components work together.
Examples:
Sensor → Navigation logic
Navigation → Priority manager

c) System Testing
Testing the whole system as one.
Example:
Simulating real-world usage (movement, obstacles, falls)

| Test ID | Description             | Input         | Expected Output | Actual Output  | Result |
| ------- | ----------------------- | ------------- | --------------- | -------------- | ------ |
| T1      | Normal distance reading | 2.5m          | Safe            | Safe           | Pass   |
| T2      | Near obstacle           | 0.8m          | Near alert      | Near alert     | Pass   |
| T3      | Critical obstacle       | 0.3m          | Critical alert  | Critical alert | Pass   |
| T4      | Fall detection          | Rapid drop    | Fall detected   | Fall detected  | Pass   |
| T5      | Cooldown check          | Repeated fall | Ignored         | Ignored        | Pass   |
| T6      | Invalid sensor value    | -1            | Rejected        | Rejected       | Pass   |
