# UFB Input Board

![](images/adapter_1.4.JPG)
![](images/input_1.0.JPG)

## What the Hell is This Project?

This project is pretty simple: take a Brook UFB and add support for a display and input remapping for various definitions of cheap.  Also, keep the additional lag introduced to below 100 microseconds for all modes.[^1]

The input board has 32 inputs of which 29 can be remapped to one or more outputs and supports the Brook cable harness to make things that much easier.  All of the inputs are labeled as well which should make hooking things up pretty easy.

The adapter board is responsible for pushing the UFB's buttons, and each output has an LED associated with it to assist with troubleshooting.  Even better, there's a USB-C connector on the adapter board that replaces the Type-B on the UFB.[^2]

The final bit is SD card support.  Profiles are loaded on the microSD card on the input board.

## Profiles

To configure profiles, default layout, and the display address, create the `profiles.json` file in the root of the SD card.

### File Structure

```json
{
    "display": {
        "address": "0x3C",
        "default_layout": 0,
        "type": "SSD1306",
        "resolution": "128x64"
    },
    "profiles": [
        {
            "name": "Symphony of the Night (PSX)",
            "layout": 2,
            "mappings": [
                [ 1, [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18]],
                [ 2, [1, 2, 3, 4, 5, 6, 7, 8]],
                [14, [9, 10, 11, 12, 13, 14, 15, 16]]
            ]
        },
        {
            "name": "Hitbox Profile",
            "layout": 1,
            "mappings": [
                [12, [1, 2]]
            ]
        }
    ]
}
```

## Display

## Display Type

The firmware supports a few different I2C displays and resolutions.  For 128x64 pixel OLED displays, common sizes of available displays are 0.96", 1.3", and 2.42".  For 128x32 pixel OLED displays, the only currently supported IC is the `SSD1306`.  Currently supported ICs for 128x64 pixel OLED displays are:

- `SSD1306` (2.42", 0.98" OLED displays)
- `SH1106` (1.3" OLED displays)
- `CH1116` (1.53" OLED displays)

The default display type is the `SSD1306` and resolution is `128x64`, but if you happen to have an SH1106 display, you can set that in the config file.

```json
"display": {
    "type": "SH1106"
}
```

```json
"display": {
    "resolution": "128x32",
    "type": "SSD1306"
}
```

## Display Address

The display address is set in the configuration file, but defaults to `0x3C` if it's not defined.  If you want to disable the display, you can set an address of `0x00` which will prevent the screen from turning on.  The value of "address" needs to be a string, and it needs to be hexadecimal otherwise your display will not work.  In the file above, the address `0x3C` is used.

```json
"display": {
    "address": "0x3C"
}
```

## Default Layout

The default layout sets the default way that outputs are shown on the display.  In the example above, the default layout is configured to be a hitbox/stickless configuration.  This can be overriden on a profile-by-profile basis.  See the layout section for more information.

```json
"display": {
    "default_layout": 0
}
```

## Profiles

Profiles control how the 29 inputs are mapped to the 18 outputs.  The default profile is always `Passthrough (1:1)` which is associated with profile 1.  Any profiles defined in the `profiles.json` file will show up in slot 2 and above in the order they appear in the `profiles.json` file.

The passthrough mode will have the lowest lag and will pass the inputs directly to the Brook UFB.

A user-defined profile will use what's called the mapping mode.  This does add an additional stage of processing to the inputs which will increase lag, but not by a ton.  The more mappings added, the more time to process and the higher the lag.  However, the lag for the entire mapping processing stage shouldn't exceed 100 microseconds in the absolute worst-case scenario.

### Lag

| Mode | Absolute Worst | Typical Worst | Minimum | Polling Interval | Processing Interval |
|:-:|:-:|:-:|:-:|:-:|:-:|
| `passthrough` | 85.2 μsec | 56.8 μsec | 42.6 μsec | 14.2 μsec | 42.6 μsec |
| `mapping` | 86.8 μsec* | 57.6 μsec* | 43.4 μsec | 14.2 μsec | 43.4 μsec |

Notes:
- **Absolute Worst** occurs when an input gets processed and immediately after the processing interval another input is read.  This leads to a lag of two times the processing interval.
- **Typical Worst** occurs when there wasn't an input being processed in the previous interval and it reads an input.  This leads to a lag of the polling interval plus the processing interval.
- **Minimum** is the processing interval for each mode to include reading the inputs from the four `74HC165`s and writing the outputs to the three `74HC595`s.  The lag for this should be the processing interval.
- **Polling Interval** is how long it takes to read all of the inputs.
- **Processing Interval** is how long it takes to read all the inputs, detect a change, process the inputs, and write all of the outputs.
- The stated lag does not include any lag associated with the Brook Universal Fighting Board.
- If the _Profile Enable_ button is held down, this adds 5 to 10 microseconds of lag across all modes.

### Name

The `name` field is what the display will show when you select the profile.  It's just a string that contains the name of the profile.  If you _don't_ set this, then you'll see the default name of _Unnamed Profile_.

### Mappings

The `mappings` is pretty straightforward.  The first value is the input number on the input board.  There are 29 available inputs, and the first 18 have defaults associated with them.  However, those are just suggestions and really only apply to passthrough mode (Profile 1).  You can map and input to any number of outputs (including none).

The second is an array of output values.  The input will get matched to any output that is in that array.  For the first profile, the `L2` button gets mapped to every output so if you press `L2` then all of the outputs get activated.

```json
"profiles": [
    {
        "name": "Symphony of the Night (PSX)",
        "layout": 2,
        "mappings": [
            [ 1, [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18]],
            [ 2, [1, 2, 3, 4, 5, 6, 7, 8]],
            [14, [9, 10, 11, 12, 13, 14, 15, 16]]
        ]
    },
```

In the example above, in the "Symphony of the Night (PSX)" profile there are three button maps defined: the first is associated with `L2` (input 1), the second with `R2` (input 2), and the third with `Down` (input 14).

For the `L2` input, it presses literally all of the outputs.

For the `R2` input, it presses the first eight outputs (`L2`, `R2`, `Circle`, `Cross`, `L1`, `R1`, `Triangle`, and `Square`).[^3]

For the `Down` input, it presses `Select`, `Start`, `Home`, all of the directional buttons, and `L3`.

Remember, there are 29 inputs available that can be reassigned, not just the first 18 "default" ones.

### Layout

The `layout` changes how the outputs are display on the screen.  The default is a standard fightstick configuration.  To change it to a different layout, simply add it to the profile with the value associated with the layout below.

| Number | Description |
|:-:|:--|
| `0` | Standard fightstick configuration |
| `1` | Standard hitbox configuration |
| `2` | Generic controller layout (PS, XBox) |

### Selecting a Profile

Inputs 31 (`P-`) and 32 (`P+`) need to be connected to buttons/momentary switches.  Input 30 (`PE`) can be connected to either a latching switch or a momentary switch.  When `PE` is enabled (on) then `P-` and `P+` will cycle through the profiles.

When input 30 (`PE`) is pressed, the display will show **Unlocked** at the top-right corner of the display.

| Input | Label | Action |
|:-:|:-:|:--|
| `30` | `PE` | Enable changing the profile (unlock profile selection). |
| `31` | `P-` | Selects and activates the previous profile. |
| `32` | `P+` | Selects and activates the next profile. |

## Inputs and Outputs

| Input/Output | PS | XBox | Wii U | Switch | Fightin' |
|:--:|:--:|:--:|:--:|:--:|:--:|
| `1` | L2 | LT | ZL | ZL | 4K |
| `2` | R2 | RT | ZR | ZR | 3K |
| `3` | Circle | B | A | A | 2K |
| `4` | Cross | A | B | B | 1K |
| `5` | L1 | LB | L | L | 4P |
| `6` | R1 | RB | R | R | 3P |
| `7` | Triangle | Y | X | X | 2P |
| `8` | Square | X | Y | Y | 1P |
| `9` | Start | Start | Start | `+` | Start |
| `10` | Home | Home  | -- | Home | Home |
| `11` | Select | Select | Select | `-` | Select |
| `12` | Left | Left | Left | Left | Left |
| `13` | Right | Right | Right | Right | Right |
| `14` | Down | Down | Down | Down | Down |
| `15` | Up | Up | Up | Up | Up |
| `16` | L3 | LTSB | -- |LTSB | L3 |
| `17` | R3 | RTSB | -- |RTSB | R3 |
| `18` | TP Key | -- | -- | -- | Capture |

For inputs, there are 11 undefined inputs: `19` through `29` (`A1` through `A11`).  These can be mapped to any output.  More information about this can be found in the _Mapping_ section.  Inputs 30, 31, and 32 are reserved for profile selection and cannot be remapped.

### Compatibility

While this should be compatible with everything the Brook UFB is compatible with, there are two operational modes: single cable (a USB-C cable connected to the adapter board) and dual cable mode (a USB-C cable connected to the adapter board and another USB cable connected to the input board).

#### Single Cable

1. Ensure the `VUSB` jumper on the adapter board is **enabled**.
2. Connect the USB-C cable from the input board into your device.
3. Enjoy!

#### Dual Cable

1. Ensure the `VUSB` jumper on the adapter board is **disabled**.
2. Connect the USB-C cable from the input board into your device.
3. Connect the USB cable from the adapter board into the device.  If that's not possible then use a USB hub because your setup has angered the gods.[^4]
4. Enjoy!

#### Platforms/Devices

| Platform | Cable Mode | Notes |
|:-:|:-:|:--|
| Computer | Single* | Most of the computers I've tested this with work with a single cable, however some laptops seem to really not like it and just keep rebooting the whole setup. |


### Footnotes

[^1]: This board is full of "old-school" components.  The shift registers on the input board (`SN74HC165`) started manufacture in 1982 as well as the ones on the adapter board (`SN74HC595`).

[^2]: You _have_ to use the Type-C connector.  This feeds the +5V to all of the boards and more importantly doesn't go directly to the Brook UFB...which means we can delay the Brook UFB's boot sequence until after everything is ready to go on the input and adapter boards.

[^3]: The first eight inputs are also known as `4K`, `3K`, `2K`, `1K`, `4P`, `3P`, `2P`, and `1P`.

[^4]: In this specific case, it's not you, it's definitely me.