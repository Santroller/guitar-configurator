function Component()
{
}
Component.prototype.createOperationsForArchive = function(archive)
{
    component.createOperationsForArchive(archive);
    windir = installer.environmentVariable("windir");
    if (windir == "") {
        windir = "c:\\windows";
    }
    pnputil = windir + "\\sysnative\\pnputil.exe";
    file = installer.value("TargetDir").replace(/\//g, '\\') + "\\drivers\\atmel_usb_dfu.inf"
    file = "'"+file+"'"
    component.addElevatedOperation("Execute", "pnputil.exe", "-i", "-a", file);
}