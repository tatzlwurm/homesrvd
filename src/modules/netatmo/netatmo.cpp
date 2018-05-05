#include <iostream>
#include <fstream>
#include <string>

#include <memory>

#include <algorithm>

#include "netatmo.h"



// to get the access token
// curl "https://api.netatmo.com/oauth2/token" -d "grant_type=password&client_id=5aaf718bac34a52d188b48d1&client_secret=JDmYM1eU65DQiA8dhHTAJIlTdtFZhxQwwLlN5euhq&username=stephan_psenner@yahoo.it&password=EIx_ooP0" --header "application/x-www-form-urlencoded"

//to get the values from the own station
//https://api.netatmo.com/api/getstationsdata?access_token=57d1cd786d1dbdc8938b45d4|184f34589395a182fd1f616e3574335c&device_id=70:ee:50:17:ac:1a
//from other stations
//https://api.netatmo.com/api/getpublicdata?access_token=57d1cd786d1dbdc8938b45d4|2acad91dae4e2294587109832f7755d7&lat_ne=3&lon_ne=4&lat_sw=-2&lon_sw=-2&filter=true&required_data=temperature


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}






int netatmo::readconfig() {

	Json::Value root;
	Json::CharReaderBuilder reader;

	std::ifstream jsonfile("./config/Netatmo.config.json", std::ifstream::binary);
	std::string errs;
	bool ok = Json::parseFromStream(reader, jsonfile, &root, &errs);
	if ( !ok )
	{
		// report to the user the failure and their locations in the document.
		std::cout  << errs << "\n";
		return -1;
	}

	device_id = root["NetatmoConfig"]["device_id"].asString();
	client_id = root["NetatmoConfig"]["client_id"].asString();
	client_secret = root["NetatmoConfig"]["client_secret"].asString();

	std::cout << device_id +"\n"+ client_id +"\n"+client_secret+"\n" << std::endl;
	
	config_OK = true;

	return 0;

}

int netatmo::GetAccessTokenByUserLogin(std::string &username, std::string &password) {
	
	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());

	std::string readBuffer;

	int retval;

	//read netatmo configuration
	if (!config_OK) {
		retval = readconfig();
		if (retval < 0){
			return retval;
		}
	} 
	
	
	CURL *curl;
	CURLcode res;

	/* In windows, this will init the winsock stuff */ 
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */ 
	curl = curl_easy_init();
	if(curl) {
		/* First set the URL that is about to receive our POST. This URL can
			just as well be a https:// URL if that is what should receive the
			data. */ 
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.netatmo.com/oauth2/token");
		/* Now specify the POST data */ 
		
		std::string str = "grant_type=password&client_id=" + client_id + "&client_secret=" + client_secret + "&username=" + username + "&password=" + password;

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str.c_str() );
		
		
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);



		#ifdef SKIP_PEER_VERIFICATION
			/*
			* If you want to connect to a site who isn't using a certificate that is
			* signed by one of the certs in the CA bundle you have, you can skip the
			* verification of the server's certificate. This makes the connection
			* A LOT LESS SECURE.
			*
			* If you have a CA cert for the server stored someplace else than in the
			* default bundle, then the CURLOPT_CAPATH option might come handy for
			* you.
			*/ 
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		#endif

		#ifdef SKIP_HOSTNAME_VERIFICATION
			/*
			* If the site you're connecting to uses a different host name that what
			* they have mentioned in their server certificate's commonName (or
			* subjectAltName) fields, libcurl will refuse to connect. You can skip
			* this check, but this will make the connection less secure.
			*/ 
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		#endif


		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		} else {

			std::string const mystring = readBuffer;
			char const* start = mystring.c_str();
			char const* stop = start + mystring.size();
			std::string errs;
			reader->parse(start, stop, &root, &errs);

			access_token = root["access_token"].asString();
			refresh_token = root["refresh_token"].asString();
			expires_in = root["expires_in"].asUInt();
		}
			

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return 0;
}









int netatmo::GetValue() {

	int retval;

	//read netatmo configuration
	if (!config_OK) {
		retval = readconfig();
		if (retval < 0){
			return retval;
		}
	} 
	
	getstationdata();

	return 0;
};

int netatmo::getstationdata() {


	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());

	std::string readBuffer;

	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl) {
		std::string str = "https://api.netatmo.com/api/getstationsdata?access_token=" + access_token + "&device_id=" + device_id;
		
		curl_easy_setopt(curl, CURLOPT_URL, str.c_str() );
		
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);



		#ifdef SKIP_PEER_VERIFICATION
			/*
			* If you want to connect to a site who isn't using a certificate that is
			* signed by one of the certs in the CA bundle you have, you can skip the
			* verification of the server's certificate. This makes the connection
			* A LOT LESS SECURE.
			*
			* If you have a CA cert for the server stored someplace else than in the
			* default bundle, then the CURLOPT_CAPATH option might come handy for
			* you.
			*/ 
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		#endif

		#ifdef SKIP_HOSTNAME_VERIFICATION
			/*
			* If the site you're connecting to uses a different host name that what
			* they have mentioned in their server certificate's commonName (or
			* subjectAltName) fields, libcurl will refuse to connect. You can skip
			* this check, but this will make the connection less secure.
			*/ 
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		#endif

	
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		} else {

			std::string const mystring = readBuffer;
			char const* start = mystring.c_str();
			char const* stop = start + mystring.size();
			std::string errs;
			reader->parse(start, stop, &root, &errs);

		}

		parsestationdata(readBuffer);
	
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	
	curl_global_cleanup();

	return 0;
};


int netatmo::parsestationdata(std::string &readBuffer) {


	Json::Value root;
	Json::CharReaderBuilder rbuilder;
	std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());

	std::string const mystring = readBuffer;
	char const* start = mystring.c_str();
	char const* stop = start + mystring.size();
	std::string errs;
	reader->parse(start, stop, &root, &errs);

	
	const Json::Value& devices = root["body"]["devices"];

	MainStationData.BaseStation.module_name = devices[0]["station_name"].asString();
	MainStationData.BaseStation.Temperature = devices[0]["dashboard_data"]["Temperature"].asDouble();
	MainStationData.BaseStation.Humidity = devices[0]["dashboard_data"]["Humidity"].asDouble();
	MainStationData.BaseStation.Noise = devices[0]["dashboard_data"]["Noise"].asDouble();
	MainStationData.BaseStation.Pressure = devices[0]["dashboard_data"]["Pressure"].asDouble();
	MainStationData.BaseStation.CO2 = devices[0]["dashboard_data"]["CO2"].asDouble();
	MainStationData.BaseStation.time_utc = devices[0]["dashboard_data"]["time_utc"].asDouble();

    for (unsigned int i = 0; i < devices.size(); i++){

		const Json::Value& modules = devices[i]["modules"];

    	for (unsigned int j = 0; j < modules.size(); j++){
			if (modules[j]["type"] == "NAModule1" ) {
				MainStationData.OutsideModule.module_name = modules[j]["type"].asString();
				MainStationData.OutsideModule.Temperature = modules[j]["dashboard_data"]["Temperature"].asDouble();
				MainStationData.OutsideModule.Humidity = modules[j]["dashboard_data"]["Humidity"].asDouble();
				MainStationData.OutsideModule.battery_percent = modules[j]["battery_percent"].asDouble();
				MainStationData.OutsideModule.time_utc = modules[j]["dashboard_data"]["time_utc"].asDouble();
			} else if (modules[j]["type"] == "NAModule2" ) {

				MainStationData.WindModule.module_name = modules[j]["type"].asString();

				const Json::Value& WindHistoric = modules[j]["dashboard_data"]["WindHistoric"];
				double tmpWindStrength = 0;
				unsigned int k = 0;
				for (k = 0; k < WindHistoric.size(); k++){
					tmpWindStrength = tmpWindStrength + WindHistoric[j]["WindStrength"].asDouble();
				}
				MainStationData.WindModule.WindStrength = tmpWindStrength / std::max(int(k)-1,1);
				MainStationData.WindModule.time_utc = modules[j]["dashboard_data"]["time_utc"].asDouble();
				MainStationData.WindModule.battery_percent = modules[j]["battery_percent"].asDouble();
			} else if (modules[j]["type"] == "NAModule3" ) {
				MainStationData.RainModule.module_name = modules[j]["type"].asString();
				MainStationData.RainModule.Rain = modules[j]["dashboard_data"]["Rain"].asDouble();
				MainStationData.RainModule.time_utc = modules[j]["dashboard_data"]["time_utc"].asDouble();
				MainStationData.RainModule.battery_percent = modules[j]["battery_percent"].asDouble();
			}        
		}
		
    }
	return 0;
}