function Component()
{
}
Component.prototype.createOperationsForArchive = function(archive)
{
    component.createOperationsForArchive(archive);
    component.addOperation("CreateShortcut", "@TargetDir@/guitar-configurator.exe", "@StartMenuDir@/GuitarConfigurator.lnk",
        "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/guitar-configurator.exe",
        "iconId=0", "description=Guitar Configuration tool");
}