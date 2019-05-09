import React from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import CircularProgress from "@material-ui/core/CircularProgress";
import "./App.css";
import blueGrey from "@material-ui/core/colors/blueGrey";
import {ipcRenderer} from "electron";
import {Guitar} from "../common/avr-types";
import UnoInstall from "./UnoInstall";
import MicroInstall from "./MicroInstall";
import { Button } from "@material-ui/core";
const styles = ({palette, spacing} : Theme) => createStyles({
  root: {
    display: "flex",
    alignItems: "center"
  },
  wrapper: {
    margin: spacing.unit,
    position: "relative"
  },
  progress: {
    color: blueGrey[500],
    position: "absolute",
    top: -6,
    left: -6,
    zIndex: 1
  }
});
interface Props extends WithStyles < typeof styles > {}
interface State {
  guitar: Guitar;
}
class App extends React.Component<Props, State> {
  componentWillMount() {
    this.setState({});
  }
  componentDidMount() {
    ipcRenderer.on("guitar", (event : Event, guitar : Guitar) => {
      this.setState({guitar});
    });
    ipcRenderer.send("search");
  }
  startConfiguring() {
    //route to config page
  }
  render() {
    const {classes} = this.props;
    return (<div className="App">
      <header className="App-header">
        <div className={classes.wrapper}>
          <img src={require("./images/icon.png")} className="App-logo" alt="logo"/>
          <CircularProgress className={classes.progress} size="100%"/>
        </div>
        <p>
          Welcome to the Ardwiino Guitar configuration tool.
          <br/>Please connect your Ardwiino Guitar, or a new Arduino. {process.platform === "win32" && "If you are configuring an existing guitar, please press start and select on your guitar"}
          <br/> {
            this.state.guitar && (
              this.state.guitar.board.name == "micro"
              ? "Found Ardwiino powered by Arduino micro"
              : "Found Ardwiino powered by Arduino Uno")
          }
          <br/>
          <br/>
          {this.state.guitar && (<Button variant="contained" onClick={this.startConfiguring}>Start configuring</Button>)}
        </p>
      </header>
    </div>);
  }
}

export default withStyles(styles)(App);
