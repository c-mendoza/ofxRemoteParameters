//
// Created by Cristobal Mendoza on 3/10/21.
// http://cuppetellimendoza.com
//

#ifndef OFX_REMOTE_PARAMETERS_SERVER_H
#define OFX_REMOTE_PARAMETERS_SERVER_H

#include <utils/ofXml.h>
#include "ofxOsc.h"
#include "ofPath.h"
#include "ofThread.h"
#include <unordered_map>
#include <typeindex>
#include "ofxRemoteParameters.h"

namespace ofxRemoteParameters
{
	/**
	 * @brief Serves an ofParameterGroup to a ofxRemoteParameters::Client, and reacts to messages sent to it via OSC.
	 * Note that the current implementation can respond to a single client at a time, but the Server will
	 * react to messages sent to it from any source.
	 */
	class Server
	{
	public:
		/**
		 * @brief Use this constant to exclusively set the ofLogLevel of the Server.
		 * @example ofSetLogLevel(Server::ModuleName, OF_LOG_WARNING);
		 */
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

			/**
			 * @brief These are not implemented by the Server yet.
			 */
			struct Argument
			{
				std::string identifier;
				std::string info;
				std::string type;
				std::string value;
			};

			/**
			 * @brief Not implemented by Server yet.
			 */
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
			 * @brief A friendlier name that is used in the client UI. The convention is to start with
			 * a capital letter and use regular spaces for multiple words:
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
		/**
		 * @brief Sets up and starts the Server.
		 * @param parameters The ofParameterGroup that the Server will manage. It is highly recommended that the group
		 * have a name.
		 * @param inPort The OSC input port. Defaults to ofxRemoteParameters::DefaultServerInPort.
		 * @param outPort The OSC output port. Defaults to ofxRemoteParameters::DefaultServerOutPort.
		 */
		void setup(ofParameterGroup& parameters,
				   int inPort = DefaultServerInPort,
				   int outPort = DefaultServerOutPort);

		/**
		 * @brief The Server needs to continuously check for incoming OSC messages by calling Server::update(), and this
		 * method determines whether Server should do this automatically.\line
		 * @note The auto update is done on the ofMainLoop::loopEvent, which means that there is no OpenGL context available
		 * when update() is called.
		 *
		 * @param autoUpdate
		 */
		void setAutoUpdate(bool autoUpdate);
		void sendMessage(ofxOscMessage& m);
		void sendReply(ServerMethod& method, ofxOscMessage& m);
		void addServerMethod(ServerMethod&& method);
		void setParameter(std::string path, std::string value);
		void addCustomDeserializer(std::string path,
								   std::function<void(std::string serializedString)> customDeserializer);
		void close();

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
			 * @brief Lambda that provides the minimum and maximum of the ofParameter to the addon. Normally this function is
			 * defined automatically via templates.
			 * It needs to return an std::pair with two strings, .first is the min and .second is the max.
			 * It could be something like this:
			 * @example
			 * @code
			 * TypeInfo info;
			 * info.getLimits = [](std::shared_ptr<ofAbstractParameter> parameter)
				{
					auto castParameter = std::dynamic_pointer_cast<ofParameter<ParameterType>>(parameter);
					std::pair<std::string, std::string> limits;
					limits.first = ofToString(castParameter->getMin());
					limits.second = ofToString(castParameter->getMax());
					return limits;
				};
			 * @endcode
			 */
			std::function<std::pair<std::string, std::string>(
					std::shared_ptr<ofAbstractParameter> parameter)> getLimits;
		};

		/**
		 * @brief Use this method to add Server support for additional ofParameter types. The type needs to be (de)serializable
		 * via ofToString(), which means that you need to implement the OF-friendly << and >> for the type.
		 * See ofRectangle.cpp (line 831) for an example.
		 * @tparam ParameterType The type that you will be adding to Server. The type will be wrapped by an ofParameter, so
		 * ParameterType should be something like `int` or `ofRectangle`.
		 * @param typeName The human-readable name for the type. In general, it should be the textual representation of the type name.
		 * @param hasLimits Determines if the type should have a minimum and maximum. Whether a type should have limits should be
		 * a function of the data that it represents: an int can have a minimum and a maximum, but limits for an ofColor make less sense.
		 */
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
					auto castParameter = std::dynamic_pointer_cast<ofParameter<ParameterType>>(parameter);
					std::pair<std::string, std::string> limits;
					limits.first = ofToString(castParameter->getMin());
					limits.second = ofToString(castParameter->getMax());
					return limits;
				};
			}
			auto result = typeRegistry.insert({std::type_index(typeid(ofParameter<ParameterType>)), info});
			if (!result.second) ofLogWarning("ModelServer") << "Tried to add an existing type";
		}


		/**
		 * @brief Use this method to add Server support for additional ofParameter types. It is highly recommended that you
		 * use the addParameterType(std::string typeName, bool hasLimits = false) method instead.
		 * @tparam ParameterType The type that you will be adding to Server. The type will be wrapped by an ofParameter.
		 * @param info The TypeInfo struct that describes the type.
		 */
		template<typename ParameterType>
		void addParameterType(TypeInfo info)
		{
			auto result = typeRegistry.insert({std::type_index(typeid(ofParameter<ParameterType>)), info});
			if (!result.second) ofLogWarning("ModelServer") << "Tried to add an existing type";
		}

		/**
		 * @brief Checks for incoming OSC messages. If auto updates are enabled you do not need to call this
		 * method.
		 * @see Server::setAutoUpdate(bool).
		 */
		void update();

		struct ParameterData
		{
			std::string parameterPath;
			std::string parameterValue;
		};

		/**
		 * @brief Re-sends the ofParameterGroup and ServerMethods to the connected Client. This is useful when the
		 * parameters are changed server-side and you want the Client to reflect those changes. Also useful if new
		 * paremeters are added to the ofParameterGroup.
		 * @note In most situations you do not need to call this method.
		 */
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
			auto castParameter = std::dynamic_pointer_cast<ofParameter<T>>
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

		ofThreadChannel<ParameterData> parameterThreadChannel;
		std::mutex serverMutex;
	};
}

#endif //OFX_REMOTE_PARAMETERS_SERVER_H
