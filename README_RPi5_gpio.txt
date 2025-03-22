//////////////////////////////////////////////////////
//
// RaspberryPi5 read and measure pulsed input of GPIO13(pin#33)
// and set similar output state to GPIO27(pin#13)
// The read state is also outputed to GPIO27 to which you can connect
// e.g. a LED or oscilliscope.
// The libgpiod-dev library is used.
//
// Two files are provided:
//
// multiplepulseread.c measures the time  in microseconds which it will take 
// to measure a number(defined by PulseNumbers) of pulses (high state).
// The timing starts when the pulse changes from 0 to 1 and stops
// when is changes back from 1 to 0
//
// singlepulseread.c  counts status read loops of a single pulse
// (high state) and sets output GPIO27(pin#13) to the same state
//
// Robert J. Heerekop 2025