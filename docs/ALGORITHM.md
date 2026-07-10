# Adaptive Cruise Control Algorithm

## Workflow

1. Read distance from ultrasonic sensor.
2. Compare measured distance with safety threshold.
3. Adjust PWM duty cycle based on obstacle distance.
4. Reduce vehicle speed when obstacle approaches.
5. Stop the motor if the minimum safe distance is reached.
6. Resume normal speed when the path is clear.

The algorithm continuously repeats these steps during operation.
