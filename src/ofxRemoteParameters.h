//
// Created by cmendoza on 3/9/21.
//

#ifndef OFXREMOTEPARAMETERS_OFXREMOTEPARAMETERS_H
#define OFXREMOTEPARAMETERS_OFXREMOTEPARAMETERS_H

#include <string>
#include <vector>

namespace ofxRemoteParameters
{
	const std::string ApiRoot = "ofxpsMethod";
	const std::string ApiResponse = "ofxpsResponse";
	const std::string MethodGetModel = "getModel";
	const std::string MethodConnect = "connect";
	const int DefaultServerOutPort = 12001;
	const int DefaultServerInPort = 12000;

	std::vector<std::string> getLocalIPs();
}
#endif //OFXREMOTEPARAMETERS_OFXREMOTEPARAMETERS_H
