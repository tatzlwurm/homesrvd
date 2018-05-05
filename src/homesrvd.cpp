#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>

#include "modules/netatmo/netatmo.h"


std::string tmp_netatmo_username = "stephan_psenner@yahoo.it";
std::string tmp_netatmo_password = "EIx_ooP0";
    


std::mutex myMutex;




void loop(long threadid) {
	std::lock_guard<std::mutex> guard(myMutex);
	long tid;
	tid = threadid;
	std::cout << " Tread ID, " << tid << std::endl;
}


void fnetatmo(long threadid) {
	netatmo netatmo1;
	netatmo1.GetAccessTokenByUserLogin(tmp_netatmo_username,tmp_netatmo_password);
	netatmo1.GetValue();


	std::cout << "module_name : " << netatmo1.MainStationData.BaseStation.module_name << std::endl;
	std::cout << "Humidity : " << netatmo1.MainStationData.BaseStation.Humidity << std::endl;
	std::cout << "Noise : " << netatmo1.MainStationData.BaseStation.Noise << std::endl;
	std::cout << "Pressure : " << netatmo1.MainStationData.BaseStation.Pressure << std::endl;
	std::cout << "Temperature : " << netatmo1.MainStationData.BaseStation.Temperature << std::endl;
	std::cout << "time_utc : " << netatmo1.MainStationData.BaseStation.time_utc << std::endl;


	std::cout << "module_name : " << netatmo1.MainStationData.OutsideModule.module_name << std::endl;
	std::cout << "Temperature : " << netatmo1.MainStationData.OutsideModule.Temperature << std::endl;
	std::cout << "Humidity : " << netatmo1.MainStationData.OutsideModule.Humidity << std::endl;
	std::cout << "battery_percent : " << netatmo1.MainStationData.OutsideModule.battery_percent << std::endl;
	std::cout << "time_utc : " << netatmo1.MainStationData.OutsideModule.time_utc << std::endl;

	std::cout << "module_name : " << netatmo1.MainStationData.WindModule.module_name << std::endl;
	std::cout << "WindStrength : " << netatmo1.MainStationData.WindModule.WindStrength << std::endl;
	std::cout << "battery_percent : " << netatmo1.MainStationData.WindModule.battery_percent << std::endl;
	std::cout << "time_utc : " << netatmo1.MainStationData.WindModule.time_utc << std::endl;

	std::cout << "module_name : " << netatmo1.MainStationData.RainModule.module_name << std::endl;
	std::cout << "Rain : " << netatmo1.MainStationData.RainModule.Rain << std::endl;
	std::cout << "battery_percent : " << netatmo1.MainStationData.RainModule.battery_percent << std::endl;
	std::cout << "time_utc : " << netatmo1.MainStationData.RainModule.time_utc << std::endl;



}



int main() {
	
	std::thread loopthread(loop,1);
	std::thread netatmothread(fnetatmo,2);


	loopthread.join();
	netatmothread.join();

}
