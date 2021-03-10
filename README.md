# ofxParameterServer

This addon provides a a straightforward way to control `ofParameters` from clients via OSC. The main goal is to be able to manipulate `ofParameters` remotely via platform-agnostic clients, with a minimum of setup on the server side. In the typical setup, the server is the `ofApp` you want to control remotely, and the client modifies the values of the `ofParameters`. A fully-featured client written in Flutter, [name here], is provided in a separate repository.

## Compatibility
* Tested on OF 0.11.x, but should work on 0.10.x.
* Not compatible with 0.9.x and earlier.
* Tested on Mac and Linux, but should work on Windows.

## Usage
1. Declare an `ofxParameterServer`.
1. Declare an `ofParameterGroup` that will hold all of your parameters.
2. On your app's `setup()`, call `yourServerInstance.setup(yourOfParameterGroup)`.
3. And that's it! Any changes that the client makes on the parameters in the `ofParameterGroup` should be reflected in your app.

### Built-in types
ofxParameterServer comes with some built-in parameter types that it works with:
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

### Custom types
Serving custom types is a two step process:
1. Register the type using 	ofxParameterServer::addParameterType
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

Be aware that the functions need to be declared at global scope, `ofxParameterServer` might not find them.
