const m = require('zigbee-herdsman-converters/lib/modernExtend');
const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const ota = require('zigbee-herdsman-converters/lib/ota');
const utils = require('zigbee-herdsman-converters/lib/utils');
const globalStore = require('zigbee-herdsman-converters/lib/store');
const e = exposes.presets;
const ea = exposes.access;

const definition = {
    zigbeeModel: ["BeeLight_v2.0"],
    model: "BeeLight_v2.0",
    vendor: "Kampi",
    description: "Zigbee based Light and Environment Sensor (https://github.com/Kampi/BeeLight)",

    // Define the converter functions to parse the Zigbee messages
    fromZigbee: [fz.battery, fz.illuminance, fz.temperature, fz.pressure, fz.humidity],
    toZigbee: [],

    configure: async (device, coordinatorEndpoint, logger) => {    
        const endpoint = device.getEndpoint(10);

        // Bind the entpoint to get the data from the device
        await reporting.bind(endpoint, coordinatorEndpoint, ["genPowerCfg", "msIlluminanceMeasurement", "msTemperatureMeasurement", "msPressureMeasurement", "msRelativeHumidity"]);

        //await reporting.temperature(endpoint, {
        //    min: 10,      // mindestens alle 10 Sekunden
        //    max: 15,     // spätestens alle 5 Minuten
        //    change: 50,   // bei Änderung von 0,5 °C (Wert = 50 bei 0.01°C Auflösung)
        //    });

        //await reporting.batteryPercentageRemaining(endpoint, {
        //    min: 10,      // mindestens alle 10 Sekunden
        //    max: 15,     // spätestens alle 5 Minuten
        //    change: 1,   // bei Änderung von 0,5 °C (Wert = 50 bei 0.01°C Auflösung)
        //    });
    },

    // Expose the following information via MQTT
    exposes: [e.battery(), e.illuminance(), e.temperature(), e.pressure(), e.humidity()],
};

module.exports = definition;
