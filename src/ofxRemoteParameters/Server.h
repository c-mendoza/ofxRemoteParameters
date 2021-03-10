//
// Created by Cristobal Mendoza on 2/18/20.
//

#ifndef OFX_REMOTE_PARAMETERS_SERVER_H
#define OFX_REMOTE_PARAMETERS_SERVER_H

#include <utils/ofXml.h>
#include "ofxOsc.h"
#include "ofPath.h"
#include "ofThread.h"
#include <unordered_map>
#include <typeindex>

/**
 * @brief Types are deserialized by OF with ofFromString, which calls the << operator
 */

namespace ofxRemoteParameters
{
	static const std::string ApiRoot = "ofxpsMethod";
	static const std::string ApiResponse = "ofxpsResponse";

	class Server : public ofThread
	{
	public:

		static const std::string ModuleName;
		static const std::string AttributeName_UiName;
		static const std::string AttributeName_Name;
		static const std::string AttributeName_Type;
		static const std::string NodeName_Value;
		static const std::string NodeName_Min;
		static const std::string NodeName_Max;

		class ServerMethod
		{
		public:
			ServerMethod(std::string ident,
						 std::string uiName,
						 std::function<void(ServerMethod&, ofxOscMessage&, Server&)> action)
			{
				identifier = ident;
				this->action = action;
				this->uiName = uiName;
			}

			struct Argument
			{
				std::string identifier;
				std::string info;
				std::string type;
				std::string value;
			};

			void addArgument(Argument& arg)
			{
				arguments.push_back(arg);
			}

			void execute(ofxOscMessage& message, Server& server)
			{
				action(*this, message, server);
			}


		protected:
			/**
			 * @brief The name of the method, used for calling its action.
			 * This name needs to be legal for use as an OSC path.
			 */
			std::string identifier;

			/**
			 * @brief A friendlier name that is used in the mobile UI. For the moment this name
			 * is used by buttons. The convention is to start with a capital letter and
			 * use regular spaces for multiple words:
			 * yes:  Create mask
			 * no:   createMask
			 */
			std::string uiName;

			/**
			* @brief The action that the method performs once called. The function parameters
			* are:
			* ServerMethod&: A reference to the ServerMethod that called this action.
			* ofxOscMessage&: A reference to the OSC Message that called this method.
			* ModelServer&: A reference to the ofxParameterServer that received the message and
			* dispatched the method.
			**/
			std::function<void(ServerMethod&, ofxOscMessage&, Server&)> action;

			std::vector<Argument> arguments;
		public:
			const std::string& getIdentifier() const
			{
				return identifier;
			}

			const std::string& getUiName() const
			{
				return uiName;
			}

		};

		Server();
		~Server();
		void setup(ofParameterGroup& parameters,
				   int inPort = 12000,
				   int outPort = 12001);
		void threadedFunction();
		void sendMessage(ofxOscMessage& m);
		void sendReply(ServerMethod& method, ofxOscMessage& m);
		void addServerMethod(ServerMethod&& method);
		void setParameter(std::string path, std::string value);
		void addCustomDeserializer(std::string path,
								   std::function<void(std::string serializedString)> customDeserializer);

		struct TypeInfo
		{
			/**
			 * @brief The human-readable name for the type. It will be wrapped by an ofParameter
			 */
			std::string name;
			/**
			 * @brief Set to true if the ofParameter has a minimum and maximum
			 */
			bool hasLimits;
			/**
			 * @brief Lambda that provides the minimum and maximum of the ofParameter to the addon.
			 * It needs to return an std::pair with two strings, .first is the min and .second is the max.
			 * It could be something like this:
			 *
			 * TypeInfo info;
			 * info.getLimits = [](std::shared_ptr<ofAbstractParameter> parameter)
				{
					auto castParameter = std::dynamic_pointer_cast<ofParameter<ParameterType>>(parameter);
					std::pair<std::string, std::string> limits;
					limits.first = ofToString(castParameter->getMin());
					limits.second = ofToString(castParameter->getMax());
					return limits;
				};
			 */
			std::function<std::pair<std::string, std::string>(
					std::shared_ptr<ofAbstractParameter> parameter)> getLimits;
		};

		template<typename ParameterType>
		void addParameterType(std::string typeName, bool hasLimits = false)
		{
			TypeInfo info;
			info.name = typeName;
			info.hasLimits = hasLimits;
			if (hasLimits)
			{
				// This lambda casts the parameter to its actual type and provides min and max.
				// It is called by serializeParameter()
				info.getLimits = [](std::shared_ptr<ofAbstractParameter> parameter)
				{
					auto castParameter = std::dynamic_pointer_cast<ofParameter < ParameterType>>
					(parameter);
					std::pair<std::string, std::string> limits;
					limits.first = ofToString(castParameter->getMin());
					limits.second = ofToString(castParameter->getMax());
					return limits;
				};
			}
			auto result = typeRegistry.insert({std::type_index(typeid(ofParameter < ParameterType > )), info});
			if (!result.second) ofLogWarning("ModelServer") << "Tried to add an existing type";
		}

		template<typename ParameterType>
		void addParameterType(TypeInfo info)
		{
			auto result = typeRegistry.insert({std::type_index(typeid(ofParameter < ParameterType > )), info});
			if (!result.second) ofLogWarning("ModelServer") << "Tried to add an existing type";
		}

		void update();

		struct ParameterData
		{
			std::string parameterPath;
			std::string parameterValue;
		};

		void syncParameters();

	private:
		ofXml createMetaModel();
		bool sendMetaModel();
		void parseMessage(ofxOscMessage& m);
		void serializeParameterGroup(std::shared_ptr<ofParameterGroup> params, ofXml& xml);
		void serializeParameter(std::shared_ptr<ofAbstractParameter> parameter, ofXml& xml);
		void serializeMethods(ofXml& xml);
		int inPort;
		int outPort;
		ofxOscSender oscSender;
		ofxOscReceiver oscReceiver;
		std::shared_ptr<ofParameterGroup> group;
		std::unique_ptr<of::priv::AbstractEventToken> loopListener;

		/// {type hash code, type friendly name}

		std::unordered_map<std::type_index, TypeInfo> typeRegistry = {};

		std::unordered_map<std::string, std::function<void(std::string)>> customDeserializers = {};

		std::unordered_map<std::string, ServerMethod> serverMethods;

		template<typename T>
		bool isParameterType(std::shared_ptr<ofAbstractParameter> parameter)
		{
			auto castParameter = std::dynamic_pointer_cast<ofParameter < T>>
			(parameter);
			if (castParameter)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		std::pair<bool, std::shared_ptr<ofAbstractParameter>>
		findParamWithEscapedName(ofParameterGroup& groupToSearch, std::string escapedName);

		bool useCustomDeserializer(const std::string& path, const std::string& value) const;

		ofThreadChannel <ParameterData> parameterThreadChannel;
		std::mutex serverMutex;
	};
}
//std::ostream& operator<<(std::ostream& os, const ofPath& path);
//std::istream& operator>>(std::istream& is, ofPath& path);

#endif //OFX_REMOTE_PARAMETERS_SERVER_H
