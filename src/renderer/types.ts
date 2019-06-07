import { Guitar } from "../common/avr-types";

export const LOAD_GUITAR = 'LOAD_GUITAR';
export interface MainState {
    guitar?: Guitar;
}