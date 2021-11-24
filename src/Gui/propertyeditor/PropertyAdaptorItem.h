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


#ifndef _PROPERTY_ADAPTOR_ITEM_H
#define _PROPERTY_ADAPTOR_ITEM_H
#include "PropertyItem.h"

namespace Gui {
namespace PropertyEditor {

class GuiExport PropertyAdaptorItem : public PropertyItem
{
    Q_OBJECT
        PROPERTYITEM_HEADER

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget* editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget* editor) const;

protected:
    PropertyAdaptorItem();
    ~PropertyAdaptorItem();
    QVariant value(const App::Property*) const override;//填入model中的数据
    virtual void setValue(const QVariant&);
};

} // namespace PropertyEditor
} // namespace Gui

#endif // PROPERTYEDITORITEM_H
