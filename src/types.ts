
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

export enum INA226AverageMode {
    /**< 1 average */
    INA226_AVG_1   = 0,        
    /**< 4 averages */
    INA226_AVG_4   = 1,        
    /**< 16 averages */
    INA226_AVG_16  = 2,        
    /**< 64 averages */
    INA226_AVG_64  = 3,        
    /**< 128 averages */
    INA226_AVG_128 = 4,        
    /**< 256 averages */
    INA226_AVG_256 = 5,        
    /**< 512 averages */
    INA226_AVG_512 = 6,        
    /**< 1024 averages */
    INA226_AVG_1024= 7,        
}

/**
 * INA226 conversion times
 */
export enum INA226ConversionTime {
    /**< 140 us */
    CONVERSION_TIME_140_US  = 0,
    /**< 204 us */
    CONVERSION_TIME_204_US  = 1,       
    /**< 332 us */ 
    CONVERSION_TIME_332_US  = 2,    
    /**< 588 us */    
    CONVERSION_TIME_588_US  = 3,        
    /**< 1.1 ms */
    CONVERSION_TIME_1P1_MS  = 4,       
    /**< 2.116 ms */ 
    CONVERSION_TIME_2P116_MS= 5,       
    /**< 4.156 ms */
    CONVERSION_TIME_4P156_MS= 6,
    /**< 8.244 ms */
    CONVERSION_TIME_8P244_MS= 7,
}

/**
 * INA226 sensor information.
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
    /**
     * Basic init of INA226
     * 
     * params:
     *  - *bus* - I2C bus number
     *  - *addr* - I2C address
     *  - *r* - shunt resistor value in ohms
     *  - *conversion_time_shunt* - shunt voltage conversion time
     *  - *conversion_time_bus* - bus voltage conversion time
     *  - *averaging_mode* - averaging mode
     *  - *mode* - operating mode
     */
    init: (
        bus: number, addr: number, r: number,
        conversion_time_shunt: INA226ConversionTime,
        conversion_time_bus: INA226ConversionTime,
        averaging_mode: INA226AverageMode,
        mode: INA226Mode
    ) => void,

    /// Return the shunt voltage in millivolts.
    getShuntVoltage: () => number,

    /// Return the bus voltage in millivolts.
    getBusVoltage: () => number,

    /// Return the current in milliamps.
    getCurrent: () => number,

    /// Return the power in milliwatts.
    getPower: () => number,

    /// Do a software reset of the INA226 sensor.
    softReset: () => void,

    /// Get the sensor information.
    getSensorInfo: () => INA226Info,

    /// Set the mode of the INA226 sensor.
    setMode: (mode: INA226Mode) => void,
}
