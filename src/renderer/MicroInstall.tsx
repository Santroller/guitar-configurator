import React from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import "./App.css";
import {Typography, Link, CircularProgress, Button} from "@material-ui/core";
import {blueGrey} from "@material-ui/core/colors";
import {Guitar} from "../common/avr-types";
import { ipcRenderer } from "electron";
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
  button: {
    margin: spacing.unit
  },
  text: {
    width: "80%"
  }
});
interface Props extends WithStyles < typeof styles > {
  guitar: Guitar;
}
function setFreq(freq: number, guitar: Guitar) {
  guitar.config.cpu_freq = freq;
  //Route to the programmer.
}
const MicroInstall: React.FunctionComponent<Props> = props => {
  const {classes} = props;
  return (<div className="App">
    <header className="App-header">
      <div className={classes.wrapper}>
        <img src={require("./images/icon.png")} className="App-logo" alt="logo"/>
        <CircularProgress className={classes.progress} size="100%"/>
      </div>
      <p className={classes.text}>
        <Typography variant="h5">
          There are multiple Arduino Pro Micro variants. Please select if you are using a 3.3v or a 5v Arduino
        </Typography>
        <Button variant="contained" className={classes.button} onClick={()=>setFreq(8000000, props.guitar)}>3.3v</Button>
        <Button variant="contained" className={classes.button} onClick={()=>setFreq(16000000, props.guitar)}>5v</Button>
      </p>
    </header>
  </div>);
};

export default withStyles(styles)(MicroInstall);
