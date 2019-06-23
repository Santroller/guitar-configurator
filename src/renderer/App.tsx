import React from "react";
import "./App.css";
import MicroInstall from "./MicroInstall";
import { Route, MemoryRouter } from "react-router-dom";
import Welcome from "./Welcome";
import Program from "./Program";
import Config from "./Config";
import HoodloaderProgram from "./HoodloaderProgram";

const App: React.FunctionComponent = () => (
  <div className="App">
    <MemoryRouter>
      <header />
      <Route path="/" exact component={Welcome} />
      <Route path="/install/micro" component={MicroInstall} />
      <Route path="/install/uno" component={HoodloaderProgram} />
      <Route path="/install/program" component={Program} />
      <Route path="/config" component={Config} />
    </MemoryRouter>
  </div>
);

export default App;
