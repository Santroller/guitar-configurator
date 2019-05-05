import React from "react";
import {withStyles, createStyles, Theme, WithStyles} from "@material-ui/core/styles";
import CircularProgress from "@material-ui/core/CircularProgress";
import "./App.css";
import blueGrey from '@material-ui/core/colors/blueGrey';
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
    position: 'absolute',
    top: -6,
    left: -6,
    zIndex: 1,
  },
});
interface Props extends WithStyles<typeof styles> {}
const App: React.FC<Props> = props => {
  const {classes} = props;
  return (<div className="App">
    <header className="App-header">
      <div className={classes.wrapper}>
        <img src={require("./icon.png")} className="App-logo" alt="logo"/>
        <CircularProgress className={classes.progress} size='100%'/>
      </div>
      <p>
        Welcome to the Ardwiino Guitar configuration tool.
        <br />Please connect your Ardwiino Guitar, or a new Arduino.
      </p>
    </header>
  </div>);
};

export default withStyles(styles)(App);
