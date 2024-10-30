# Kimg converter
This is a toolkit for converting Image's to my own custom image format Kimg. and from Kimg to PNG


## Structure
**Files:**
- the `create/` folder is the source code for making Kimg's from other images(most generic formats, eg. PNG, JPG, BMP, and TGA).
- the `render/` folder is the source code for turn Kimg's back to PNG's(only PNG supported for now)

**Kimg File Format:**
- 6 byte header for width and height, each dimension can be 3 bytes for near infinite scalability, this allows images up to `16,777,215 x 16,777,215`.
- the following bytes after the header allow for lossless quality. The following bytes consist of sets of 3 bytes up until the end of the image one for red one for green and one for blue. this makes non-RGB images impossible, sorry.
