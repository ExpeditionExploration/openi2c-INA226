
/**
 * INA226 mode enumeration.
 */
export enum INA226Mode {
    /** power down */
    POWER_DOWN = 0x0,
    /** shunt voltage triggered */
    SHUNT_VOLTAGE_TRIGGERED = 0x1,
    /** bus voltage triggered */
    BUS_VOLTAGE_TRIGGERED = 0x2,
    /** shunt and bus triggered */
    SHUNT_BUS_VOLTAGE_TRIGGERED = 0x3,
    SHUTDOWN = 0x4,
    /** shunt voltage continuous */
    SHUNT_VOLTAGE_CONTINUOUS = 0x5,
    /** bus voltage continuous */
    BUS_VOLTAGE_CONTINUOUS = 0x6,
    /** shunt and bus voltage continuous */
    SHUNT_BUS_VOLTAGE_CONTINUOUS = 0x7,
}

/**
 * INA219 sensor information.
 */
export type INA226Info = {
    /** chip name */
    chipName: string;         
    /** manufacturer name */       
    manufacturerName: string; 
    /** chip interface name */     
    interface: string;     
    /** chip min supply voltage */            
    supplyVoltageMinV: number;   
    /** chip max supply voltage */     
    supplyVoltageMaxV: number;      
    /** chip max current */  
    maxCurrentMilliA: number;
    /** chip min operating temperature */              
    temperatureMin: number;     
    /** chip max operating temperature */        
    temperatureMax: number;  
    /** driver version */           
    driverVersion: number;           
}

export type INA226 = {
    /// Basic initialization of the INA219 sensor.
    init: (bus: number, addr: number, r: number) => Promise<void>,

    /// Return the shunt voltage in millivolts.
    getShuntVoltage: () => Promise<number>,

    /// Return the bus voltage in millivolts.
    getBusVoltage: () => Promise<number>,

    /// Return the current in milliamps.
    getCurrent: () => Promise<number>,

    /// Return the power in milliwatts.
    getPower: () => Promise<number>,

    /// Do a software reset of the INA219 sensor.
    softReset: () => Promise<void>,

    /// Get the sensor information.
    getSensorInfo: () => Promise<INA226Info>,

    /// Set the mode of the INA219 sensor.
    setMode: (mode: INA226Mode) => Promise<void>,
}
