#include "PreCompiled.h"

#include "PropertyDataSpecs.h"

#include "Application.h"
#include "Document.h"
#include "DocumentObject.h"

#include <Base/Console.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
FC_LOG_LEVEL_INIT("App", false, true);

TYPESYSTEM_SOURCE(App::PropertyDataSpecs, App::Property)

using namespace std;
namespace App {

    const string PropertyDataSpecs::subPropValueKey = "value";
    string PropertyDataSpecs::DataSpec::toString()const {
        string rValue = "{\"type\" : \"";
        //string
        rValue += to_string(type) + "\",\"" + subPropValueKey + "\": \"" + value + "\", \"group\" : \"" + group + "\"}";
        return rValue;
    }

    PropertyDataSpecs::DataSpec PropertyDataSpecs::DataSpec::fromString(const string&str) {
        DataSpec rValue;
        int type = -1;

        boost::property_tree::ptree dataSpecsTree;

        try {
            boost::property_tree::read_json(stringstream(str), dataSpecsTree);
            rValue.type = dataSpecsTree.get<int>("type");
            rValue.value = dataSpecsTree.get<string>(subPropValueKey);
#if 0
            switch (type) {
            case emProperryTypeString:
                break;
            default:
                FC_ERR(__FUNCTION__ << ",invalid type: " << type);
                break;
            }
#endif
            rValue.group= dataSpecsTree.get<string>("group");
            rValue.printInfo();
            
        }
        catch (std::exception& e) {
            FC_ERR(__FUNCTION__<<", " << e.what());
        }
        return rValue;
    }

    void PropertyDataSpecs::DataSpec::printInfo()const {
        FC_TRACE("type: " << type << "; vlaue: " << value << "; group : " << group);
    }


    PropertyDataSpecs::PropertyDataSpecs()
    {

    }

    PropertyDataSpecs::~PropertyDataSpecs()
    {
        for (auto adaptorPair : m_propertyAdaptors)
            delete adaptorPair.second;
    }

    //**************************************************************************
    // Base class implementer


    int PropertyDataSpecs::getSize() const
    {
        return static_cast<int>(_lValueList.size());
    }

    void PropertyDataSpecs::setValue(const std::string& key, const DataType& value)
    {
        aboutToSetValue();
        _lValueList[key] = value;
        hasSetValue();
    }

    void PropertyDataSpecs::setValues(const std::map<std::string, DataType>& map)
    {
        aboutToSetValue();
        _lValueList = map;
        hasSetValue();
    }



    const PropertyDataSpecs::DataType& PropertyDataSpecs::operator[] (const std::string& key) const
    {
        static DataType empty;
        auto it = _lValueList.find(key);
        if (it != _lValueList.end())
            return it->second;
        else
            return empty;
    }


    PyObject* PropertyDataSpecs::getPyObject()
    {
        PyObject* dict = PyDict_New();

        for (auto it = _lValueList.begin(); it != _lValueList.end(); ++it) {
#if 0
            PyObject* item = PyUnicode_DecodeUTF8(it->second.c_str(), it->second.size(), nullptr);
            if (!item) {
                Py_DECREF(dict);
                throw Base::UnicodeError("UTF8 conversion failure at PropertyDataSpecs::getPyObject()");
            }
            PyDict_SetItemString(dict, it->first.c_str(), item);
            Py_DECREF(item);
#endif
        }

        return dict;
    }

    void PropertyDataSpecs::setPyObject(PyObject* value)
    {
#if 0
        if (PyDict_Check(value)) {

            std::map<std::string, std::string> values;
            // get key and item list
            PyObject* keyList = PyDict_Keys(value);

            PyObject* itemList = PyDict_Values(value);
            Py_ssize_t nSize = PyList_Size(keyList);

            for (Py_ssize_t i = 0; i < nSize; ++i) {

                // check on the key:
                std::string keyStr;
                PyObject* key = PyList_GetItem(keyList, i);
                if (PyUnicode_Check(key)) {
                    keyStr = PyUnicode_AsUTF8(key);
                }
                else {
                    std::string error = std::string("type of the key need to be unicode or string, not");
                    error += key->ob_type->tp_name;
                    throw Base::TypeError(error);
                }

                // check on the item:
                PyObject* item = PyList_GetItem(itemList, i);
                if (PyUnicode_Check(item)) {
                    values[keyStr] = PyUnicode_AsUTF8(item);
                }
                else {
                    std::string error = std::string("type in list must be string or unicode, not ");
                    error += item->ob_type->tp_name;
                    throw Base::TypeError(error);
                }
            }

            setValues(values);
        }
        else {
            std::string error = std::string("type must be a dict object");
            error += value->ob_type->tp_name;
            throw Base::TypeError(error);
        }
#endif
    }

    unsigned int PropertyDataSpecs::getMemSize() const
    {
        size_t size = 0;
        for (auto it = _lValueList.begin(); it != _lValueList.end(); ++it) {
            size += sizeof(it->second);//if this work?
            size += it->first.size();
        }
        return size;
    }

    void PropertyDataSpecs::Save(Base::Writer& writer) const
    {
        writer.Stream() << writer.ind() << "<Map count=\"" << getSize() << "\">" << endl;
        writer.incInd();
        for (auto it = _lValueList.begin(); it != _lValueList.end(); ++it) {
            writer.Stream() << writer.ind() << "<Item key=\"" << encodeAttribute(it->first)
                << "\" value=\"" << encodeAttribute(it->second.toString()) << "\"/>" << endl;
        }

        writer.decInd();
        writer.Stream() << writer.ind() << "</Map>" << endl;
    }

    void PropertyDataSpecs::Restore(Base::XMLReader& reader)
    {
        // read my Element
        reader.readElement("Map");
        // get the value of my Attribute
        int count = reader.getAttributeAsInteger("count");

        std::map<std::string, DataType> values;
        for (int i = 0; i < count; i++) {
            reader.readElement("Item");
            const auto & dataSpecsStr = reader.getAttribute("value");
            values[reader.getAttribute("key")] = DataType::fromString(dataSpecsStr);
        }

        reader.readEndElement("Map");

        // assignment
        setValues(values);
    }

    Property* PropertyDataSpecs::Copy() const
    {
        PropertyDataSpecs* p = new PropertyDataSpecs();
        p->_lValueList = _lValueList;
        return p;
    }

    void PropertyDataSpecs::Paste(const Property& from)
    {
        aboutToSetValue();
        _lValueList = dynamic_cast<const PropertyDataSpecs&>(from)._lValueList;
        hasSetValue();
    }

    const std::map<std::string, PropertyAdaptor*>& PropertyDataSpecs::getPropertyAdaptors()const {
        return m_propertyAdaptors;
    }

    void PropertyDataSpecs::setPropertyAdaptors(std::map<std::string, PropertyAdaptor*>& adaptors) {
        m_propertyAdaptors = adaptors;//todo ”≈ªØ
    }

    std::vector<PropertyAdaptor*> PropertyDataSpecs::getAdaptorVector()const{
        std::vector<PropertyAdaptor*> adaptors;
        for (auto item : m_propertyAdaptors)
            adaptors.push_back(item.second);
        return adaptors;
    }

    TYPESYSTEM_SOURCE(App::PropertyAdaptor, App::Property)

    std::shared_ptr<PropertyAdaptor> PropertyAdaptor::fromDataSpecs(const PropertyDataSpecs::DataSpec& dataSpec) {

        return std::make_shared<PropertyAdaptor>(dataSpec.type, dataSpec.docu.c_str(), dataSpec.group.c_str(), dataSpec.value);
    }

    PropertyAdaptor::PropertyAdaptor(int type, const char* docu, const char* group, const std::string& value) :
        m_type(type), m_docu(docu), m_group(group), m_value(value)
    {
        
    }

    PropertyAdaptor::PropertyAdaptor() {

    }

    PropertyAdaptor::~PropertyAdaptor() {

    }

    void PropertyAdaptor::print() {
        FC_TRACE("type: " << m_type << "; vlaue: " << m_value << "; group : " << m_group << "; docu: " << m_docu);
    }


}//namespace App