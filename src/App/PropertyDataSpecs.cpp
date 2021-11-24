#include "PreCompiled.h"

#include "PropertyDataSpecs.h"

#include "Application.h"
#include "Document.h"
#include "DocumentObject.h"

#include <Base/Console.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include <boosst/json.hpp>  解析起来应该更快，需要1.75版Boost

#include <cstring>

FC_LOG_LEVEL_INIT("App", false, true);

TYPESYSTEM_SOURCE(App::PropertyDataSpecs, App::Property)

using namespace std;
namespace App {
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
        return static_cast<int>(m_propertyAdaptors.size());
    }

#if 0
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
#endif

    PyObject* PropertyDataSpecs::getPyObject()
    {
        PyObject* dict = PyDict_New();
#if 0
        for (auto it = _lValueList.begin(); it != _lValueList.end(); ++it) {

            PyObject* item = PyUnicode_DecodeUTF8(it->second.c_str(), it->second.size(), nullptr);
            if (!item) {
                Py_DECREF(dict);
                throw Base::UnicodeError("UTF8 conversion failure at PropertyDataSpecs::getPyObject()");
            }
            PyDict_SetItemString(dict, it->first.c_str(), item);
            Py_DECREF(item);

        }
#endif

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


    //大概值即可 参考 App::Property:getMemSize()说明
    unsigned int PropertyDataSpecs::getMemSize() const
    {
        size_t size = 0;
        for (auto& adaptorPair : m_propertyAdaptors) {
            size += sizeof(adaptorPair.second);//if this work?
            size += adaptorPair.first.size();
        }
        return size;
    }

    void PropertyDataSpecs::Save(Base::Writer& writer) const
    {
        writer.Stream() << writer.ind() << "<Map count=\"" << getSize() << "\">" << endl;
        writer.incInd();
        for (auto & adaptorPair:m_propertyAdaptors) {
            writer.Stream() << writer.ind() << "<Item key=\"" << encodeAttribute(adaptorPair.first)
                << "\" value=\"" << encodeAttribute(adaptorPair.second->getValueAsString()) << "\"/>" << endl;
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
        const auto adaptorSize = m_propertyAdaptors.size();
        if (adaptorSize != count) {
            FC_WARN("sum of properties aren't same! adaptor : " << adaptorSize << "; properties in document : " << count
                << "; object name : " << getContainer()->getFullName());
        }
        const auto endIt = m_propertyAdaptors.end();
        for (int i = 0; i < count; i++) {
            reader.readElement("Item");
            const auto& key = reader.getAttribute("key");
            const auto it = m_propertyAdaptors.find(key);
            if (it  == endIt) {
                FC_WARN("missing property : " << key << "; object name : " << getContainer()->getFullName());
                continue;
            }
            const auto& dataSpecsStr = reader.getAttribute("value");
            it->second->setValueByString(dataSpecsStr);
        }

        reader.readEndElement("Map");
    }

    //todo:
    Property* PropertyDataSpecs::Copy() const
    {
        PropertyDataSpecs* p = new PropertyDataSpecs();
        //p->_lValueList = _lValueList;
        return p;
    }

    //todo:
    void PropertyDataSpecs::Paste(const Property& from)
    {
        aboutToSetValue();
        //_lValueList = dynamic_cast<const PropertyDataSpecs&>(from)._lValueList;
        hasSetValue();
    }

    const std::map<std::string, PropertyAdaptor*>& PropertyDataSpecs::getPropertyAdaptors()const {
        return m_propertyAdaptors;
    }

    void PropertyDataSpecs::setPropertyAdaptors(std::map<std::string, PropertyAdaptor*>& adaptors) {
        m_propertyAdaptors = adaptors;//todo 优化
    }

    std::vector<PropertyAdaptor*> PropertyDataSpecs::getAdaptorVector()const{
        std::vector<PropertyAdaptor*> adaptors;
        for (auto item : m_propertyAdaptors)
            adaptors.push_back(item.second);
        return adaptors;
    }

    TYPESYSTEM_SOURCE(App::PropertyAdaptor, App::Property)

    PropertyAdaptor::PropertyAdaptor(const char* name , int type, const char* docu, const char* group, const std::string& value) :
        m_type(type),  m_value(value)
    {
        auto length = strlen(name);
        myName = new char[++length];
        memcpy((void*)myName, name, length);

        length = strlen(docu);
        m_docu = new char[++length];
        memcpy((void*)m_docu, docu, length);

        length = strlen(group);
        m_group = new char[++length];
        memcpy((void*)m_group, group, length);
    }

    PropertyAdaptor::~PropertyAdaptor() {
        delete[]m_docu;
        delete[]m_group;
        delete[]myName;
    }

    PropertyAdaptor::PropertyAdaptor() {

    }
#if  USE_TEMPLATE
    template<typename T>
    T PropertyAdaptor::getValueByType()const {
        boost::property_tree::ptree valueTree;
        try {
            stringstream streamInput(m_value);
            boost::property_tree::read_json(streamInput, valueTree);
            return valueTree.get<T>("Value");
        }
        catch (std::exception& e) {
            FC_ERR(__FUNCTION__ << e.what());
        }
    }
#endif

#if 1
    void PropertyAdaptor::setValueByString(const std::string& str) {
        boost::property_tree::ptree valueTree;
        stringstream outputStr;
        try {
            valueTree.put("Value", str);
            boost::property_tree::write_json(outputStr, valueTree);
            m_value = outputStr.str();
        }
        catch (std::exception& e) {
            FC_ERR(__FUNCTION__ << e.what());
        }
    }

    std::string PropertyAdaptor::getValueAsString()const {
        boost::property_tree::ptree valueTree;
        try {
            stringstream streamInput(m_value);
            boost::property_tree::read_json(streamInput, valueTree);
            return valueTree.get<std::string>("Value");
        }
        catch (std::exception& e) {
            FC_ERR(__FUNCTION__ << e.what());
        }
    }
#endif

    

    void PropertyAdaptor::print() {
        FC_TRACE("type: " << m_type << "; vlaue: " << m_value << "; group : " << m_group << "; docu: " << m_docu);
    }

    unsigned int PropertyAdaptor::getMemSize(void) const {
        size_t size = 0;
        size += m_value.size();
        //to do
        return size;
    }


}//namespace App