# CompatTelControl

A lightweight Windows utility for controlling Microsoft Compatibility Appraiser / CompatTelRunner activity.

**Author:** Teng Chuan-Liang  
**Version:** 1.1  
**License:** MIT License

---

## Overview

CompatTelControl is a small Windows console utility designed to stop a currently running `CompatTelRunner.exe` process and disable the scheduled tasks that commonly launch Microsoft Compatibility Appraiser.

The program also provides a restore function so the scheduled tasks can be enabled again when needed.

The utility does **not** delete or modify `CompatTelRunner.exe`.

---

## Features

- Stops a running `CompatTelRunner.exe`
- Disables Microsoft Compatibility Appraiser
- Disables ProgramDataUpdater
- Displays the current scheduled-task status
- Restores / enables the scheduled tasks
- Automatically requests Administrator privileges through UAC
- Includes Windows executable Version Information
- Designed for Windows 10 and Windows 11
- Source code can be built with Windows Driver Kit 7.1

---

## Scheduled Tasks

CompatTelControl controls the following Windows scheduled tasks:

```text
\Microsoft\Windows\Application Experience\Microsoft Compatibility Appraiser

\Microsoft\Windows\Application Experience\ProgramDataUpdater
```

---

## Requirements

- Windows 10 or Windows 11
- Administrator privileges

For compiling the source code:

- Windows Driver Kit 7.1
- WDK path used by the supplied build script:

```text
C:\WinDDK\7600.16385.1
```

---

## Source Files

```text
comptelctrl.c     Main program source code
comptelctrl.rc    Windows VERSIONINFO resource
build_wdk7.cmd    WDK 7.1 build script
```

The generated files such as `.obj` and `.res` are build artifacts and do not need to be stored in the repository.

---

## Building with WDK 7.1

Place the following files in the same directory:

```text
comptelctrl.c
comptelctrl.rc
build_wdk7.cmd
```

Open a command prompt and run:

```bat
build_wdk7.cmd
```

If the build succeeds, the executable will be created as:

```text
comptelctrl.exe
```

The build process consists of:

```text
1. Compile the C source
2. Compile the Windows VERSIONINFO resource
3. Link the executable
```

---

## Usage

Run:

```text
comptelctrl.exe
```

If the program is not already running with Administrator privileges, Windows will display a UAC elevation request.

The main menu provides:

```text
1. Disable and stop CompatTelRunner activity
2. Restore / enable scheduled tasks
3. Show scheduled-task status
0. Exit
```

### Disable

Choose:

```text
1
```

The program will attempt to:

- Stop the relevant scheduled tasks
- Terminate a currently running `CompatTelRunner.exe`
- Disable the Microsoft Compatibility Appraiser task
- Disable the ProgramDataUpdater task

### Restore

Choose:

```text
2
```

This re-enables the scheduled tasks controlled by CompatTelControl.

### Check Status

Choose:

```text
3
```

This displays information about the scheduled tasks.

---

## Windows Version Information

The executable contains Windows VERSIONINFO data.

Right-click:

```text
comptelctrl.exe
```

and select:

```text
Properties -> Details
```

The executable contains information including:

```text
Product Name:       CompatTelControl
File Version:       1.1.0.0
Product Version:    1.1.0.0
Author / Company:   Teng Chuan-Liang
Copyright:          Copyright (C) 2026 Teng Chuan-Liang.
```

---

## Important Notes

CompatTelRunner and Microsoft Compatibility Appraiser are Windows components.

CompatTelControl disables the scheduled tasks that normally launch the compatibility assessment activity, but it does not remove Windows system files.

A Windows feature update or major system update may recreate or re-enable these scheduled tasks. If this occurs, CompatTelControl can be run again.

---

## Disclaimer

This software is provided as-is without warranty.

Changes to Windows scheduled tasks may affect Windows compatibility assessment or diagnostic functionality.

Users should understand the changes being made and use the restore option if compatibility assessment functionality is required again.

---

## License

This project is licensed under the MIT License.

See the `LICENSE` file for details.

---

## Author

**Teng Chuan-Liang**

Copyright (C) 2026 Teng Chuan-Liang. All rights reserved.
