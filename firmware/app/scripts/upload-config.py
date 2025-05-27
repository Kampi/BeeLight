import os
import pylink

from west import log
from west.commands import WestCommand
from pathlib import Path

THIS_ZEPHYR = Path(__file__).parent.parent.parent / "zephyr"
ZEPHYR_BASE = Path(os.environ.get("ZEPHYR_BASE", THIS_ZEPHYR))

class UploadConfigWestCommand(WestCommand):
    def __init__(self):
        super().__init__(
            "upload-config",
            "Upload a prodiction config",
            """Use to upload a production config.""",
        )

        log.inf("Zephyr Base", ZEPHYR_BASE)