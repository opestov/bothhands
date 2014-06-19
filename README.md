bothhands
=========

Small Windows program that helps to use both hands for pressing keyboard combinations.

## Description

For some reasons I use only left modifier when press key combinations on
my keyboard (e.g. Ctrl-c, Alt-TAB, Shift-3 actually all of them).
This program is an attempt to change the situation.
I want to press modifiers by both hands. Actually, I want to use both
hands to press key combinations. When modifier is pressed by left hand
then another key should be pressed by right hand (and vice versa).

The program adds hook to the OS, listens for low-level keyboard events
and produces system sound when:

- exactly two keys are pressed AND
- one of them is modifier AND
- both keys are pressed by the same hand

## Configuration

User can define left hand and right hand zones in `keys.ini` by
adding key description to the corresponding section.
If key is missed from configuration then there will be no sound when it
is pressed. Keys are specified by scan codes. It allows for program to
work with different layouts.

I've used [SharpKeys](http://www.randyrants.com/sharpkeys/) to
find these codes for my keyboard.

## Usage

You need two files to run program: `bothhands.exe` and `keys.ini`.
After start you will see application icon in tray area.
You can close program by opening popup menu and selecting *Exit* item.

## Credits

The [inih parser](http://code.google.com/p/inih/) is used to read
configuration file.

The application icon was taken from
[Free FatCow-Farm Fresh Icons](http://www.fatcow.com/free-icons).

Other resources:

- [Desktop App User Interface](http://msdn.microsoft.com/en-us/library/windows/desktop/ff657751%28v=vs.85%29.aspx)
- [Win32 Programming](http://www.functionx.com/win32/index.htm)
- [Educational winapi keylogger](https://github.com/Totktonada/keylogger)
