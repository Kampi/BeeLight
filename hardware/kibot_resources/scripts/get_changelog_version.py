import re
import sys
import argparse

def get_last_version(file_path):
    try:
        with open(file_path, "r") as file:
            content = file.read()

        # Regex to match version entries
        version_pattern = r"## \[(.*?)\]"
        versions = re.findall(version_pattern, content)

        if(not versions):
            return "N/A"

        # Check the last entry
        last_version = versions[0]  # First match is the most recent

        if(last_version.lower() == "unreleased"):
            if(len(versions) > 1):
                second_last_version = versions[1]
                return f"{second_last_version}+ (Unreleased)"
            else:
                return "(Unreleased)"
        else:
            return last_version

    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.", file = sys.stderr)
        sys.exit(1)
    except (IOError, OSError) as e:
        print(f"Error: An error occurred: {e}", file = sys.stderr)
        sys.exit(1)

if(__name__ == "__main__"):
    parser = argparse.ArgumentParser(description = "Get the latest version from a changelog file.")
    parser.add_argument("-f", "--file", required = True, help = "Path to the changelog file.")
    args = parser.parse_args()

    version = get_last_version(args.file)
    print(version)
