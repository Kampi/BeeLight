import os

from pathlib import Path
from west.commands import WestCommand

class UploadConfigWestCommand(WestCommand):
    def __init__(self):
        super().__init__(
            "upload-config",
            "Upload a prodiction config",
            """Use to upload a production config.""",
        )

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name, help = self.help, description = self.description
        )

        parser.add_argument(
            "--path",
            type = str,
            help = "Path for the input and output configuration",
            default = str(Path(__file__).parent.parent.parent) + os.path.sep + "app",
        )

        parser.add_argument(
            "--input",
            type = str,
            help = "Input configuration file",
            default = "zigbee_config.yml",
        )

        parser.add_argument(
            "--output",
            type = str,
            help = "Compiled configuration file",
            default = "zigbee_config.hex",
        )

        parser.add_argument(
            "--offset",
            type = int,
            default = 0x17A000,
            help = "Flash offset for the configuration file (taken from PM_ZBOSS_NVRAM_END_ADDRESS)",
        )

        return parser

    def do_run(self, args, unknown_args):
        os.system("nrfutil nrf5sdk-tools zigbee production_config {} {} --offset {}".format(args.path + os.path.sep + args.input, args.path + os.path.sep + args.output, args.offset))
        os.system("nrfjprog --program {} --verify".format(args.path + os.path.sep + args.output))