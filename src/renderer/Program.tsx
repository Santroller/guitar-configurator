import React from "react";
import { withStyles, createStyles, Theme, WithStyles } from "@material-ui/core/styles";
import { Guitar } from "../common/avr-types";
import { LinearProgress, Button } from "@material-ui/core";
import { ipcRenderer } from "electron";
import { GetApp } from "@material-ui/icons";
import { connect } from "react-redux";
import { MainState } from "./types";
import { RouteComponentProps, Link } from "react-router-dom";
const styles = ({ spacing }: Theme) => createStyles({
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
interface Props extends RouteComponentProps, WithStyles<typeof styles> {
  guitar?: Guitar;
}
interface State {
  percentage: number;
  state: string;
}
class Program extends React.Component<Props, State> {
  componentWillMount() {
    this.setState({ percentage: 0 });
  }
  componentDidMount() {
    if (!this.props.guitar) {
      this.props.history.push("/");
      return;
    }
    ipcRenderer.on("program", (event: Event, state: State) => {
      this.setState(state);
    });
    ipcRenderer.send("program", this.props.guitar);
  }
  render() {
    const { classes } = this.props;
    return (<div className="App">
      <header className="App-header">
        <div className={classes.wrapper}>
          <GetApp className={classes.icon} />
          <LinearProgress className={classes.progress} variant="determinate" value={this.state.percentage} />
          <h3>{this.state.state == "avrdude" ? "Uploading" : "Downloading"}... ({this.state.percentage.toFixed(1)}%)</h3>
          {this.state.percentage >= 100 && <Button variant="contained" component={props => <Link {...props} to={"/config"} />}><GetApp /> Back to configuration</Button>}
        </div>
      </header>
    </div>);
  }
}

const mapStateToProps = (state: MainState) => {
  return {
    guitar: state.guitar
  }
}
export default connect(mapStateToProps)(withStyles(styles)(Program));

