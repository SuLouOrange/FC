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

	static const char* getCharMem(const string& str);
	const char* getCharMem(const string & str) {
		const char* originChar = str.c_str();
		auto length = strlen(originChar);
		const char* charMem = new char[++length];
		memcpy((void*)charMem, originChar, length);
		return charMem;
	}

	void ExtensionPropertyData::addPropertiesOnObject(App::DocumentObject* object) {
		auto extension = new ExtensionPropertyData();
		if (!object)
			throw(string("object is null"));
		extension->initExtension(object);
		boost::property_tree::ptree properTree;
		try {
			boost::property_tree::read_json(path, properTree);

			auto& dataSpecsTree = properTree.get_child("ParameterSpecs");
			string name, group, doc, value, typeStr;
			int typeEm = -1;
		//	string typeStr;
			for (auto& item : dataSpecsTree) {
				name = item.second.get<string>(nameKey);
				group = item.second.get<string>(groupKey);
				doc = item.second.get<string>(docuKey);
				typeStr = item.second.get<string>(typeKey);
				//value = "{\"Value\" : \""; \
				value += item.second.get<string>(valueKey);//始终为string,需二次解析 \
				value += "\"}";

				//addDynamicProperty(PropertyContainer &pc, const char* type, const char* name=0, const char* group=0,\
				const char* doc = 0, short attr = 0, bool ro = false, bool hidden = false);
				extension->dynamicProps.addDynamicProperty(*object, getCharMem(typeStr), getCharMem(name), getCharMem(group), getCharMem(doc)\
				);//to do more
			}
		}
		catch (std::exception& e) {
			FC_ERR(__FUNCTION__ << e.what());
		}
	}

	ExtensionPropertyData::ExtensionPropertyData() {
		initExtensionType(ExtensionPropertyData::getExtensionClassTypeId());
		Extension::m_isPythonExtension = true;
	};

	ExtensionPropertyData::~ExtensionPropertyData() {
		//charMem destructor todo;

	};

	void ExtensionPropertyData::extensionGetPropertyMap(std::map<std::string, Property*>& Map) const {
		return dynamicProps.getPropertyMap(Map);
	}

	void ExtensionPropertyData::extensionGetPropertyList(std::vector<Property*>& List) const {
		return dynamicProps.getPropertyList(List);
	}

	Property* ExtensionPropertyData::extensionGetPropertyByName(const char* name) const  {
		return dynamicProps.getDynamicPropertyByName(name);
	}

}//naemspace App