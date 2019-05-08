import React from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import {Guitar} from "../common/avr-types";
import {LinearProgress} from "@material-ui/core";
import {ipcRenderer} from "electron";
import {GetApp} from "@material-ui/icons";
const styles = ({palette, spacing} : Theme) => createStyles({
  root: {
    display: "flex",
    alignItems: "center"
  },
  wrapper: {
    margin: spacing.unit,
    position: "relative",
    fontSize: 32
  },
  icon: {
    fontSize: "25vh"
  },
  progress: {
    width: "75vw"
  }
});
interface Props extends WithStyles < typeof styles > {
  guitar: Guitar;
}
interface State {
  percentage: number;
  state: string;
}
class Program extends React.Component<Props, State> {
  componentWillMount() {
    this.setState({percentage: 0});
  }
  componentDidMount() {
    ipcRenderer.on("program", (event : Event, state : State) => {
      this.setState(state);
    });
    ipcRenderer.send("program", this.props.guitar);
  }
  render() {
    const {classes} = this.props;
    return (<div className="App">
      <header className="App-header">
        <div className={classes.wrapper}>
          <GetApp className={classes.icon}/>
          <LinearProgress className={classes.progress} variant="determinate" value={this.state.percentage}/>
          <h3>{this.state.state == "avrdude"?"Uploading":"Downloading"}... ({this.state.percentage.toFixed(1)}%)</h3>
        </div>
      </header>
    </div>);
  }
}

export default withStyles(styles)(Program);
