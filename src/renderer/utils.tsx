import { MenuItem, Select } from "@material-ui/core";
import React from "react";
import { Guitar } from "../common/avr-types";

export function generateSelect(variable: string, guitar: Guitar, loadGuitar: (guitar: Guitar) => void, type: { [key: number]: string }) {

    function updateGuitar(event: React.FormEvent<HTMLSelectElement>) {
        guitar.config[variable] = parseInt((event.target as any).value);
        loadGuitar(guitar);
    }
    return (
        <Select
          value={guitar.config[variable]}
          onChange={updateGuitar}
          inputProps={{
            name: variable,
            id: variable+'-simple',
          }}
        >{Object.values(type).map(s => Number(s)).filter(s => !isNaN(s)).map(s => <MenuItem value={s} key={s}>{type[s]}</MenuItem>)}</Select>
    )
}