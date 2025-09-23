
import { bindings } from ".";
import { INA226Mode, INA226ConversionTime, INA226AverageMode } from "./types";
import { sleep } from "./utils";

async function main() {
    // Initialize the INA226 sensor
    bindings.init(1, 0x40, 0.1,
        INA226ConversionTime.CONVERSION_TIME_1P1_MS,
        INA226ConversionTime.CONVERSION_TIME_1P1_MS,
        INA226AverageMode.INA226_AVG_16,
        INA226Mode.SHUNT_BUS_VOLTAGE_CONTINUOUS
    );
    console.log("INA226 initialized successfully.");
    console.log("Sensor information:");
    const info = bindings.getSensorInfo();
    console.log(`chipName: ${info.chipName}`);
    console.log(`manufacturerName: ${info.manufacturerName}`);
    console.log(`interface: ${info.interface}`);
    console.log(`supplyVoltageMinV: ${info.supplyVoltageMinV} V`);
    console.log(`supplyVoltageMaxV: ${info.supplyVoltageMaxV} V`);
    console.log(`maxCurrentMilliA: ${info.maxCurrentMilliA} mA`);
    console.log(`temperatureMin: ${info.temperatureMin} °C`);
    console.log(`temperatureMax: ${info.temperatureMax} °C`);
    console.log(`driverVersion: ${info.driverVersion}`);
    console.log("-------------------------------");
    console.log("Reset and re-initialize the INA226 sensor.");

    while (true) {
        const shuntVoltage = bindings.getShuntVoltage();
        const busVoltage = bindings.getBusVoltage();
        const current = bindings.getCurrent();
        console.log(`Shunt Voltage: ${shuntVoltage.toFixed(2)} mV`);
        console.log(`Bus Voltage: ${busVoltage.toFixed(2)} mV`);
        console.log(`Current: ${current.toFixed(2)} mA`);
        console.log(`Power: ${bindings.getPower().toFixed(2)} mW`);
        await sleep(1000); // Wait for 1 second before the next reading
        console.log("-------------------------------");
    }
}

main()
