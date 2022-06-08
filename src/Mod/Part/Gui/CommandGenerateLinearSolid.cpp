#include "PreCompiled.h"


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
#include <Mod/Part/App/PartFeatures.h>

//to do precompile
#include <GC_MakeSegment.hxx>
# include <gp_Pln.hxx>
# include <gp_Pnt.hxx>

FC_LOG_LEVEL_INIT("", false, true)

static int fAddItemsToSkecher(Sketcher::SketchObject* pSketchObject);
static Part::Extrusion* fMakeExtrusion(Sketcher::SketchObject* pSketchObject, double length);
static Part::Sweep* fMakeSweep(Sketcher::SketchObject* pSketchObject, App::DocumentObject* pSpineObj);
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
Part::Extrusion* fMakeExtrusion(Sketcher::SketchObject* pSketchObject, double length) {
    FC_MSG("");
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) {
        FC_ERR(__FUNCTION__ << "(" << __LINE__ << ")");
        return nullptr;
    }
    Part::Extrusion* pExtrusion = new Part::Extrusion;
    activeDoc->addObject(pExtrusion);

    //write property
    pExtrusion->Base.setValue(pSketchObject);
    pExtrusion->DirMode.setValue("Normal");

    //to check link, maybe there is somthing useful
  
    pExtrusion->LengthFwd.setValue(length);
    pExtrusion->Solid.setValue(true);
    return pExtrusion;
}


Part::Sweep* fMakeSweep(Sketcher::SketchObject* pSketchObject, App::DocumentObject *pSpineObj) {
    FC_MSG("");
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) {
        FC_ERR(__FUNCTION__ << "(" << __LINE__ << ")");
        return nullptr;
    }
    Part::Sweep* pSweep = new Part::Sweep;
    activeDoc->addObject(pSweep);

    pSweep->Sections.setValue(pSketchObject);
    pSweep->Spine.setValue(pSpineObj);
  
    pSweep->Solid.setValue(true);
    return pSweep;
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

    int selObjNum, validNum;
    selObjNum = validNum = 0;
    for (auto selObj: vecSelObj) {
        FC_MSG(__FUNCTION__ << " get selObj " << selObjNum << " : " << selObj.FeatName << "; type: " << selObj.TypeName << " * ********");
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
            
            pObject = selObj.pObject;
            Part::Part2DObject* p2DObj = reinterpret_cast<Part::Part2DObject*>(selObj.pObject);

            //to make a s skecher plane
           //1.get global position
            Base::Placement basePlacement = p2DObj->globalPlacement();
            FC_MSG(" get a obj of aimed type : "<< aimTypeStr <<"  " << validNum << " : " << selObj.FeatName << " , to do extrusion on it !!!!!!!!");
            Base::Vector3d posGlobal = basePlacement.getPosition();
            FC_MSG("its pos in global (" << posGlobal[0] << ", " << posGlobal[1] << ", " << posGlobal[2] << ") ");

#if 0
            std::vector<App::Property*> list;
            list.clear();
            p2DObj->extensionGetPropertyList(list);
#endif
            //2.get normal vector, length;
            App::Property* pStartRawProperty, * pEndRawProperty, * pRawLengthProperty, * pPointsPropertyRaw;
            pStartRawProperty = p2DObj->getPropertyByName("Start");
            pEndRawProperty = p2DObj->getPropertyByName("End");
            pRawLengthProperty = p2DObj->getPropertyByName("Length");
            pPointsPropertyRaw = p2DObj->getPropertyByName("Points");
            if (pRawLengthProperty == nullptr || pStartRawProperty == nullptr || pEndRawProperty == nullptr || pPointsPropertyRaw == nullptr) {
                FC_WARN(" not valid obj: " << p2DObj->getNameInDocument());
                selObjNum++;
                continue;
            }

            App::PropertyVectorDistance* pStartPosProperty, * pEndPosProperty;
            App::PropertyLength* pLengthProperty;
            App::PropertyVectorList* pPointList;
#if 0
            Base::Type type = pPointsPropertyRaw->getTypeId();
            const char * typeName = type.getName();
#endif
            pStartPosProperty = reinterpret_cast<App::PropertyVectorDistance*>(pStartRawProperty);
            pEndPosProperty = reinterpret_cast<App::PropertyVectorDistance*>(pEndRawProperty);
            pLengthProperty = reinterpret_cast<App::PropertyLength*>(pRawLengthProperty);
            pPointList = reinterpret_cast<App::PropertyVectorList*>(pPointsPropertyRaw);


            Base::Vector3d dirVector(pEndPosProperty->getValue() - pStartPosProperty->getValue());
            //Base::Vector3d normalVector(pEndPosProperty->getValue() - pStartPosProperty->getValue());
            dirVector = dirVector.Normalize();

            FC_MSG("direction normal vector (" << dirVector[0] << ", " << dirVector[1] << ", " << dirVector[2] << ") ");

            if (pPointList->getSize() != 2) {
                FC_ERR(p2DObj->getNameInDocument() << " not a two points obj!" << " end##########");
                selObjNum++;
                continue;
            }


            //3.make sketcher
            Sketcher::SketchObject* pSketchObject = new Sketcher::SketchObject;

            Base::Rotation rotation = getRotationOutNormalVec(dirVector);
            Base::Placement placement(posGlobal, rotation);

            pSketchObject->transformPlacement(placement);
            activeDoc->addObject(pSketchObject);


            fAddItemsToSkecher(pSketchObject);
#if 0
            Part::Extrusion *pExtrusion = fMakeExtrusion(pSketchObject, pLengthProperty->getValue());
            if(pExtrusion)
            {
                FC_MSG(validNum << ".create  " << pExtrusion->getNameInDocument() << " of type " << pExtrusion->getTypeId().getName() << " and "\
                    << pSketchObject->getNameInDocument() << " of type " << pSketchObject->getTypeId().getName() << " based on " << p2DObj->getNameInDocument()\
                    << ",the length of " << pExtrusion->getNameInDocument() << " is " << pLengthProperty->getValue() << pLengthProperty->getUnit().getString().toUtf8().constData());
                validNum++;
            }
#else
            Part::Sweep* pSweep = fMakeSweep(pSketchObject, p2DObj);
            
#endif

#if 0
            std::map<std::string, App::Property*> map;
            map.clear();
            p2DObj->getPropertyMap(map);
#endif         
        }
        selObjNum++;
    }//for loop

    if (!selObjNum) {
        FC_ERR(__FUNCTION__ << " no selected obj!" << " end##########");
    }
    else if (!validNum)
        FC_ERR(" no valid obj!" << " end##########");
    else
        FC_MSG(" total sel obj number " << selObjNum << ", total " << validNum << " valid object.  end##########");

    FC_MSG("(" << __LINE__ << ")");
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


//
//===========================================================================
// Part_DrawLinearSolid
//===========================================================================
DEF_STD_CMD_A(CmdPartDrawLinearSolid)

typedef struct {
    int pointIndex;
    Base::Vector3d firstPointVec;
    void reset() {
        pointIndex = 0;
        firstPointVec = Base::Vector3d(0, 0, 0);
    }
}controlData;
static controlData cmdData;

static int drawAction(Base::Vector3d p1, Base::Vector3d p2);
Base::Vector3d parsePoint(const SbVec2s & vec2, Gui::View3DInventorViewer* pViewer){
    Base::Vector3d point;
    SbPlane horizonZero(SbVec3f(0, 0, 1), SbVec3f(0, 0, 0));

    SbVec3f focalPoint = pViewer->getPointOnFocalPlane(vec2);

    SbLine viewLine;
    viewLine.setPosDir(focalPoint, pViewer->getViewDirection());
    SbVec3f zeroPoint;
    if (horizonZero.intersect(viewLine, zeroPoint)) {
        focalPoint = zeroPoint;
    }

    point.x = focalPoint[0];
    point.y = focalPoint[1];
    point.z = focalPoint[2];
    FC_MSG("get point success (" << point.x << ", " << point.y << ", " << point.z << ")");
    return point;
}

void callBackFunc(void* ud, SoEventCallback* n) {
    FC_MSG("start");
    Gui::View3DInventorViewer* pViewer = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());
    Base::Vector3d Vec3d(0,0,0);
    const SoEvent* pSoEvent = n->getEvent();
    const SoType SoEventType(pSoEvent->getTypeId());

    bool IsMouse = SoEventType.isDerivedFrom(SoMouseButtonEvent::getClassTypeId());
    if (IsMouse && SO_MOUSE_PRESS_EVENT(pSoEvent, BUTTON1)) {
        const SoMouseButtonEvent* pMouseEvent = reinterpret_cast<const SoMouseButtonEvent*>(pSoEvent);
        if (pMouseEvent->getState() == SoButtonEvent::DOWN) {
            const SbVec2s pos(pMouseEvent->getPosition());
            Vec3d = parsePoint(pos, pViewer);
            if (cmdData.pointIndex == 0) {
                cmdData.firstPointVec = Vec3d;
                cmdData.pointIndex++;
            }
            else if (cmdData.pointIndex == 1) {
                drawAction(cmdData.firstPointVec, Vec3d);
                pViewer->removeEventCallback(SoEvent::getClassTypeId(), callBackFunc);
            }
        }
    }
}

int drawAction(Base::Vector3d p1, Base::Vector3d p2) {
    FC_MSG(__FUNCTION__ << "start");
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) {
        FC_ERR(__FUNCTION__);
        return 1;
    }
#if 0
    Part::Part2DObject * p2dObj = new Part::Part2DObject;
    Part::GeomLineSegment* p = new  Part::GeomLineSegment();
    p->setPoints(p1, p2);
    activeDoc->addObject(p);
#endif


    return 0;
}

CmdPartDrawLinearSolid::CmdPartDrawLinearSolid()
    : Command("Part_DrawLinearSolid")
{
    sAppModule = "Part";
    sGroup = QT_TR_NOOP("Part");
    sMenuText = QT_TR_NOOP("DrawLinearSolid");
    sToolTipText = QT_TR_NOOP("Draw a Linear Solid");
    sWhatsThis = "Part_DrawLinearSolid";
    sStatusTip = sToolTipText;
    sPixmap = "Part_DrawLinearSolid";
}

void CmdPartDrawLinearSolid::activated(int iMsg)
{
    Q_UNUSED(iMsg);


    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) {
        FC_ERR(__FUNCTION__);
        return;
    }
    cmdData.reset();

    

#if 1
    Gui::Document* activeGui = Gui::Application::Instance->getDocument(activeDoc);
    if (!activeGui) {
        FC_ERR(__FUNCTION__);
        return;
    }

    Gui::MDIView* pView = activeGui->getActiveView();
    if (auto viewer= static_cast<Gui::View3DInventor*>(pView)->getViewer()) {
        auto root = viewer->getSceneGraph();
        SoWriteAction myAction;
        const char* fileDir = "D:/TestData/sceneGragh.c";
        bool result = myAction.getOutput()->openFile(fileDir);
        if (!result) {
            FC_ERR(__FUNCTION__ << ", cna't open " << fileDir);
            return;
        }
        myAction.getOutput()->setBinary(FALSE);
        myAction.apply(root);
        myAction.getOutput()->closeFile();
        FC_MSG(__FUNCTION__ << ", output scene gragh to " << fileDir << ", success!");
    }

    //p3DViewer->addEventCallback(SoEvent::getClassTypeId(),callBackFunc);
#endif

}

bool CmdPartDrawLinearSolid::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}



void CreatePartGenerateLinearSolidCommands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdPartGenerateLinearSolid());
    rcCmdMgr.addCommand(new CmdPartDrawLinearSolid());
    FC_MSG(__FUNCTION__);
}


