#ifndef _APP_EXTENSION_PROPERTY_DATA_H_
#define _APP_EXTENSION_PROPERTY_DATA_H_
#include "Extension.h"
#include "DynamicProperty.h"
//#include "PropertyDataSpecs.h"
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

        //App::PropertyDataSpecs PropertySpecs;
       Property* extensionGetPropertyByName(const char* name) const override;
    protected:
        DynamicProperty dynamicProps;
    };
}
#endif