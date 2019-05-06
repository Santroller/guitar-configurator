import React from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import {Guitar, MemoryLocation} from "../common/avr-types";
import {LinearProgress} from "@material-ui/core";
import {ipcRenderer} from "electron";
const styles = ({palette, spacing} : Theme) => createStyles({
  root: {
    flexGrow: 1
  }
});
interface Props extends WithStyles < typeof styles > {
  guitar: Guitar;
}
interface State {
  location: MemoryLocation;
  percentage: number;
  time: string;
}
class Program extends React.Component<Props, State> {
  componentWillMount() {
    this.setState({location: MemoryLocation.EEPROM, percentage: 0, time: "0:00s"});
  }
  componentDidMount() {
    ipcRenderer.on('program', (event : Event, state: State)=> {
      this.setState(state);
    })
    ipcRenderer.send('program');
  }
  render() {
    const {classes} = this.props;
    return (<div className="App">
      <header className="App-header">
        <div className={classes.root}>
          Programming guitar.
          {this.state.percentage}
          <LinearProgress variant="determinate" value={this.state.percentage}/>
        </div>
      </header>
    </div>);
  }
}

export default withStyles(styles)(Program);
