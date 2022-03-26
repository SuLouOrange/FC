/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <Bnd_Box.hxx>
# include <gp_Pln.hxx>
# include <BRep_Builder.hxx>
# include <BRepBndLib.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <Geom_Plane.hxx>
# include <Geom_Surface.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Compound.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
# include <TopExp_Explorer.hxx>
# include <BRepAlgoAPI_Fuse.hxx>
# include <gp_Pln.hxx>
#endif

#include <Base/Placement.h>

#include "FeatureRelax.h"
#include "FaceMaker.h"
#include <Mod/Part/App/Part2DObject.h>
PROPERTY_SOURCE(Part::FeatureRelax, Part::Feature)

namespace Part{
FeatureRelax::FeatureRelax()
{
    double L = 2000.0, W = 2000.0, H = 3000.0;

    BRepPrimAPI_MakeBox mkBox(L, W, H);
    TopoDS_Shape ResultShape = mkBox.Shape();
    Shape.setValue(ResultShape);
}

short FeatureRelax::mustExecute() const
{
    if (FaceMakerClass.isTouched())
        return 1;
    if (Sources.isTouched())
        return 1;
    return Part::Feature::mustExecute();
}

void FeatureRelax::setupObject()
{
    this->FaceMakerClass.setValue("Part::FaceMakerBullseye");
    Feature::setupObject();
}

App::DocumentObjectExecReturn *FeatureRelax::execute(void)
{
    return App::DocumentObject::StdReturn;
}
}//namespace Part

