import { Guitar } from "../common/avr-types";
import { LOAD_GUITAR } from "./types";
interface LoadGuitarAction {
    type: typeof LOAD_GUITAR
    payload: Guitar
}

export type ActionTypes = LoadGuitarAction;

export function loadGuitar(guitar: Guitar): ActionTypes {
    return {
        type: LOAD_GUITAR,
        payload: guitar
    }
}