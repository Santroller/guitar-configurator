import React from "react";
import "./App.css";
import UnoInstall from "./UnoInstall";
import MicroInstall from "./MicroInstall";
import { Route, HashRouter } from "react-router-dom";
import Welcome from "./Welcome";
import Program from "./Program";
import Config from "./Config";

const App: React.FunctionComponent = () => (
  <div className="App">
    <HashRouter>
      <header />
      <Route path="" exact component={Welcome} />
      <Route path="install/micro" component={MicroInstall} />
      <Route path="install/uno" component={UnoInstall} />
      <Route path="install/program" component={Program} />
      <Route path="config" component={Config} />
    </HashRouter>
  </div>
);

export default App;
