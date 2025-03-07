# Complete STM32F401 Development Environment Setup Guide

## Required Software
1. ARM GNU Toolchain
2. OpenOCD
3. Visual Studio Code
4. ST-LINK USB drivers
5. Make (optional but recommended)
6. STM32CubeF4 Package

## Installation Steps

### 1. ARM GNU Toolchain
1. Visit: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
2. Download the latest version for Windows (select AArch32 bare-metal target - arm-none-eabi)
3. Run the installer, following default options
4. Add the bin directory to your system PATH:
   * Open System Properties → Advanced → Environment Variables
   * Under System Variables, find PATH
   * Add the path (typically `C:\Program Files (x86)\Arm GNU Toolchain\bin`)

### 2. OpenOCD
1. Visit: https://github.com/openocd-org/openocd/releases
2. Download the latest Windows release (zip file)
3. Extract to `C:\OpenOCD` (or your preferred location)
4. Add the `bin` directory to your system PATH
5. Note the location of the `scripts` folder - you'll need this later for configuration files

### 3. ST-LINK USB Drivers
1. Visit: https://www.st.com/en/development-tools/stsw-link009.html
2. Download and install the latest ST-LINK USB drivers
3. Connect your STM32F401 board and verify Windows recognizes it

### 4. Visual Studio Code
1. Download VS Code from https://code.visualstudio.com/
2. Install the following extensions:
   * "Cortex-Debug" by marus25
   * "C/C++" by Microsoft
   * "CMake Tools" by Microsoft (optional)

### 5. Make (Optional)
1. Download MinGW from https://sourceforge.net/projects/mingw/
2. During installation, select the `mingw32-base` package
3. Add MinGW's bin directory to your system PATH

### 6. STM32CubeF4 Package
1. Visit: https://www.st.com/en/embedded-software/stm32cubef4.html
2. Create a free ST account if you don't have one
3. Download "STM32CubeF4" package
4. Extract to a permanent location (e.g., `C:\STM32Cube\STM32CubeF4`)

### 7. SVD File
1. Visit: https://www.st.com/en/microcontrollers-microprocessors/stm32f401cc.html
2. Navigate to Resources/Tools & Software
3. Search for and download the CMSIS-SVD file for STM32F401
4. Alternative: Download from https://github.com/posborne/cmsis-svd/tree/master/data/STMicro
5. Save the SVD file to your project root directory

## Project Configuration

### Create Project Structure
1. Create a new folder for your project
2. Inside the project folder, create a `.vscode` directory
3. Copy the SVD file to your project root directory

### Configure VS Code

Create the following files in your `.vscode` folder:

#### 1. `launch.json`:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug (OpenOCD)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "openocd",
            "cwd": "${workspaceRoot}",
            "executable": "${workspaceRoot}/build/your_project.elf",
            "configFiles": [
                "C:/OpenOCD/scripts/interface/stlink.cfg",
                "C:/OpenOCD/scripts/target/stm32f4x.cfg"
            ],
            "svdFile": "${workspaceRoot}/STM32F401.svd",
            "preLaunchTask": "build",
            "runToEntryPoint": "main",
            "armToolchainPath": "C:/Program Files (x86)/Arm GNU Toolchain/bin",
            "openOCDPath": "C:/OpenOCD/bin",
            "device": "STM32F401CC",
            "debuggerArgs": []
        }
    ]
}
```

#### 2. `c_cpp_properties.json`:
```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/STM32Cube/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc",
                "C:/STM32Cube/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include",
                "C:/STM32Cube/STM32CubeF4/Drivers/CMSIS/Include"
            ],
            "defines": [
                "USE_HAL_DRIVER",
                "STM32F401xC",
                "DEBUG",
                "STM32F4",
                "__ARM_ARCH_7M__"
            ],
            "compilerPath": "C:/Program Files (x86)/Arm GNU Toolchain/bin/arm-none-eabi-gcc.exe",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-arm",
            "browse": {
                "path": [
                    "${workspaceFolder}"
                ],
                "limitSymbolsToIncludedHeaders": true
            }
        }
    ],
    "version": 4
}
```

#### 3. `tasks.json`:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "make",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

## Verification Steps

1. Open Command Prompt and verify installations:
```bash
arm-none-eabi-gcc --version
openocd --version
```

2. Connect your STM32F401 board

3. Open Device Manager to verify the ST-LINK appears under "Universal Serial Bus devices"

4. Check file paths:
   - Verify OpenOCD scripts folder contains:
     * `interface/stlink.cfg`
     * `target/stm32f4x.cfg`
   - Verify STM32CubeF4 installation contains:
     * `Drivers/STM32F4xx_HAL_Driver/Inc`
     * `Drivers/CMSIS/Device/ST/STM32F4xx/Include`
     * `Drivers/CMSIS/Include`

## Important Notes

1. All paths in configuration files should match your actual installation directories
2. The `executable` path in `launch.json` should match your project name
3. Update the `device` field in `launch.json` if using a different STM32F401 variant
4. Always use forward slashes (`/`) in paths, even on Windows
5. Make sure all environment variables (PATH) are set correctly before launching VS Code

## Troubleshooting

1. If OpenOCD fails to connect:
   - Check USB drivers installation
   - Verify ST-LINK connection in Device Manager
   - Confirm OpenOCD configuration file paths

2. If compilation fails:
   - Verify ARM toolchain is in PATH
   - Check include paths in `c_cpp_properties.json`
   - Ensure all required libraries are present

3. If IntelliSense shows errors:
   - Reload VS Code after configuration changes
   - Verify all include paths exist
   - Check if defines match your hardware
