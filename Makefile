CXXFLAGS := -std=c++20 -Wall -O3 -fPIC -flto
SRCS := \
	biquad.cpp \
	butterworth_lowpass.cpp \
	butterworth_highpass.cpp \
	delay.cpp \
	descriptor.cpp \
	gain.cpp \
	high_shelf.cpp \
	invert.cpp \
	linkwitz_riley_highpass.cpp \
	linkwitz_riley_lowpass.cpp \
	low_shelf.cpp \
	peaking.cpp \
	# end

OBJS := $(SRCS:.cpp=.o)

po-plugins.so: $(OBJS)
	$(CXX) -shared $(CXXFLAGS) -Wl,--no-undefined -o $@ $^

check: po-plugins.so
	sox -b 16 -Dr 44100 -n impulse.wav synth 1s square
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav linkwitz_riley_lowpass_2.wav po-plugins.so linkwitz_riley_lowpass_1ch 1000 2
	./analyse linkwitz_riley_lowpass_2.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav linkwitz_riley_lowpass_4.wav po-plugins.so linkwitz_riley_lowpass_1ch 1000 4
	./analyse linkwitz_riley_lowpass_4.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav linkwitz_riley_highpass_2.wav po-plugins.so linkwitz_riley_highpass_1ch 1000 2
	./analyse linkwitz_riley_highpass_2.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav linkwitz_riley_highpass_4.wav po-plugins.so linkwitz_riley_highpass_1ch 1000 4
	./analyse linkwitz_riley_highpass_4.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav low_shelf.wav po-plugins.so low_shelf_1ch 1000 -10 0.70710678
	./analyse low_shelf.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav high_shelf.wav po-plugins.so low_shelf_1ch 1000 -10 0.70710678
	./analyse high_shelf.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav peaking.wav po-plugins.so peaking_1ch 1000 -10 0.70710678
	./analyse peaking.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav delay.wav po-plugins.so delay_1ch 5
	./analyse delay.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav invert.wav po-plugins.so invert_1ch
	./analyse invert.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav gain.wav po-plugins.so gain_1ch -10
	./analyse gain.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav butterworth_lowpass_1.wav po-plugins.so butterworth_lowpass_1ch 1000 1
	./analyse butterworth_lowpass_1.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav butterworth_lowpass_2.wav po-plugins.so butterworth_lowpass_1ch 1000 2
	./analyse butterworth_lowpass_2.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav butterworth_lowpass_3.wav po-plugins.so butterworth_lowpass_1ch 1000 3
	./analyse butterworth_lowpass_3.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav butterworth_lowpass_4.wav po-plugins.so butterworth_lowpass_1ch 1000 4
	./analyse butterworth_lowpass_4.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav butterworth_highpass_1.wav po-plugins.so butterworth_highpass_1ch 1000 1
	./analyse butterworth_highpass_1.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav butterworth_highpass_2.wav po-plugins.so butterworth_highpass_1ch 1000 2
	./analyse butterworth_highpass_2.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav butterworth_highpass_3.wav po-plugins.so butterworth_highpass_1ch 1000 3
	./analyse butterworth_highpass_3.wav
	LADSPA_PATH=`pwd` applyplugin -s 5 impulse.wav butterworth_highpass_4.wav po-plugins.so butterworth_highpass_1ch 1000 4
	./analyse butterworth_highpass_4.wav

clean:
	rm -f po-plugins.so $(OBJS) *.wav *.png

