# inkscape-layers

Splits Inkscape SVG layers into individual SVG documents

## Usage

    Usage: inkscape-layers input_file [-o output_directory] [-n name_mode | --name-label | --name-id | --name-index ]

    name_mode may be one of the following:
      label (--name-label) Default. Uses the user-defined layer name from Inkscape
      id    (--name-id)    Uses the layer's id attribute
      index (--name-index) Uses the layer's position in the svg file

Examines `input_svg` for each Inkscape layer and exports them to the same directory as `input_svg` with "-" and the layer's name appended to the end before the file extension.

If `output_directory` is specified, the files will be placed in that directory.

For example, `inkscape-layers test.svg` will produce `test-1.svg`, `test-2.svg`, `test-3.svg` because the included `test.svg` has three layers, named "1", "2", and "3".

## Building

`make`

**libxml++** is required.

## Warning

This program was not tested on SVGs with unnamed layers or layers with duplicate names.
