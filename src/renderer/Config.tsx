import React, { Dispatch } from "react";
import { withStyles, createStyles, Theme, WithStyles } from "@material-ui/core/styles";
import "./App.css";
import { Guitar, InputType, OutputType, Subtype, TiltSensor } from '../common/avr-types';
import { RouteComponentProps, Link } from "react-router-dom";
import { connect } from "react-redux";
import { MainState } from "./types";
import { loadGuitar, ActionTypes } from "./actions";
import { List, ListItem, ListItemIcon, ListItemText, Button } from "@material-ui/core";
import { VideogameAsset, SettingsInputComponent, Games, Settings, Keyboard, RotateRight, GetApp } from "@material-ui/icons";
import { generateSelect } from "./utils";
import { ipcRenderer } from "electron";
const styles = ({ spacing }: Theme) => createStyles({
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
interface Props extends RouteComponentProps, WithStyles<typeof styles> {
  guitar?: Guitar;
  loadGuitar: (guitar: Guitar) => void;
}
const Configuration: React.FunctionComponent<Props> = props => {
  if (!props.guitar) {
    props.history.push("/");
    return (<div></div>);
  }
  return (<div>
    <List component="nav">
      <ListItem button>
        <ListItemIcon>
          <SettingsInputComponent />
        </ListItemIcon>
        <ListItemText primary="Input Type" />
        {generateSelect("input_type", props.guitar, props.loadGuitar, InputType)}
      </ListItem>
      <ListItem button>
        <ListItemIcon>
          <VideogameAsset />
        </ListItemIcon>
        <ListItemText primary="Output Type" />
        {generateSelect("output_type", props.guitar, props.loadGuitar, OutputType)}
      </ListItem>
      <ListItem button>
        <ListItemIcon>
          <Games />
        </ListItemIcon>
        <ListItemText primary="Controller Type" />
        {generateSelect("subtype", props.guitar, props.loadGuitar, Subtype)}
      </ListItem>
      {props.guitar.config.input_type == InputType.Direct && (<ListItem button>
        <ListItemIcon>
          <Settings />
        </ListItemIcon>
        <ListItemText primary="Pin Bindings" />
      </ListItem>)}

      {props.guitar.config.output_type == OutputType.Keyboard && (<ListItem button>
        <ListItemIcon>
          <Keyboard />
        </ListItemIcon>
        <ListItemText primary="Keyboard binding" />
      </ListItem>)}

      {Subtype[props.guitar.config.subtype].indexOf("Guitar") != -1 && (<ListItem button>
        <ListItemIcon>
          <RotateRight />
        </ListItemIcon>
        <ListItemText primary="Tilt configuration" />
        {generateSelect("tilt_type", props.guitar, props.loadGuitar, TiltSensor)}
      </ListItem>)}
    </List>
    <Button variant="contained" onClick={() => ipcRenderer.send("uploadConfig", props.guitar)}><GetApp /> Send Configuration To Controller</Button>
    <Button variant="contained" component={props => <Link {...props} to={"/install/program"} />}><GetApp /> Update Firmware</Button>
  </div>);
};

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
export default connect(mapStateToProps, mapDispatchToProps)(withStyles(styles)(Configuration));
