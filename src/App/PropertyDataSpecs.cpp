#include "PreCompiled.h"

#include "PropertyDataSpecs.h"

#include "Application.h"
#include "Document.h"
#include "DocumentObject.h"

TYPESYSTEM_SOURCE(App::PropertyDataSpecs, App::Property)

using namespace std;
namespace App {
    PropertyDataSpecs::PropertyDataSpecs()
    {

    }

    PropertyDataSpecs::~PropertyDataSpecs()
    {

    }

    //**************************************************************************
    // Base class implementer


    int PropertyDataSpecs::getSize() const
    {
        return static_cast<int>(_lValueList.size());
    }

    void PropertyDataSpecs::setValue(const std::string& key, const std::string& value)
    {
        aboutToSetValue();
        _lValueList[key] = value;
        hasSetValue();
    }

    void PropertyDataSpecs::setValues(const std::map<std::string, std::string>& map)
    {
        aboutToSetValue();
        _lValueList = map;
        hasSetValue();
    }



    const std::string& PropertyDataSpecs::operator[] (const std::string& key) const
    {
        static std::string empty;
        std::map<std::string, std::string>::const_iterator it = _lValueList.find(key);
        if (it != _lValueList.end())
            return it->second;
        else
            return empty;
    }


    PyObject* PropertyDataSpecs::getPyObject()
    {
        PyObject* dict = PyDict_New();

        for (std::map<std::string, std::string>::const_iterator it = _lValueList.begin(); it != _lValueList.end(); ++it) {
            PyObject* item = PyUnicode_DecodeUTF8(it->second.c_str(), it->second.size(), nullptr);
            if (!item) {
                Py_DECREF(dict);
                throw Base::UnicodeError("UTF8 conversion failure at PropertyDataSpecs::getPyObject()");
            }
            PyDict_SetItemString(dict, it->first.c_str(), item);
            Py_DECREF(item);
        }

        return dict;
    }

    void PropertyDataSpecs::setPyObject(PyObject* value)
    {
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
    }

    unsigned int PropertyDataSpecs::getMemSize() const
    {
        size_t size = 0;
        for (std::map<std::string, std::string>::const_iterator it = _lValueList.begin(); it != _lValueList.end(); ++it) {
            size += it->second.size();
            size += it->first.size();
        }
        return size;
    }

    void PropertyDataSpecs::Save(Base::Writer& writer) const
    {
        writer.Stream() << writer.ind() << "<Map count=\"" << getSize() << "\">" << endl;
        writer.incInd();
        for (std::map<std::string, std::string>::const_iterator it = _lValueList.begin(); it != _lValueList.end(); ++it) {
            writer.Stream() << writer.ind() << "<Item key=\"" << encodeAttribute(it->first)
                << "\" value=\"" << encodeAttribute(it->second) << "\"/>" << endl;
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

        std::map<std::string, std::string> values;
        for (int i = 0; i < count; i++) {
            reader.readElement("Item");
            values[reader.getAttribute("key")] = reader.getAttribute("value");
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
}//namespace App