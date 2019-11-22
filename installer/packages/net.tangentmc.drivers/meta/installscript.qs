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
    component.addElevatedOperation("Execute", pnputil, "-i", "-a", "@TargetDir@\\drivers\\atmel_usb_dfu.inf");
}