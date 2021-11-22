#ifndef _PROPERTY_DATA_SPECS_
#define _PROPERTY_DATA_SPECS_
#include "Property.h"

#include <memory>
#include <string>
#include <list>
#include <vector>

namespace App {
    class AppExport PropertyDataSpecs : public Property
    {
        TYPESYSTEM_HEADER();

    public:

        /**
         * A constructor.
         * A more elaborate description of the constructor.
         */
        PropertyDataSpecs();

        /**
         * A destructor.
         * A more elaborate description of the destructor.
         */
        virtual ~PropertyDataSpecs();

        virtual int getSize(void) const;

        /** Sets the property
         */
        void setValue(void) {}
        void setValue(const std::string& key, const std::string& value);
        void setValues(const std::map<std::string, std::string>&);

        /// index operator
        const std::string& operator[] (const std::string& key) const;

        void  set1Value(const std::string& key, const std::string& value) { _lValueList.operator[] (key) = value; }

        const std::map<std::string, std::string>& getValues(void) const { return _lValueList; }

        virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::PropertyDataSpecsItem"; }

        virtual PyObject* getPyObject(void);
        virtual void setPyObject(PyObject*);

        virtual void Save(Base::Writer& writer) const;
        virtual void Restore(Base::XMLReader& reader);

        virtual Property* Copy(void) const;
        virtual void Paste(const Property& from);

        virtual unsigned int getMemSize(void) const;


    private:
        std::map<std::string, std::string> _lValueList;
    };
}//namespace App
#endif