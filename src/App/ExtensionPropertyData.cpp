#include "PreCompiled.h"

#include "ExtensionPropertyData.h"
#include "DocumentObject.h"
#include <Base/Console.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

FC_LOG_LEVEL_INIT("App", false, true);

EXTENSION_PROPERTY_SOURCE(App::ExtensionPropertyData,App::Extension);
using namespace std;

namespace App{
	static const  string path = "E:/repo/openSource/build_ptreeJson/FamilyInstanceParams.json";
	static const  string nameKey = "Name";
	static const  string typeKey = "Type";
	static const  string valueKey = "Value";
	static const  string groupKey = "Group";
	static const  string docuKey = "Docu";
	void ExtensionPropertyData::addPropertiesOnObject(App::DocumentObject* object) {
		auto extension = new ExtensionPropertyData();
		if (!object)
			throw(string("object is null"));
		extension->initExtension(object);
		auto& adaptors = extension->PropertySpecs.getPropertyAdaptors();
		for (auto adaptorPair : adaptors)
			adaptorPair.second->setContainer(object);//不会merge但是要保证能get到container
	}

	ExtensionPropertyData::ExtensionPropertyData() {
		initExtensionType(ExtensionPropertyData::getExtensionClassTypeId());
		Extension::m_isPythonExtension = true;
		EXTENSION_ADD_PROPERTY_TYPE(PropertySpecs, (), "", PropertyType(Prop_None), "");

#if 0
		map<string, PropertyDataSpecs::DataType> propertySpecs; ;
		propertySpecs.insert({ "SingleBuildingName", {PropertyDataSpecs::emProperryTypeString,"{ \"value\" : \"thsBuildings\"}","Specialty"} });
		propertySpecs.insert({ "Model",               {PropertyDataSpecs::emProperryTypeString,"{\"value\" : \"basic001\"}","Specialty"} });
#endif
		map<string,PropertyAdaptor*> adaptors;
		boost::property_tree::ptree properTree;
		try {
			boost::property_tree::read_json(path, properTree);

			auto & dataSpecsTree = properTree.get_child("ParameterSpecs");
			string name, group, doc, value;
			int typeEm = -1;

			for (auto& item : dataSpecsTree) {
				name = item.second.get<string>(nameKey);
				group = item.second.get<string>(groupKey);
				doc = item.second.get<string>(groupKey);
				typeEm = item.second.get<int>(typeKey);
				const auto & result = adaptors.emplace(name, new PropertyAdaptor(typeEm, doc.c_str(), group.c_str(), value));
				if (!result.second) {
					FC_ERR("error occur,same name: " << name);
				}
				else {
					auto prop = result.first->second;
					prop->myName = name.c_str();
					prop->print();
				}
			}
		}
		catch (std::exception& e) {
			FC_ERR(__FUNCTION__ << e.what());
		}

		PropertySpecs.setPropertyAdaptors(adaptors);
	};

	ExtensionPropertyData::~ExtensionPropertyData() {

	};

}//naemspace App