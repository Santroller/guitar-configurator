declare module "xinput" {
    export interface XinputCapabilities {
        type: number;
        subType: number;
        flags: number;
    }
    export function IsConnected(controllerNum: number): boolean;    
    export function GetState(controllerNum: number): any;    
    export function GetCapabilities(controllerNum: number): XinputCapabilities;    
}
  