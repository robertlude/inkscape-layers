# inkscape-layers

Splits Inkscape SVG layers into individual SVG documents

## Usage

    inkscape-layers v0.2.0
      by Robert Lude <rob@ertlu.de>

    Usage:
      inkscape-layers input_file options

    Options:
      -o output_directory, --output-directory output_directory
        Specifies where to put the resulting SVG files
      -n name_mode, --name-label, --name-id, --name-index
        Specifies how to name the resulting SVG files. Acceptable values for name_mode are:
          label   Default. Uses the user-defined layer name from Inkscape
          id      Uses the layer's id attribute
          index   Uses the layer's position in the original SVG file
      -v visibility_mode, --visibility-all, --visibility-visible
        Specifies whether or not to skip invisible layers, or rather which layers to isolate.
        Acceptable values for visiblity_mode are:
          visible   Default. Only exports layers which are visible
          all       Exports each layer

Examines `input_svg` for each Inkscape layer and exports them to the same directory as `input_svg` with "-" and the layer's name appended to the end before the file extension.

If `output_directory` is specified, the files will be placed in that directory.

For example, `inkscape-layers test.svg` will produce `test-1.svg`, `test-2.svg`, `test-3.svg` because the included `test.svg` has three layers, named "1", "2", and "3".

## Building

    git clone --recursive git@github.com:robertlude/inkscape-layers.git
    cd inkscape-layers
    make

### Requirements

* libxml++ must be present
* g++ with support for C++11 must be present

## Warnings

* This program was not tested on SVGs with unnamed layers or layers with duplicate names.
* This program was not tested on SVGs with sublayers.

Will these cause the command to fail? I don't know, but I thought you should be aware.
