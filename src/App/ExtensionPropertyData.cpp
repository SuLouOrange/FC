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
#if 1 
		auto& adaptors = extension->PropertySpecs.getPropertyAdaptors();
		for (auto adaptorPair : adaptors)
			adaptorPair.second->setContainer(object);//不会merge但是要保证能get到container
#endif
	}

	ExtensionPropertyData::ExtensionPropertyData() {
		initExtensionType(ExtensionPropertyData::getExtensionClassTypeId());
		Extension::m_isPythonExtension = true;
		EXTENSION_ADD_PROPERTY_TYPE(PropertySpecs, (), "", PropertyType(Prop_None), "");
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
				doc = item.second.get<string>(docuKey);
				typeEm = item.second.get<int>(typeKey);
				value = "{\"Value\" : \"";
				value += item.second.get<string>(valueKey);//始终为string,需二次解析
				value += "\"}";
				auto adaptor = new PropertyAdaptor(name.c_str(), typeEm, doc.c_str(), group.c_str(), value);
				const auto& result = adaptors.emplace(name, adaptor);
				if (!result.second) {
					FC_ERR("error occur,same name: " << name);
					delete adaptor;
				}
				else {
					auto prop = result.first->second;
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