#include "PreCompiled.h"

#ifndef _PreComp_
# include <QApplication>
# include <QDir>
# include <QFileInfo>
# include <QLineEdit>
#endif

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/Document.h>

#include <Gui/InventorAll.h>
#include <Gui/View3DInventor.h>

//===========================================================================
// Part_LayCylinder
//===========================================================================
DEF_STD_CMD_A_CB(CmdPartLayCylinder)

//static void CmdPartLayCylinder::CBFunction(void* ud, SoEventCallback* n);

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


    //2.add cb

    if (p3DViewer == nullptr) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
        return;
    }

    p3DViewer->addEventCallback(SoEvent::getClassTypeId(), CBFunction);

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

void  CmdPartLayCylinder::CBFunction(void* ud, SoEventCallback* n) {
    printf("%s(%d), start\n", __FUNCTION__, __LINE__);
    Gui::View3DInventorViewer* pViewer = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

    const SoEvent* pSoEvent = n->getEvent();
    const SoType SoEventType(pSoEvent->getTypeId());

    bool IsMouse = SoEventType.isDerivedFrom(SoMouseButtonEvent::getClassTypeId());
    if (IsMouse && SO_MOUSE_PRESS_EVENT(pSoEvent, BUTTON1)) {
        pViewer->removeEventCallback(SoEvent::getClassTypeId(), CmdPartLayCylinder::CBFunction);
        printf("%s(%d), mouse button1 press!\n", __FUNCTION__, __LINE__);
    }

}

void CreateLayPartCommands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdPartLayCylinder());
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
}
