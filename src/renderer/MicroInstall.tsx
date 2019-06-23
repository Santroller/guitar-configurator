import React, { Dispatch } from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import "./App.css";
import {Typography, CircularProgress, Button} from "@material-ui/core";
import {Guitar} from "../common/avr-types";
import { RouteComponentProps } from "react-router-dom";
import { connect } from "react-redux";
import { MainState } from "./types";
import { loadGuitar, ActionTypes } from "./actions";
const styles = ({palette, spacing} : Theme) => createStyles({
  root: {
    display: "flex",
    alignItems: "center"
  },
  wrapper: {
    margin: spacing.unit,
    position: "relative"
  },
  button: {
    margin: spacing.unit
  },
  text: {
    width: "80%"
  }
});
interface Props extends RouteComponentProps, WithStyles < typeof styles > {
  guitar?: Guitar;
  loadGuitar: (guitar: Guitar) => void;
}
function setFreq(freq: number, props: Props) {
  props.guitar!.config.cpu_freq = freq;
  props.loadGuitar(props.guitar!);
}
const MicroInstall: React.FunctionComponent<Props> = props => {
  const {classes} = props;
  console.log(props);
  if (!props.guitar) props.history.push("/");
  if (props.guitar!.config.cpu_freq != 0) props.history.push("/config");
  return (<div>
      <div className={classes.wrapper}>
        <img src={require("./images/controller.png")} className="App-logo" alt="logo"/>
        <CircularProgress size="40vmin"/>
      </div>
      <Typography variant="h5">
        There are multiple Arduino Pro Micro variants. Please select if you are using a 3.3v or a 5v Arduino
      </Typography>
      <div>
        <Button variant="contained" className={classes.button} onClick={()=>setFreq(8000000, props)}>3.3v</Button>
        <Button variant="contained" className={classes.button} onClick={()=>setFreq(16000000, props)}>5v</Button>
      </div>
  </div>);
};

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
export default connect(mapStateToProps, mapDispatchToProps)(withStyles(styles)(MicroInstall));
