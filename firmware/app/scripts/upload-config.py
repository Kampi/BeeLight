#!/usr/bin/env python

# Copyright (c) 2025 Daniel Kampert
# SPDX-License-Identifier: Apache-2.0

import os
import fileinput
import subprocess

from pathlib import Path
from west.commands import WestCommand

class UploadConfigWestCommand(WestCommand):
    def __init__(self):
        super().__init__(
            "upload-config",
            "Upload a production config",
            """Upload the production config to a sensor.""",
        )

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name, help = self.help, description = self.description
        )

        parser.add_argument(
            "--path",
            type = str,
            default = str(Path(__file__).parent.parent.parent) + os.path.sep + "app",
            help = "Path for the input and output configuration",
        )

        parser.add_argument(
            "--input",
            type = str,
            default = "zigbee_config.yml",
            help = "Input configuration file",
        )

        parser.add_argument(
            "--output",
            type = str,
            default = "zigbee_config.hex",
            help = "Compiled configuration file",
        )

        parser.add_argument(
            "--offset",
            type = int,
            default = 0x17A000,
            help = "Flash offset for the configuration file (taken from PM_ZBOSS_NVRAM_END_ADDRESS)",
        )

        parser.add_argument(
            "--address",
            type = str,
            default = "",
            help = "Zigbee device address. Set to '' to use a random address (Default)",
        )

        return parser

    def do_run(self, args, unknown_args):
        # Use a random adress if the length is invalid
        if(len(args.address) < 16):
            address = os.urandom(8).hex().upper()
        else:
            address = args.address

        for line in fileinput.input(args.path + os.path.sep + args.input, inplace = True):
            if("extended_address" in line):
                print("extended_address: {}".format(address), end = "\n")
            else:
                print(line, end = "")

        subprocess.run("nrfutil nrf5sdk-tools zigbee production_config {} {} --offset {}".format(args.path + os.path.sep + args.input, args.path + os.path.sep + args.output, args.offset), check = True)
        subprocess.run("nrfjprog --program {} --verify".format(args.path + os.path.sep + args.output), check = True)
