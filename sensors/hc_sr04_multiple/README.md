In this example, we have multiple HC-SR04 sensors (as many as the digital pins are available with interrupt processing).

For a Nano, this number is about 6 if you use D0 to D12 (D13 is the default LED pin). Each sensor requires two digital pins to work.

The QPN framework has a state machine limit of 7, so 6 + 1 measurement tracking statemachine is what you can do with this sensor array.