
import { bindings } from ".";
import { INA226Mode } from "./types";
import { sleep } from "./utils";

async function main() {
    // Initialize the INA226 sensor
    await bindings.init(1, 0x40, 0.1);
    console.log("INA226 initialized successfully.");
    console.log("Sensor information:");
    const info = await bindings.getSensorInfo();
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
    console.log("Reset and re-initialize the INA219 sensor.");

    while (true) {
        const shuntVoltage = await bindings.getShuntVoltage();
        const busVoltage = await bindings.getBusVoltage();
        const current = await bindings.getCurrent();
        console.log(`Shunt Voltage: ${shuntVoltage} mV`);
        console.log(`Bus Voltage: ${busVoltage} mV`);
        console.log(`Current: ${current} mA`);
        console.log(`Power: ${bindings.getPower()} mW`);
        await sleep(1000); // Wait for 1 second before the next reading
        console.log("-------------------------------");
    }
}

main()
