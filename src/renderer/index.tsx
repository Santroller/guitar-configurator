import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import * as serviceWorker from './serviceWorker';
import { createMuiTheme, MuiThemeProvider } from '@material-ui/core/styles';
import { mainReducer } from './reducers';
import { createStore } from 'redux'
import { Provider } from 'react-redux'
const store = createStore(mainReducer);
const theme = createMuiTheme({
    palette: {
      type: 'dark',
    },
  });
ReactDOM.render( <Provider store={store}><MuiThemeProvider theme={theme}><App /></MuiThemeProvider></Provider>, document.getElementById('app'));

// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();
