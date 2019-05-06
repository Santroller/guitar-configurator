'use strict';

import {app, BrowserWindow} from 'electron';
import * as path from 'path';
import {ipcMain} from 'electron';
import {format as formatUrl} from 'url';
import {searchForGuitar, program} from './programmer';
import {defaultConfig} from './eeprom';
import {InputType, MemoryLocation} from '../common/avr-types';

const isDevelopment = process.env.NODE_ENV !== 'production';
// global reference to mainWindow (necessary to prevent window from being
// garbage collected)
let mainWindow: BrowserWindow|null;

function createMainWindow() {
  const window = new BrowserWindow();

  if (isDevelopment) {
    window.webContents.openDevTools();
  }
  if (isDevelopment) {
    window.loadURL(`http://localhost:${process.env.ELECTRON_WEBPACK_WDS_PORT}`);
  } else {
    window.loadURL(formatUrl({
      pathname: path.join(__dirname, 'index.html'),
      protocol: 'file',
      slashes: true
    }));
  }

  window.on('closed', () => {
    mainWindow = null;
  });

  window.webContents.on('devtools-opened', () => {
    window.focus();
    setImmediate(() => {
      window.focus();
    });
  });
  return window;
}

// quit application when all windows are closed
app.on('window-all-closed', () => {
  // on macOS it is common for applications to stay open until the user
  // explicitly quits
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  // on macOS it is common to re-create a window even after all windows have
  // been closed
  if (mainWindow === null) {
    mainWindow = createMainWindow();
  }
});
// create main BrowserWindow when electron is ready
app.on('ready', () => {
  mainWindow = createMainWindow();
});

ipcMain.on('search', async () => {
  mainWindow!.webContents.send('guitar', await searchForGuitar());
});

ipcMain.on('program', async () => {
  let config = defaultConfig;
  config.input_type = InputType.Wii;
  await program(
      'uno-usb', 0, config,
      (location: MemoryLocation, percentage: number, time: string) => {
        mainWindow!.webContents.send('program', {location, percentage, time});
      });
});
