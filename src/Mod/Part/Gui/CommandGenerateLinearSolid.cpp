#include "PreCompiled.h"


#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/Document.h>

#include <Gui/InventorAll.h>
#include <Gui/View3DInventor.h>
#include <Gui/ViewProvider.h>

#include <App/Document.h>


#include <Mod/Sketcher/App/SketchObject.h>

FC_LOG_LEVEL_INIT("App", true, true, true)


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

    Sketcher::SketchObject* pSketchObject = new Sketcher::SketchObject;//默认，xy平面原点

   
    Base::Vector3d pos(0, 0, 0);

    Base::Vector3d axis(0, 1, 0);//y
    Base::Rotation rotation(axis, D_PI/2);

    Base::Placement placement(pos, rotation);

    pSketchObject->transformPlacement(placement);

    


    activeDoc->addObject(pSketchObject);
    //activeDoc->setEdit()
    Gui::Document* activeGui = Gui::Application::Instance->getDocument(activeDoc);
    if (!activeGui) {
        FC_ERR(__FUNCTION__ << "(" << __LINE__ << ")");
        return;
    }
    Gui::ViewProvider* pVp = Gui::Application::Instance->getViewProvider(pSketchObject);
    if (!pVp) {
        FC_ERR(__FUNCTION__ << "(" << __LINE__ << ")");
        return;
    }
    activeGui->setEdit(pVp);

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


void CreatePartGenerateLinearSolidCommands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdPartGenerateLinearSolid());
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
}
