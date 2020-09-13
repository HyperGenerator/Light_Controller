#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	beatrecognition = new BeatRecognitionClass("192.168.1.34", 65432); //init beatrecognition thread


	//--test
	ALight light0 = { 0, "dmx1", "127.0.0.1", 65432, { {"red", 0, 255},{"blue", 1, 255},{"green", 0, 255},{"strobe", 1, 255}  } };
	ALight light1 = { 1, "dmx2", "192.168.1.99" ,23, { {"red", 2, 255},{"blue", 3, 255} } };
	//ALight light2 = { 2, "dmx3", "192.168.1.18" ,23, { {"violett", 4, 255},{"purple", 5, 255} } };
	//ALight light3 = { 3, "dmx4", "127.0.0.1" ,65432, { {"violett", 4, 255},{"purple", 5, 255} } };

	//activeLights.add_light_fixture(light0);
	activeLights.add_light_fixture(light0);
	//activeLights.add_light_fixture(light2);
	//activeLights.add_light_fixture(light0);
	
	activeLights.start_Send_threads();
	
	build_light_selection(); //build gui for light selection


	
	
}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	
}

//--------------------------------------------------------------
void ofApp::build_light_selection()
{
	if (light_selection_gui != nullptr)
		light_selection_gui->clear();
	light_selection_gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);

	//--- Light fixture selector build ---
	light_selection_gui->addMatrix(light_fixture_selection_title, activeLights.get_number_of_lights(), true)->setRadioMode(false);
	light_selection_gui->getMatrix(light_fixture_selection_title)->setWidth(ofGetWindowWidth() / 2 - 5, 200);

	for (int i = 0; i < activeLights.get_number_of_lights(); i++) {
		std::string _label = activeLights.get_Alight(i)->name;
		light_selection_gui->getMatrix(light_fixture_selection_title)->getButtonAtIndex(i)->setLabel(_label);
	}
	light_selection_gui->onMatrixEvent(this, &ofApp::onlight_selection);

}

//--------------------------------------------------------------
void ofApp::onChannelchange(ofxDatGuiSliderEvent& event) {
	
	std::string _label = event.target->getLabel();
	activeLights.set_AChannel(_label, event.value);
	
	/*
	std::cout << event.target->getName() << "\t";
	std::cout << event.target->getIndex() << "\t";
	std::cout << event.value << std::endl;
	*/
}
//--------------------------------------------------------------
void ofApp::onlight_selection(ofxDatGuiMatrixEvent event) {
	// --- build gui for channel manipulation 
	int index = event.child;
	std::string label = event.target->getButtonAtIndex(index)->getLabel();
	ALight* _selected_light = activeLights.get_Alight(label);
	if (channel_gui != nullptr) channel_gui->clear(); //delete previous gui
	
	channel_gui = new ofxDatGui(ofxDatGuiAnchor::BOTTOM_LEFT); //new gui
	
	for (int i = 0; i < _selected_light->channel_list.size(); i++) {
		std::string _label = _selected_light->channel_list[i].channel_name;
		channel_gui->addSlider(_label, 0, 255, 125);
		channel_gui->getSlider(_label)->setPrecision(0);
		channel_gui->getSlider(_label)->setValue(float(_selected_light->channel_list[i].value));
		
	}
	channel_gui->onSliderEvent(this, &ofApp::onChannelchange);


	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'x') {
		beatrecognition->get_answer();

		//auto t1 = std::chrono::high_resolution_clock::now();
		
		//auto t2 = std::chrono::high_resolution_clock::now();
		//std::cout << "duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
	}
	else if (key == 'y') {


	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::exit() {
	activeLights.stop_Send_threads();
	beatrecognition->~BeatRecognitionClass();
}
