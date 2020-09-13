#pragma once
#include "ofMain.h"
#include <chrono>
#include "ofxGui.h"
#include "ofxDatGui.h"

#define NETWORK_ENABLED 0 //disable to turn off networking
#define BEATDETECTION_ENABLED 0 //disable to turn off networking

//data structures
struct ABeat {
	float bpm;
	std::time_t timestamp;
};
struct AChannel {
	std::string channel_name;
	uint8_t channel;
	uint8_t value;
};
struct ALight {
	uint8_t id;
	std::string name;
	std::string ip_address;
	unsigned short port;
	std::vector<AChannel> channel_list;
};
#include "ActiveLightsClass.h"
#include "BeatRecognitionClass.h"

class ofApp : public ofBaseApp{
	private:
	ActiveLightsClass activeLights; //handles all network communicatiuon and stores values of all channels
	BeatRecognitionClass* beatrecognition;
	//---Light fixture selector
	std::string light_fixture_selection_title = "Light fixture selection";
	ofxDatGui* light_selection_gui;


	//ofxDatGui* light_fixture_gui;
	ofxDatGui* channel_gui = nullptr;


	
	

	public:
		void setup();
		void update();
		void draw();
		void build_light_selection();
		void onChannelchange(ofxDatGuiSliderEvent& event);
		void onlight_selection(ofxDatGuiMatrixEvent event);
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		
		void exit();
		
};
