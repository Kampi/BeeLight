# Contribute

## Table of Contents

- [Contribute](#contribute)
  - [Table of Contents](#table-of-contents)
  - [Hardware](#hardware)
  - [Maintainer](#maintainer)

## Hardware

1) Create a development branch for the changes
2) Do your work on this development branch
3) Modify `CHANGELOG.md` (unreleased section) and also the relevant text variables in `kibot_pre_set_text_variables.yaml` to prepare a release

```sh
    # Changelog
    # - variable: '@RELEASE_TITLE_VAR@1.0.0'
    #   command: '@GET_TITLE_CMD@ 1.0.0'
    # - variable: '@RELEASE_BODY_VAR@1.0.0'
    #   command: '@GET_BODY_CMD@ 1.0.0'
```

4) Make sure the text variables in the `Revision History` have the correct name too
5) Push the changes to the development branch with `CHECKED` set in the CI/CD pipeline
6) Create a PR for the changes
7) Merge the PR to main
8) Locally, you checkout main and pull the changes
9) You create a tag (e.g. `1.0.0`) and push the tag (`git push origin 1.0.0`). This will trigger the release workflow

## Maintainer

- [Daniel Kampert](mailto:daniel.kameprt@kampis-elektroecke.de)
