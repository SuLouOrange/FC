﻿#include "PreCompiled.h"


#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/Document.h>

#include <Gui/InventorAll.h>
#include <Gui/View3DInventor.h>
#include <Gui/ViewProvider.h>
//#include <Gui/WaitCursor>

#include <App/Document.h>
#include <App/GeofeaturePy.h>


#include <Mod/Sketcher/App/SketchObject.h>
#include <Mod/Sketcher/App/Constraint.h>
#include <Mod/Part/App/Geometry.h>
#include <Mod/Part/App/FeatureExtrusion.h>

//to do precompile
#include <GC_MakeSegment.hxx>
# include <gp_Pln.hxx>
# include <gp_Pnt.hxx>

FC_LOG_LEVEL_INIT("", false, true)

static int fAddItemsToSkecher(Sketcher::SketchObject* pSketchObject);
static int fMakeExtrusion(Sketcher::SketchObject* pSketchObject);
static Base::Vector3d getOrthonormalVector3(const Base::Vector3d& vec);
static Base::Rotation getRotationOutNormalVec(const Base::Vector3d& normaUnitVec);

Base::Vector3d getOrthonormalVector3(const Base::Vector3d& vec) {
    Base::Vector3d resultVec;
    int imin = 0;
    for (int i = 0; i < 3; ++i)
        if (std::abs(vec[i]) < std::abs(vec[imin]))
            imin = i;
    resultVec[imin] = 1;

    for (int i = 0; i < 3; i++)
        resultVec[i] -= vec[imin] * vec[i];
    return resultVec;
}

Base::Rotation getRotationOutNormalVec(const Base::Vector3d& normaUnitVec) {

    const Base::Vector3d v3 = normaUnitVec;//v3 as z axis
    auto v1 = getOrthonormalVector3(v3);
    auto v2 = v3.Cross(v1);

    Base::Matrix4D matrix4D(v1[0], v2[0], v3[0], 0, \
                            v1[1], v2[1], v3[1], 0, \
                            v1[2], v2[2], v3[2], 0, \
                             0, 0, 0, 1);
    return Base::Rotation(matrix4D);
}


int fAddItemsToSkecher(Sketcher::SketchObject* pSketchObject) {
    FC_MSG("");
    std::vector<Part::GeomLineSegment*> vecLineSegment;
    for (int i = 0; i < 4; i++) {
        Part::GeomLineSegment* p = new  Part::GeomLineSegment();
        vecLineSegment.push_back(p);
    }

    int edgeLength = 20;
    vecLineSegment[0]->setPoints(Base::Vector3d(edgeLength, edgeLength, 0), Base::Vector3d(edgeLength, -edgeLength, 0));
    vecLineSegment[1]->setPoints(Base::Vector3d(edgeLength, -edgeLength, 0), Base::Vector3d(-edgeLength, -edgeLength, 0));
    vecLineSegment[2]->setPoints(Base::Vector3d(-edgeLength, -edgeLength, 0), Base::Vector3d(-edgeLength, edgeLength, 0));
    vecLineSegment[3]->setPoints(Base::Vector3d(-edgeLength, edgeLength, 0), Base::Vector3d(edgeLength, edgeLength, 0));


    for (int i = 0; i < vecLineSegment.size(); i++)
        pSketchObject->addGeometry(vecLineSegment[i]);


    //3.create constraints
    std::vector<Sketcher::Constraint*> vecConstraint;

    // conincident
    for (int i = 0; i < 4; i++) {
        Sketcher::Constraint* p = new  Sketcher::Constraint();
        vecConstraint.push_back(p);
        vecConstraint[i]->First = i;
        vecConstraint[i]->FirstPos = Sketcher::PointPos::end;
        int secondId = i + 1;
        if (i == 3)
            secondId = 0;
        vecConstraint[i]->Second = secondId;
        vecConstraint[i]->SecondPos = Sketcher::PointPos::start;
        vecConstraint[i]->Type = Sketcher::ConstraintType::Coincident;
    }
    //to do add more constraint

    pSketchObject->addConstraints(vecConstraint);
    return 0;
}


//does unit need to adapt?
int fMakeExtrusion(Sketcher::SketchObject* pSketchObject, double length) {
    FC_MSG("");
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) {
        FC_ERR(__FUNCTION__ << "(" << __LINE__ << ")");
        return 1;
    }
    Part::Extrusion* pExtrusion = new Part::Extrusion;
    activeDoc->addObject(pExtrusion);

    //write property
    pExtrusion->Base.setValue(pSketchObject);
    pExtrusion->DirMode.setValue("Normal");

    //to check link, maybe there is somthing useful
  
    pExtrusion->LengthFwd.setValue(length);
    pExtrusion->Solid.setValue(true);
    return 0;
}


//
//===========================================================================
// Part_GenerateLinearSolid
//===========================================================================
DEF_STD_CMD_A(CmdPartGenerateLinearSolid)


CmdPartGenerateLinearSolid::CmdPartGenerateLinearSolid()
    : Command("Part_GenerateLinearSolid")
{
    sAppModule = "Part";
    sGroup = QT_TR_NOOP("Part");
    sMenuText = QT_TR_NOOP("GenerateLinearSolid");
    sToolTipText = QT_TR_NOOP("Generate a Linear Solid");
    sWhatsThis = "Part_GenerateLinearSolid";
    sStatusTip = sToolTipText;
    sPixmap = "Part_GenerateLinearSolid";
}

void CmdPartGenerateLinearSolid::activated(int iMsg)
{
    Q_UNUSED(iMsg);


    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) {
        FC_ERR(__FUNCTION__ << "(" << __LINE__ << ")");
        return;
    }

#if 0
    Gui::Document* activeGui = Gui::Application::Instance->getDocument(activeDoc);
    if (!activeGui) {
        FC_MSG(__FUNCTION__ << "(" << __LINE__ << ")");
        return;
    }

    Gui::MDIView* pView = activeGui->getActiveView();
    Gui::View3DInventorViewer* p3DViewer = nullptr;
    if (pView->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        FC_MSG(__FUNCTION__ << "(" << __LINE__ << ")");
        p3DViewer = reinterpret_cast<Gui::View3DInventor*>(pView)->getViewer();
    }
    else {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }

    if (p3DViewer == nullptr) {
        FC_MSG(__FUNCTION__ << "(" << __LINE__ << ")");
        return;
    }
#endif

    //0.get  lines
    std::vector<App::DocumentObject*> vecSelectionLineSegment = Gui::Selection().getObjectsOfType(Part::GeomLineSegment::getClassTypeId());
    std::vector<Gui::SelectionSingleton::SelObj>&& vecSelObj = Gui::Selection().getSelection();

    int i = 0;
    for (auto selObj: vecSelObj) {
        FC_MSG(__FUNCTION__ << " get selObj " << i << " : " << selObj.FeatName << "; type: " << selObj.TypeName << " * ********");
        App::DocumentObject* pObject = nullptr;
        std::string typeStr(selObj.TypeName);
#if 0
        std::string typeStr(selObj.TypeName);
        std::string pyTypeFlag("Py");
        if (typeStr.find_last_of(pyTypeFlag) == typeStr.size() - pyTypeFlag.size()) {
            FC_MSG(" selObj " << selObj.FeatName << " is Py object");
            auto pObject = 
        }


        App::DocumentObject* pObject = selObj.pObject;
        if (pObject->isDerivedFrom(App::GeoFeaturePy::getClassTypeId()))
            i++;
#endif
        // can't filter some types, for instance rectangle...,maybe subType name is useful
        std::string aimTypeStr("Part::Part2DObjectPython");
        if (typeStr == aimTypeStr) {
            static int j = 0;
            pObject = selObj.pObject;
            Part::Part2DObject* p2DObj = reinterpret_cast<Part::Part2DObject*>(selObj.pObject);

            //to make a s skecher plane
           //1.get global position
            Base::Placement basePlacement = p2DObj->globalPlacement();
            FC_MSG(" get a obj of aimed type : "<< aimTypeStr <<"  " << j++ << " : " << selObj.FeatName << " , to do extrusion on it !!!!!!!!");
            Base::Vector3d posGlobal = basePlacement.getPosition();
            FC_MSG("its pos in global (" << posGlobal[0] << ", " << posGlobal[1] << ", " << posGlobal[2] << ") ");

#if 0
            std::vector<App::Property*> list;
            list.clear();
            p2DObj->extensionGetPropertyList(list);
#endif
            //2.get normal vector, length;
            App::Property* pStartRawProperty, * pEndRawProperty, * pRawLengthProperty;
            pStartRawProperty = p2DObj->getPropertyByName("Start");
            pEndRawProperty = p2DObj->getPropertyByName("End");
            pRawLengthProperty = p2DObj->getPropertyByName("Length");

            App::PropertyVectorDistance* pStartPosProperty, * pEndPosProperty;
            App::PropertyLength* pLengthProperty;
#if 1
            Base::Type type = pRawLengthProperty->getTypeId();
            const char * typeName = type.getName();
#endif
            pStartPosProperty = reinterpret_cast<App::PropertyVectorDistance*>(pStartRawProperty);
            pEndPosProperty = reinterpret_cast<App::PropertyVectorDistance*>(pEndRawProperty);
            pLengthProperty = reinterpret_cast<App::PropertyLength*>(pRawLengthProperty);

            Base::Vector3d dirVector(pEndPosProperty->getValue() - pStartPosProperty->getValue());
            //Base::Vector3d normalVector(pEndPosProperty->getValue() - pStartPosProperty->getValue());
            dirVector = dirVector.Normalize();

            FC_MSG("direction normal vector (" << dirVector[0] << ", " << dirVector[1] << ", " << dirVector[2] << ") ");


            //3.make sketcher
            Sketcher::SketchObject* pSketchObject = new Sketcher::SketchObject;

            Base::Rotation rotation = getRotationOutNormalVec(dirVector);
            Base::Placement placement(posGlobal, rotation);

            pSketchObject->transformPlacement(placement);
#if 0
            //3.make skecher
            BRepBuilderAPI_MakeFace builder(gp_Pln(gp_Pnt(posGlobal[0], posGlobal[1], posGlobal[2]), gp_Dir(dirVector[0], dirVector[0], dirVector[0])));
            if (!builder.IsDone())
                return;
            TopoDS_Shape myShape = builder.Shape();
            myShape.Infinite(Standard_True);
            //Shape.setValue(myShape);
           


          
            pSketchObject->Shape.setValue(myShape);
#endif
            activeDoc->addObject(pSketchObject);


            fAddItemsToSkecher(pSketchObject);
            fMakeExtrusion(pSketchObject, pLengthProperty->getValue());

#if 0
            std::map<std::string, App::Property*> map;
            map.clear();
            p2DObj->getPropertyMap(map);
#endif         
        }
        i++;
    }

    if (i == 0) {
        FC_ERR(__FUNCTION__ << " no selected obj!" << " end##########");
    }
    else
        FC_MSG(__FUNCTION__ << " total sel obj number " << i << " end##########");
#if 0
    //1.创建sketcherObj默认，xy平面原点
    Sketcher::SketchObject* pSketchObject = new Sketcher::SketchObject;

   
    Base::Vector3d pos(0, 0, 0);

    Base::Vector3d axis(0, 1, 0);//y
    Base::Rotation rotation(axis, D_PI/2);

    //make a skether out of a line,to handle the rotation

    Base::Placement placement(pos, rotation);

    pSketchObject->transformPlacement(placement);

#endif

#if 0
    //2.add cb
    p3DViewer->addEventCallback(SoEvent::getClassTypeId(), CBFunction<Part::Cylinder>);
#endif
    FC_MSG(__FUNCTION__ << "(" << __LINE__ << ")");
    commitCommand();
    updateActive();
    //runCommand(Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
}

bool CmdPartGenerateLinearSolid::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

Sketcher::SketchObject* fMakeSkecherPlane(Part::Part2DObject* pBaseObj) {
    //
    return nullptr;
}


void CreatePartGenerateLinearSolidCommands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdPartGenerateLinearSolid());
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
}
