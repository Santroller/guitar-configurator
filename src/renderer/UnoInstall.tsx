import React, { Dispatch } from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import "./App.css";
import {Typography, Link, CircularProgress} from "@material-ui/core";
import {ipcRenderer} from "electron";
import {Guitar} from "../common/avr-types";
import { RouteComponentProps } from "react-router-dom";
import { MainState } from "./types";
import { ActionTypes, loadGuitar } from "./actions";
import { connect } from "react-redux";
const styles = ({palette, spacing} : Theme) => createStyles({
  root: {
    display: "flex",
    alignItems: "center"
  },
  wrapper: {
    margin: spacing.unit,
    position: "relative"
  },
  text: {
    width: "80%"
  }
});
interface Props extends RouteComponentProps, WithStyles < typeof styles > {
  guitar?: Guitar;
  loadGuitar: (guitar: Guitar) => void;
}

class UnoInstall extends React.Component < Props, {} > {
  componentDidMount() {
    ipcRenderer.on("guitar", (event : Event, guitar : Guitar) => {
      if (guitar.board.manufacturer == "NicoHood") {
        this.props.loadGuitar(guitar);
      } else {
        ipcRenderer.send("search");
      }
    });
  }
  render() {
    const {classes} = this.props;
    if (this.props.guitar && this.props.guitar.board.manufacturer == "NicoHood") {
      this.props.history.push("/config");
    }
    return (<div className="App">
      <header className="App-header">
      <div className={classes.wrapper}>
        <img src={require("./images/controller.png")} className="App-logo" alt="logo"/>
        <CircularProgress size="40vmin"/>
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

const mapStateToProps = (state: MainState) => {
  return {
    guitar: state.guitar
  }
}
const mapDispatchToProps = (dispatch: Dispatch<ActionTypes>) => {
  return {
    loadGuitar: (guitar: Guitar)=>dispatch(loadGuitar(guitar))
  }
}
export default connect(mapStateToProps, mapDispatchToProps)(withStyles(styles)(UnoInstall));
