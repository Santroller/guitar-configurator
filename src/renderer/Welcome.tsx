import React, { Dispatch } from "react";
import { withStyles, createStyles, Theme, WithStyles } from "@material-ui/core/styles";
import CircularProgress from "@material-ui/core/CircularProgress";
import "./App.css";
import { ipcRenderer } from "electron";
import { Guitar, Subtype } from "../common/avr-types";
import { Button } from "@material-ui/core";
import { Link } from "react-router-dom";
import { MainState } from './types';
import { connect } from "react-redux";
import { ActionTypes, loadGuitar } from './actions';
const styles = ({ palette, spacing }: Theme) => createStyles({
  root: {
    display: "flex",
    alignItems: "center"
  },
  wrapper: {
    margin: spacing.unit,
    position: "relative"
  }
});
interface Props extends WithStyles<typeof styles> {
  guitar?: Guitar;
  loadGuitar: (guitar: Guitar) => void;
}
class Welcome extends React.Component<Props> {
  componentDidMount() {
    ipcRenderer.on("guitar", (event: Event, guitar: Guitar) => {
      this.props.loadGuitar(guitar);
    });
    ipcRenderer.send("search");
  }
  getTo() {
    if (this.props.guitar!.board) {
      return "/install/" + (this.props.guitar && this.props.guitar.board.name.split("-")[0]);
    }
    return "/config";
  }
  getImage() {
    if (this.props.guitar && !this.props.guitar!.updating && Subtype[this.props.guitar!.config.subtype].indexOf("Guitar") != -1) {
      return (<img src={require("./images/icon.png")} className="App-logo" alt="logo" />);
    }
    return (<img src={require("./images/controller.png")} className="App-logo" alt="logo" />);
  }
  getStatus() {
    if (this.props.guitar) {
      if (this.props.guitar.board) {
        if (this.props.guitar.board.processor == "atmega8u2") {
          return (
            <p>You currently have an Arduino Uno connected that is Revision 2 or lower. Currently, this is an unsupported device.</p>
          )
        }
        return (<p>Detected {this.props.guitar.board!.cleanName}</p>);
      }
      return (<p>Detected {Subtype[this.props.guitar!.config.subtype]}</p>);
    }
    return (<p>Please connect your Ardwiino Controller, or a new Arduino.</p>);
  }
  supported() {
    return this.props.guitar && (!this.props.guitar.board || this.props.guitar.board.processor != "atmega8u2");
  }
  render() {
    const { classes } = this.props;
    return (<div>
      <div className={classes.wrapper}>
        {this.getImage()}
        <CircularProgress size="40vmin" />
      </div>
      <p>Welcome to the Ardwiino Controller configuration tool.</p>
      <p>Please note that this tool only supports a single controller at a time.</p>
      {this.getStatus()}
      {this.props.guitar && this.supported() && (<Button variant="contained" component={props => <Link {...props} to={this.getTo()} />}>Start configuring</Button>)}
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
    loadGuitar: (guitar: Guitar) => dispatch(loadGuitar(guitar))
  }
}
export default connect(mapStateToProps, mapDispatchToProps)(withStyles(styles)(Welcome));
