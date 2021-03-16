# ofxRemoteParameters

This [openFrameworks](https://openframeworks.cc) addon provides a a straightforward way to remotely manipulate `ofParameters` via an OSC connection. It uses a client-server architecture, where the server is the `ofApp` you want to control remotely, and the client modifies the values of the `ofParameters`. The addon focuses on the following:
* Minimal code to get it running: You just call a setup function, and for most situations that is all you need to do.
* Straightforward extensibility: Adding new data types is possible and simple.
* Platform-independent clients: Because it uses OSC, any client that follows the simple protocol can interface with your ofApp.
* BYO UI: In OF you can use ofxGui or ofxImGui, which is just a simple matter of passing the ofParameterGroup to either of those UI addons.

## Overview
The addon has two classes, which are independent from one another:
* `ofxRemoteParameters::Server`, which serves a "model" that includes the ofParameterGroup.
* `ofxRemoteParameters::Client`, which is an OF client implementation.

The Server communicates to bi-directionally via OSC, and defines a simple API that is used to request data and handle responses. Upon connection, a client requests the model from the server; the model is an XML representation of an ofParameterGroup, with additional metadata for each ofParameter specifying type, name, minimum and maximum values, in addition to the value itself. Because of this, clients do not need to know the structure of the ofParameterGroup ahead of time, and can instead recreate it based on the sent metadata.

Once a client has the model representation, it can use it to create a local clone so that it can be manipulated. Changes can then be sent to the server.

The Server can also respond to messages that can modify the model unidirectionally (See Server API).

## Compatibility
* Tested on OF 0.11.x, but should work on 0.10.x.
* Probably not compatible with 0.9.x and earlier.
* Tested on Mac and Linux, but should work on Windows.

## Quick Start
1. Compile and run the `basic_server` example. Leave it running.
2. Locate the `of_client` example. In `ofApp.cpp`, change the value of `SERVER_IP_ADDRESS` to the ip address of the server. Compile and run.
1. In the of_client app, press the "connect" ofxGui button.
1. If the connection is successful, `of_client` should receive and display the parameters of `basic_server`. Changing the parameters in `of_client` and they should change in `basic_server`!

## Usage
### Server
1. Declare an `ofxRemoteParameters::Server`.
1. Declare an `ofParameterGroup` that will hold all of your parameters.
2. On your app's `setup()`, call `yourServerInstance.setup(yourOfParameterGroup)`.
3. And that's it! Any changes that the client makes on the parameters in the `ofParameterGroup` should be reflected in your app.

### OF Client
1. Declare an `ofxRemoteParameters::Client`
1. Declare an `ofParameterGroup` that will mirror the remote parameters.
1. Call `myClient.setup(parameterGroup, serverAddress)`
1. Call `myClient.connect()`
1. Done! Now you can feed the ofParameterGroup to ofxGui or [ofxImGui](https://github.com/jvcleave/ofxImGui/) to manipulate the parameters and they will be synced with the Server's!

### Built-in Types
The Server comes with some built-in parameter types that it works with:
* int
* float
* double
* bool
* std::string
* ofFloatColor
* ofParameterGroup
* glm::vec2
* glm::vec3
* glm::vec4
* ofRectangle
* ofQuaternion
* ofMatrix3x3
* ofMatrix4x4

### Custom Types
Serving custom types is a two step process:
1. Register the type using 	Server::addParameterType:
```C++
myServer.addParameterType<myType>("myType");
```
2. Make your custom type (de)serializable with `ofToString`. As an example, this is how OF (de)serializes) `ofRectangle`:
```C++
ostream& operator<<(ostream& os, const ofRectangle& rect){
	os << rect.position << ", " << rect.width << ", " << rect.height;
	return os;
}
istream& operator>>(istream& is, ofRectangle& rect){
	is >> rect.position;
	is.ignore(2);
	is >> rect.width;
	is.ignore(2);
	is >> rect.height;
	return is;
}
```
Thus, the general approach is:
```C++
inline std::ostream& operator<<(std::ostream& os, const customType& instance)
{
	os << [[some function that turns instance into a string]]
	return os;
}
inline std::istream& operator>>(std::istream& is, customType& instance)
{
  ... (get values from the stream and assign them to your instance)
	return is;
}
```
Be aware that the functions need to be declared at global scope, otherwise `ofxRemoteParameters` might not find them.

The client needs to be aware of the new type as well. The provided `ofxRemoteParameters::Client` also has an `addParameterType<T>(string)` method that works in a similar way to Server's. See the examples for more details.

## Server API
The Server uses a simple API for communication via OSC, which you can use to build your own client. To talk to the Server, your OSC message should have the following address:

`/ofxrpMethod/(methodId)`

The Server will respond with the following:

`/ofxrpResponse/(medthodId)`

Any response payload will be in the OSC message arguments.

There are 3 built-in methods:
#### connect
Send this to the Server to register yourself as the Client.
<br>Outbound OSC Arguments: none.
<br>Response OSC Argument 0: string "OK" if successful.

#### getModel
Send this after calling `connect` to retrieve the ofParameterGroup from the server in XML format. See below for the XML format of the response.
<br>Outbound OSC Arguments: none.
<br>Response OSC Argument 0: a string (XML) representation of the model.

#### set
Send this to set the value of an ofParameter.
<br>Outbound OSC Argument 0: Parameter path as a string.
<br>Outbound OSC Argument 1: Parameter value as a string.
<br>Response: none.

### Model XML
The model is sent as an XML string.
The basic format is as follows:
```XML
<ofxRemoteParameters>
	<Parameters>
		... (parameters go here)
	</Parameters>
	<Methods>
		... (server methods go here)
	</Methods>
</ofxRemoteParameters>
```
#### Parameter XML Format
```xml
<Escaped_Param_Name type="type name" name="Non-escaped parameter name">
	<value>(the parameter value as a string)</value>
	<min>(optional minimum)</min>
	<max>(optional maximum)</max>
</Escaped_Param_Name>
```

#### Server Method XML Format
```xml
	<methodName uiName="A friendlier name" />
```

#### Complete XML Example
```xml
<ofxRemoteParameters>
	<Parameters>
		<Parameter_Server_Simple_Example type="group" name="Parameter Server Simple Example">
			<Circle_Radius type="float" name="Circle Radius">
				<value>10</value>
				<min>1</min>
				<max>500</max>
			</Circle_Radius>
			<Circle_Color type="color" name="Circle Color">
				<value>255, 165, 0, 255</value>
			</Circle_Color>
			<Circle_Position type="vec2" name="Circle Position">
				<value>200, 200</value>
				<min>0, 0</min>
				<max>1024, 768</max>
			</Circle_Position>
		</Parameter_Server_Simple_Example>
	</Parameters>
	<Methods>
		<set uiName="Set parameter" />
		<connect uiName="Connect" />
		<getModel uiName="Get model" />
	</Methods>
</ofxRemoteParameters>
```
