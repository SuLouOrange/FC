/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <algorithm>
# include <QComboBox>
# include <QFontDatabase>
# include <QLayout>
# include <QLocale>
# include <QPixmap>
# include <QTextStream>
# include <QTimer>
# include <QApplication>
# include <QPalette>
# include <QtGlobal>
#endif

#include "PropertyAdaptorItem.h"

#include <App/PropertyDataSpecs.h>
#include <Base/Console.h>

FC_LOG_LEVEL_INIT("PropertyView", false, true);

using namespace Gui::PropertyEditor;
using namespace Gui::Dialog;

PROPERTYITEM_SOURCE(Gui::PropertyEditor::PropertyAdaptorItem)
PropertyAdaptorItem::PropertyAdaptorItem() {

}
PropertyAdaptorItem::~PropertyAdaptorItem() {

}

QVariant PropertyAdaptorItem::value(const App::Property* prop) const{
    assert(prop && prop->getTypeId().isDerivedFrom(App::PropertyAdaptor::getClassTypeId()));
    auto propAdaptor = reinterpret_cast<const App::PropertyAdaptor*>(prop);
    
    try {
        switch (propAdaptor->getDataType()) {
        case App::PropertyDataSpecs::emProperryTypeString: {
            const auto & value = propAdaptor->getValueAsString();
            return QString::fromUtf8(value.c_str());
            break; 
        }
        default:
            break;
        }
    }
    catch (std::exception& e) {
        FC_ERR(__FUNCTION__ << ", " << e.what());
    } 
    return QVariant();
}

void PropertyAdaptorItem::setValue(const QVariant& value)
{
    auto  propertyAdaptor = reinterpret_cast<App::PropertyAdaptor*>(propertyItems.front());
    if (!hasExpression()) {
        switch (propertyAdaptor->getDataType()) {
        case App::PropertyDataSpecs::emProperryTypeString: {
            QString val = value.toString();
            propertyAdaptor->setValueByString(val.toStdString());
            break;
        }
        default:
            break;
        }

    }
}

QWidget* PropertyAdaptorItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    ExpLineEdit* le = new ExpLineEdit(parent);
    le->setFrame(false);
    le->setReadOnly(isReadOnly());
    QObject::connect(le, SIGNAL(textChanged(const QString&)), receiver, method);
    if (isBound()) {
        le->bind(getPath());
        le->setAutoApply(autoApply());
    }

    return le;
}

void PropertyAdaptorItem::setEditorData(QWidget* editor, const QVariant& data) const
{
    QLineEdit* le = qobject_cast<QLineEdit*>(editor);
    le->setText(data.toString());
}

QVariant PropertyAdaptorItem::editorData(QWidget* editor) const
{
    QLineEdit* le = qobject_cast<QLineEdit*>(editor);
    return QVariant(le->text());
}

#include "moc_PropertyAdaptorItem.cpp"

