//
// Created by cmendoza on 3/9/21.
//

#ifndef OFXREMOTEPARAMETERS_OFXREMOTEPARAMETERS_H
#define OFXREMOTEPARAMETERS_OFXREMOTEPARAMETERS_H

#include <string>
#include <vector>

namespace ofxRemoteParameters
{
	const std::string ApiRoot = "/ofxrpMethod";
	const std::string ApiResponse = "/ofxrpResponse";
	const std::string MethodGetModel = "getModel";
	const std::string MethodConnect = "connect";
	const std::string MethodSetParam = "set";
	const int DefaultServerOutPort = 12001;
	const int DefaultServerInPort = 12000;

	const int VersionMajor = 0;
	const int VersionMinor = 1;

}
#endif //OFXREMOTEPARAMETERS_OFXREMOTEPARAMETERS_H
