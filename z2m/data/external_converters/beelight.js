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
    fromZigbee: [fz.battery, fz.illuminance, fz.temperature, fz.pressure, fz.humidity],
    toZigbee: [],

    // Expose the following informations via MQTT
    exposes: [e.battery(), e.illuminance(), e.temperature(), e.pressure(), e.humidity()],
};

module.exports = definition;
