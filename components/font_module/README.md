# ESP32-S3 Bare-Metal Graphics Engine: Font Dictionary (`font_module.h`)

## Overview
This header file acts as the Read-Only Memory (ROM) for the custom graphics engine. It contains `ascii_8x8_font`, a 2D array that mathematically defines the visual representation of 95 standard ASCII characters (from Space ` ` to Tilde `~`). 

## Memory Architecture & Efficiency
By declaring this array as `const`, the C compiler is instructed to place this data into the ESP32-S3's read-only flash memory (`.rodata` section) rather than consuming precious internal SRAM. 

The array dimensions are `[95][8]`:
* **95 Rows:** Represents the total number of printable ASCII characters supported.
* **8 Columns (Bytes):** Represents the 8 horizontal rows of pixels that make up a single character.

## How It Works: The Bitmap Matrix



This font does not use complex vector graphics. It uses pure binary bitmapping. Each character is contained within an 8x8 pixel grid. 

* **The Index Offset:** In standard ASCII, the first printable character (Space) is decimal `32`. To save memory, this array starts the Space character at index `0`. Therefore, to look up any character, the graphics engine simply subtracts 32 from its ASCII value: `ascii_8x8_font[ *(user_text) - 32 ]`.
* **The Binary Translation:** Each hexadecimal value in the array translates to an 8-bit binary number. 
  * A `1` bit represents a drawn pixel (Foreground color).
  * A `0` bit represents an empty pixel (Background color).

**Example: The Letter 'A' (ASCII 65)**
Offset Index: 65 - 32 = `33`.
The array at index `33` contains: `{0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00}`

If you convert those hex values to binary, you can physically see the letter 'A' in the code:
* `0x18` -> `00011000`
* `0x3C` -> `00111100`
* `0x66` -> `01100110`
* `0x66` -> `01100110`
* `0x7E` -> `01111110` (The crossbar)
* `0x66` -> `01100110`
* `0x66` -> `01100110`
* `0x00` -> `00000000` (Spacing buffer)