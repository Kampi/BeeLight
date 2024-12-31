const {Zcl} = require("zigbee-herdsman");
const m = require("zigbee-herdsman-converters/lib/modernExtend");
const fz = require("zigbee-herdsman-converters/converters/fromZigbee");
const tz = require("zigbee-herdsman-converters/converters/toZigbee");
const exposes = require("zigbee-herdsman-converters/lib/exposes");
const reporting = require("zigbee-herdsman-converters/lib/reporting");
const ota = require("zigbee-herdsman-converters/lib/ota");
const utils = require("zigbee-herdsman-converters/lib/utils");
const globalStore = require("zigbee-herdsman-converters/lib/store");
const logger = require("zigbee-herdsman-converters/lib/logger");
const e = exposes.presets;
const ea = exposes.access;

const definition = {
    zigbeeModel: ["BeeLight_v2.0"],
    model: "BeeLight_v2.0",
    vendor: "Kampi",
    description: "Zigbee based Light and Environment Sensor (https://github.com/Kampi/BeeLight)",

    // Define the converter functions to parse the Zigbee messages
    fromZigbee: [fz.battery, fz.illuminance, fz.temperature, fz.pressure, fz.humidity, fz.BeeLight_co2, fz.BeeLight_iaq, fz.BeeLight_voc],
    toZigbee: [],

    configure: async (device, coordinatorEndpoint, logger) => {    
        const endpoint = device.getEndpoint(10);

        // Bind the entpoint to get the data from the device
        await reporting.bind(endpoint, coordinatorEndpoint, ["genPowerCfg", "msIlluminanceMeasurement", "msTemperatureMeasurement", "msPressureMeasurement", "msRelativeHumidity", "msCO2", "msIAQ", "msVOC"]);
    },

    // Expose the sensor values via MQTT
    exposes: [
	    e.battery(),
		e.illuminance(),
		e.temperature(),
		e.pressure(),
		e.humidity(),
		e.numeric("iaq", ea.STATE)
        	.withUnit("idx")
        	.withDescription("Indoor Air Quality index")
        	.withValueMin(0)
        	.withValueMax(500),
		e.numeric("voc", ea.STATE)
        	.withUnit("ppm")
        	.withDescription("Volatile Organic Compounds equivalent")
        	.withValueMin(0)
        	.withValueMax(10000),
		e.numeric("co2", ea.STATE)
        	.withUnit("ppm")
        	.withDescription("Carbon Dioxide equivalent")
        	.withValueMin(400)
	],
};

module.exports = definition;
