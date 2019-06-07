import { MainState, LOAD_GUITAR } from './types';
import { ActionTypes } from './actions';

const initialState: MainState = {
}
export function mainReducer(state = initialState,action: ActionTypes): MainState {
    switch(action.type) {
        case LOAD_GUITAR:
        return {
            guitar: action.payload
        }
        default: return state
    }
}