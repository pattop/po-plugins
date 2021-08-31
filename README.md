# Patrick Oppenlander's LADSPA Plugins
_WARNING: These plugins are currently using LADSPA development ids which means that if you choose to try them they may clash with other plugins on your system._

## Features
* Permissive licensing
* All plugins support from 1 to 8 channels

## Plugins
Replace 'N' with the number of channels you would like to process.

| Description | Plugin Label | Control Ports |
| - | - | - |
| Butterworth Highpass<br>Butterworth Lowpass | butterworth_highpass_Nch<br>butterworth_lowpass_Nch | Cutoff Frequency (Hz)<br>Filter Order (1, 2, 3 or 4)|
| High Shelf<br>Low Shelf | high_shelf_Nch<br>low_shelf_Nch | Centre Frequency (Hz)<br>Gain (dB)<br>Bandwidth (Q) |
| Linkwitz Riley Highpass<br>Linkwitz Riley Lowpass | linkwitz_riley_highpass_Nch<br>linkwitz_riley_lowpass_Nch | Crossover Frequency (Hz)<br>Filter Order (2 or 4) |
| Peaking | peaking_Nch | Centre Frequency (Hz)<br>Gain (dB)<br>Bandwidth (Q) |
| Delay | delay_Nch | Delay (ms) |
| Gain | gain_Nch | Gain (dB) |
| Invert | invert_Nch | |
