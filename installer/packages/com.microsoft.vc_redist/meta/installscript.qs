function Component()
{
}
Component.prototype.createOperationsForArchive = function(archive)
{
    component.createOperationsForArchive(archive);
    if(systemInfo.currentCpuArchitecture.search("64") < 0) {
        component.addElevatedOperation("Execute", "@TargetDir@\\VC_redist.x86.exe", "/install", "/quiet", "/norestart");
    } else {
        component.addElevatedOperation("Execute", "@TargetDir@\\VC_redist.x64.exe", "/install", "/quiet", "/norestart");
    }
}