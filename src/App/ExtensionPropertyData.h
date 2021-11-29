#ifndef _APP_EXTENSION_PROPERTY_DATA_H_
#define _APP_EXTENSION_PROPERTY_DATA_H_
#include "Extension.h"
#include "DynamicProperty.h"
namespace App {
    class AppExport ExtensionPropertyData :public App::Extension {
        EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(App::ExtensionPropertyData);
    public:
        static void addPropertiesOnObject(App::DocumentObject* object);

        ExtensionPropertyData();
        ~ExtensionPropertyData();

        /// get all properties of the class (including properties of the parent)
       void extensionGetPropertyMap(std::map<std::string, Property*>& Map) const override;
        /// get all properties of the class (including properties of the parent)
       void extensionGetPropertyList(std::vector<Property*>& List) const override;

       Property* extensionGetPropertyByName(const char* name) const override;
       /// get the name of a property
       const char* extensionGetPropertyName(const Property* prop) const override;

       /// get the Type of a Property
       short extensionGetPropertyType(const Property* prop) const override;
       /// get the Type of a named Property
       short extensionGetPropertyType(const char* name) const override;
       /// get the Group of a Property
       const char* extensionGetPropertyGroup(const Property* prop) const override;
       /// get the Group of a named Property
       const char* extensionGetPropertyGroup(const char* name) const override;
       /// get the Group of a Property
       const char* extensionGetPropertyDocumentation(const Property* prop) const override;
       /// get the Group of a named Property
       const char* extensionGetPropertyDocumentation(const char* name) const override;
    protected:
        DynamicProperty dynamicProps;
    };
}
#endif