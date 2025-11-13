"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || (function () {
    var ownKeys = function(o) {
        ownKeys = Object.getOwnPropertyNames || function (o) {
            var ar = [];
            for (var k in o) if (Object.prototype.hasOwnProperty.call(o, k)) ar[ar.length] = k;
            return ar;
        };
        return ownKeys(o);
    };
    return function (mod) {
        if (mod && mod.__esModule) return mod;
        var result = {};
        if (mod != null) for (var k = ownKeys(mod), i = 0; i < k.length; i++) if (k[i] !== "default") __createBinding(result, mod, k[i]);
        __setModuleDefault(result, mod);
        return result;
    };
})();
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.GPDF_COMMANDS = exports.TIME_LOOKUP = void 0;
exports.setupAttributes = setupAttributes;
exports.setupConfigureForReporting = setupConfigureForReporting;
exports.setupConfigureForBinding = setupConfigureForBinding;
exports.setupConfigureForReading = setupConfigureForReading;
exports.forceDeviceType = forceDeviceType;
exports.forcePowerSource = forcePowerSource;
exports.linkQuality = linkQuality;
exports.battery = battery;
exports.deviceTemperature = deviceTemperature;
exports.identify = identify;
exports.onOff = onOff;
exports.commandsOnOff = commandsOnOff;
exports.poll = poll;
exports.writeTimeDaily = writeTimeDaily;
exports.iasArmCommandDefaultResponse = iasArmCommandDefaultResponse;
exports.iasGetPanelStatusResponse = iasGetPanelStatusResponse;
exports.customTimeResponse = customTimeResponse;
exports.BeeLight_iaq = BeeLight_iaq;
exports.BeeLight_co2 = BeeLight_co2;
exports.BeeLight_voc = BeeLight_voc;
exports.illuminance = illuminance;
exports.temperature = temperature;
exports.pressure = pressure;
exports.flow = flow;
exports.humidity = humidity;
exports.soilMoisture = soilMoisture;
exports.windSpeed = windSpeed;
exports.occupancy = occupancy;
exports.co2 = co2;
exports.pm25 = pm25;
exports.light = light;
exports.commandsLevelCtrl = commandsLevelCtrl;
exports.commandsColorCtrl = commandsColorCtrl;
exports.lightingBallast = lightingBallast;
exports.lock = lock;
exports.windowCovering = windowCovering;
exports.commandsWindowCovering = commandsWindowCovering;
exports.iasZoneAlarm = iasZoneAlarm;
exports.iasWarning = iasWarning;
exports.electricityMeter = electricityMeter;
exports.gasMeter = gasMeter;
exports.genericGreenPower = genericGreenPower;
exports.commandsScenes = commandsScenes;
exports.enumLookup = enumLookup;
exports.numeric = numeric;
exports.binary = binary;
exports.text = text;
exports.actionEnumLookup = actionEnumLookup;
exports.quirkAddEndpointCluster = quirkAddEndpointCluster;
exports.quirkCheckinInterval = quirkCheckinInterval;
exports.reconfigureReportingsOnDeviceAnnounce = reconfigureReportingsOnDeviceAnnounce;
exports.skipDefaultResponse = skipDefaultResponse;
exports.deviceEndpoints = deviceEndpoints;
exports.deviceAddCustomCluster = deviceAddCustomCluster;
exports.ignoreClusterReport = ignoreClusterReport;
exports.bindCluster = bindCluster;
exports.thermostat = thermostat;
const node_assert_1 = __importDefault(require("node:assert"));
const zigbee_herdsman_1 = require("zigbee-herdsman");
const fz = __importStar(require("../converters/fromZigbee"));
const tz = __importStar(require("../converters/toZigbee"));
const constants = __importStar(require("../lib/constants"));
const logger_1 = require("../lib/logger");
const globalStore = __importStar(require("../lib/store"));
const exposes_1 = require("./exposes");
const light_1 = require("./light");
const utils_1 = require("./utils");
function getEndpointsWithCluster(device, cluster, type) {
    if (!device.endpoints) {
        throw new Error(`${device.ieeeAddr} ${device.endpoints}`);
    }
    const endpoints = type === "input"
        ? device.endpoints.filter((ep) => ep.getInputClusters().find((c) => ((0, utils_1.isNumber)(cluster) ? c.ID === cluster : c.name === cluster)))
        : device.endpoints.filter((ep) => ep.getOutputClusters().find((c) => ((0, utils_1.isNumber)(cluster) ? c.ID === cluster : c.name === cluster)));
    if (endpoints.length === 0) {
        throw new Error(`Device ${device.ieeeAddr} has no ${type} cluster ${cluster}`);
    }
    return endpoints;
}
const NS = "zhc:modernextend";
const IAS_EXPOSE_LOOKUP = {
    occupancy: exposes_1.presets.binary("occupancy", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device detected occupancy"),
    contact: exposes_1.presets.binary("contact", exposes_1.access.STATE, false, true).withDescription("Indicates whether the device is opened or closed"),
    smoke: exposes_1.presets.binary("smoke", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device detected smoke"),
    water_leak: exposes_1.presets.binary("water_leak", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device detected a water leak"),
    carbon_monoxide: exposes_1.presets.binary("carbon_monoxide", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device detected carbon monoxide"),
    sos: exposes_1.presets.binary("sos", exposes_1.access.STATE, true, false).withLabel("SOS").withDescription("Indicates whether the SOS alarm is triggered"),
    vibration: exposes_1.presets.binary("vibration", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device detected vibration"),
    alarm: exposes_1.presets.binary("alarm", exposes_1.access.STATE, true, false).withDescription("Indicates whether the alarm is triggered"),
    gas: exposes_1.presets.binary("gas", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device detected gas"),
    alarm_1: exposes_1.presets.binary("alarm_1", exposes_1.access.STATE, true, false).withDescription("Indicates whether IAS Zone alarm 1 is active"),
    alarm_2: exposes_1.presets.binary("alarm_2", exposes_1.access.STATE, true, false).withDescription("Indicates whether IAS Zone alarm 2 is active"),
    tamper: exposes_1.presets.binary("tamper", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device is tampered").withCategory("diagnostic"),
    rain: exposes_1.presets.binary("rain", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device detected rainfall"),
    pressure: exposes_1.presets.binary("pressure", exposes_1.access.STATE, true, false).withDescription("Indicates whether the device detected pressure"),
    battery_low: exposes_1.presets
        .binary("battery_low", exposes_1.access.STATE, true, false)
        .withDescription("Indicates whether the battery of the device is almost empty")
        .withCategory("diagnostic"),
    supervision_reports: exposes_1.presets
        .binary("supervision_reports", exposes_1.access.STATE, true, false)
        .withDescription("Indicates whether the device issues reports on zone operational status")
        .withCategory("diagnostic"),
    restore_reports: exposes_1.presets
        .binary("restore_reports", exposes_1.access.STATE, true, false)
        .withDescription("Indicates whether the device issues reports on alarm no longer being present")
        .withCategory("diagnostic"),
    ac_status: exposes_1.presets
        .binary("ac_status", exposes_1.access.STATE, true, false)
        .withDescription("Indicates whether the device mains voltage supply is at fault")
        .withCategory("diagnostic"),
    test: exposes_1.presets
        .binary("test", exposes_1.access.STATE, true, false)
        .withDescription("Indicates whether the device is currently performing a test")
        .withCategory("diagnostic"),
    trouble: exposes_1.presets
        .binary("trouble", exposes_1.access.STATE, true, false)
        .withDescription("Indicates whether the device is currently having trouble")
        .withCategory("diagnostic"),
    battery_defect: exposes_1.presets
        .binary("battery_defect", exposes_1.access.STATE, true, false)
        .withDescription("Indicates whether the device battery is defective")
        .withCategory("diagnostic"),
};
exports.TIME_LOOKUP = {
    MAX: 65000,
    "4_HOURS": 14400,
    "1_HOUR": 3600,
    "30_MINUTES": 1800,
    "5_MINUTES": 300,
    "2_MINUTES": 120,
    "1_MINUTE": 60,
    "10_SECONDS": 10,
    "5_SECONDS": 5,
    "1_SECOND": 1,
    MIN: 0,
};
function convertReportingConfigTime(time) {
    if ((0, utils_1.isString)(time)) {
        if (!(time in exports.TIME_LOOKUP))
            throw new Error(`Reporting time '${time}' is unknown`);
        return exports.TIME_LOOKUP[time];
    }
    return time;
}
async function setupAttributes(entity, coordinatorEndpoint, cluster, config, configureReporting = true, read = true) {
    const endpoints = (0, utils_1.isEndpoint)(entity) ? [entity] : getEndpointsWithCluster(entity, cluster, "input");
    const ieeeAddr = (0, utils_1.isEndpoint)(entity) ? entity.deviceIeeeAddress : entity.ieeeAddr;
    for (const endpoint of endpoints) {
        logger_1.logger.debug(`Configure reporting: ${configureReporting}, read: ${read} for ${ieeeAddr}/${endpoint.ID} ${cluster} ${JSON.stringify(config)}`, "zhc:setupattribute");
        // Split into chunks of 4 to prevent to message becoming too big.
        const chunks = (0, utils_1.splitArrayIntoChunks)(config, 4);
        if (configureReporting) {
            await endpoint.bind(cluster, coordinatorEndpoint);
            for (const chunk of chunks) {
                await endpoint.configureReporting(cluster, chunk.map((a) => ({
                    minimumReportInterval: convertReportingConfigTime(a.min),
                    maximumReportInterval: convertReportingConfigTime(a.max),
                    reportableChange: a.change,
                    attribute: a.attribute,
                })));
            }
        }
        if (read) {
            for (const chunk of chunks) {
                try {
                    // Don't fail configuration if reading this attribute fails
                    // https://github.com/Koenkk/zigbee-herdsman-converters/pull/7074
                    await endpoint.read(cluster, 
                    // @ts-expect-error too dynamic to properly type
                    chunk.map((a) => ((0, utils_1.isString)(a) ? a : (0, utils_1.isObject)(a.attribute) ? a.attribute.ID : a.attribute)));
                }
                catch (e) {
                    logger_1.logger.debug(`Reading attribute failed: ${e}`, "zhc:setupattribute");
                }
            }
        }
    }
}
function setupConfigureForReporting(cluster, attribute, args) {
    const { config = false, access = undefined, endpointNames = undefined, singleEndpoint = false } = args;
    const configureReporting = !!config;
    const read = !!(access & exposes_1.access.GET);
    if (configureReporting || read) {
        const configure = async (device, coordinatorEndpoint, definition) => {
            const reportConfig = config ? { ...config, attribute: attribute } : { attribute, min: -1, max: -1, change: -1 };
            let endpoints;
            if (endpointNames) {
                (0, node_assert_1.default)(!singleEndpoint, "`endpointNames` cannot be used together with `singleEndpoint`");
                const definitionEndpoints = definition.endpoint(device);
                const endpointIds = endpointNames.map((e) => definitionEndpoints[e]);
                endpoints = device.endpoints.filter((e) => endpointIds.includes(e.ID));
            }
            else {
                endpoints = getEndpointsWithCluster(device, cluster, "input");
                if (singleEndpoint) {
                    endpoints = [endpoints[0]];
                }
            }
            for (const endpoint of endpoints) {
                await setupAttributes(endpoint, coordinatorEndpoint, cluster, [reportConfig], configureReporting, read);
            }
        };
        return configure;
    }
    return undefined;
}
function setupConfigureForBinding(cluster, clusterType, endpointNames) {
    const configure = async (device, coordinatorEndpoint, definition) => {
        if (endpointNames) {
            const definitionEndpoints = definition.endpoint(device);
            const endpointIds = endpointNames.map((e) => definitionEndpoints[e]);
            const endpoints = device.endpoints.filter((e) => endpointIds.includes(e.ID));
            for (const endpoint of endpoints) {
                await endpoint.bind(cluster, coordinatorEndpoint);
            }
        }
        else {
            const endpoints = getEndpointsWithCluster(device, cluster, clusterType);
            for (const endpoint of endpoints) {
                await endpoint.bind(cluster, coordinatorEndpoint);
            }
        }
    };
    return configure;
}
function setupConfigureForReading(cluster, attributes, endpointNames) {
    const configure = async (device, coordinatorEndpoint, definition) => {
        if (endpointNames) {
            const definitionEndpoints = definition.endpoint(device);
            const endpointIds = endpointNames.map((e) => definitionEndpoints[e]);
            const endpoints = device.endpoints.filter((e) => endpointIds.includes(e.ID));
            for (const endpoint of endpoints) {
                await endpoint.read(cluster, attributes);
            }
        }
        else {
            const endpoints = getEndpointsWithCluster(device, cluster, "input");
            for (const endpoint of endpoints) {
                await endpoint.read(cluster, attributes);
            }
        }
    };
    return configure;
}
// #region General
function forceDeviceType(args) {
    const configure = [
        (device, coordinatorEndpoint, definition) => {
            device.type = args.type;
            device.save();
        },
    ];
    return { configure, isModernExtend: true };
}
function forcePowerSource(args) {
    const configure = [
        (device, coordinatorEndpoint, definition) => {
            device.powerSource = args.powerSource;
            device.save();
        },
    ];
    return { configure, isModernExtend: true };
}
function linkQuality(args = {}) {
    const { reporting = false, attribute = "zclVersion", reportingConfig = { min: "1_HOUR", max: "4_HOURS", change: 0 } } = args;
    // Exposes is empty because the application (e.g. Z2M) adds a linkquality sensor
    // for every device already.
    const exposes = [];
    const fromZigbee = [
        {
            cluster: "genBasic",
            type: ["attributeReport", "readResponse"],
            convert: (model, msg, publish, options, meta) => {
                return { linkquality: msg.linkquality };
            },
        },
    ];
    const result = { exposes, fromZigbee, isModernExtend: true };
    if (reporting) {
        result.configure = [setupConfigureForReporting("genBasic", attribute, { config: reportingConfig, access: exposes_1.access.GET })];
    }
    return result;
}
function battery(args = {}) {
    const { percentage = true, voltage = false, lowStatus = false, percentageReporting = true, voltageReporting = false, dontDividePercentage = false, percentageReportingConfig = { min: "1_HOUR", max: "MAX", change: 10 }, voltageReportingConfig = { min: "1_HOUR", max: "MAX", change: 10 }, lowStatusReportingConfig = undefined, voltageToPercentage = undefined, } = args;
    const exposes = [];
    if (percentage) {
        exposes.push(exposes_1.presets
            .numeric("battery", exposes_1.access.STATE_GET)
            .withUnit("%")
            .withDescription("Remaining battery in %")
            .withValueMin(0)
            .withValueMax(100)
            .withCategory("diagnostic"));
    }
    if (voltage) {
        exposes.push(exposes_1.presets.numeric("voltage", exposes_1.access.STATE_GET).withUnit("mV").withDescription("Reported battery voltage in millivolts").withCategory("diagnostic"));
    }
    if (lowStatus) {
        exposes.push(exposes_1.presets.binary("battery_low", exposes_1.access.STATE, true, false).withDescription("Empty battery indicator").withCategory("diagnostic"));
    }
    const fromZigbee = [
        {
            cluster: "genPowerCfg",
            type: ["attributeReport", "readResponse"],
            convert: (model, msg, publish, options, meta) => {
                const payload = {};
                if (msg.data.batteryPercentageRemaining !== undefined && msg.data.batteryPercentageRemaining < 255) {
                    // Some devices do not comply to the ZCL and report a
                    // batteryPercentageRemaining of 100 when the battery is full (should be 200).
                    let percentage = msg.data.batteryPercentageRemaining;
                    percentage = dontDividePercentage ? percentage : percentage / 2;
                    if (percentage)
                        payload.battery = (0, utils_1.precisionRound)(percentage, 2);
                }
                if (msg.data.batteryVoltage !== undefined && msg.data.batteryVoltage < 255) {
                    // Deprecated: voltage is = mV now but should be V
                    if (voltage)
                        payload.voltage = msg.data.batteryVoltage * 100;
                    if (voltageToPercentage) {
                        payload.battery = (0, utils_1.batteryVoltageToPercentage)(payload.voltage, voltageToPercentage);
                    }
                }
                if (msg.data.batteryAlarmState !== undefined) {
                    const battery1Low = (msg.data.batteryAlarmState & (1 << 0) ||
                        msg.data.batteryAlarmState & (1 << 1) ||
                        msg.data.batteryAlarmState & (1 << 2) ||
                        msg.data.batteryAlarmState & (1 << 3)) > 0;
                    const battery2Low = (msg.data.batteryAlarmState & (1 << 10) ||
                        msg.data.batteryAlarmState & (1 << 11) ||
                        msg.data.batteryAlarmState & (1 << 12) ||
                        msg.data.batteryAlarmState & (1 << 13)) > 0;
                    const battery3Low = (msg.data.batteryAlarmState & (1 << 20) ||
                        msg.data.batteryAlarmState & (1 << 21) ||
                        msg.data.batteryAlarmState & (1 << 22) ||
                        msg.data.batteryAlarmState & (1 << 23)) > 0;
                    if (lowStatus)
                        payload.battery_low = battery1Low || battery2Low || battery3Low;
                }
                return payload;
            },
        },
    ];
    const toZigbee = [
        {
            key: ["battery", "voltage"],
            convertGet: async (entity, key, meta) => {
                // Don't fail GET request if reading fails
                // Split reading is needed for more clear debug logs
                const ep = (0, utils_1.determineEndpoint)(entity, meta, "genPowerCfg");
                try {
                    await ep.read("genPowerCfg", ["batteryPercentageRemaining"]);
                }
                catch (e) {
                    logger_1.logger.debug(`Reading batteryPercentageRemaining failed: ${e}, device probably doesn't support it`, "zhc:setupattribute");
                }
                try {
                    await ep.read("genPowerCfg", ["batteryVoltage"]);
                }
                catch (e) {
                    logger_1.logger.debug(`Reading batteryVoltage failed: ${e}, device probably doesn't support it`, "zhc:setupattribute");
                }
            },
        },
    ];
    const result = { exposes, fromZigbee, toZigbee, configure: [], isModernExtend: true };
    if (percentageReporting || voltageReporting) {
        if (percentageReporting) {
            result.configure.push(setupConfigureForReporting("genPowerCfg", "batteryPercentageRemaining", {
                config: percentageReportingConfig,
                access: exposes_1.access.STATE_GET,
                singleEndpoint: true,
            }));
        }
        if (voltageReporting) {
            result.configure.push(setupConfigureForReporting("genPowerCfg", "batteryVoltage", {
                config: voltageReportingConfig,
                access: exposes_1.access.STATE_GET,
                singleEndpoint: true,
            }));
        }
        result.configure.push((0, utils_1.configureSetPowerSourceWhenUnknown)("Battery"));
    }
    if (voltageToPercentage || dontDividePercentage) {
        const meta = { battery: {} };
        if (voltageToPercentage)
            meta.battery.voltageToPercentage = voltageToPercentage;
        if (dontDividePercentage)
            meta.battery.dontDividePercentage = dontDividePercentage;
        result.meta = meta;
    }
    if (lowStatusReportingConfig) {
        result.configure.push(setupConfigureForReporting("genPowerCfg", "batteryAlarmState", {
            config: lowStatusReportingConfig,
            access: exposes_1.access.STATE_GET,
            singleEndpoint: true,
        }));
    }
    return result;
}
function deviceTemperature(args = {}) {
    return numeric({
        name: "device_temperature",
        cluster: "genDeviceTempCfg",
        attribute: "currentTemperature",
        reporting: { min: "5_MINUTES", max: "1_HOUR", change: 1 },
        description: "Temperature of the device",
        unit: "°C",
        access: "STATE_GET",
        entityCategory: "diagnostic",
        ...args,
    });
}
function identify(args = { isSleepy: false }) {
    const { isSleepy } = args;
    const normal = exposes_1.presets.enum("identify", exposes_1.access.SET, ["identify"]).withDescription("Initiate device identification").withCategory("config");
    const sleepy = exposes_1.presets
        .enum("identify", exposes_1.access.SET, ["identify"])
        .withDescription("Initiate device identification. This device is asleep by default." +
        "You may need to wake it up first before sending the identify command.")
        .withCategory("config");
    const exposes = isSleepy ? [sleepy] : [normal];
    const identifyTimeout = exposes_1.presets
        .numeric("identify_timeout", exposes_1.access.SET)
        .withDescription("Sets the duration of the identification procedure in seconds (i.e., how long the device would flash)." +
        "The value ranges from 1 to 30 seconds (default: 3).")
        .withValueMin(1)
        .withValueMax(30);
    const toZigbee = [
        {
            key: ["identify"],
            options: [identifyTimeout],
            convertSet: async (entity, key, value, meta) => {
                const identifyTimeout = meta.options.identify_timeout ?? 3;
                await entity.command("genIdentify", "identify", { identifytime: identifyTimeout }, (0, utils_1.getOptions)(meta.mapped, entity));
            },
        },
    ];
    return { exposes, toZigbee, isModernExtend: true };
}
function onOff(args = {}) {
    const { powerOnBehavior = true, skipDuplicateTransaction = false, configureReporting = true, endpointNames = undefined, description = undefined, ota = false, } = args;
    const exposes = description ? (0, utils_1.exposeEndpoints)(exposes_1.presets.switch(description), endpointNames) : (0, utils_1.exposeEndpoints)(exposes_1.presets.switch(), endpointNames);
    const fromZigbee = [skipDuplicateTransaction ? fz.on_off_skip_duplicate_transaction : fz.on_off];
    const toZigbee = [endpointNames ? { ...tz.on_off, endpoints: endpointNames } : tz.on_off];
    if (powerOnBehavior) {
        exposes.push(...(0, utils_1.exposeEndpoints)(exposes_1.presets.power_on_behavior(["off", "on", "toggle", "previous"]), endpointNames));
        fromZigbee.push(fz.power_on_behavior);
        toZigbee.push(tz.power_on_behavior);
    }
    const result = { exposes, fromZigbee, toZigbee, isModernExtend: true };
    if (ota)
        result.ota = ota;
    if (configureReporting) {
        result.configure = [
            async (device, coordinatorEndpoint) => {
                await setupAttributes(device, coordinatorEndpoint, "genOnOff", [{ attribute: "onOff", min: "MIN", max: "MAX", change: 1 }]);
                if (powerOnBehavior) {
                    try {
                        // Don't fail configure if reading this attribute fails, some devices don't support it.
                        await setupAttributes(device, coordinatorEndpoint, "genOnOff", [{ attribute: "startUpOnOff", min: "MIN", max: "MAX", change: 1 }], false);
                    }
                    catch (e) {
                        if (e.message.includes("UNSUPPORTED_ATTRIBUTE")) {
                            logger_1.logger.debug("Reading startUpOnOff failed, this features is unsupported", "zhc:onoff");
                        }
                        else {
                            throw e;
                        }
                    }
                }
            },
            (0, utils_1.configureSetPowerSourceWhenUnknown)("Mains (single phase)"),
        ];
    }
    return result;
}
function commandsOnOff(args = {}) {
    const { commands = ["on", "off", "toggle"], bind = true, endpointNames = undefined } = args;
    let actions = commands;
    if (endpointNames) {
        actions = commands.flatMap((c) => endpointNames.map((e) => `${c}_${e}`));
    }
    const exposes = [exposes_1.presets.enum("action", exposes_1.access.STATE, actions).withDescription("Triggered action (e.g. a button click)")];
    const actionPayloadLookup = {
        commandOn: "on",
        commandOff: "off",
        commandOffWithEffect: "off",
        commandToggle: "toggle",
    };
    const fromZigbee = [
        {
            cluster: "genOnOff",
            type: ["commandOn", "commandOff", "commandOffWithEffect", "commandToggle"],
            convert: (model, msg, publish, options, meta) => {
                if ((0, utils_1.hasAlreadyProcessedMessage)(msg, model))
                    return;
                const payload = { action: (0, utils_1.postfixWithEndpointName)(actionPayloadLookup[msg.type], msg, model, meta) };
                (0, utils_1.addActionGroup)(payload, msg, model);
                return payload;
            },
        },
    ];
    const result = { exposes, fromZigbee, isModernExtend: true };
    if (bind)
        result.configure = [setupConfigureForBinding("genOnOff", "output", endpointNames)];
    return result;
}
function poll(args) {
    const optionKey = args.optionKey ?? `${args.key}_poll_interval`;
    const onEvent = [
        (event) => {
            if (event.type === "start" || (event.type === "deviceOptionsChanged" && event.data.from[optionKey] !== event.data.to[optionKey])) {
                let seconds = args.defaultIntervalSeconds;
                if (args.option) {
                    seconds = (0, utils_1.toNumber)(event.data.options[optionKey] ?? args.defaultIntervalSeconds, optionKey);
                }
                clearTimeout(globalStore.getValue(event.data.device.ieeeAddr, args.key));
                if (seconds <= 0) {
                    logger_1.logger.debug(`Not polling '${args.key}' for '${event.data.device.ieeeAddr}' since poll interval is <= 0 (got ${seconds})`, NS);
                }
                else {
                    logger_1.logger.debug(`Polling '${args.key}' for '${event.data.device.ieeeAddr}' at an interval of ${seconds}`, NS);
                    const setTimer = () => {
                        const timer = setTimeout(async () => {
                            try {
                                await args.poll(event.data.device, event.data.options);
                            }
                            catch (error) {
                                logger_1.logger.debug(`Poll of '${event.data.device.ieeeAddr}' failed (${error})`, NS);
                            }
                            if (globalStore.getValue(event.data.device.ieeeAddr, args.key) === timer) {
                                setTimer();
                            }
                        }, seconds * 1000);
                        globalStore.putValue(event.data.device.ieeeAddr, args.key, timer);
                    };
                    setTimer();
                }
            }
            else if (event.type === "stop") {
                clearTimeout(globalStore.getValue(event.data.ieeeAddr, args.key));
                globalStore.clearValue(event.data.ieeeAddr, args.key);
            }
        },
    ];
    return { onEvent, options: args.option ? [args.option] : [], isModernExtend: true };
}
function writeTimeDaily(args) {
    return poll({
        key: "time",
        defaultIntervalSeconds: 60 * 60 * 24,
        poll: (device) => {
            const time = Math.round((Date.now() - constants.OneJanuary2000) / 1000 + new Date().getTimezoneOffset() * -1 * 60);
            device
                .getEndpoint(args.endpointId)
                .write("genTime", { time: time }, { sendPolicy: "queue" })
                .catch((error) => logger_1.logger.error(`Failed to write time to '${device.ieeeAddr}' (${error})`, NS));
        },
    });
}
function iasArmCommandDefaultResponse() {
    const converter = {
        cluster: "ssIasAce",
        type: ["commandArm"],
        convert: (model, msg, publish, options, meta) => {
            // Since arm command has a response zigbee-herdsman doesn't send a default response.
            // This causes the remote to repeat the arm command, so send a default response here.
            msg.endpoint
                .defaultResponse(0, 0, 1281, msg.meta.zclTransactionSequenceNumber)
                .catch((error) => logger_1.logger.error(`Failed to send default response to '${msg.device.ieeeAddr}' (${error})`, NS));
        },
    };
    return { fromZigbee: [converter], isModernExtend: true };
}
function iasGetPanelStatusResponse() {
    const converter = {
        cluster: "ssIasAce",
        type: ["commandGetPanelStatus"],
        convert: (model, msg, publish, options, meta) => {
            if (globalStore.hasValue(msg.endpoint, "panelStatus")) {
                const payload = {
                    panelstatus: globalStore.getValue(msg.endpoint, "panelStatus"),
                    secondsremain: 0x00,
                    audiblenotif: 0x00,
                    alarmstatus: 0x00,
                };
                (0, utils_1.assertNumber)(msg.meta.zclTransactionSequenceNumber);
                msg.endpoint
                    .commandResponse("ssIasAce", "getPanelStatusRsp", payload, {}, msg.meta.zclTransactionSequenceNumber)
                    .catch((error) => logger_1.logger.error(`Failed to send panel status response '${msg.device.ieeeAddr}' (${error})`, NS));
            }
        },
    };
    return { fromZigbee: [converter], isModernExtend: true };
}
function customTimeResponse(start) {
    // The Zigbee Cluster Library specification states that the genTime.time response should be the
    // number of seconds since 1st Jan 2000 00:00:00 UTC. This extend modifies that:
    // 1970_UTC: number of seconds since the Unix Epoch (1st Jan 1970 00:00:00 UTC)
    // 2000_LOCAL: seconds since 1 January in the local time zone.
    // Disable the responses of zigbee-herdsman and respond here instead.
    const onEvent = [
        (event) => {
            if (event.type === "start" && !event.data.device.customReadResponse) {
                event.data.device.customReadResponse = (frame, endpoint) => {
                    if (frame.isCluster("genTime")) {
                        const payload = {};
                        if (start === "1970_UTC") {
                            const time = Math.round(Date.now() / 1000);
                            payload.time = time;
                            payload.localTime = time - new Date().getTimezoneOffset() * 60;
                        }
                        else if (start === "2000_LOCAL") {
                            const oneJanuary2000 = new Date("January 01, 2000 00:00:00 UTC+00:00").getTime();
                            const secondsUTC = Math.round((Date.now() - oneJanuary2000) / 1000);
                            payload.time = secondsUTC - new Date().getTimezoneOffset() * 60;
                        }
                        endpoint.readResponse("genTime", frame.header.transactionSequenceNumber, payload).catch((e) => {
                            logger_1.logger.warning(`Custom time response failed for '${event.data.device.ieeeAddr}': ${e}`, "zhc:customtimeresponse");
                        });
                        return true;
                    }
                    return false;
                };
            }
        },
    ];
    return { onEvent, isModernExtend: true };
}
// #endregion
// #region Measurement and Sensing
function BeeLight_iaq(args = {}) {
    return numeric({
        name: "BeeLight_iaq",
        cluster: "msBeelightIAQ",
        attribute: "measuredValue",
        reporting: { min: "5_MINUTES", max: "1_HOUR", change: 10 },
        description: "Measured IAQ value",
        unit: "",
        scale: 1,
        access: "STATE_GET",
        ...args,
    });
}
function BeeLight_co2(args = {}) {
    return numeric({
        name: "BeeLight_co2",
        cluster: "msBeelightCO2",
        attribute: "measuredValue",
        reporting: { min: "5_MINUTES", max: "1_HOUR", change: 100 },
        description: "Measured CO2 value",
        unit: "ppm",
        scale: 1,
        access: "STATE_GET",
        ...args,
    });
}
function BeeLight_voc(args = {}) {
    return numeric({
        name: "BeeLight_voc",
        cluster: "msBeelightVOC",
        attribute: "measuredValue",
        reporting: { min: "5_MINUTES", max: "1_HOUR", change: 5 },
        description: "Measured VOC value",
        unit: "ppm",
        scale: 1,
        access: "STATE_GET",
        ...args,
    });
}
function illuminance(args = {}) {
    const luxScale = (value, type) => {
        let result = value;
        if (type === "from") {
            result = 10 ** ((result - 1) / 10000);
        }
        return result;
    };
    const result = numeric({
        name: "illuminance",
        cluster: "msIlluminanceMeasurement",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 5 }, // 5 lux
        description: "Measured illuminance",
        unit: "lx",
        scale: luxScale,
        access: "STATE_GET",
        ...args,
    });
    const fzIlluminanceRaw = {
        cluster: "msIlluminanceMeasurement",
        type: ["attributeReport", "readResponse"],
        options: [exposes_1.options.illuminance_raw()],
        convert: (model, msg, publish, options, meta) => {
            if (options.illuminance_raw) {
                return { illuminance_raw: msg.data.measuredValue };
            }
        },
    };
    result.fromZigbee.push(fzIlluminanceRaw);
    const exposeIlluminanceRaw = (device, options) => {
        return options?.illuminance_raw ? [exposes_1.presets.illuminance_raw()] : [];
    };
    result.exposes.push(exposeIlluminanceRaw);
    return result;
}
function temperature(args = {}) {
    return numeric({
        name: "temperature",
        cluster: "msTemperatureMeasurement",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 100 },
        description: "Measured temperature value",
        unit: "°C",
        scale: 100,
        access: "STATE_GET",
        ...args,
    });
}
function pressure(args = {}) {
    return numeric({
        name: "pressure",
        cluster: "msPressureMeasurement",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 50 }, // 5 kPa
        description: "The measured atmospheric pressure",
        unit: "kPa",
        scale: 10,
        access: "STATE_GET",
        ...args,
    });
}
function flow(args = {}) {
    return numeric({
        name: "flow",
        cluster: "msFlowMeasurement",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 10 },
        description: "Measured water flow",
        unit: "m³/h",
        scale: 10,
        access: "STATE_GET",
        ...args,
    });
}
function humidity(args = {}) {
    return numeric({
        name: "humidity",
        cluster: "msRelativeHumidity",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 100 },
        description: "Measured relative humidity",
        unit: "%",
        scale: 100,
        access: "STATE_GET",
        ...args,
    });
}
function soilMoisture(args = {}) {
    return numeric({
        name: "soil_moisture",
        cluster: "msSoilMoisture",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 100 },
        description: "Measured soil moisture value",
        unit: "%",
        scale: 100,
        access: "STATE_GET",
        ...args,
    });
}
function windSpeed(args = {}) {
    return numeric({
        name: "wind_speed",
        cluster: "msWindSpeed",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 1 },
        description: "Measured wind speed value",
        unit: "m/s",
        scale: 100,
        access: "STATE_GET",
        ...args,
    });
}
function occupancy(args = {}) {
    const { reporting = true, reportingConfig = { min: "MIN", max: "1_HOUR", change: 0 }, pirConfig = undefined, ultrasonicConfig = undefined, contactConfig = undefined, endpointNames = undefined, } = args;
    const templateExposes = [exposes_1.presets.occupancy().withAccess(exposes_1.access.STATE_GET)];
    const exposes = endpointNames
        ? templateExposes.flatMap((exp) => endpointNames.map((ep) => exp.withEndpoint(ep)))
        : templateExposes;
    const fromZigbee = [
        {
            cluster: "msOccupancySensing",
            type: ["attributeReport", "readResponse"],
            options: [exposes_1.options.no_occupancy_since_false()],
            convert: (model, msg, publish, options, meta) => {
                if ("occupancy" in msg.data && (!endpointNames || endpointNames.includes((0, utils_1.getEndpointName)(msg, model, meta).toString()))) {
                    const propertyName = (0, utils_1.postfixWithEndpointName)("occupancy", msg, model, meta);
                    const payload = { [propertyName]: (msg.data.occupancy & 1) > 0 };
                    (0, utils_1.noOccupancySince)(msg.endpoint, options, publish, payload[propertyName] ? "stop" : "start");
                    return payload;
                }
            },
        },
    ];
    const toZigbee = [
        {
            key: ["occupancy"],
            convertGet: async (entity, key, meta) => {
                await (0, utils_1.determineEndpoint)(entity, meta, "msOccupancySensing").read("msOccupancySensing", ["occupancy"]);
            },
        },
    ];
    const settingsExtends = [];
    const settingsTemplate = {
        cluster: "msOccupancySensing",
        description: "",
        endpointNames: endpointNames,
        access: "ALL",
        entityCategory: "config",
    };
    const attributesForReading = [];
    if (pirConfig) {
        if (pirConfig.includes("otu_delay")) {
            settingsExtends.push(numeric({
                name: "occupancy_timeout",
                attribute: "pirOToUDelay",
                valueMin: 0,
                valueMax: 65534,
                unit: "s",
                ...settingsTemplate,
                description: "Time in seconds before occupancy is cleared after the last detected movement.",
            }));
            attributesForReading.push("pirOToUDelay");
        }
        if (pirConfig.includes("uto_delay")) {
            settingsExtends.push(numeric({
                name: "pir_uto_delay",
                attribute: "pirUToODelay",
                valueMin: 0,
                valueMax: 65534,
                ...settingsTemplate,
            }));
            attributesForReading.push("pirUToODelay");
        }
        if (pirConfig.includes("uto_threshold")) {
            settingsExtends.push(numeric({
                name: "pir_uto_threshold",
                attribute: "pirUToOThreshold",
                valueMin: 1,
                valueMax: 254,
                ...settingsTemplate,
            }));
            attributesForReading.push("pirUToOThreshold");
        }
    }
    if (ultrasonicConfig) {
        if (pirConfig.includes("otu_delay")) {
            settingsExtends.push(numeric({
                name: "ultrasonic_otu_delay",
                attribute: "ultrasonicOToUDelay",
                valueMin: 0,
                valueMax: 65534,
                ...settingsTemplate,
            }));
            attributesForReading.push("ultrasonicOToUDelay");
        }
        if (pirConfig.includes("uto_delay")) {
            settingsExtends.push(numeric({
                name: "ultrasonic_uto_delay",
                attribute: "ultrasonicUToODelay",
                valueMin: 0,
                valueMax: 65534,
                ...settingsTemplate,
            }));
            attributesForReading.push("ultrasonicUToODelay");
        }
        if (pirConfig.includes("uto_threshold")) {
            settingsExtends.push(numeric({
                name: "ultrasonic_uto_threshold",
                attribute: "ultrasonicUToOThreshold",
                valueMin: 1,
                valueMax: 254,
                ...settingsTemplate,
            }));
            attributesForReading.push("ultrasonicUToOThreshold");
        }
    }
    if (contactConfig) {
        if (pirConfig.includes("otu_delay")) {
            settingsExtends.push(numeric({
                name: "contact_otu_delay",
                attribute: "contactOToUDelay",
                valueMin: 0,
                valueMax: 65534,
                ...settingsTemplate,
            }));
            attributesForReading.push("contactOToUDelay");
        }
        if (pirConfig.includes("uto_delay")) {
            settingsExtends.push(numeric({
                name: "contact_uto_delay",
                attribute: "contactUToODelay",
                valueMin: 0,
                valueMax: 65534,
                ...settingsTemplate,
            }));
            attributesForReading.push("contactUToODelay");
        }
        if (pirConfig.includes("uto_threshold")) {
            settingsExtends.push(numeric({
                name: "contact_uto_threshold",
                attribute: "contactUToOThreshold",
                valueMin: 1,
                valueMax: 254,
                ...settingsTemplate,
            }));
            attributesForReading.push("contactUToOThreshold");
        }
    }
    settingsExtends.map((extend) => exposes.push(...extend.exposes));
    settingsExtends.map((extend) => fromZigbee.push(...extend.fromZigbee));
    settingsExtends.map((extend) => toZigbee.push(...extend.toZigbee));
    const configure = [];
    if (attributesForReading.length > 0)
        configure.push(setupConfigureForReading("msOccupancySensing", attributesForReading, endpointNames));
    if (reporting) {
        configure.push(setupConfigureForReporting("msOccupancySensing", "occupancy", {
            config: reportingConfig,
            access: exposes_1.access.STATE_GET,
            endpointNames: endpointNames,
        }));
    }
    return { exposes, fromZigbee, toZigbee, configure, isModernExtend: true };
}
function co2(args = {}) {
    return numeric({
        name: "co2",
        cluster: "msCO2",
        label: "CO2",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 0.00005 }, // 50 ppm change
        description: "Measured value",
        unit: "ppm",
        scale: 0.000001,
        access: "STATE_GET",
        ...args,
    });
}
function pm25(args = {}) {
    return numeric({
        name: "pm25",
        cluster: "pm25Measurement",
        attribute: "measuredValue",
        reporting: { min: "10_SECONDS", max: "1_HOUR", change: 1 },
        description: "Measured PM2.5 (particulate matter) concentration",
        unit: "µg/m³",
        access: "STATE_GET",
        ...args,
    });
}
function light(args = {}) {
    const { effect = true, powerOnBehavior = true, configureReporting = false, ota = false, color = undefined, levelConfig = undefined, turnsOffAtBrightness1 = false, endpointNames = undefined, levelReportingConfig = undefined, moveToLevelWithOnOffDisable = false, } = args;
    let { colorTemp = undefined } = args;
    if (colorTemp) {
        colorTemp = { startup: true, ...colorTemp };
    }
    const argsColor = color
        ? {
            modes: ["xy"],
            applyRedFix: false,
            enhancedHue: true,
            ...((0, utils_1.isObject)(color) ? color : {}),
        }
        : false;
    const lightExpose = (0, utils_1.exposeEndpoints)(exposes_1.presets.light().withBrightness(), endpointNames);
    const fromZigbee = [fz.on_off, fz.brightness, fz.level_config];
    const toZigbee = [
        endpointNames ? { ...tz.light_onoff_brightness, endpoints: endpointNames } : tz.light_onoff_brightness,
        tz.ignore_transition,
        tz.level_config,
        tz.ignore_rate,
        tz.light_brightness_move,
        tz.light_brightness_step,
    ];
    const meta = {};
    if (colorTemp || argsColor) {
        fromZigbee.push(fz.color_colortemp);
        if (colorTemp && argsColor)
            toZigbee.push(tz.light_color_colortemp);
        else if (colorTemp)
            toZigbee.push(tz.light_colortemp);
        else if (argsColor)
            toZigbee.push(tz.light_color);
        toZigbee.push(tz.light_color_mode, tz.light_color_options);
    }
    if (colorTemp) {
        lightExpose.forEach((e) => {
            e.withColorTemp(colorTemp.range);
        });
        toZigbee.push(tz.light_colortemp_move, tz.light_colortemp_step);
        if (colorTemp.startup) {
            toZigbee.push(tz.light_colortemp_startup);
            lightExpose.forEach((e) => {
                e.withColorTempStartup(colorTemp.range);
            });
        }
    }
    if (argsColor) {
        lightExpose.forEach((e) => {
            e.withColor(argsColor.modes);
        });
        toZigbee.push(tz.light_hue_saturation_move, tz.light_hue_saturation_step);
        if (argsColor.modes.includes("hs")) {
            meta.supportsHueAndSaturation = true;
        }
        if (argsColor.applyRedFix) {
            meta.applyRedFix = true;
        }
        if (!argsColor.enhancedHue) {
            meta.supportsEnhancedHue = false;
        }
    }
    if (levelConfig) {
        lightExpose.forEach((e) => {
            levelConfig.features ? e.withLevelConfig(levelConfig.features) : e.withLevelConfig();
        });
        toZigbee.push(tz.level_config);
    }
    const exposes = lightExpose;
    if (effect) {
        const effects = exposes_1.presets.effect();
        if (color) {
            effects.values.push("colorloop", "stop_colorloop");
        }
        exposes.push(...(0, utils_1.exposeEndpoints)(effects, endpointNames));
        toZigbee.push(tz.effect);
    }
    if (powerOnBehavior) {
        exposes.push(...(0, utils_1.exposeEndpoints)(exposes_1.presets.power_on_behavior(["off", "on", "toggle", "previous"]), endpointNames));
        fromZigbee.push(fz.power_on_behavior);
        toZigbee.push(tz.power_on_behavior);
    }
    if (turnsOffAtBrightness1) {
        meta.turnsOffAtBrightness1 = turnsOffAtBrightness1;
    }
    if (moveToLevelWithOnOffDisable) {
        meta.moveToLevelWithOnOffDisable = true;
    }
    const configure = [
        async (device, coordinatorEndpoint, definition) => {
            await (0, light_1.configure)(device, coordinatorEndpoint, true);
            if (configureReporting) {
                await setupAttributes(device, coordinatorEndpoint, "genOnOff", [{ attribute: "onOff", min: "MIN", max: "MAX", change: 1 }]);
                await setupAttributes(device, coordinatorEndpoint, "genLevelCtrl", [
                    { attribute: "currentLevel", min: "5_SECONDS", max: "MAX", change: 1, ...(levelReportingConfig || {}) },
                ]);
                if (colorTemp) {
                    await setupAttributes(device, coordinatorEndpoint, "lightingColorCtrl", [
                        { attribute: "colorTemperature", min: "10_SECONDS", max: "MAX", change: 1 },
                    ]);
                }
                if (argsColor) {
                    const attributes = [];
                    if (argsColor.modes.includes("xy")) {
                        attributes.push({ attribute: "currentX", min: "10_SECONDS", max: "MAX", change: 1 }, { attribute: "currentY", min: "10_SECONDS", max: "MAX", change: 1 });
                    }
                    if (argsColor.modes.includes("hs")) {
                        attributes.push({ attribute: argsColor.enhancedHue ? "enhancedCurrentHue" : "currentHue", min: "10_SECONDS", max: "MAX", change: 1 }, { attribute: "currentSaturation", min: "10_SECONDS", max: "MAX", change: 1 });
                    }
                    await setupAttributes(device, coordinatorEndpoint, "lightingColorCtrl", attributes);
                }
            }
        },
        (0, utils_1.configureSetPowerSourceWhenUnknown)("Mains (single phase)"),
    ];
    const result = { exposes, fromZigbee, toZigbee, configure, meta, isModernExtend: true };
    if (ota)
        result.ota = ota;
    return result;
}
function commandsLevelCtrl(args = {}) {
    const { commands = [
        "brightness_move_to_level",
        "brightness_move_up",
        "brightness_move_down",
        "brightness_step_up",
        "brightness_step_down",
        "brightness_stop",
    ], bind = true, endpointNames = undefined, } = args;
    let actions = commands;
    if (endpointNames) {
        actions = commands.flatMap((c) => endpointNames.map((e) => `${c}_${e}`));
    }
    const exposes = [
        exposes_1.presets.enum("action", exposes_1.access.STATE, actions).withDescription("Triggered action (e.g. a button click)").withCategory("diagnostic"),
    ];
    const fromZigbee = [fz.command_move_to_level, fz.command_move, fz.command_step, fz.command_stop];
    const result = { exposes, fromZigbee, isModernExtend: true };
    if (bind)
        result.configure = [setupConfigureForBinding("genLevelCtrl", "output", endpointNames)];
    return result;
}
function commandsColorCtrl(args = {}) {
    const { commands = [
        "color_temperature_move_stop",
        "color_temperature_move_up",
        "color_temperature_move_down",
        "color_temperature_step_up",
        "color_temperature_step_down",
        "enhanced_move_to_hue_and_saturation",
        "move_to_hue_and_saturation",
        "color_hue_step_up",
        "color_hue_step_down",
        "color_saturation_step_up",
        "color_saturation_step_down",
        "color_loop_set",
        "color_temperature_move",
        "color_move",
        "hue_move",
        "hue_stop",
        "move_to_saturation",
        "move_to_hue",
    ], bind = true, endpointNames = undefined, } = args;
    let actions = commands;
    if (endpointNames) {
        actions = commands.flatMap((c) => endpointNames.map((e) => `${c}_${e}`));
    }
    const exposes = [
        exposes_1.presets.enum("action", exposes_1.access.STATE, actions).withDescription("Triggered action (e.g. a button click)").withCategory("diagnostic"),
    ];
    const fromZigbee = [
        fz.command_move_color_temperature,
        fz.command_step_color_temperature,
        fz.command_enhanced_move_to_hue_and_saturation,
        fz.command_move_to_hue_and_saturation,
        fz.command_step_hue,
        fz.command_step_saturation,
        fz.command_color_loop_set,
        fz.command_move_to_color_temp,
        fz.command_move_to_color,
        fz.command_move_hue,
        fz.command_move_to_saturation,
        fz.command_move_to_hue,
    ];
    const result = { exposes, fromZigbee, isModernExtend: true };
    if (bind)
        result.configure = [setupConfigureForBinding("lightingColorCtrl", "output", endpointNames)];
    return result;
}
function lightingBallast() {
    const result = {
        fromZigbee: [fz.lighting_ballast_configuration],
        toZigbee: [tz.ballast_config],
        exposes: [
            new exposes_1.Numeric("ballast_minimum_level", exposes_1.access.ALL)
                .withValueMin(1)
                .withValueMax(254)
                .withDescription("Specifies the minimum light output of the ballast"),
            new exposes_1.Numeric("ballast_maximum_level", exposes_1.access.ALL)
                .withValueMin(1)
                .withValueMax(254)
                .withDescription("Specifies the maximum light output of the ballast"),
        ],
        configure: [setupConfigureForReading("lightingBallastCfg", ["minLevel", "maxLevel"])],
        isModernExtend: true,
    };
    return result;
}
function lock(args) {
    const { endpointNames = undefined, pinCodeCount, readPinCodeOnProgrammingEvent = false } = args;
    const fromZigbee = [fz.lock, fz.lock_operation_event, fz.lock_programming_event, fz.lock_pin_code_response, fz.lock_user_status_response];
    const toZigbee = [{ ...tz.lock, endpoints: endpointNames }, tz.pincode_lock, tz.lock_userstatus, tz.lock_auto_relock_time, tz.lock_sound_volume];
    const exposes = [
        exposes_1.presets.lock(),
        exposes_1.presets.pincode(),
        exposes_1.presets.lock_action(),
        exposes_1.presets.lock_action_source_name(),
        exposes_1.presets.lock_action_user(),
        exposes_1.presets.auto_relock_time().withValueMin(0).withValueMax(3600),
        exposes_1.presets.sound_volume(),
    ];
    const configure = [
        setupConfigureForReporting("closuresDoorLock", "lockState", { config: { min: "MIN", max: "1_HOUR", change: 0 }, access: exposes_1.access.STATE_GET }),
    ];
    const meta = { pinCodeCount: pinCodeCount };
    const result = { fromZigbee, toZigbee, exposes, configure, meta, isModernExtend: true };
    if (endpointNames) {
        result.exposes = (0, utils_1.flatten)(exposes.map((expose) => endpointNames.map((endpoint) => expose.clone().withEndpoint(endpoint))));
    }
    if (readPinCodeOnProgrammingEvent) {
        fromZigbee.push(fz.lock_programming_event_read_pincode);
    }
    return result;
}
function windowCovering(args) {
    const { stateSource = "lift", configureReporting = true, controls, coverInverted = false, coverMode, endpointNames = undefined } = args;
    let coverExpose = exposes_1.presets.cover();
    if (controls.includes("lift"))
        coverExpose = coverExpose.withPosition();
    if (controls.includes("tilt"))
        coverExpose = coverExpose.withTilt();
    const exposes = [coverExpose];
    const fromZigbee = [fz.cover_position_tilt];
    const toZigbee = [{ ...tz.cover_state, endpoints: endpointNames }, tz.cover_position_tilt];
    const result = { exposes, fromZigbee, toZigbee, isModernExtend: true };
    if (configureReporting) {
        const configure = [];
        if (controls.includes("lift")) {
            configure.push(setupConfigureForReporting("closuresWindowCovering", "currentPositionLiftPercentage", {
                config: { min: "1_SECOND", max: "MAX", change: 1 },
                access: exposes_1.access.STATE_GET,
            }));
        }
        if (controls.includes("tilt")) {
            configure.push(setupConfigureForReporting("closuresWindowCovering", "currentPositionTiltPercentage", {
                config: { min: "1_SECOND", max: "MAX", change: 1 },
                access: exposes_1.access.STATE_GET,
            }));
        }
        result.configure = configure;
    }
    if (coverInverted || stateSource === "tilt") {
        const meta = {};
        if (coverInverted)
            meta.coverInverted = true;
        if (stateSource === "tilt")
            meta.coverStateFromTilt = true;
        result.meta = meta;
    }
    if (coverMode) {
        result.toZigbee.push(tz.cover_mode);
        result.exposes.push(exposes_1.presets.cover_mode());
    }
    if (endpointNames) {
        result.exposes = (0, utils_1.flatten)(exposes.map((expose) => endpointNames.map((endpoint) => expose.clone().withEndpoint(endpoint))));
    }
    return result;
}
function commandsWindowCovering(args = {}) {
    const { commands = ["open", "close", "stop"], bind = true, endpointNames = undefined } = args;
    let actions = commands;
    if (endpointNames) {
        actions = commands.flatMap((c) => endpointNames.map((e) => `${c}_${e}`));
    }
    const exposes = [
        exposes_1.presets.enum("action", exposes_1.access.STATE, actions).withDescription("Triggered action (e.g. a button click)").withCategory("diagnostic"),
    ];
    const actionPayloadLookup = {
        commandUpOpen: "open",
        commandDownClose: "close",
        commandStop: "stop",
    };
    const fromZigbee = [
        {
            cluster: "closuresWindowCovering",
            type: ["commandUpOpen", "commandDownClose", "commandStop"],
            convert: (model, msg, publish, options, meta) => {
                if ((0, utils_1.hasAlreadyProcessedMessage)(msg, model))
                    return;
                const payload = { action: (0, utils_1.postfixWithEndpointName)(actionPayloadLookup[msg.type], msg, model, meta) };
                (0, utils_1.addActionGroup)(payload, msg, model);
                return payload;
            },
        },
    ];
    const result = { exposes, fromZigbee, isModernExtend: true };
    if (bind)
        result.configure = [setupConfigureForBinding("closuresWindowCovering", "output", endpointNames)];
    return result;
}
function iasZoneAlarm(args) {
    const exposes = [];
    let invertAlarmPayload = args.zoneType === "contact";
    if (args.invertAlarm)
        invertAlarmPayload = !invertAlarmPayload;
    const bothAlarms = args.zoneAttributes.includes("alarm_1") && args.zoneAttributes.includes("alarm_2");
    let alarm1Name = "alarm_1";
    let alarm2Name = "alarm_2";
    if (args.zoneType === "generic") {
        args.zoneAttributes.forEach((attr) => {
            let expose = IAS_EXPOSE_LOOKUP[attr];
            if (args.description) {
                expose = expose.clone().withDescription(args.description);
            }
            exposes.push(expose);
        });
    }
    else {
        if (bothAlarms) {
            exposes.push(exposes_1.presets
                .binary(`${args.zoneType}_alarm_1`, exposes_1.access.STATE, true, false)
                .withDescription(`${IAS_EXPOSE_LOOKUP[args.zoneType].description} (alarm_1)`));
            alarm1Name = `${args.zoneType}_alarm_1`;
            exposes.push(exposes_1.presets
                .binary(`${args.zoneType}_alarm_2`, exposes_1.access.STATE, true, false)
                .withDescription(`${IAS_EXPOSE_LOOKUP[args.zoneType].description} (alarm_2)`));
            alarm2Name = `${args.zoneType}_alarm_2`;
        }
        else {
            exposes.push(IAS_EXPOSE_LOOKUP[args.zoneType]);
            alarm1Name = args.zoneType;
            alarm2Name = args.zoneType;
        }
        args.zoneAttributes.forEach((attr) => {
            if (attr !== "alarm_1" && attr !== "alarm_2")
                exposes.push(IAS_EXPOSE_LOOKUP[attr]);
        });
    }
    if (args.manufacturerZoneAttributes)
        args.manufacturerZoneAttributes.forEach((attr) => {
            let expose = exposes_1.presets.binary(attr.name, exposes_1.access.STATE, attr.valueOn, attr.valueOff).withDescription(attr.description);
            if (attr.entityCategory)
                expose = expose.withCategory(attr.entityCategory);
            exposes.push(expose);
        });
    const timeoutProperty = `${args.zoneType}_timeout`;
    const fromZigbee = [
        {
            cluster: "ssIasZone",
            type: ["commandStatusChangeNotification", "attributeReport", "readResponse"],
            options: args.alarmTimeout
                ? [
                    exposes_1.presets
                        .numeric(timeoutProperty, exposes_1.access.SET)
                        .withValueMin(0)
                        .withDescription(`Time in seconds after which ${args.zoneType} is cleared after detecting it (default 90 seconds).`),
                ]
                : [],
            convert: (model, msg, publish, options, meta) => {
                if (args.alarmTimeout) {
                    const timeout = options?.[timeoutProperty] != null ? Number(options[timeoutProperty]) : 90;
                    clearTimeout(globalStore.getValue(msg.endpoint, "timer"));
                    if (timeout !== 0) {
                        const timer = setTimeout(() => publish({ [alarm1Name]: false, [alarm2Name]: false }), timeout * 1000);
                        globalStore.putValue(msg.endpoint, "timer", timer);
                    }
                }
                const isChange = msg.type === "commandStatusChangeNotification";
                const zoneStatus = "zonestatus" in msg.data ? msg.data.zonestatus : msg.data.zoneStatus;
                if (zoneStatus !== undefined) {
                    let payload = {};
                    if (args.zoneAttributes.includes("tamper")) {
                        payload = { tamper: (zoneStatus & (1 << 2)) > 0, ...payload };
                    }
                    if (args.zoneAttributes.includes("battery_low")) {
                        payload = { battery_low: (zoneStatus & (1 << 3)) > 0, ...payload };
                    }
                    if (args.zoneAttributes.includes("supervision_reports")) {
                        payload = { supervision_reports: (zoneStatus & (1 << 4)) > 0, ...payload };
                    }
                    if (args.zoneAttributes.includes("restore_reports")) {
                        payload = { restore_reports: (zoneStatus & (1 << 5)) > 0, ...payload };
                    }
                    if (args.zoneAttributes.includes("trouble")) {
                        payload = { trouble: (zoneStatus & (1 << 6)) > 0, ...payload };
                    }
                    if (args.zoneAttributes.includes("ac_status")) {
                        payload = { ac_status: (zoneStatus & (1 << 7)) > 0, ...payload };
                    }
                    if (args.zoneAttributes.includes("test")) {
                        payload = { test: (zoneStatus & (1 << 8)) > 0, ...payload };
                    }
                    if (args.zoneAttributes.includes("battery_defect")) {
                        payload = { battery_defect: (zoneStatus & (1 << 9)) > 0, ...payload };
                    }
                    let alarm1Payload = (zoneStatus & 1) > 0;
                    let alarm2Payload = (zoneStatus & (1 << 1)) > 0;
                    if (invertAlarmPayload) {
                        alarm1Payload = !alarm1Payload;
                        alarm2Payload = !alarm2Payload;
                    }
                    // Can't just alarm1Payload || alarm2Payload as an unused alarm's bit might be always 1 or random in the received data
                    let addTimeout = false;
                    if (args.zoneAttributes.includes("alarm_1")) {
                        payload = { [alarm1Name]: alarm1Payload, ...payload };
                        addTimeout ||= alarm1Payload;
                    }
                    if (args.zoneAttributes.includes("alarm_2")) {
                        payload = { [alarm2Name]: alarm2Payload, ...payload };
                        addTimeout ||= alarm2Payload;
                    }
                    if (isChange && args.keepAliveTimeout > 0) {
                        // This sensor continuously sends occupation updates as long as motion is detected; (re)start a timeout
                        // each time we receive one, in case the clearance message gets lost. Normally, these kinds of sensors
                        // send a clearance message, so this is an additional safety measure.
                        clearTimeout(globalStore.getValue(msg.endpoint, "timeout"));
                        if (addTimeout) {
                            // At least one zone active
                            const timer = setTimeout(() => publish({ [alarm1Name]: false, [alarm2Name]: false }), args.keepAliveTimeout * 1000);
                            globalStore.putValue(msg.endpoint, "timeout", timer);
                        }
                        else {
                            globalStore.clearValue(msg.endpoint, "timeout");
                        }
                    }
                    if (args.manufacturerZoneAttributes)
                        args.manufacturerZoneAttributes.forEach((attr) => {
                            payload = { [attr.name]: (zoneStatus & (1 << attr.bit)) > 0, ...payload };
                        });
                    return payload;
                }
            },
        },
    ];
    let configure;
    if (args.zoneStatusReporting) {
        configure = [
            async (device, coordinatorEndpoint) => {
                await setupAttributes(device, coordinatorEndpoint, "ssIasZone", [{ attribute: "zoneStatus", min: "MIN", max: "MAX", change: 0 }]);
            },
        ];
    }
    return { fromZigbee, exposes, isModernExtend: true, ...(configure && { configure }) };
}
function iasWarning(args = {}) {
    const { reversePayload = false } = args;
    const warningMode = { stop: 0, burglar: 1, fire: 2, emergency: 3, police_panic: 4, fire_panic: 5, emergency_panic: 6 };
    // levels for siren, strobe and squawk are identical
    const level = { low: 0, medium: 1, high: 2, very_high: 3 };
    const exposes = [
        exposes_1.presets
            .composite("warning", "warning", exposes_1.access.SET)
            .withFeature(exposes_1.presets.enum("mode", exposes_1.access.SET, Object.keys(warningMode)).withDescription("Mode of the warning (sound effect)"))
            .withFeature(exposes_1.presets.enum("level", exposes_1.access.SET, Object.keys(level)).withDescription("Sound level"))
            .withFeature(exposes_1.presets.enum("strobe_level", exposes_1.access.SET, Object.keys(level)).withDescription("Intensity of the strobe"))
            .withFeature(exposes_1.presets.binary("strobe", exposes_1.access.SET, true, false).withDescription("Turn on/off the strobe (light) during warning"))
            .withFeature(exposes_1.presets.numeric("strobe_duty_cycle", exposes_1.access.SET).withValueMax(10).withValueMin(0).withDescription("Length of the flash cycle"))
            .withFeature(exposes_1.presets.numeric("duration", exposes_1.access.SET).withUnit("s").withDescription("Duration in seconds of the alarm")),
    ];
    const toZigbee = [
        {
            key: ["warning"],
            convertSet: async (entity, key, value, meta) => {
                const values = {
                    // @ts-expect-error ignore
                    mode: value.mode || "emergency",
                    // @ts-expect-error ignore
                    level: value.level || "medium",
                    // @ts-expect-error ignore
                    strobe: value.strobe != null ? value.strobe : true,
                    // @ts-expect-error ignore
                    duration: value.duration != null ? value.duration : 10,
                    // @ts-expect-error ignore
                    strobeDutyCycle: value.strobe_duty_cycle != null ? value.strobe_duty_cycle * 10 : 0,
                    // @ts-expect-error ignore
                    strobeLevel: value.strobe_level != null ? utils.getFromLookup(value.strobe_level, strobeLevel) : 1,
                };
                // biome-ignore lint/suspicious/noImplicitAnyLet: ignored using `--suppress`
                let info;
                if (reversePayload) {
                    info = (0, utils_1.getFromLookup)(values.mode, warningMode) + ((values.strobe ? 1 : 0) << 4) + ((0, utils_1.getFromLookup)(values.level, level) << 6);
                }
                else {
                    info = ((0, utils_1.getFromLookup)(values.mode, warningMode) << 4) + ((values.strobe ? 1 : 0) << 2) + (0, utils_1.getFromLookup)(values.level, level);
                }
                const payload = {
                    startwarninginfo: info,
                    warningduration: values.duration,
                    strobedutycycle: values.strobeDutyCycle,
                    strobelevel: values.strobeLevel,
                };
                await entity.command("ssIasWd", "startWarning", payload, (0, utils_1.getOptions)(meta.mapped, entity));
            },
        },
    ];
    return { toZigbee, exposes, isModernExtend: true };
}
function genericMeter(args = {}) {
    const { tariffs = false } = args;
    if (args.cluster !== "electrical") {
        const divisors = new Set([
            args.cluster === "metering" && (0, utils_1.isObject)(args.power) ? args.power?.divisor : false,
            (0, utils_1.isObject)(args.energy) ? args.energy?.divisor : false,
            (0, utils_1.isObject)(args.producedEnergy) ? args.producedEnergy?.divisor : false,
        ]);
        divisors.delete(false);
        const multipliers = new Set([
            args.cluster === "metering" && (0, utils_1.isObject)(args.power) ? args.power?.multiplier : false,
            (0, utils_1.isObject)(args.energy) ? args.energy?.multiplier : false,
            (0, utils_1.isObject)(args.producedEnergy) ? args.producedEnergy?.multiplier : false,
        ]);
        multipliers.delete(false);
        if (multipliers.size > 1 || divisors.size > 1) {
            throw new Error(`When cluster is metering, power and energy divisor/multiplier should be equal, got divisors=${Array.from(divisors).join(", ")}, multipliers=${Array.from(multipliers).join(", ")}`);
        }
    }
    if (args.cluster === "electrical" && args.producedEnergy) {
        throw new Error(`Produced energy is not supported with cluster 'electrical', use 'both' or 'metering'`);
    }
    if (args.cluster === "metering" && args.acFrequency) {
        throw new Error(`AC frequency is not supported with cluster 'metering', use 'both' or 'electrical'`);
    }
    if (args.cluster === "metering" && args.powerFactor) {
        throw new Error(`Power factor is not supported with cluster 'metering', use 'both' or 'electrical'`);
    }
    if (args.cluster === "metering" && args.electricalMeasurementType === "dc") {
        throw new Error(`DC attributes are not supported with cluster 'metering', use 'both' or 'ac'`);
    }
    let exposes = [];
    let fromZigbee;
    let toZigbee;
    const changeLookup = {
        gas: {
            power: 0.01,
            energy: 0.1,
        },
        electricity: {
            power: 0.005,
            energy: 0.1,
        },
    };
    const configureLookup = {
        haElectricalMeasurement: {
            // Report change with every 5W change
            power: { attribute: "activePower", divisor: "acPowerDivisor", multiplier: "acPowerMultiplier", forced: args.power, change: 5 },
            power_phase_b: {
                attribute: "activePowerPhB",
                divisor: "acPowerDivisor",
                multiplier: "acPowerMultiplier",
                forced: args.power,
                change: 5,
            },
            power_phase_c: {
                attribute: "activePowerPhC",
                divisor: "acPowerDivisor",
                multiplier: "acPowerMultiplier",
                forced: args.power,
                change: 5,
            },
            // Report change with every 0.05A change
            current: {
                attribute: "rmsCurrent",
                divisor: "acCurrentDivisor",
                multiplier: "acCurrentMultiplier",
                forced: args.current,
                change: 0.05,
            },
            // Report change every 1 Hz
            ac_frequency: {
                attribute: "acFrequency",
                divisor: "acFrequencyDivisor",
                multiplier: "acFrequencyMultiplier",
                forced: (0, utils_1.isObject)(args.acFrequency) ? args.acFrequency : false,
                change: 1,
            },
            current_phase_b: {
                attribute: "rmsCurrentPhB",
                divisor: "acCurrentDivisor",
                multiplier: "acCurrentMultiplier",
                forced: args.current,
                change: 0.05,
            },
            current_phase_c: {
                attribute: "rmsCurrentPhC",
                divisor: "acCurrentDivisor",
                multiplier: "acCurrentMultiplier",
                forced: args.current,
                change: 0.05,
            },
            current_neutral: {
                attribute: "neutralCurrent",
                divisor: "acCurrentDivisor",
                multiplier: "acCurrentMultiplier",
                forced: args.current,
                change: 0.05,
            },
            power_factor: {
                attribute: "powerFactor",
                change: 10,
            },
            // Report change with every 5V change
            voltage: {
                attribute: "rmsVoltage",
                divisor: "acVoltageDivisor",
                multiplier: "acVoltageMultiplier",
                forced: args.voltage,
                change: 5,
            },
            voltage_phase_b: {
                attribute: "rmsVoltagePhB",
                divisor: "acVoltageDivisor",
                multiplier: "acVoltageMultiplier",
                forced: args.voltage,
                change: 5,
            },
            voltage_phase_c: {
                attribute: "rmsVoltagePhC",
                divisor: "acVoltageDivisor",
                multiplier: "acVoltageMultiplier",
                forced: args.voltage,
                change: 5,
            },
            // Report change with every 100mW change
            dc_power: { attribute: "dcPower", divisor: "dcPowerDivisor", multiplier: "dcPowerMultiplier", forced: args.power, change: 100 },
            // Report change with every 100mV change
            dc_voltage: {
                attribute: "dcVoltage",
                divisor: "dcVoltageDivisor",
                multiplier: "dcVoltageMultiplier",
                forced: args.voltage,
                change: 100,
            },
            // Report change with every 100mA change
            dc_current: {
                attribute: "dcCurrent",
                divisor: "dcCurrentDivisor",
                multiplier: "dcCurrentMultiplier",
                forced: args.current,
                change: 100,
            },
        },
        seMetering: {
            // Report change with every 5W change
            power: {
                attribute: "instantaneousDemand",
                divisor: "divisor",
                multiplier: "multiplier",
                forced: args.power,
                change: changeLookup[args.type].power,
            },
            // Report change with every 0.1kWh change
            energy: {
                attribute: "currentSummDelivered",
                divisor: "divisor",
                multiplier: "multiplier",
                forced: args.energy,
                change: changeLookup[args.type].energy,
            },
            produced_energy: {
                attribute: "currentSummReceived",
                divisor: "divisor",
                multiplier: "multiplier",
                forced: (0, utils_1.isObject)(args.producedEnergy) ? args.producedEnergy : false,
                change: 0.1,
            },
            energy_tier_1: {
                attribute: "currentTier1SummDelivered",
                divisor: "divisor",
                multiplier: "multiplier",
                forced: false,
                change: 0.1,
            },
            energy_tier_2: {
                attribute: "currentTier2SummDelivered",
                divisor: "divisor",
                multiplier: "multiplier",
                forced: false,
                change: 0.1,
            },
            produced_energy_tier_1: {
                attribute: "currentTier1SummReceived",
                divisor: "divisor",
                multiplier: "multiplier",
                forced: false,
                change: 0.1,
            },
            produced_energy_tier_2: {
                attribute: "currentTier2SummReceived",
                divisor: "divisor",
                multiplier: "multiplier",
                forced: false,
                change: 0.1,
            },
            status: {
                attribute: "status",
                change: 1,
            },
            extended_status: {
                attribute: "extendedStatus",
                change: 1,
            },
        },
    };
    if (args.power === false) {
        delete configureLookup.haElectricalMeasurement.power;
        delete configureLookup.seMetering.power;
        delete configureLookup.haElectricalMeasurement.power_phase_b;
        delete configureLookup.haElectricalMeasurement.power_phase_c;
        delete configureLookup.haElectricalMeasurement.dc_power;
    }
    if (args.voltage === false) {
        delete configureLookup.haElectricalMeasurement.voltage;
        delete configureLookup.haElectricalMeasurement.voltage_phase_b;
        delete configureLookup.haElectricalMeasurement.voltage_phase_c;
        delete configureLookup.haElectricalMeasurement.dc_voltage;
    }
    if (args.current === false) {
        delete configureLookup.haElectricalMeasurement.current;
        delete configureLookup.haElectricalMeasurement.current_phase_b;
        delete configureLookup.haElectricalMeasurement.current_phase_c;
        delete configureLookup.haElectricalMeasurement.current_neutral;
        delete configureLookup.haElectricalMeasurement.dc_current;
    }
    if (args.energy === false) {
        delete configureLookup.seMetering.energy;
    }
    if (args.producedEnergy === false) {
        delete configureLookup.seMetering.produced_energy;
    }
    if (args.powerFactor === false) {
        delete configureLookup.haElectricalMeasurement.power_factor;
    }
    if (args.acFrequency === false) {
        delete configureLookup.haElectricalMeasurement.ac_frequency;
    }
    if (args.threePhase === false) {
        delete configureLookup.haElectricalMeasurement.power_phase_b;
        delete configureLookup.haElectricalMeasurement.power_phase_c;
        delete configureLookup.haElectricalMeasurement.current_phase_b;
        delete configureLookup.haElectricalMeasurement.current_phase_c;
        delete configureLookup.haElectricalMeasurement.current_neutral;
        delete configureLookup.haElectricalMeasurement.voltage_phase_b;
        delete configureLookup.haElectricalMeasurement.voltage_phase_c;
    }
    if (args.electricalMeasurementType === "dc") {
        delete configureLookup.haElectricalMeasurement.power;
        delete configureLookup.haElectricalMeasurement.voltage;
        delete configureLookup.haElectricalMeasurement.current;
        delete configureLookup.haElectricalMeasurement.power_factor;
        delete configureLookup.haElectricalMeasurement.ac_frequency;
        delete configureLookup.haElectricalMeasurement.power_phase_b;
        delete configureLookup.haElectricalMeasurement.power_phase_c;
        delete configureLookup.haElectricalMeasurement.current_phase_b;
        delete configureLookup.haElectricalMeasurement.current_phase_c;
        delete configureLookup.haElectricalMeasurement.current_neutral;
        delete configureLookup.haElectricalMeasurement.voltage_phase_b;
        delete configureLookup.haElectricalMeasurement.voltage_phase_c;
    }
    if (args.electricalMeasurementType === "ac") {
        delete configureLookup.haElectricalMeasurement.dc_power;
        delete configureLookup.haElectricalMeasurement.dc_voltage;
        delete configureLookup.haElectricalMeasurement.dc_current;
    }
    if (args.status === false) {
        delete configureLookup.seMetering.status;
    }
    if (args.extendedStatus === false) {
        delete configureLookup.seMetering.extended_status;
    }
    if (tariffs === false) {
        delete configureLookup.seMetering.energy_tier_1;
        delete configureLookup.seMetering.energy_tier_2;
        delete configureLookup.seMetering.produced_energy_tier_1;
        delete configureLookup.seMetering.produced_energy_tier_2;
    }
    if (args.cluster === "both") {
        if (args.power !== false)
            exposes.push(exposes_1.presets.power().withAccess(exposes_1.access.STATE_GET));
        if (args.voltage !== false)
            exposes.push(exposes_1.presets.voltage().withAccess(exposes_1.access.STATE_GET));
        if (args.acFrequency !== false)
            exposes.push(exposes_1.presets.ac_frequency().withAccess(exposes_1.access.STATE_GET));
        if (args.powerFactor !== false)
            exposes.push(exposes_1.presets.power_factor().withAccess(exposes_1.access.STATE_GET));
        if (args.current !== false)
            exposes.push(exposes_1.presets.current().withAccess(exposes_1.access.STATE_GET));
        if (args.energy !== false)
            exposes.push(exposes_1.presets.energy().withAccess(exposes_1.access.STATE_GET));
        if (args.producedEnergy !== false)
            exposes.push(exposes_1.presets.produced_energy().withAccess(exposes_1.access.STATE_GET));
        if (tariffs === true) {
            exposes.push(exposes_1.presets.numeric("energy_tier_1", exposes_1.access.STATE_GET).withUnit("kWh").withDescription("Energy consumed in tariff 1 (peak/high) - OBIS 1.8.1"), exposes_1.presets.numeric("energy_tier_2", exposes_1.access.STATE_GET).withUnit("kWh").withDescription("Energy consumed in tariff 2 (off-peak/low) - OBIS 1.8.2"), exposes_1.presets
                .numeric("produced_energy_tier_1", exposes_1.access.STATE_GET)
                .withUnit("kWh")
                .withDescription("Energy produced in tariff 1 (peak/high) - OBIS 2.8.1"), exposes_1.presets
                .numeric("produced_energy_tier_2", exposes_1.access.STATE_GET)
                .withUnit("kWh")
                .withDescription("Energy produced in tariff 2 (off-peak/low) - OBIS 2.8.2"));
        }
        fromZigbee = [args.fzElectricalMeasurement ?? fz.electrical_measurement, args.fzMetering ?? fz.metering];
        const useMeteringForPower = args.power !== false && args.power?.cluster === "metering";
        toZigbee = [
            useMeteringForPower ? tz.metering_power : tz.electrical_measurement_power,
            tz.acvoltage,
            tz.accurrent,
            tz.currentsummdelivered,
            tz.currentsummreceived,
            tz.frequency,
            tz.powerfactor,
        ];
        if (useMeteringForPower) {
            delete configureLookup.haElectricalMeasurement.power;
        }
        else {
            delete configureLookup.seMetering.power;
        }
    }
    else if (args.cluster === "metering" && args.type === "electricity") {
        if (args.power !== false)
            exposes.push(exposes_1.presets.power().withAccess(exposes_1.access.STATE_GET));
        if (args.energy !== false)
            exposes.push(exposes_1.presets.energy().withAccess(exposes_1.access.STATE_GET));
        if (args.producedEnergy !== false)
            exposes.push(exposes_1.presets.produced_energy().withAccess(exposes_1.access.STATE_GET));
        if (tariffs === true) {
            exposes.push(exposes_1.presets.numeric("energy_tier_1", exposes_1.access.STATE_GET).withUnit("kWh").withDescription("Energy consumed in tariff 1 (peak/high) - OBIS 1.8.1"), exposes_1.presets.numeric("energy_tier_2", exposes_1.access.STATE_GET).withUnit("kWh").withDescription("Energy consumed in tariff 2 (off-peak/low) - OBIS 1.8.2"), exposes_1.presets
                .numeric("produced_energy_tier_1", exposes_1.access.STATE_GET)
                .withUnit("kWh")
                .withDescription("Energy produced in tariff 1 (peak/high) - OBIS 2.8.1"), exposes_1.presets
                .numeric("produced_energy_tier_2", exposes_1.access.STATE_GET)
                .withUnit("kWh")
                .withDescription("Energy produced in tariff 2 (off-peak/low) - OBIS 2.8.2"));
        }
        fromZigbee = [args.fzMetering ?? fz.metering];
        toZigbee = [tz.metering_power, tz.currentsummdelivered, tz.currentsummreceived];
        delete configureLookup.haElectricalMeasurement;
    }
    else if (args.cluster === "metering" && args.type === "gas") {
        if (args.power !== false)
            exposes.push(exposes_1.presets.numeric("power", exposes_1.access.STATE_GET).withUnit("m³/h").withDescription("Instantaneous gas flow in m³/h"));
        if (args.energy !== false)
            exposes.push(exposes_1.presets.numeric("energy", exposes_1.access.ALL).withUnit("m³").withDescription("Total gas consumption in m³"));
        fromZigbee = [args.fzMetering ?? fz.gas_metering];
        toZigbee = [
            {
                key: ["energy"],
                convertGet: async (entity, key, meta) => {
                    const ep = (0, utils_1.determineEndpoint)(entity, meta, "seMetering");
                    await ep.read("seMetering", ["currentSummDelivered"]);
                },
                convertSet: async (entity, key, value, meta) => {
                    (0, utils_1.assertNumber)(value);
                    await entity.write("seMetering", { currentSummDelivered: Math.round(value * 100) });
                    return { state: { energy: value } };
                },
            },
            tz.metering_power,
            tz.metering_status,
            tz.metering_extended_status,
        ];
        delete configureLookup.haElectricalMeasurement;
    }
    else if (args.cluster === "electrical") {
        if (args.power !== false)
            exposes.push(exposes_1.presets.power().withAccess(exposes_1.access.STATE_GET));
        if (args.voltage !== false)
            exposes.push(exposes_1.presets.voltage().withAccess(exposes_1.access.STATE_GET));
        if (args.current !== false)
            exposes.push(exposes_1.presets.current().withAccess(exposes_1.access.STATE_GET));
        if (args.acFrequency !== false)
            exposes.push(exposes_1.presets.ac_frequency().withAccess(exposes_1.access.STATE_GET));
        if (args.powerFactor !== false)
            exposes.push(exposes_1.presets.power_factor().withAccess(exposes_1.access.STATE_GET));
        fromZigbee = [args.fzElectricalMeasurement ?? fz.electrical_measurement];
        toZigbee = [tz.electrical_measurement_power, tz.acvoltage, tz.accurrent, tz.frequency, tz.powerfactor];
        delete configureLookup.seMetering;
    }
    if (args.threePhase === true) {
        exposes.push(exposes_1.presets.power_phase_b().withAccess(exposes_1.access.STATE_GET), exposes_1.presets.power_phase_c().withAccess(exposes_1.access.STATE_GET), exposes_1.presets.voltage_phase_b().withAccess(exposes_1.access.STATE_GET), exposes_1.presets.voltage_phase_c().withAccess(exposes_1.access.STATE_GET), exposes_1.presets.current_phase_b().withAccess(exposes_1.access.STATE_GET), exposes_1.presets.current_phase_c().withAccess(exposes_1.access.STATE_GET), exposes_1.presets.current_neutral().withAccess(exposes_1.access.STATE_GET));
        toZigbee.push(tz.electrical_measurement_power_phase_b, tz.electrical_measurement_power_phase_c, tz.acvoltage_phase_b, tz.acvoltage_phase_c, tz.accurrent_phase_b, tz.accurrent_phase_c, tz.accurrent_neutral);
    }
    if (args.endpointNames) {
        exposes = (0, utils_1.flatten)(exposes.map((expose) => args.endpointNames.map((endpoint) => expose.clone().withEndpoint(endpoint))));
    }
    const result = { exposes, fromZigbee, toZigbee, isModernExtend: true };
    if (args.configureReporting) {
        result.configure = [
            async (device, coordinatorEndpoint) => {
                for (const [cluster, properties] of Object.entries(configureLookup)) {
                    for (const endpoint of getEndpointsWithCluster(device, cluster, "input")) {
                        const items = [];
                        for (const property of Object.values(properties)) {
                            let change = property.change;
                            let min = "10_SECONDS";
                            let max = "MAX";
                            // Check if this property has a divisor and multiplier, e.g. AC frequency doesn't.
                            if ("divisor" in property) {
                                // In case multiplier or divisor was provided, use that instead of reading from device.
                                if (property.forced && (property.forced.divisor || property.forced.multiplier)) {
                                    endpoint.saveClusterAttributeKeyValue(cluster, {
                                        [property.divisor]: property.forced.divisor ?? 1,
                                        [property.multiplier]: property.forced.multiplier ?? 1,
                                    });
                                    endpoint.save();
                                }
                                else {
                                    await endpoint.read(cluster, [property.divisor, property.multiplier]);
                                }
                                const divisor = endpoint.getClusterAttributeValue(cluster, property.divisor);
                                (0, utils_1.assertNumber)(divisor, property.divisor);
                                const multiplier = endpoint.getClusterAttributeValue(cluster, property.multiplier);
                                (0, utils_1.assertNumber)(multiplier, property.multiplier);
                                change = property.change * (divisor / multiplier);
                            }
                            if ("forced" in property && property.forced) {
                                if ("min" in property.forced) {
                                    min = property.forced.min;
                                }
                                if ("max" in property.forced) {
                                    max = property.forced.max;
                                }
                                if ("change" in property.forced) {
                                    change = property.forced.change;
                                }
                            }
                            items.push({ attribute: property.attribute, min, max, change });
                        }
                        if (items.length) {
                            await setupAttributes(endpoint, coordinatorEndpoint, cluster, items);
                        }
                    }
                }
            },
        ];
    }
    return result;
}
function electricityMeter(args = {}) {
    return genericMeter({
        type: "electricity",
        cluster: "both",
        electricalMeasurementType: "ac",
        configureReporting: true,
        threePhase: false,
        producedEnergy: false,
        acFrequency: false,
        powerFactor: false,
        status: false,
        extendedStatus: false,
        ...args,
    });
}
function gasMeter(args = {}) {
    return genericMeter({
        type: "gas",
        cluster: "metering",
        configureReporting: true,
        status: true,
        extendedStatus: true,
        producedEnergy: false,
        ...args,
    });
}
// #endregion
// #region Other extends
/**
 * Version of the GP spec: 1.1.1
 */
exports.GPDF_COMMANDS = {
    /*0x00*/ 0: "identify",
    /*0x10*/ 16: "recall_scene0",
    /*0x11*/ 17: "recall_scene1",
    /*0x12*/ 18: "recall_scene2",
    /*0x13*/ 19: "recall_scene3",
    /*0x14*/ 20: "recall_scene4",
    /*0x15*/ 21: "recall_scene5",
    /*0x16*/ 22: "recall_scene6",
    /*0x17*/ 23: "recall_scene7",
    /*0x18*/ 24: "store_scene0",
    /*0x19*/ 25: "store_scene1",
    /*0x1a*/ 26: "store_scene2",
    /*0x1b*/ 27: "store_scene3",
    /*0x1c*/ 28: "store_scene4",
    /*0x1d*/ 29: "store_scene5",
    /*0x1e*/ 30: "store_scene6",
    /*0x1f*/ 31: "store_scene7",
    /*0x20*/ 32: "off",
    /*0x21*/ 33: "on",
    /*0x22*/ 34: "toggle",
    /*0x23*/ 35: "release",
    /*0x30*/ 48: "move_up", // with payload
    /*0x31*/ 49: "move_down", // with payload
    /*0x32*/ 50: "step_up", // with payload
    /*0x33*/ 51: "step_down", // with payload
    /*0x34*/ 52: "level_control_stop",
    /*0x35*/ 53: "move_up_with_on_off", // with payload
    /*0x36*/ 54: "move_down_with_on_off", // with payload
    /*0x37*/ 55: "step_up_with_on_off", // with payload
    /*0x38*/ 56: "step_down_with_on_off", // with payload
    /*0x40*/ 64: "move_hue_stop",
    /*0x41*/ 65: "move_hue_up", // with payload
    /*0x42*/ 66: "move_hue_down", // with payload
    /*0x43*/ 67: "step_hue_up", // with payload
    /*0x44*/ 68: "step_huw_down", // with payload
    /*0x45*/ 69: "move_saturation_stop",
    /*0x46*/ 70: "move_saturation_up", // with payload
    /*0x47*/ 71: "move_saturation_down", // with payload
    /*0x48*/ 72: "step_saturation_up", // with payload
    /*0x49*/ 73: "step_saturation_down", // with payload
    /*0x4a*/ 74: "move_color", // with payload
    /*0x4b*/ 75: "step_color", // with payload
    /*0x50*/ 80: "lock_door",
    /*0x51*/ 81: "unlock_door",
    /*0x60*/ 96: "press11",
    /*0x61*/ 97: "release11",
    /*0x62*/ 98: "press12",
    /*0x63*/ 99: "release12",
    /*0x64*/ 100: "press22",
    /*0x65*/ 101: "release22",
    /*0x66*/ 102: "short_press11",
    /*0x67*/ 103: "short_press12",
    /*0x68*/ 104: "short_press22",
    /*0x69*/ 105: "press_8bit_vector", // with payload
    /*0x6a*/ 106: "release_8bit_vector", // with payload
    /*0xa0*/ 160: "attribute_reporting", // with payload
    /*0xa1*/ 161: "manufacture_specific_attr_reporting", // with payload
    /*0xa2*/ 162: "multi_cluster_reporting", // with payload
    /*0xa3*/ 163: "manufacturer_specific_mcluster_reporting", // with payload
    /*0xa4*/ 164: "request_attributes", // with payload
    /*0xa5*/ 165: "read_attributes_response", // with payload
    /*0xa6*/ 166: "zcl_tunneling", // with payload
    /*0xa8*/ 168: "compact_attribute_reporting", // with payload
    /*0xaf*/ 175: "any_sensor_command_a0_a3", // with payload
    /*0xe0*/ 224: "commissioning", // with payload
    /*0xe1*/ 225: "decommissioning",
    /*0xe2*/ 226: "success",
    /*0xe3*/ 227: "channel_request", // with payload
    /*0xe4*/ 228: "application_description", // with payload
    //-- sent to GPD
    // /*0xf0*/ 240: "commissioning_reply",
    // /*0xf1*/ 241: "write_attributes",
    // /*0xf2*/ 242: "read_attributes",
    // /*0xf3*/ 243: "channel_configuration",
    // /*0xf6*/ 246: "zcl_tunneling",
};
function genericGreenPower() {
    const exposes = [
        exposes_1.presets.action(Object.values(exports.GPDF_COMMANDS)),
        exposes_1.presets.list("payload", exposes_1.access.STATE, exposes_1.presets.numeric("payload", exposes_1.access.STATE).withDescription("Byte")).withDescription("Payload of the command"),
    ];
    const fromZigbee = [
        {
            cluster: "greenPower",
            type: ["commandNotification", "commandCommissioningNotification"],
            convert: (model, msg, publish, options, meta) => {
                const commandID = msg.data.commandID;
                if ((0, utils_1.hasAlreadyProcessedMessage)(msg, model, msg.data.frameCounter, `${msg.device.ieeeAddr}_${commandID}`))
                    return;
                if (commandID >= 0xe0)
                    return; // Skip op commands
                const gpdfCommandStr = exports.GPDF_COMMANDS[commandID];
                const payloadBuf = "raw" in msg.data.commandFrame ? msg.data.commandFrame.raw : undefined;
                return {
                    action: gpdfCommandStr ?? `unknown_${commandID}`,
                    payload: payloadBuf?.length > 0 ? Array.from(payloadBuf) : [],
                };
            },
        },
    ];
    return { exposes, fromZigbee, isModernExtend: true };
}
function commandsScenes(args = {}) {
    const { commands = ["recall", "store", "add", "remove", "remove_all"], bind = true, endpointNames = undefined } = args;
    // biome-ignore lint/style/noNonNullAssertion: ignored using `--suppress`
    let actions = commands;
    if (endpointNames) {
        actions = commands.flatMap((c) => endpointNames.map((e) => `${c}_${e}`));
    }
    const exposesArray = [exposes_1.presets.enum("action", exposes_1.access.STATE, actions).withDescription("Triggered scene action (e.g. recall a scene)")];
    const actionPayloadLookup = {
        commandRecall: "recall",
        commandStore: "store",
        commandAdd: "add",
        commandRemove: "remove",
        commandRemoveAll: "remove_all",
    };
    const fromZigbee = [
        {
            cluster: "genScenes",
            type: ["commandRecall", "commandStore", "commandAdd", "commandRemove", "commandRemoveAll"],
            convert: (model, msg, publish, options, meta) => {
                if ((0, utils_1.hasAlreadyProcessedMessage)(msg, model))
                    return;
                let trailing = "";
                if (msg.type === "commandRecall" || msg.type === "commandStore") {
                    (0, node_assert_1.default)("sceneid" in msg.data);
                    trailing = `_${msg.data.sceneid}`;
                }
                const payload = {
                    action: (0, utils_1.postfixWithEndpointName)(actionPayloadLookup[msg.type] + trailing, msg, model, meta),
                };
                (0, utils_1.addActionGroup)(payload, msg, model);
                return payload;
            },
        },
    ];
    const result = { exposes: exposesArray, fromZigbee, isModernExtend: true };
    if (bind)
        result.configure = [setupConfigureForBinding("genScenes", "output", endpointNames)];
    return result;
}
function enumLookup(args) {
    const { name, lookup, cluster, attribute, description, zigbeeCommandOptions, endpointName, reporting, entityCategory, label } = args;
    const attributeKey = (0, utils_1.isString)(attribute) ? attribute : attribute.ID;
    const access = exposes_1.access[args.access ?? "ALL"];
    let expose = exposes_1.presets.enum(name, access, Object.keys(lookup)).withDescription(description);
    if (endpointName)
        expose = expose.withEndpoint(endpointName);
    if (entityCategory)
        expose = expose.withCategory(entityCategory);
    if (label !== undefined)
        expose = expose.withLabel(label);
    const fromZigbee = [
        {
            cluster: cluster.toString(),
            type: ["attributeReport", "readResponse"],
            convert: (model, msg, publish, options, meta) => {
                if (attributeKey in msg.data && (!endpointName || (0, utils_1.getEndpointName)(msg, model, meta) === endpointName)) {
                    // skip undefined value
                    if (msg.data[attributeKey] !== undefined)
                        return { [expose.property]: (0, utils_1.getFromLookupByValue)(msg.data[attributeKey], lookup) };
                }
            },
            // biome-ignore lint/suspicious/noExplicitAny: generic
        },
    ];
    const toZigbee = [
        {
            key: [name],
            convertSet: access & exposes_1.access.SET
                ? async (entity, key, value, meta) => {
                    const payloadValue = (0, utils_1.getFromLookup)(value, lookup);
                    const payload = (0, utils_1.isString)(attribute)
                        ? { [attribute]: payloadValue }
                        : { [attribute.ID]: { value: payloadValue, type: attribute.type } };
                    await (0, utils_1.determineEndpoint)(entity, meta, cluster).write(cluster, 
                    // XXX: too dynamic for typing
                    payload, zigbeeCommandOptions);
                    return { state: { [key]: value } };
                }
                : undefined,
            convertGet: access & exposes_1.access.GET
                ? async (entity, key, meta) => {
                    await (0, utils_1.determineEndpoint)(entity, meta, cluster).read(cluster, 
                    // XXX: too dynamic for typing
                    [attributeKey], zigbeeCommandOptions);
                }
                : undefined,
        },
    ];
    const configure = [setupConfigureForReporting(cluster, attribute, { config: reporting, access })];
    return { exposes: [expose], fromZigbee, toZigbee, configure, isModernExtend: true };
}
function numeric(args) {
    const { name, cluster, attribute, description, zigbeeCommandOptions, unit, reporting, valueMin, valueMax, valueStep, scale, label, entityCategory, precision, fzConvert, } = args;
    const endpoints = args.endpointNames;
    const attributeKey = (0, utils_1.isString)(attribute) ? attribute : attribute.ID;
    const access = exposes_1.access[args.access ?? "ALL"];
    const exposes = [];
    const createExpose = (endpoint) => {
        let expose = exposes_1.presets.numeric(name, access).withDescription(description);
        if (endpoint)
            expose = expose.withEndpoint(endpoint);
        if (unit)
            expose = expose.withUnit(unit);
        if (valueMin !== undefined)
            expose = expose.withValueMin(valueMin);
        if (valueMax !== undefined)
            expose = expose.withValueMax(valueMax);
        if (valueStep !== undefined)
            expose = expose.withValueStep(valueStep);
        if (label !== undefined)
            expose = expose.withLabel(label);
        if (entityCategory)
            expose = expose.withCategory(entityCategory);
        return expose;
    };
    // Generate for multiple endpoints only if required.
    if (!endpoints) {
        exposes.push(createExpose(undefined));
    }
    else {
        for (const endpoint of endpoints) {
            exposes.push(createExpose(endpoint));
        }
    }
    const fromZigbee = [
        {
            cluster: cluster.toString(),
            type: ["attributeReport", "readResponse"],
            convert: fzConvert ??
                ((model, msg, publish, options, meta) => {
                    if (attributeKey in msg.data) {
                        const endpoint = endpoints?.find((e) => (0, utils_1.getEndpointName)(msg, model, meta) === e);
                        if (endpoints && !endpoint) {
                            return;
                        }
                        let value = msg.data[attributeKey];
                        (0, utils_1.assertNumber)(value);
                        if (scale !== undefined) {
                            value = typeof scale === "number" ? value / scale : scale(value, "from");
                        }
                        (0, utils_1.assertNumber)(value);
                        if (precision != null)
                            value = (0, utils_1.precisionRound)(value, precision);
                        const expose = exposes.length === 1 ? exposes[0] : exposes.find((e) => e.endpoint === endpoint);
                        return { [expose.property]: value };
                    }
                }),
            // biome-ignore lint/suspicious/noExplicitAny: generic
        },
    ];
    const toZigbee = [
        {
            key: [name],
            convertSet: access & exposes_1.access.SET
                ? async (entity, key, value, meta) => {
                    (0, utils_1.assertNumber)(value, key);
                    let payloadValue = value;
                    if (scale !== undefined) {
                        payloadValue = typeof scale === "number" ? payloadValue * scale : scale(payloadValue, "to");
                    }
                    (0, utils_1.assertNumber)(payloadValue);
                    if (precision != null)
                        payloadValue = (0, utils_1.precisionRound)(payloadValue, precision);
                    const payload = (0, utils_1.isString)(attribute)
                        ? { [attribute]: payloadValue }
                        : { [attribute.ID]: { value: payloadValue, type: attribute.type } };
                    await (0, utils_1.determineEndpoint)(entity, meta, cluster).write(cluster, 
                    // XXX: too dynamic for typing
                    payload, zigbeeCommandOptions);
                    return { state: { [key]: value } };
                }
                : undefined,
            convertGet: access & exposes_1.access.GET
                ? async (entity, key, meta) => {
                    await (0, utils_1.determineEndpoint)(entity, meta, cluster).read(cluster, 
                    // XXX: too dynamic for typing
                    [attributeKey], zigbeeCommandOptions);
                }
                : undefined,
        },
    ];
    const configure = [setupConfigureForReporting(cluster, attribute, { config: reporting, access, endpointNames: endpoints })];
    return { exposes, fromZigbee, toZigbee, configure, isModernExtend: true };
}
function binary(args) {
    const { name, valueOn, valueOff, cluster, attribute, description, zigbeeCommandOptions, endpointName, reporting, label, entityCategory } = args;
    const attributeKey = (0, utils_1.isString)(attribute) ? attribute : attribute.ID;
    const access = exposes_1.access[args.access ?? "ALL"];
    let expose = exposes_1.presets.binary(name, access, valueOn[0], valueOff[0]).withDescription(description);
    if (endpointName)
        expose = expose.withEndpoint(endpointName);
    if (label)
        expose = expose.withLabel(label);
    if (entityCategory)
        expose = expose.withCategory(entityCategory);
    const fromZigbee = [
        {
            cluster: cluster.toString(),
            type: ["attributeReport", "readResponse"],
            convert: (model, msg, publish, options, meta) => {
                if (attributeKey in msg.data && (!endpointName || (0, utils_1.getEndpointName)(msg, model, meta) === endpointName)) {
                    return { [expose.property]: msg.data[attributeKey] === valueOn[1] ? valueOn[0] : valueOff[0] };
                }
            },
            // biome-ignore lint/suspicious/noExplicitAny: generic
        },
    ];
    const toZigbee = [
        {
            key: [name],
            convertSet: access & exposes_1.access.SET
                ? async (entity, key, value, meta) => {
                    const payloadValue = value === valueOn[0] ? valueOn[1] : valueOff[1];
                    const payload = (0, utils_1.isString)(attribute)
                        ? { [attribute]: payloadValue }
                        : { [attribute.ID]: { value: payloadValue, type: attribute.type } };
                    await (0, utils_1.determineEndpoint)(entity, meta, cluster).write(cluster, 
                    // XXX: too dynamic for typing
                    payload, zigbeeCommandOptions);
                    return { state: { [key]: value } };
                }
                : undefined,
            convertGet: access & exposes_1.access.GET
                ? async (entity, key, meta) => {
                    await (0, utils_1.determineEndpoint)(entity, meta, cluster).read(cluster, 
                    // XXX: too dynamic for typing
                    [attributeKey], zigbeeCommandOptions);
                }
                : undefined,
        },
    ];
    const configure = [setupConfigureForReporting(cluster, attribute, { config: reporting, access })];
    return { exposes: [expose], fromZigbee, toZigbee, configure, isModernExtend: true };
}
function text(args) {
    const { name, cluster, attribute, description, zigbeeCommandOptions, endpointName, reporting, entityCategory, validate } = args;
    const attributeKey = (0, utils_1.isString)(attribute) ? attribute : attribute.ID;
    const access = exposes_1.access[args.access ?? "ALL"];
    let expose = exposes_1.presets.text(name, access).withDescription(description);
    if (endpointName)
        expose = expose.withEndpoint(endpointName);
    if (entityCategory)
        expose = expose.withCategory(entityCategory);
    const fromZigbee = [
        {
            cluster: cluster.toString(),
            type: ["attributeReport", "readResponse"],
            convert: (model, msg, publish, options, meta) => {
                if (attributeKey in msg.data && (!endpointName || (0, utils_1.getEndpointName)(msg, model, meta) === endpointName)) {
                    return { [expose.property]: msg.data[attributeKey] };
                }
            },
            // biome-ignore lint/suspicious/noExplicitAny: generic
        },
    ];
    const toZigbee = [
        {
            key: [name],
            convertSet: access & exposes_1.access.SET
                ? async (entity, key, value, meta) => {
                    void validate(value);
                    const payload = (0, utils_1.isString)(attribute) ? { [attribute]: value } : { [attribute.ID]: { value, type: attribute.type } };
                    await (0, utils_1.determineEndpoint)(entity, meta, cluster).write(cluster, 
                    // XXX: too dynamic for typing
                    payload, zigbeeCommandOptions);
                    return { state: { [key]: value } };
                }
                : undefined,
            convertGet: access & exposes_1.access.GET
                ? async (entity, key, meta) => {
                    await (0, utils_1.determineEndpoint)(entity, meta, cluster).read(cluster, 
                    // XXX: too dynamic for typing
                    [attributeKey], zigbeeCommandOptions);
                }
                : undefined,
        },
    ];
    const configure = [setupConfigureForReporting(cluster, attribute, { config: reporting, access })];
    return { exposes: [expose], fromZigbee, toZigbee, configure, isModernExtend: true };
}
function actionEnumLookup(args) {
    const { actionLookup: lookup, attribute, cluster, buttonLookup, commands } = args;
    const attributeKey = (0, utils_1.isString)(attribute) ? attribute : attribute.ID;
    const parse = args.parse;
    let actions = Object.keys(lookup).flatMap((a) => (args.endpointNames ? args.endpointNames.map((e) => `${a}_${e}`) : [a]));
    // allows direct external input to be used by other extends in the same device
    if (args.extraActions)
        actions = actions.concat(args.extraActions);
    const expose = exposes_1.presets.enum("action", exposes_1.access.STATE, actions).withDescription("Triggered action (e.g. a button click)").withCategory("diagnostic");
    const fromZigbee = [
        {
            cluster: cluster.toString(),
            // XXX: typing is rather annoying here, asserted for now...
            type: (commands || ["attributeReport", "readResponse"]),
            convert: (model, msg, publish, options, meta) => {
                if (attributeKey in msg.data) {
                    const msgValue = parse ? parse(msg, attributeKey) : msg.data[attributeKey];
                    let value = (0, utils_1.getFromLookupByValue)(msgValue, lookup);
                    // endpointNames is used when action endpoint names don't overlap with other endpoint names
                    if (args.endpointNames)
                        value = (0, utils_1.postfixWithEndpointName)(value, msg, model, meta);
                    // buttonLookup is used when action endpoint names overlap with other endpoint names
                    if (args.buttonLookup) {
                        const endpointName = (0, utils_1.getFromLookupByValue)(msg.endpoint.ID, buttonLookup);
                        value = `${value}_${endpointName}`;
                    }
                    return { [expose.property]: value };
                }
            },
        },
    ];
    return { exposes: [expose], fromZigbee, isModernExtend: true };
}
function quirkAddEndpointCluster(args) {
    const { endpointID, inputClusters, outputClusters } = args;
    const configure = [
        (device, coordinatorEndpoint, definition) => {
            const endpoint = device.getEndpoint(endpointID);
            if (endpoint === undefined) {
                logger_1.logger.error(`Quirk: cannot add clusters to endpoint ${endpointID}, endpoint does not exist!`, "zhc:quirkaddendpointcluster");
                return;
            }
            inputClusters?.forEach((cluster) => {
                const clusterID = (0, utils_1.isString)(cluster) ? zigbee_herdsman_1.Zcl.Utils.getCluster(cluster, device.manufacturerID, device.customClusters).ID : cluster;
                if (!endpoint.inputClusters.includes(clusterID)) {
                    logger_1.logger.debug(`Quirk: adding input cluster ${clusterID} to endpoint ${endpointID}.`, "zhc:quirkaddendpointcluster");
                    endpoint.inputClusters.push(clusterID);
                }
            });
            outputClusters?.forEach((cluster) => {
                const clusterID = (0, utils_1.isString)(cluster) ? zigbee_herdsman_1.Zcl.Utils.getCluster(cluster, device.manufacturerID, device.customClusters).ID : cluster;
                if (!endpoint.outputClusters.includes(clusterID)) {
                    logger_1.logger.debug(`Quirk: adding output cluster ${clusterID} to endpoint ${endpointID}.`, "zhc:quirkaddendpointcluster");
                    endpoint.outputClusters.push(clusterID);
                }
            });
            device.save();
        },
    ];
    return { configure, isModernExtend: true };
}
function quirkCheckinInterval(timeout) {
    const configure = [
        (device, coordinatorEndpoint, definition) => {
            device.checkinInterval = typeof timeout === "number" ? timeout : exports.TIME_LOOKUP[timeout];
            device.save();
        },
    ];
    return { configure, isModernExtend: true };
}
function reconfigureReportingsOnDeviceAnnounce() {
    const onEvent = [
        async (event) => {
            if (event.type === "deviceAnnounce") {
                for (const endpoint of event.data.device.endpoints) {
                    for (const c of endpoint.configuredReportings) {
                        await endpoint.configureReporting(c.cluster.name, [
                            {
                                // @ts-expect-error dynamic, expected valid since already applied
                                attribute: c.attribute.name,
                                minimumReportInterval: c.minimumReportInterval,
                                maximumReportInterval: c.maximumReportInterval,
                                reportableChange: c.reportableChange,
                            },
                        ]);
                    }
                }
            }
        },
    ];
    return { onEvent, isModernExtend: true };
}
function skipDefaultResponse() {
    const onEvent = [
        (event) => {
            if (event.type === "start") {
                event.data.device.skipDefaultResponse = true;
            }
        },
    ];
    return { onEvent, isModernExtend: true };
}
function deviceEndpoints(args) {
    const result = {
        meta: { multiEndpoint: true },
        endpoint: (d) => args.endpoints,
        isModernExtend: true,
    };
    if (args.multiEndpointSkip)
        result.meta.multiEndpointSkip = args.multiEndpointSkip;
    return result;
}
function deviceAddCustomCluster(clusterName, clusterDefinition) {
    const addCluster = (device) => {
        if (!device.customClusters[clusterName]) {
            device.addCustomCluster(clusterName, clusterDefinition);
        }
    };
    const onEvent = [(event) => event.type === "start" && addCluster(event.data.device)];
    const configure = [async (device) => addCluster(device)];
    return { onEvent, configure, isModernExtend: true };
}
function ignoreClusterReport(args) {
    const fromZigbee = [
        {
            cluster: args.cluster.toString(),
            type: ["attributeReport", "readResponse"],
            convert: (model, msg, publish, options, meta) => { },
        },
    ];
    return { fromZigbee, isModernExtend: true };
}
function bindCluster(args) {
    const configure = [setupConfigureForBinding(args.cluster, args.clusterType, args.endpointNames)];
    return { configure, isModernExtend: true };
}
const SETPOINT_LOOKUP = {
    occupiedHeatingSetpoint: { property: "occupied_heating_setpoint", tzConverter: tz.thermostat_occupied_heating_setpoint },
    unoccupiedHeatingSetpoint: { property: "unoccupied_heating_setpoint", tzConverter: tz.thermostat_unoccupied_heating_setpoint },
    occupiedCoolingSetpoint: { property: "occupied_cooling_setpoint", tzConverter: tz.thermostat_occupied_cooling_setpoint },
    unoccupiedCoolingSetpoint: { property: "unoccupied_cooling_setpoint", tzConverter: tz.thermostat_unoccupied_cooling_setpoint },
};
const SETPOINT_LIMIT_LOOKUP = {
    maxHeatSetpointLimit: { tzConverter: tz.thermostat_max_heat_setpoint_limit, expose: exposes_1.presets.max_heat_setpoint_limit },
    minHeatSetpointLimit: { tzConverter: tz.thermostat_min_heat_setpoint_limit, expose: exposes_1.presets.min_heat_setpoint_limit },
    maxCoolSetpointLimit: { tzConverter: tz.thermostat_max_cool_setpoint_limit, expose: exposes_1.presets.max_cool_setpoint_limit },
    minCoolSetpointLimit: { tzConverter: tz.thermostat_min_cool_setpoint_limit, expose: exposes_1.presets.min_cool_setpoint_limit },
};
function thermostat(args = {}) {
    const { localTemperature = undefined, localTemperatureCalibration = undefined, setpoints = undefined, setpointsLimit = {}, systemMode = undefined, runningState = undefined, runningMode = undefined, fanMode = undefined, piHeatingDemand = undefined, temperatureSetpointHold = false, temperatureSetpointHoldDuration = false, } = args;
    const repConfigChange0 = { min: "MIN", max: "1_HOUR", change: 0 };
    const repConfigChange10 = { min: "MIN", max: "1_HOUR", change: 10 };
    const exposes = [];
    const fromZigbee = [];
    const toZigbee = [];
    const configure = [];
    const expose = exposes_1.presets.climate().withLocalTemperature(undefined, localTemperature?.values?.description ?? undefined);
    exposes.push(expose);
    if (!localTemperature?.fromZigbee?.skip) {
        fromZigbee.push(fz.thermostat);
    }
    if (!localTemperature?.toZigbee?.skip) {
        toZigbee.push(tz.thermostat_local_temperature);
    }
    if (!localTemperature?.configure?.skip) {
        configure.push(setupConfigureForBinding("hvacThermostat", "input"), setupConfigureForReporting("hvacThermostat", "localTemp", {
            config: localTemperature?.configure?.reporting ?? repConfigChange10,
            access: localTemperature?.configure?.access ?? exposes_1.access.STATE_GET,
        }));
    }
    if (localTemperatureCalibration) {
        const { min, max, step } = localTemperatureCalibration.values === true ? { min: -12.8, max: 12.8, step: 0.1 } : localTemperatureCalibration.values;
        expose.withLocalTemperatureCalibration(min, max, step);
        if (!localTemperatureCalibration.toZigbee?.skip) {
            toZigbee.push(tz.thermostat_local_temperature_calibration);
        }
        if (!localTemperatureCalibration.configure?.skip) {
            configure.push(setupConfigureForReporting("hvacThermostat", "localTemperatureCalibration", {
                config: localTemperatureCalibration.configure?.reporting ?? false,
                access: localTemperatureCalibration.configure?.access ?? exposes_1.access.STATE_GET,
            }));
        }
    }
    for (const key of Object.keys(setpoints.values)) {
        const { min, max, step } = setpoints.values[key];
        const { property, tzConverter } = SETPOINT_LOOKUP[key];
        expose.withSetpoint(property, min, max, step);
        if (!setpoints.toZigbee?.skip) {
            toZigbee.push(tzConverter);
        }
        if (!setpoints.configure?.skip) {
            configure.push(setupConfigureForReporting("hvacThermostat", key, {
                config: setpoints.configure?.reporting ?? repConfigChange10,
                access: setpoints.configure?.access ?? exposes_1.access.STATE_GET,
            }));
        }
    }
    if (systemMode) {
        expose.withSystemMode(systemMode.values);
        if (!systemMode.toZigbee?.skip) {
            toZigbee.push(tz.thermostat_system_mode);
        }
        if (!systemMode.configure?.skip) {
            configure.push(setupConfigureForReporting("hvacThermostat", "systemMode", {
                config: systemMode.configure?.reporting ?? repConfigChange0,
                access: systemMode.configure?.access ?? exposes_1.access.STATE_GET,
            }));
        }
    }
    if (runningState) {
        expose.withRunningState(runningState.values);
        if (!runningState.toZigbee?.skip) {
            toZigbee.push(tz.thermostat_running_state);
        }
        if (!runningState.configure?.skip) {
            configure.push(setupConfigureForReporting("hvacThermostat", "runningState", {
                config: runningState.configure?.reporting ?? repConfigChange0,
                access: runningState.configure?.access ?? exposes_1.access.STATE_GET,
            }));
        }
    }
    if (runningMode) {
        expose.withRunningMode(runningMode);
        toZigbee.push(tz.thermostat_running_mode);
        configure.push(setupConfigureForReporting("hvacThermostat", "runningMode", { config: repConfigChange0, access: exposes_1.access.STATE_GET }));
    }
    if (fanMode) {
        expose.withFanMode(fanMode);
        toZigbee.push(tz.fan_mode);
        configure.push(setupConfigureForBinding("hvacFanCtrl", "input"), setupConfigureForReporting("hvacFanCtrl", "fanMode", { config: repConfigChange0, access: exposes_1.access.STATE_GET }));
    }
    if (piHeatingDemand) {
        expose.withPiHeatingDemand(piHeatingDemand.values !== true ? piHeatingDemand.values : undefined);
        if (!piHeatingDemand.toZigbee?.skip) {
            toZigbee.push(tz.thermostat_pi_heating_demand);
        }
        if (!piHeatingDemand.configure?.skip) {
            configure.push(setupConfigureForReporting("hvacThermostat", "pIHeatingDemand", {
                config: piHeatingDemand.configure?.reporting ?? repConfigChange0,
                access: piHeatingDemand.configure?.access ?? exposes_1.access.STATE_GET,
            }));
        }
    }
    if (temperatureSetpointHold) {
        exposes.push(exposes_1.presets
            .binary("temperature_setpoint_hold", exposes_1.access.ALL, true, false)
            .withDescription("Prevent changes. `false` = run normally. `true` = prevent from making changes."));
        toZigbee.push(tz.thermostat_temperature_setpoint_hold);
        configure.push(setupConfigureForReporting("hvacThermostat", "tempSetpointHold", { config: repConfigChange0, access: exposes_1.access.STATE_GET }));
    }
    if (temperatureSetpointHoldDuration) {
        exposes.push(exposes_1.presets
            .numeric("temperature_setpoint_hold_duration", exposes_1.access.ALL)
            .withValueMin(0)
            .withValueMax(65535)
            .withDescription("Period in minutes for which the setpoint hold will be active (65535 - forever)"));
        toZigbee.push(tz.thermostat_temperature_setpoint_hold_duration);
        configure.push(setupConfigureForReading("hvacThermostat", ["tempSetpointHoldDuration"]));
    }
    for (const key of Object.keys(setpointsLimit)) {
        const { min, max, step } = setpointsLimit[key];
        const { expose, tzConverter } = SETPOINT_LIMIT_LOOKUP[key];
        exposes.push(expose(min, max, step));
        toZigbee.push(tzConverter);
        configure.push(setupConfigureForReporting("hvacThermostat", key, { config: repConfigChange0, access: exposes_1.access.STATE_GET }));
    }
    return { exposes, fromZigbee, toZigbee, configure, isModernExtend: true };
}
// #endregion
//# sourceMappingURL=modernExtend.js.map