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
      console.log(guitar);
    });
    ipcRenderer.send("search");
  }
  getTo() {
    return "/install/" + (this.props.guitar && this.props.guitar.board.name.split("-")[0]);
  }
  getImage() {
    if (this.props.guitar && Subtype[this.props.guitar!.config.subtype].indexOf("Guitar") != -1) {
      return (<img src={require("./images/icon.png")} className="App-logo" alt="logo" />);
    }
    return (<img src={require("./images/controller.png")} className="App-logo" alt="logo" />);
  }
  getStatus() {
    if (this.props.guitar && !this.props.guitar.updating) {
      if (this.props.guitar.updating) {
        return (<p>Detected Arduino {this.props.guitar.board.name.charAt(0) + this.props.guitar.board.name.split("-")[0].substring(1)}</p>);
      }
      return (<p>Detected {Subtype[this.props.guitar!.config.subtype]}</p>);
    }
    let ret = (<p>Please connect your Ardwiino Controller, or a new Arduino.</p>);
    if (process.platform == "win32") {
      return (<div>{ret}<br /><p>If you are configuring an existing controller, please press start and select on your controller</p></div>);
    }
    return ret;
  }
  render() {
    const { classes } = this.props;
    return (<div>
      <div className={classes.wrapper}>
        {this.getImage()}
        <CircularProgress size="40vmin" />
      </div>
      <p>Welcome to the Ardwiino Controller configuration tool.</p>
      {this.getStatus()}
      {this.props.guitar && (<Button variant="contained" component={props => <Link {...props} to={this.getTo()} />}>Start configuring</Button>)}
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
