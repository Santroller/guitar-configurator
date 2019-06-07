import React, { Dispatch } from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import "./App.css";
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
const Configuration: React.FunctionComponent<Props> = props => {
  const {classes} = props;
  if (!props.guitar) props.history.push("/");
  return (<div>
      Start configuring.
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
export default connect(mapStateToProps, mapDispatchToProps)(withStyles(styles)(Configuration));
