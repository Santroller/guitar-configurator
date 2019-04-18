import {app, BrowserWindow} from 'electron';
import {ipcMain} from 'electron';
import * as path from 'path';

import {build, connect, getVariables, listPorts, program} from './programmer';

let mainWindow: Electron.BrowserWindow;
function createWindow() {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    height: 600,
    width: 800,
    webPreferences: {nodeIntegration: true},
  });
  // and load the index.html of the app.
  mainWindow.loadFile(path.join(__dirname, 'index.html'));

  // Open the DevTools.
  // mainWindow.webContents.openDevTools();
  // Emitted when the window is closed.
  mainWindow.on('closed', () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;
  });
}

app.on('browser-window-created',function(e,window) {
  window.setMenuBarVisibility(false);
  window.maximize();
});
// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow);

// Quit when all windows are closed.
app.on('window-all-closed', () => {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  // On OS X it"s common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow();
  }
});

ipcMain.on('connect', () => {
  connect();
});
ipcMain.on('build', (evt: Event, options: any) => {
  build(
      options,
      status => {
        mainWindow.webContents.send('status', status);
      },
      () => {
        mainWindow.webContents.send('built');
        setTimeout(async ()=> {
          mainWindow.webContents.send('list', await listPorts());
        },500)
      });
});
ipcMain.on('program', (evt: Event, port: string) => {
  program(port, status => {
    mainWindow.webContents.send('status', status);
  });
});

ipcMain.on('list', async () => {
  mainWindow.webContents.send('list', await listPorts());
});
ipcMain.on('init', async () => {
  setInterval(async ()=>mainWindow && mainWindow.webContents.send('list', await listPorts()), 100);
  mainWindow.webContents.send('list', await listPorts());
  mainWindow.webContents.send('vars', await getVariables());
});
// I hate this, but could not find a way to properley catch some serial port
// errors
process.on('unhandledRejection', (error: Error) => {
  mainWindow.webContents.send('error', error.message);
  console.log(error);
});
