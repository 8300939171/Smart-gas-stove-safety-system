# Smart-gas-stove-safety-system
## Control Loop Operation

The `loop()` function is the core runtime logic of the system. It continuously monitors the supply voltage and signal condition, detects faults, updates the OLED display, controls status LEDs, and manages relay operation.

### Sensor Monitoring

The controller continuously reads:

* Supply voltage (`VCC`)
* Signal level (`SIG`)
* Signal activity status

These readings are averaged to improve stability and reduce noise.

### Fault Detection

The system checks for three possible fault conditions:

* **VCC Fault** → Triggered when supply voltage drops below the threshold.
* **SIGNAL Fault** → Triggered when signal strength is too low.
* **DEGRADED Fault** → Triggered when the signal becomes unstable or inactive.

If no abnormal condition is detected, the system remains in **SAFE** mode.

### SAFE Mode

When the system operates normally:

* Green LED is turned ON
* Relay remains OFF
* OLED displays `SAFE`

This indicates healthy operation.

### Fault Mode

If a fault is detected:

* Red LED is turned ON
* Fault type is displayed on the OLED
* User is prompted to activate bypass mode

Displayed information includes:

* `FAULT DETECTED`
* Fault type (`VCC`, `SIGNAL`, or `DEGRADED`)
* `Press for bypass`

### Bypass Mode

Bypass mode allows manual override during fault conditions.

When bypass is activated:

* Relay is forced ON
* Yellow LED is turned ON
* OLED displays `BYPASS`

This mode temporarily ignores detected faults until reset or restarted.

### Button Handling

The bypass button is monitored continuously during fault conditions.
When pressed:

* Input is debounced
* `bypassActive` flag is enabled
* System switches to bypass mode

### System Status Summary

| State  | Relay     | LED Indicator | OLED Message |
| ------ | --------- | ------------- | ------------ |
| SAFE   | OFF       | Green         | SAFE         |
| FAULT  | Protected | Red           | Fault Type   |
| BYPASS | ON        | Yellow        | BYPASS       |
