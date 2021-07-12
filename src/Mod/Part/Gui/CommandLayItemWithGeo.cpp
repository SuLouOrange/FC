#include "PreCompiled.h"


#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/Document.h>
#include <Gui/ToolBarManager.h>
#include <Gui/WorkbenchManager.h>
#include <Gui/Workbench.h>

#include <Gui/InventorAll.h>
#include <Gui/View3DInventor.h>

#include <App/Document.h>


#include <Mod/Part/App/FeaturePartBox.h>
#include <Mod/Part/App/PrimitiveFeature.h>

#include <Base/Console.h>

static const char* logKey = "CommandLayItemWithGeo";

FC_LOG_LEVEL_INIT(logKey, false, true)


template<typename objType>
void CBFunction(void* ud, SoEventCallback* n) {
    printf("%s(%d), start\n", __FUNCTION__, __LINE__);
    Gui::View3DInventorViewer* pViewer = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

    const SoEvent* pSoEvent = n->getEvent();
    const SoType SoEventType(pSoEvent->getTypeId());

    bool IsMouse = SoEventType.isDerivedFrom(SoMouseButtonEvent::getClassTypeId());
    if (IsMouse && SO_MOUSE_PRESS_EVENT(pSoEvent, BUTTON1)) {
        const SoMouseButtonEvent* pMouseEvent = reinterpret_cast<const SoMouseButtonEvent*>(pSoEvent);
        if (pMouseEvent->getState() == SoButtonEvent::DOWN) {
            const SbVec2s pos(pMouseEvent->getPosition());
            Base::Vector3d Vec3d;


            SbPlane horizonZero(SbVec3f(0, 0, 1), SbVec3f(0, 0, 0));

            SbVec3f focalPoint = pViewer->getPointOnScreen(pos);

            //
            SbLine viewLine;
            viewLine.setPosDir(focalPoint, pViewer->getViewDirection());
            SbVec3f zeroPoint;
            if (horizonZero.intersect(viewLine, zeroPoint)) {
                focalPoint = zeroPoint;
            }

            Vec3d.x = focalPoint[0];
            Vec3d.y = focalPoint[1];
            Vec3d.z = focalPoint[2];

            //*******new a obj
            objType* pObj = new objType;

            Base::Placement placement;
            placement.setPosition(Vec3d);
            pObj->transformPlacement(placement);
            App::Document* pAppDoc = App::GetApplication().getActiveDocument();
            pAppDoc->addObject(pObj);
            //*******obj work done


            Gui::Command::updateActive();
            pViewer->removeEventCallback(SoEvent::getClassTypeId(), CBFunction<objType>);
        }


        printf("%s(%d), mouse button1 press!\n", __FUNCTION__, __LINE__);
    }

}

//===========================================================================
// Part_LayCylinder
//===========================================================================
DEF_STD_CMD_A(CmdPartLayCylinder)


CmdPartLayCylinder::CmdPartLayCylinder()
    : Command("Part_LayCylinder")
{
    sAppModule = "Part";
    sGroup = QT_TR_NOOP("Part");
    sMenuText = QT_TR_NOOP("LayCylinder");
    sToolTipText = QT_TR_NOOP("Lay a Cylinder");
    sWhatsThis = "Part_LayCylinder";
    sStatusTip = sToolTipText;
    sPixmap = "Part_LayCylinder";
}

void CmdPartLayCylinder::activated(int iMsg)
{
    Q_UNUSED(iMsg);
#if 0
    QString cmd;
    cmd = qApp->translate("CmdPartLayCylinder", "LayCylinder");
    openCommand((const char*)cmd.toUtf8());
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    runCommand(Doc, "App.ActiveDocument.addObject(\"Part::Cylinder\",\"Cylinder\")");
    cmd = QString::fromLatin1("App.ActiveDocument.ActiveObject.Label = \"%1\"")
        .arg(qApp->translate("CmdPartCylinder", "Cylinder"));
    runCommand(Doc, cmd.toUtf8());
#endif
    //1.get 3DInventorViewer
    //Gui::View3DInventorViewer* pViewer = reinterpret_cast<Gui::View3DInventorViewer*>(getActiveGuiDocument()->getActiveView());

    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }
    Gui::Document* activeGui = Gui::Application::Instance->getDocument(activeDoc);
    if (!activeGui) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }

    Gui::MDIView* pView = activeGui->getActiveView();
    Gui::View3DInventorViewer* p3DViewer = nullptr;
    if (pView->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        p3DViewer = reinterpret_cast<Gui::View3DInventor*>(pView)->getViewer();
    }
    else {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }

    if (p3DViewer == nullptr) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }

    using namespace std;
    //remove tool bar
    Gui::Workbench* pCurWorkbench = Gui::WorkbenchManager::instance()->active();
    //std::list<std::string> toolBarList = pCurWorkbench->listToolbars();
    std::list<std::pair<std::string, std::list<std::string>>> toolBarCMD = pCurWorkbench->getToolbarItems();

    std::list<std::string> toReplaceCMDList;
    const string aimGroupStr = "GenerateLinearSolid";
    for (auto CMD_group : toolBarCMD) {
        if (CMD_group.first == aimGroupStr) {
            toReplaceCMDList = CMD_group.second;
            break;
        }
    }

    if (toReplaceCMDList.empty())
        FC_WARN("not find aim command group: " << aimGroupStr);
    else {
        int cnt = 0;
        for (auto cmd : toReplaceCMDList) {
            FC_MSG(" tool bar " << cnt++ << ":" << cmd << " to be replaced");
        }
    }
    

        
    //Gui::ToolBarManager* pToolBarManager = Gui::ToolBarManager::getInstance();

    
    //2.add cb
    p3DViewer->addEventCallback(SoEvent::getClassTypeId(), CBFunction<Part::Cylinder>);

    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    commitCommand();
    updateActive();
    //runCommand(Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
}

bool CmdPartLayCylinder::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


//===========================================================================
// Part_LayBox
//===========================================================================
DEF_STD_CMD_A(CmdPartLayBox)


CmdPartLayBox::CmdPartLayBox()
    : Command("Part_LayBox")
{
    sAppModule = "Part";
    sGroup = QT_TR_NOOP("Part");
    sMenuText = QT_TR_NOOP("LayBox");
    sToolTipText = QT_TR_NOOP("Lay a Box");
    sWhatsThis = "Part_LayBox";
    sStatusTip = sToolTipText;
    sPixmap = "Part_LaBox";
}

void CmdPartLayBox::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    //1.get 3DInventorViewer
    //Gui::View3DInventorViewer* pViewer = reinterpret_cast<Gui::View3DInventorViewer*>(getActiveGuiDocument()->getActiveView());

    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }
    Gui::Document* activeGui = Gui::Application::Instance->getDocument(activeDoc);
    if (!activeGui) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }

    Gui::MDIView* pView = activeGui->getActiveView();
    Gui::View3DInventorViewer* p3DViewer = nullptr;
    if (pView->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        p3DViewer = reinterpret_cast<Gui::View3DInventor*>(pView)->getViewer();
    }
    else {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }

    if (p3DViewer == nullptr) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }

    //2.add cb
    p3DViewer->addEventCallback(SoEvent::getClassTypeId(), CBFunction<Part::Box>);

    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    commitCommand();
    updateActive();
    //runCommand(Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
}

bool CmdPartLayBox::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}




void CreateLayPartCommands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdPartLayCylinder());
    rcCmdMgr.addCommand(new CmdPartLayBox());
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
}
