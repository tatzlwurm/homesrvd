//=================================
// include guard
#ifndef __NETATMO_H_INCLUDED__
#define __NETATMO_H_INCLUDED__


#define SKIP_PEER_VERIFICATION

//=================================
// included dependencies

#include <string>

#include <curl/curl.h>

#include <src/lib/jsoncpp/include/json/json.h>


class netatmo {

	bool config_OK;
	
	std::string device_id;
	std::string client_id;
	std::string client_secret;

	std::string access_token;
	std::string refresh_token;
	double expires_in;
	
	

	int readconfig();

    bool checkaccesstokenvalidity(); 
	int refreshaccesstoken(std::string &client_id, std::string &client_secret, std::string &username, std::string &password);
	
	int getstationdata();
	int parsestationdata(std::string &readBuffer);

  public:
	
	int GetAccessTokenByUserLogin(std::string &username, std::string &password);

	int GetValue();


	struct NetatmoBaseStation {
		std::string module_name;
		double time_utc;
		
		double Temperature;
		double Humidity;
		double Noise;
		double Pressure;
		double CO2;
	};

	struct NetatmoOutsideModule {
		std::string module_name;
		double battery_percent;
		double time_utc;
		
		double Temperature;
		double Humidity;
	};

	struct NetatmoRainModule {
		std::string module_name;
		double battery_percent;
		double time_utc;
		
		double Rain;
	};

	struct NetatmoWindModule {
		std::string module_name;
		double battery_percent;
		
		double WindAngle;
		double WindStrength;
		double time_utc;
		
	};

	struct NetatmoStationData {
		NetatmoBaseStation BaseStation;
		NetatmoOutsideModule OutsideModule;
		NetatmoRainModule RainModule;	
		NetatmoWindModule WindModule;	
	} MainStationData;
		
};



#endif // __NETATMO_H_INCLUDED__ 


