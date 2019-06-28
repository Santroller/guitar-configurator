import React, { Dispatch } from "react";
import { withStyles, createStyles, Theme, WithStyles } from "@material-ui/core/styles";
import { Guitar } from "../common/avr-types";
import { ipcRenderer } from "electron";
import { GetApp } from "@material-ui/icons";
import { connect } from "react-redux";
import { MainState } from "./types";
import { RouteComponentProps } from "react-router-dom";
import { ActionTypes, loadGuitar } from "./actions";
import { LinearProgress } from "@material-ui/core";
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
    loadGuitar: (guitar: Guitar) => void;
}
interface State {
    percentage: number;
    state: string;
}
class HoodloaderProgram extends React.Component<Props, State> {
    state = {
        percentage: 0,
        state: ""
    }
    componentDidMount() {
        if (!this.props.guitar) {
            this.props.history.push("/");
            return;
        }
        if (this.props.guitar!.board.manufacturer == "NicoHood") {
            this.props.history.push("/config");
            return;
        }
        ipcRenderer.on("program", (event: Event, state: State) => {
            this.setState(state);
            if (state.percentage >= 100) {
                ipcRenderer.on("guitar", (event: Event, guitar: Guitar) => {
                    if (guitar.board.manufacturer == "NicoHood") {
                        this.props.loadGuitar(guitar);

                    } else {
                        ipcRenderer.send("search");
                    }
                });
                ipcRenderer.send("search");
            }
        });
        ipcRenderer.send("programHoodloader", this.props.guitar);
    }
    renderUnoHookups() {
        return (
            <div>
                <p>Please unplug your Arduino, and hook up the following components: </p>
                <img width="50%" src={require("./images/uno-hoodloader.png")}></img>
            </div>
        )
    }
    renderProgress() {
        const { classes } = this.props;
        return (<div>
            <GetApp className={classes.icon} />
            <LinearProgress className={classes.progress} variant="determinate" value={this.state.percentage} />
            <h3>Uploading... ({this.state.percentage.toFixed(1)}%)</h3>
        </div>)
    }
    render() {
        if (this.props.guitar!.board.manufacturer == "NicoHood") {
            this.props.history.push("/config");
        }
        const { classes } = this.props;
        return (
            <div className={classes.wrapper}>
                {this.state.percentage < 100 && this.renderProgress()}
                {this.state.percentage >= 100 && this.renderUnoHookups()}
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
export default connect(mapStateToProps, mapDispatchToProps)(withStyles(styles)(HoodloaderProgram));

