# BeeLight

## Table of Contents

- [ZigBee light sensor](#zigbee-light-sensor)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Directory structure](#directory-structure)
  - [Purchasing](#purchasing)
  - [Maintainer](#maintainer)

## About

ZigBee based light sensor with [ZigBee2MQTT](https://www.zigbee2mqtt.io/]´) support for Home Assistant etc.

![PCB Top side](/docs/images/Image_Complete.png)

## Install the device to ZigBee2MQTT

Enable the external converter by adding the following to your Zigbee2MQTT `configuration.yaml`:

```yaml
external_converters:
  - lightSensor.js
```
More info: https://www.zigbee2mqtt.io/advanced/support-new-devices/01_support_new_devices.html

## Directory structure

- `cad`: 3D model of the complete PCB
- `docs`: All kinds of project documentation like schematics, BOM, etc.
  - `drawings`: 2D drawings for subcomponents, etc.
  - `images`
- `project`: KiCad project for the PCB

## Purchasing

## Maintainer

- [Daniel Kampert](mailto:daniel.kameprt@kampis-elektroecke.de)
