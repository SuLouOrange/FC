#include "PreCompiled.h"

#include "ExtensionPropertyData.h"
#include "DocumentObject.h"

EXTENSION_PROPERTY_SOURCE(App::ExtensionPropertyData,App::Extension);
using namespace std;
namespace App{
	void ExtensionPropertyData::addPropertiesOnObject(App::DocumentObject* object) {
		auto extension = new ExtensionPropertyData();
		if (!object)
			throw(string("object is null"));
		extension->initExtension(object);
	}

	ExtensionPropertyData::ExtensionPropertyData() {
		initExtensionType(ExtensionPropertyData::getExtensionClassTypeId());
		Extension::m_isPythonExtension = true;
		EXTENSION_ADD_PROPERTY_TYPE(PropertySpecs, (), "", PropertyType(Prop_None), "");

		map<string, PropertyDataSpecs::DataType> propertySpecs; ;
		propertySpecs.insert({ "SingleBuildingName", {PropertyDataSpecs::emProperryTypeString,"thsBuildings","Specialty"} });
		propertySpecs.insert({ "Model",               {PropertyDataSpecs::emProperryTypeString,"basic001","Specialty"} });
		PropertySpecs.setValues(propertySpecs);
	};

	ExtensionPropertyData::~ExtensionPropertyData() {

	};

}//naemspace App