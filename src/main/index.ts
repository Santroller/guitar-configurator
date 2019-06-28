require('hazardous');
require('update-electron-app')({repo: "sanjay900/guitar-configurator"})
import {app, BrowserWindow} from 'electron';
import * as path from 'path';
import {ipcMain} from 'electron';
import {format as formatUrl} from 'url';
import {searchForGuitar, program, programHoodloader} from './programmer';
import {Guitar} from '../common/avr-types';
import {hasMultipleChips} from './boards';
if(require('electron-squirrel-startup')) app.quit();

const isDevelopment = process.env.NODE_ENV !== 'production';
// global reference to mainWindow (necessary to prevent window from being
// garbage collected)
let mainWindow: BrowserWindow|null;

function createMainWindow() {

  const window = new BrowserWindow();

  window.webContents.openDevTools();
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
  const guitar = await searchForGuitar();
  mainWindow!.webContents.send('guitar', guitar);
});

ipcMain.on('programHoodloader', async (evt: Event, guitar: Guitar) => {
  await programHoodloader(guitar, (percentage: number, state: string) => {
    mainWindow!.webContents.send('program', {percentage, state});
  });
})

ipcMain.on('program', async (evt: Event, guitar: Guitar) => {
  let boards = [guitar.board.name];
  // There are two boards on the uno, so program both.
  if (hasMultipleChips(guitar.board)) {
    boards = ['uno-main', 'uno-usb'];
  }
  let current = 0;
  for (let board of boards) {
    await program(board, guitar, (percentage: number, state: string) => {
      percentage /= boards.length;
      percentage += current;
      mainWindow!.webContents.send('program', {percentage, state});
    });
    current += 100 / boards.length;
  }
});
