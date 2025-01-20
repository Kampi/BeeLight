# BeeLight - ZigBee based light & environment sensor for Home Automation

## Table of Contents

- [BeeLight - ZigBee based light \& environment sensor for Home Automation](#beelight---zigbee-based-light--environment-sensor-for-home-automation)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Install the device to ZigBee2MQTT](#install-the-device-to-zigbee2mqtt)
  - [Directory structure](#directory-structure)
  - [Purchasing](#purchasing)
  - [Ressources](#ressources)
  - [Maintainer](#maintainer)

## About

Open-Source ZigBee-based light and environmental sensor with [ZigBee2MQTT](https://www.zigbee2mqtt.io/]) support for your Home Automation with I. e. Home Assistant.

![PCB Top side](/docs/images/Image_Complete.png)

![Block diagram](/docs/images/Block%20diagram.png)

## Install the device to ZigBee2MQTT

Enable the external converter by copying the external converter to your `ZigBee2MQTT` `data` directory.

## Directory structure

- `cad`: 3D model of the complete PCB
- `docs`: All kinds of project documentation like schematics, BOM, etc.
  - `drawings`: 2D drawings for subcomponents, etc.
  - `images`
- `project`: KiCad project for the PCB

## Purchasing

> NOTE  
> You have to provide a Mouser API key with a variable called `MOUSER_KEY` to your CI/CD pipeline to make use of the KiCost feature in the pipeline.

## Ressources

- [Adding new ZigBee2MQTT devices](https://www.zigbee2mqtt.io/advanced/support-new-devices/01_support_new_devices.html)

## Maintainer

- [Daniel Kampert](mailto:daniel.kameprt@kampis-elektroecke.de)
