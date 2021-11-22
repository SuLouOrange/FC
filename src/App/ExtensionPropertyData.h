#ifndef _APP_EXTENSION_PROPERTY_DATA_H_
#define _APP_EXTENSION_PROPERTY_DATA_H_
#include "Extension.h"
#include "PropertyDataSpecs.h"
namespace App {
    class AppExport ExtensionPropertyData :public App::Extension {
        EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(App::ExtensionPropertyData);
    public:
        static void addPropertiesOnObject(App::DocumentObject* object);

        ExtensionPropertyData();
        ~ExtensionPropertyData();

        App::PropertyDataSpecs PropertySpecs;
    };
}
#endif