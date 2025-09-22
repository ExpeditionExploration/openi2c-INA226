
const binding = require('bindings');
import {     
    INA226Mode,
    INA226Info,
} from "./types"
import { type INA226 } from "./types";
import * as utils from './utils';

const bindings: INA226 = binding('ina226_native');

// Bindins
export {
    bindings,
    utils,
    INA226Mode as INA219Mode, INA226Info as INA219Info
}
