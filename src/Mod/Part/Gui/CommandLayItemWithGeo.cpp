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

    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    commitCommand();
    updateActive();
    runCommand(Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
}

bool CmdPartLayCylinder::isActive(void)
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
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
}
