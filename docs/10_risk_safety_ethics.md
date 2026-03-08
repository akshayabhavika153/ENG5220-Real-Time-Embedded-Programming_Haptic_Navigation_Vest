# Risk, Safety & Ethics

## 1. Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Motor overheating on skin | Medium | High | Use driver circuit with current limiting; mount motors outside fabric |
| Battery short circuit | Low | High | Use regulated 5V supply with overcurrent protection |
| I2C bus failure | Medium | Medium | Retry up to 3 times; graceful degradation (continue without sensor) |
| False fall detection | Medium | Low | Tune thresholds to avoid false positives during normal walking |
| False obstacle alert | Low | Medium | LiDAR has ≥90% reliability; filter outlier readings |
| Electrical shock | Very Low | High | All components operate at ≤5V (safe extra-low voltage) |
| Wire detachment during use | Medium | Medium | Secure all wiring with cable ties and strain relief |

---

## 2. Safety Considerations

### Electrical Safety
- All components operate at 3.3V or 5V DC — well within safe voltage limits
- Motor driver circuit isolates Raspberry Pi GPIO from motor current
- Flyback diodes across motors prevent voltage spikes damaging the Pi
- Common ground between Pi and motor supply

### Mechanical Safety
- Vest weight target <1.5 kg — comfortable for extended wear
- No sharp edges or exposed wiring in final implementation
- Components mounted securely to prevent movement during use

### Software Safety
- Motors default to OFF on power loss (fail-safe state)
- Graceful shutdown on SIGINT to prevent state corruption
- Error handling for I2C failures — logged and skipped, not crashed

---

## 3. Ethical Considerations

### User Autonomy
- The vest is an assistive aid, not a replacement for user judgement
- Users maintain full control of their movement and decisions
- The system provides warnings, not commands

### Privacy
- No camera or audio recording — only distance and motion data
- No data stored permanently — sensor data is processed in real-time and discarded
- No network connectivity — all processing is local on the Raspberry Pi

### Accessibility
- Haptic feedback is intuitive — no training required for basic use
- Audio alerts supplement vibration for hearing-capable users
- System operates independently — no smartphone or internet required

### Inclusivity
- Designed for diverse body types — adjustable vest
- Cost target £52.40 — affordable compared to guide dogs or commercial devices
- Open source — community can modify and improve