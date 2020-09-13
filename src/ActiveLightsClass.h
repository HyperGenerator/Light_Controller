#pragma once
#include "WIFI_light_thread.h"

class ActiveLightsClass{
	private:
		bool threads_running;
		unsigned short max_acceptable_channels;
		std::vector<AChannel*> channel_address_map; //each channel in the list points to its address
		//---helper function
		void remap_channels() {
			channel_address_map.clear();
			unsigned short _numb_of_channels = 0;
			//---count all channels
			for (auto& _alight : active_lights) {
				_numb_of_channels += _alight.channel_list.size();
			}			
			channel_address_map.resize(max_acceptable_channels+1);

			//---fill channel_address_map
			for (short i = 0; i <= max_acceptable_channels; i++) {
				channel_address_map[i] = nullptr;
			}
			//set channels
			for (short i = 0; i < active_lights.size(); i++) {
				for (short j = 0; j < active_lights[i].channel_list.size(); j++) {
					channel_address_map[active_lights[i].channel_list[j].channel] = active_lights[i].channel_list.data() + j;
				}				
			}
			
			
		} //remaps all channels in active_lights to their addresses
	public:
		
		std::vector<ALight> active_lights;	
		std::vector<WIFI_light_thread*> active_threads;
		
		ActiveLightsClass():
			threads_running(false),
			max_acceptable_channels(512)
		{}
		void add_light_fixture(ALight& _l) {
			active_lights.push_back(_l);
			remap_channels();
		}
		
		void set_AChannel(uint8_t _channel, uint8_t _value) {

			channel_address_map[_channel]->value = _value;			
		}
		void set_AChannel(std::string& _label, uint8_t _value) {
			for (int i = 0; i < channel_address_map.size(); i++) {
				if (channel_address_map[i] != nullptr){				
					if (channel_address_map[i]->channel_name == _label)
						channel_address_map[i]->value = _value;					
				}
			}
		}
		
		int get_number_of_lights() {
			return active_lights.size();
		}
		ALight* get_Alight(std::string& _label) {
			for (short i = 0; i < active_lights.size(); i++) {
				if (active_lights[i].name == _label)
					return &active_lights[i];
			}
			return nullptr; //nothing matching found
		}
		ALight* get_Alight(int& _index) {
			return &active_lights[_index];
		}

		void start_Send_threads() {
			//--- start dongle threads| for each ip adress group create an unique thread
			if (!active_lights.empty()) {
				//---find all unique ip addresses in the Alight vector
				std::vector<std::string> _unique_ip;
				for (const auto& _alight : active_lights) {
					std::string _current_ip = _alight.ip_address;
					bool _i_can_add = true;
					for (const auto& _ip : _unique_ip) {
						if (_ip == _current_ip)
							_i_can_add = false;
					}
					if (_i_can_add)
						_unique_ip.push_back(_current_ip);
				}

				//--- add threads to list for each IP adress group
				for (const auto& _cur_ip : _unique_ip) {
					std::vector<ALight*> temp_list; //list with all pointers to the addresses of the Alight elements
					for (int i = 0; i < active_lights.size(); i++) {
						if (active_lights[i].ip_address == _cur_ip)
							temp_list.push_back(active_lights.data() + i);
					}
					WIFI_light_thread* new_thread = new WIFI_light_thread(temp_list); //new thread for the Light fixture
					active_threads.push_back(new_thread);
				}
				//--- start all threads
				std::cout <<"[Mainthread] " <<"Number of threads created: " << active_threads.size() << std::endl;
				for (const auto& _current_thread : active_threads) {
					_current_thread->startThread(true); //blocking, non-verbose
				}
				threads_running = true;
			}
		}
		void stop_Send_threads() {
			if (threads_running) {
				for (const auto& _current_thread : active_threads) {
					_current_thread->waitForThread(true);
				}
				active_threads.clear();
				threads_running = false;
				std::cout << "All threads stopped" << std::endl;
			}
		}
};
