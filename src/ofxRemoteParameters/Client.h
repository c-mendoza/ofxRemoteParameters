//
// Created by cmendoza on 3/9/21.
//

#ifndef OFXREMOTEPARAMETERS_CLIENT_H
#define OFXREMOTEPARAMETERS_CLIENT_H

#include <ofThread.h>
#include <unordered_map>
#include "ofxOsc.h"
#include "ofxRemoteParameters.h"

namespace ofxRemoteParameters
{
	class Client
	{
	public:
		Client();
		/**
		 * @brief Sets up a new Client;
		 * @param parameterGroup The ofParameterGroup that the client will manage. You don't need to set its name, it will
		 * assume the name of the ofParameterGroup coming from the Server.
		 * @param serverAddress The ip address of the Server.
		 * @param serverInPort The input port of the SERVER. Defaults to DefaultServerInPort.
		 * @param serverOutPort The output port of the SERVER. Defaults to DefaultServerOutPort.
		 * @return True if the OSC sender and receiver were set up succesfully, but there is no additional error checking performed
		 * (i.e. checking for incorrect ports or ip addresses).
		 */
		bool setup(ofParameterGroup& parameterGroup, std::string serverAddress, int serverInPort = DefaultServerInPort,
				   int serverOutPort = DefaultServerOutPort);
		void connect();

		/**
		 * @brief Adds support for new ofParameter types. The type must also be supported by the Server
		 * @tparam T The "inner" type for the ofParameter. i.e. for an ofParameter&lt;int&gt; you would make `T` be `int`
		 * @param typeName The human-readable name for the type. This must match the type name used by the Server.
		 */
		template<class T>
		void addParameterType(std::string typeName)
		{
			auto f = [this](std::string name, std::string value, std::string min, std::string max, ofEventListeners& el)
			{
				auto pValue = ofFromString<T>(value);
				auto param = ofParameter<T>(name, pValue);
				if (!min.empty() && !max.empty())
				{
					auto pMin = ofFromString<T>(min);
					auto pMax = ofFromString<T>(max);
					param.setMin(pMin);
					param.setMax(pMax);
				}
				auto paramPtr = param.newReference();
				el.push(param.newListener([this, paramPtr](T& value) {
					setRemoteParameter(paramPtr);
				}));
				return paramPtr;
			};

			typeConstructors[typeName] = f;
		}

		/**
		 * @brief Calls a Server Method of the given name.
		 * @param methodName The name of the method.
		 */
		void callServerMethod(std::string& methodName);

		/**
		 * @brief Use this constant to exclusively set the ofLogLevel of the Client.
		 * @example ofSetLogLevel(Client::LogModuleName, OF_LOG_WARNING);
		 */
		const static std::string LogModuleName;

		/**
		 * @brief Notifies when the model is received from the Server
		 */
		ofEvent<void> modelLoadedEvent;

	private:
		void downloadModel();

		ofxOscReceiver oscReceiver;
		ofxOscSender oscSender;
		ofEventListener loopListener;
		ofEventListeners paramListeners;
		std::shared_ptr<ofParameterGroup> parameterGroup;
		bool isConnected = false;
		std::unordered_map<std::string, std::function<std::shared_ptr<ofAbstractParameter>(
				std::string name,
				std::string value,
				std::string min,
				std::string max,
				ofEventListeners& el)>> typeConstructors;

	protected:
		void parseReceivedMessage(ofxOscMessage& message);
		void parseModel(const std::string& basicString);
		void parseGroup(ofParameterGroup& paramGroup, ofXml& groupXml);
		void setRemoteParameter(std::shared_ptr<ofAbstractParameter> p);
	};

}
#endif //OFXREMOTEPARAMETERS_CLIENT_H
