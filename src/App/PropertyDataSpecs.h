#ifndef _PROPERTY_DATA_SPECS_
#define _PROPERTY_DATA_SPECS_
#include "Property.h"

#include <memory>
#include <string>
#include <list>
#include <vector>

namespace App {
    

    /*
    *PropertyAdaptor{
    * type:int
    * value :string
    * group: 未保存在这里
    * getValue();//向item提供数据
    * 
    * 
    *}
    */

    class PropertyAdaptor;

    class AppExport PropertyDataSpecs : public Property
    {
        TYPESYSTEM_HEADER();

    public:

        enum {
            emProperryTypeString,
            emProperryTypeSum,
        };

        static const std::string subPropValueKey;
        struct AppExport DataSpec {
            int type = -1;
            std::string value;
            std::string group;
            std::string docu;
            std::string toString()const;
            static DataSpec fromString(const std::string & str);
            void printInfo()const;
        };

        typedef DataSpec DataType;

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
        void setValue(const std::string& key, const DataType& value);
        void setValues(const std::map<std::string, DataType>&);

        /// index operator
        const DataType& operator[] (const std::string& key) const;

        void  set1Value(const std::string& key, const DataType value) { _lValueList.operator[] (key) = value; }

        const std::map<std::string, DataType>& getValues(void) const { return _lValueList; }

        virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::PropertyDataSpecsItem"; }

        virtual PyObject* getPyObject(void);
        virtual void setPyObject(PyObject*);

        virtual void Save(Base::Writer& writer) const;
        virtual void Restore(Base::XMLReader& reader);

        virtual Property* Copy(void) const;
        virtual void Paste(const Property& from);

        virtual unsigned int getMemSize(void) const;

        const std::map<std::string, PropertyAdaptor*>& getPropertyAdaptors()const;
        void setPropertyAdaptors(std::map<std::string, PropertyAdaptor*>& adaptors);

        std::vector<PropertyAdaptor*> getAdaptorVector()const;

    private:
        std::map<std::string, DataType> _lValueList;
        std::map<std::string,PropertyAdaptor*> m_propertyAdaptors;

    };

    class AppExport PropertyAdaptor : public Property {
        TYPESYSTEM_HEADER();
    public:
        static std::shared_ptr<PropertyAdaptor>  fromDataSpecs(const PropertyDataSpecs::DataSpec & sataSpec);
        PropertyAdaptor(int type,const char * docu,const char * group,const std::string & value);
        PropertyAdaptor();
        ~PropertyAdaptor();
        int getDataType()const { return m_type; };
        void setDataType(int type) { m_type = type; };
        const std::string& getStrValue() const { return m_value; }
        const char* getGroup()const { return m_group; }
        const char* getDocumentation()const { return m_group; }
        Property* Copy(void) const override { return nullptr; };
        virtual void Paste(const Property& from) override { return; };
        void Save(Base::Writer& writer)const override {};
        void Restore(Base::XMLReader& reader) override {};
        virtual const char* getEditorName(void) const { return "Gui::PropertyEditor::PropertyAdaptorItem"; }
        void print();

    private:
        int m_type = -1;
        const char* m_docu = nullptr;//tool tip
        const char* m_group = nullptr;
        std::string m_value;
    };
}//namespace App
#endif