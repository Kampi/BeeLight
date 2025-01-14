const {} = require('zigbee-herdsman-converters/lib/modernExtend');
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
    zigbeeModel: ["BeeLight_v1.0"],
    model: "BeeLight_v2.0",
    vendor: "Kampi",
    description: "ZigBee based Light Sensor (https://github.com/Kampi/BeeLight)",
    fromZigbee: [fz.identify, fz.battery, fz.illuminance, fz.temperature, fz.pressure, fz.humidity],
    toZigbee: [],
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint10 = device.getEndpoint(10);

		// Bind an endpoint to specific clusters
        await reporting.bind(endpoint10, coordinatorEndpoint, ["genPowerCfg", "msIlluminanceMeasurement", "msTemperatureMeasurement", "msPressureMeasurement", "msRelativeHumidity"]);

		// Fetch the values from the device
		await reporting.batteryVoltage(endpoint10);
		await reporting.illuminance(endpoint10, {min: 1, max: constants.repInterval.MINUTES_1, change: 10});
		await reporting.batteryPercentageRemaining(endpoint10);
    },

	// Expose the following informations via MQTT
    exposes: [e.identify(), e.battery(), e.illuminance(), e.temperature(), e.pressure(), e.humidity()],
};

module.exports = definition;
