#include "PreCompiled.h"

#include "ExtensionPropertyData.h"
#include "DocumentObject.h"
#include "Document.h"
#include "Property.h"
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
				extension->dynamicProps.addDynamicProperty(*object, getCharMem(typeStr), getCharMem(name), getCharMem(group), getCharMem(doc),\
					                            App::Prop_NoRecompute);//to do more
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
		if (isSaveOrRestore())
			return;
		return dynamicProps.getPropertyMap(Map);
	}

	void ExtensionPropertyData::extensionGetPropertyList(std::vector<Property*>& List) const {
		if (isSaveOrRestore())
			return;
		return dynamicProps.getPropertyList(List);
	}

	Property* ExtensionPropertyData::extensionGetPropertyByName(const char* name) const  {
		return dynamicProps.getDynamicPropertyByName(name);
	}

	const char* ExtensionPropertyData::extensionGetPropertyName(const Property* prop) const {
		return dynamicProps.getPropertyName(prop);
	}

	short ExtensionPropertyData::extensionGetPropertyType(const Property* prop) const {
		const auto status = dynamicProps.getPropertyType(prop);
		FC_TRACE(__FUNCTION__ << ", type :" << status << ", name: " << prop->getName());
		return status;
	}

	short ExtensionPropertyData::extensionGetPropertyType(const char* name) const {
		return dynamicProps.getPropertyType(name);
	}

	const char * ExtensionPropertyData::extensionGetPropertyGroup(const Property* prop) const {
		return dynamicProps.getPropertyGroup(prop);
	}

	const char* ExtensionPropertyData::extensionGetPropertyGroup(const char* name) const {
		return dynamicProps.getPropertyGroup(name);
	}

	const char* ExtensionPropertyData::extensionGetPropertyDocumentation(const Property* prop) const {
		return dynamicProps.getPropertyDocumentation(prop);
	}

	const char* ExtensionPropertyData::extensionGetPropertyDocumentation(const char* name) const {
		return dynamicProps.getPropertyDocumentation(name);
	}


	//参考void PropertyContainer::Save (Base::Writer &writer) 
	void ExtensionPropertyData::extensionSave(Base::Writer& writer) const {
		std::map<std::string, Property*> Map;
		dynamicProps.getPropertyMap(Map);

		writer.incInd(); // indentation for 'Properties Count'
		writer.Stream() << writer.ind() << "<Properties Count=\"" << Map.size()
			<< "\" TransientCount=\"" << 0 << "\">" << endl;
	

		//std::vector<Property*> transients;
		for (auto it = Map.begin(); it != Map.end();) {
			auto prop = it->second;
			if (prop->testStatus(Property::PropNoPersist)) {
				it = Map.erase(it);
				continue;
			}
			++it;
		}

		// store normal properties
		for (auto it = Map.begin(); it != Map.end(); ++it)
		{
			writer.incInd(); // indentation for 'Property name'
			writer.Stream() << writer.ind() << "<Property name=\"" << it->first << "\" type=\""
				<< it->second->getTypeId().getName();

			dynamicProps.save(it->second, writer);//only PropData,无值

			auto status = it->second->getStatus();
			if (status)
				writer.Stream() << "\" status=\"" << status;
			writer.Stream() << "\">";

			if (it->second->testStatus(Property::Transient)
				|| it->second->getType() & Prop_Transient)
			{
				writer.decInd();
				writer.Stream() << "</Property>" << std::endl;
				continue;
			}

			writer.Stream() << std::endl;

			writer.incInd(); // indentation for the actual property

			try {
				// We must make sure to handle all exceptions accordingly so that
				// the project file doesn't get invalidated. In the error case this
				// means to proceed instead of aborting the write operation.
				it->second->Save(writer);
			}
			catch (const Base::Exception& e) {
				Base::Console().Error("%s\n", e.what());
			}
			catch (const std::exception& e) {
				Base::Console().Error("%s\n", e.what());
			}
			catch (const char* e) {
				Base::Console().Error("%s\n", e);
			}
#ifndef FC_DEBUG
			catch (...) {
				Base::Console().Error("PropertyContainer::Save: Unknown C++ exception thrown. Try to continue...\n");
			}
#endif
			writer.decInd(); // indentation for the actual property
			writer.Stream() << writer.ind() << "</Property>" << endl;
			writer.decInd(); // indentation for 'Property name'
		}
		writer.Stream() << writer.ind() << "</Properties>" << endl;
		writer.decInd(); // indentation for 'Properties Count'
	}

	//参考void PropertyContainer::Restore(Base::XMLReader& reader)
	void ExtensionPropertyData::extensionRestore(Base::XMLReader& reader) {
		reader.clearPartialRestoreProperty();
		reader.readElement("Properties");
		int Cnt = reader.getAttributeAsInteger("Count");

#if 0
		int transientCount = 0;
		if (reader.hasAttribute("TransientCount"))
			transientCount = reader.getAttributeAsUnsigned("TransientCount");
		FC_TRACE("Cnt from reader" << Cnt);
		FC_TRACE("transientCount from reader: " << transientCount);
		FC_TRACE("PropertyContainer::dynamicProps Of doccument size: " << dynamicProps.size());
		FC_TRACE("\n\n read _Property(transient) start *********");
		for (int i = 0; i < transientCount; ++i) {
			reader.readElement("_Property");
			Property* prop = getPropertyByName(reader.getAttribute("name"));
			if (prop)
				FC_TRACE("restore transient '" << prop->getName() << "'");
			if (prop && reader.hasAttribute("status"))
				prop->setStatusValue(reader.getAttributeAsUnsigned("status"));
		}
		FC_TRACE("read _Property(transient) end*********");
#endif
		FC_TRACE("\n\n read Property start *********");
		auto container = getExtendedContainer();
		if (!container) {
			FC_ERR(__FUNCTION__ << " invalid container");
			return;
		}
		for (int i = 0; i < Cnt; i++) {
			reader.readElement("Property");
			std::string PropName = reader.getAttribute("name");
			std::string TypeName = reader.getAttribute("type");
			FC_TRACE("loop " << i << ", propName:" << PropName << "; TypeName: " << TypeName);
			App::Property* prop = nullptr;
			if (!dynamicProps.getDynamicPropertyByName(PropName.c_str())) {
				prop = dynamicProps.restore(*container, PropName.c_str(), TypeName.c_str(), reader);
			}
#if 0
			if (!prop) {
				FC_TRACE("dynamicProps.restore() failed,then  try to get from memory");
				prop = dynamicProps.getDynamicPropertyByName(PropName.c_str());
				if (!prop)
					FC_TRACE("get prop from memory failed");
				else
					FC_TRACE("get prop from memory success");
			}
			else
				FC_TRACE("dynamicProps.restore() get a valid property");
#endif
			std::bitset<32> status;
			if (reader.hasAttribute("status")) {
				status = decltype(status)(reader.getAttributeAsUnsigned("status"));
#if 1
				if (prop)
					prop->setStatusValue(status.to_ulong());
#endif
			}
			
		
			// NOTE: We must also check the type of the current property because a
			// subclass of PropertyContainer might change the type of a property but
			// not its name. In this case we would force to read-in a wrong property
			// type and the behaviour would be undefined.
			try {
				// name and type match
				if (prop && strcmp(prop->getTypeId().getName(), TypeName.c_str()) == 0) {
					if (!prop->testStatus(Property::Transient)
						&& !status.test(Property::Transient)
						&& !status.test(Property::PropTransient)
						&& !(dynamicProps.getPropertyType(prop) & Prop_Transient))
					{
						FC_TRACE("restore property '" << prop->getName() << "'");
						prop->Restore(reader);
					}
					else
						FC_TRACE("skip transient '" << prop->getName() << "'");
				}
#if 0
				// name matches but not the type
				else if (prop) {
					handleChangedPropertyType(reader, TypeName.c_str(), prop);
				}
				// name doesn't match, the sub-class then has to know
				// if the property has been renamed or removed
				else {
					handleChangedPropertyName(reader, TypeName.c_str(), PropName.c_str());
				}
#endif
				if (reader.testStatus(Base::XMLReader::ReaderStatus::PartialRestoreInProperty)) {
					Base::Console().Error("Property %s of type %s was subject to a partial restore.\n", PropName.c_str(), TypeName.c_str());
					reader.clearPartialRestoreProperty();
				}
			}
			catch (const Base::XMLParseException&) {
				throw; // re-throw
			}
			catch (const Base::RestoreError&) {
				reader.setPartialRestore(true);
				reader.clearPartialRestoreProperty();
				Base::Console().Error("Property %s of type %s was subject to a partial restore.\n", PropName.c_str(), TypeName.c_str());
			}
			catch (const Base::Exception& e) {
				Base::Console().Error("%s\n", e.what());
			}
			catch (const std::exception& e) {
				Base::Console().Error("%s\n", e.what());
			}
			catch (const char* e) {
				Base::Console().Error("%s\n", e);
			}
#ifndef FC_DEBUG
			catch (...) {
				Base::Console().Error("PropertyContainer::Restore: Unknown C++ exception thrown\n");
			}
#endif
			reader.readEndElement("Property");
		}
		FC_TRACE("read Property end *********");
		reader.readEndElement("Properties");
	}

	bool ExtensionPropertyData::isSaveOrRestore()const {
		auto container = dynamic_cast<const App::DocumentObject*>(getExtendedContainer());
		if (container) {
			auto document = container->getDocument();
			if (document && (document->testStatus(App::Document::Saving) || container->testStatus(App::ObjectStatus::Restore)))
				return true;
		}
		return false;
	}
}//naemspace App