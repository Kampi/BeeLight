import * as m from "zigbee-herdsman-converters/lib/modernExtend";

export default {
    zigbeeModel: ["BeeLight_v2"],
    model: "BeeLight_v2",
    vendor: "Kampi",
    description: "Zigbee-based Light and Environment Sensor (https://github.com/Kampi/BeeLight)",
    extend: [m.BeeLight_iaq(), m.BeeLight_co2(), m.BeeLight_voc(), m.battery(), m.illuminance(), m.temperature(), m.pressure(), m.humidity()],
};
