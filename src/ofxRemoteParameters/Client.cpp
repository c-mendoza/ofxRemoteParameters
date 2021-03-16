
#include <ofAppRunner.h>
#include <ofXml.h>
#include <ofQuaternion.h>
#include <ofMatrix3x3.h>
#include <ofMatrix4x4.h>
#include "ofVectorMath.h" // For the << and >> operators of vector types
#include "Client.h"

using namespace ofxRemoteParameters;

const std::string Client::LogModuleName = "ofxRemoteParameters::Client";

Client::Client()
{
	addParameterType<int>("int");
	addParameterType<float>("float");
	addParameterType<double>("double");
	addParameterType<ofColor>("color");
	addParameterType<ofFloatColor>("floatColor");
	addParameterType<ofParameterGroup>("group");
	addParameterType<std::string>("string");
	addParameterType<bool>("boolean");
	addParameterType<glm::vec2>("vec2");
	addParameterType<glm::vec3>("vec3");
	addParameterType<glm::vec4>("vec4");
	addParameterType<ofRectangle>("ofRectangle");
	addParameterType<ofQuaternion>("ofQuaternion");
	addParameterType<ofMatrix3x3>("ofMatrix3x3");
	addParameterType<ofMatrix4x4>("ofMatrix4x4");
}

bool Client::setup(ofParameterGroup& parameterGroup, std::string serverAddress, int serverInPort, int serverOutPort)
{
	this->parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameterGroup.newReference());

	bool success = oscSender.setup(serverAddress, serverInPort);

	if (success)
	{
		ofxOscReceiverSettings settings;
		settings.port = serverOutPort;
		settings.reuse = true;
		settings.start = true;
		success = oscReceiver.setup(settings);
	}
	else
	{
		ofLogError(LogModuleName) << "OSC Sender setup failed! Address: " << serverAddress << " Port: " << serverInPort;
		return success;
	}

	if (!success)
	{
		ofLogError(LogModuleName) << "OSC Receiver setup failed!" << "Port: " << serverOutPort;
		return success;
	}

	loopListener = ofGetMainLoop()->loopEvent.newListener([this](){
		ofxOscMessage message;
		while(oscReceiver.hasWaitingMessages())
		{
			oscReceiver.getNextMessage(message);
			parseReceivedMessage(message);
		}
	});
	return success;
}

void Client::parseReceivedMessage(ofxOscMessage& m)
{
	ofLogVerbose(LogModuleName) << "RECEIVED MESSAGE ----";
	ofLogVerbose(LogModuleName) << "address = " << m.getAddress();
	for (int i = 0; i < m.getNumArgs(); i++)
	{
		ofLogVerbose(LogModuleName) << "arg " << i << " = " << m.getArgAsString(i);
	}
	ofLogVerbose(LogModuleName) << "---------------------";
	auto components = ofSplitString(m.getAddress(),
									ofToString('/'),
									false,
									true);

	if (components.size() < 3) return;

	// Only pay attention to messages that are responses:
	if (components[1] == ApiResponse.substr(1))
	{
		// Handle Connect:
		if (components[2] == MethodConnect)
		{
			// Call connect, set vars
			isConnected = true;
			downloadModel();

		}
		// Handle getModel:
		else if (components[2] == MethodGetModel)
		{
			// Parse XML, populate group
			if (isConnected)
			{
				parseModel(m.getArgAsString(0));
			}
			else
			{
				ofLogError(LogModuleName) << "You must call connect() before getting the model from the server";
			}
		}
		else
		{
			ofLogNotice(LogModuleName) << "Method response: " << components[2];
		}
	}
}

void Client::connect()
{
	ofxOscMessage message;
	message.setAddress(ApiRoot + "/" + MethodConnect);
	message.addStringArg("");
	oscSender.sendMessage(message, false);
}

void Client::downloadModel()
{
	ofxOscMessage message;
	message.setAddress(ApiRoot + "/" + MethodGetModel);
	oscSender.sendMessage(message, false);
}

void Client::parseModel(std::string xmlModel)
{
	ofXml xml;
	xml.parse(xmlModel);
	auto paramsXml = xml.findFirst("//ofxRemoteParameters/Parameters");
	if (paramsXml)
	{
		while(parameterGroup->size() > 0)
		{
			parameterGroup->remove(parameterGroup->size()-1);
		}

		paramListeners.unsubscribeAll();

		// There should be only one child here, and it should be a group
		for (auto& child : paramsXml.getChildren())
		{
			// Should be "group"
			auto type = child.getAttribute("type").getValue();
			if (type == "group")
			{
				parameterGroup->setName(child.getAttribute("name").getValue());
				parseGroup(parameterGroup->castGroup(), child);
			}
		}
	}

	// Parse Methods
	auto methodsXml = xml.findFirst("//ofxRemoteParameters/Methods");
	if (methodsXml)
	{

	}
	modelLoadedEvent.notify();
}

void Client::parseGroup(ofParameterGroup& paramGroup, ofXml& groupXml)
{

	for (auto& child : groupXml.getChildren())
	{
		auto type = child.getAttribute("type").getValue();
		try
		{
			auto tc = typeConstructors.at(type);
			auto name = child.getAttribute("name").getValue();
			if (type != "group")
			{
				auto value = child.getChild("value").getValue();
				std::string min, max;
				if (auto node = child.getChild("min"))
				{
					min = node.getValue();
				}

				if (auto node = child.getChild("max"))
				{
					max = node.getValue();
				}
				auto paramPtr = tc(name, value, min, max, paramListeners);
				paramGroup.add(*paramPtr);
			}
			else
			{
				auto subGroup = ofParameterGroup(name);
				parseGroup(subGroup, child);
				paramGroup.add(subGroup);
			}

		}
		catch (std::out_of_range& range)
		{
			ofLogWarning(LogModuleName) << "Type not found: " << type;
		}

	}
}

void Client::setRemoteParameter(std::shared_ptr<ofAbstractParameter> p)
{
	std::stringstream paramPath;
	auto names = p->getGroupHierarchyNames();
	auto index = ofFind(names, parameterGroup->getEscapedName());

	// ofxGui inserts an ofParameterGroup as a parent of any group you pass it,
	// so we traverse the hierarchy until we find the group name we are tracking:
	for (int i = index; i < names.size(); i++)
	{
		paramPath << "/" << names[i];
	}

	ofxOscMessage message;
	message.setAddress(ApiRoot + "/" + MethodSetParam);
	message.addStringArg(paramPath.str());
	message.addStringArg(p->toString());

	ofLogVerbose(LogModuleName) << " Setting param: " << paramPath.str() << " " << p->toString();

	oscSender.sendMessage(message, false);
}

void Client::callServerMethod(std::string& methodName)
{
	ofxOscMessage message;
	message.setAddress(ApiRoot + "/" + methodName);
	ofLogVerbose(LogModuleName) << "Calling server method: " << methodName;
	oscSender.sendMessage(message, false);
}