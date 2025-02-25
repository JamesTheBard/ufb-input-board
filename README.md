# UFB Input Board

![](images/input_board_and_display.jpeg)
![](images/adapter_board.jpeg)

## What the Hell is This Project?

This project is pretty simple: take a Brook UFB and add support for a display and input remapping for various definitions of cheap.  Also, keep the additional lag introduced to below 100 microseconds for all modes.[^1]

The input board has 32 inputs of which 29 can be remapped to one or more outputs and supports the Brook cable harness to make things that much easier.  All of the inputs are labeled as well which should make hooking things up pretty easy.

The adapter board is responsible for pushing the UFB's buttons, and each output has an LED associated with it to assist with troubleshooting.  Even better, there's a USB-C connector on the adapter board that replaces the Type-B on the UFB.[^2]

The final bit is SD card support.  Profiles are loaded on the microSD card on the input board.

## Profiles

To configure profiles, create the `profiles.json` file in the root of the SD card.

### File Structure

```json
{
    "profiles": [
        {
            "name": "Symphony of the Night (PSX)",
            "mappings": [
                [ 1, [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18]],
                [ 2, [1, 2, 3, 4, 5, 6, 7, 8]],
                [14, [9, 10, 11, 12, 13, 14, 15, 16]]
            ]
        },
        {
            "name": "Cool Profile 2",
            "display": 1,
            "mappings": [
                [12, [1, 2]]
            ]
        }
    ]
}
```

### Breakdown

Profiles control how the 29 inputs are mapped to the 17 outputs.  The default profile is always `Passthrough (1:1)` which is associated with profile 1.  Any profiles defined in the `profiles.json` file will show up in slot 2 and above in the order they appear in the `profiles.json` file.

The passthrough mode will have the lowest lag (around 70 microseconds worst-case scenario) and will pass the inputs directly to the Brook UFB.

A user-defined profile will use what's called the mapping mode.  This does add an additional stage of processing to the inputs which will increase lag, but not by a ton.  The more mappings added, the more time to process and the higher the lag.  However, the lag for the entire mapping processing stage shouldn't exceed 100 microseconds in the absolute worst-case scenario.

### Lag

| Mode | Absolute Worst | Typical Worst | Minimum |
|:-:|:-:|:-:|:-:|
| `passthrough` | 63.6 μsec | 47.1 μsec | 31.8 μsec |
| `mapping` | 77.6 μsec* | 54.1 μsec* | 38.8 μsec |

Notes:
- **Absolute Worst** occurs when an input gets processed and after processing another input is read.  The window for this is 31.8 microseconds in passthrough mode and 38.8 microseconds in mapping mode and pretty difficult to trigger. In mapping mode, the **Absolute Worst** and **Typical Worst** can be exceeded if a ton of button mappings are defined, but still should remain under 100 and 85 microseconds respectively.
- **Typical Worst** occurs outside of the specific case mentioned in **Absolute Worst**.  The controller polls inputs as quickly as possible, and if there are no changes in the inputs from the previous poll it skips all processing and polls again.  The interval on this is 15.3 microseconds.
- **Minimum** is the processing interval for each mode to include reading the inputs from the four `74HC165`s and writing the outputs to the three `74HC595`s.
- The stated lag does not include any lag associated with the Brook Universal Fighting Board.

### Name

The `name` field is what the display will show when you select the profile.  It's just a string that contains the name of the profile.  If you _don't_ set this, then you'll see the default name of _Unnamed Profile_.

### Mappings

The `mappings` is pretty straightforward.  The first value is the input number on the input board.  There are 29 available inputs, and the first 18 have defaults associated with them.  However, those are just suggestions and really only apply to passthrough mode (Profile 1).  You can map and input to any number of outputs (including none).

The second is an array of output values.  The input will get matched to any output that is in that array.  For the first profile, the `L2` button gets mapped to every output so if you press `L2` then all of the outputs get activated.

Remember, there are 29 inputs available that can be reassigned, not just the first 18 "default" ones.

### Display

The `display` changes how the outputs are display on the screen.  The default is a standard fightstick configuration.  To change it to a different layout, simply add it to the profile with the value associated with the layout below.

| Number | Description |
|:-:|:--|
| `0` | Standard fightstick configuration. |
| `1` | Generic controller layout (PS, XBox) |

### Selecting a Profile

Inputs 31 (`P-`) and 32 (`P+`) need to be connected to buttons/momentary switches.  Input 30 (`PE`) can be connected to either a latching switch or a momentary switch.  When `PE` is enabled (on) then `P-` and `P+` will cycle through the profiles.

When input 30 (`PE`) is pressed, the display will show **Unlocked** at the top-right corner of the display.

| Input | Label | Action |
|:-:|:-:|:--|
| `30` | `PE` | Enable changing the profile (unlock profile selection). |
| `31` | `P-` | Selects and activates the previous profile. |
| `32` | `P+` | Selects and activates the next profile. |

## Inputs and Outputs

| Input/Output | PS | XBox | Switch | Fightin' |
|:--:|:--:|:--:|:--:|:--:|
| `1` | L2 | LT | ZL | 4K |
| `2` | R2 | RT | ZR | 3K |
| `3` | Circle | B | A | 2K |
| `4` | Cross | A | B | 1K |
| `5` | L1 | LB | L | 4P |
| `6` | R1 | RB | R | 3P |
| `7` | Triangle | Y | X | 2P |
| `8` | Square | X | Y | 1P |
| `9` | Start | Start | + | Start |
| `10` | Home | Home | Home | Home |
| `11` | Select | Select | - | Select |
| `12` | Left | Left | Left | Left |
| `13` | Right | Right | Right | Right |
| `14` | Down | Down | Down | Down |
| `15` | Up | Up | Up | Up |
| `16` | L3 | LTSB | LTSB | L3 |
| `17` | R3 | RTSB | RTSB | R3 |
| `18` | TP Key | -- | -- | -- |

For inputs, there are 11 undefined inputs: `19` through `29` (`A1` through `A11`).  These can be mapped to any output.  More information about this can be found in the _Mapping_ section.  Inputs 30, 31, and 32 are reserved for profile selection and cannot be remapped.

## Footnotes

[^1]: This board is full of "old-school" components.  The shift registers on the input board (`SN74HC165`) started manufacture in 1982 as well as the ones on the adapter board (`SN74HC595`).

[^2]: You _have_ to use the Type-C connector.  This feeds the +5V to all of the boards and more importantly doesn't go directly to the Brook UFB...which means we can delay the Brook UFB's boot sequence until after everything is ready to go on the input and adapter boards.