#!/usr/bin/env python

# Copyright (c) 2025 Daniel Kampert
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import subprocess

from west.commands import WestCommand

class FormatCodeWestCommand(WestCommand):
    def __init__(self):
        super().__init__(
            "format",
            "Format the codebase",
            """Use to format all .c and .h files""",
        )

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name, help = self.help, description=self.description
        )

        parser.add_argument(
            "--dry-run",
            action="store_true",
            help="Dry run won't actually format the code.",
        )

        return parser

    def do_run(self, args, _unknown_args):
        astyle_args = ["astyle"]

        if(args.dry_run):
            astyle_args.append("--dry-run")
        astyle_args.extend(
            [
                "--options={}/../app/misc/astyle.cfg".format(os.environ["ZEPHYR_BASE"]),
                "--recursive",
                "{}/../app/src/*.c,*.h".format(os.environ["ZEPHYR_BASE"]),
            ]
        )

        with subprocess.Popen(
            astyle_args,
            stdout = subprocess.PIPE,
            stderr = subprocess.PIPE,
            universal_newlines = True,
        ) as astyle:
            fail = False
            output = astyle.communicate()
            output = output[0].split("\n")
            for line in output:
                if line.startswith("Formatted"):
                    fail = True
                print(line, end = "\n")
            if astyle.returncode != 0:
                fail = True
            sys.exit(1 if fail else 0)
