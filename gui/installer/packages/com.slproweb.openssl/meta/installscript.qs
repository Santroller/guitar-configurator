function Component()
{
}
Component.prototype.createOperationsForArchive = function(archive)
{
    component.createOperationsForArchive(archive);
    component.addElevatedOperation("Execute", "@TargetDir@\\Win64OpenSSL_Light-1_1_1d.exe", "/silent", "/verysilent");
}