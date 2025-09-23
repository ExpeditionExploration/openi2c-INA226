
const binding = require('bindings');
import {     
    INA226Mode,
    INA226Info,
    INA226AverageMode,
    INA226ConversionTime
} from "./types"
import { type INA226 } from "./types";
import * as utils from './utils';

const bindings: INA226 = binding('ina226_native');

// Bindins
export {
    bindings,
    utils,
    INA226Mode, INA226Info, INA226AverageMode, INA226ConversionTime
}
