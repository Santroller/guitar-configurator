import React from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import "./App.css";
import {Typography, Link, CircularProgress} from "@material-ui/core";
import {blueGrey} from "@material-ui/core/colors";
import {ipcRenderer} from "electron";
import {Guitar} from "../common/avr-types";
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
  },
  text: {
    width: "80%"
  }
});
interface Props extends WithStyles < typeof styles > {}

class UnoInstall extends React.Component < Props, {} > {
  componentWillMount() {
    this.setState({});
  }
  componentDidMount() {
    ipcRenderer.on("guitar", (event : Event, guitar : Guitar) => {
      if (guitar.board.manufacturer == "NicoHood") {
        //Route to the programmer.
      } else {
        ipcRenderer.send("search");
      }
    });
  }
  render() {
    const {classes} = this.props;
    return (<div className="App">
      <header className="App-header">
        <div className={classes.wrapper}>
          <img src={require("./images/icon.png")} className="App-logo" alt="logo"/>
          <CircularProgress className={classes.progress} size="100%"/>
        </div>
        <p className={classes.text}>
          <Typography variant="h5">
            Your Arduino Uno requires a custom bootloader in order to be detected as a controller. Please follow this&nbsp;
            <Link href="https://github.com/NicoHood/HoodLoader2/wiki/Hardware-Installation" target="_blank">
              tutorial
            </Link>&nbsp; for instructions on how to upload this. Once this bootloader is detected, we will continue programming your device.
          </Typography>
        </p>
      </header>
    </div>);
  }
}

export default withStyles(styles)(UnoInstall);
