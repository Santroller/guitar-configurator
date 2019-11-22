function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    windir = installer.environmentVariable("windir");
    console.log("WINDIR: " + windir);

    if (windir == "") {
        //Just in case.
        console.log("Undefined WINDIR? Assuming c:\\windows\\");
        windir = "c:\\windows";
    }

    pnputil = windir + "\\sysnative\\pnputil.exe";

    console.log("pnputil = " + pnputil);
    component.addElevatedOperation("Execute", pnputil, "-i", "-a", "@TargetDir@\\atmel_usb_dfu.inf");
}