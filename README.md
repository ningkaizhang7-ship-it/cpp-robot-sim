# 06-cpp-robot-sim-agv

Coursework and practice code from 2023–2025, kept as two independent Visual Studio solutions. The first is an OpenGL fixed-function robot simulation in three successive iterations (V6, V6.1, V6.2): it loads a BMP and a TGA texture, draws a jointed humanoid robot whose arms and legs swing while the model spins about the Y axis, and — from V6.2 — adds a mouse/keyboard free camera, a grid floor and an on-screen position/FPS readout. The second is an MFC dialog application, `Demo Jog`, that moves the axes of a Googol Technology GTS-series motion-control card one axis at a time (jog, homing, servo enable/disable, status and position clearing). Neither project is a simulator in the robotics sense: there is no dynamics, no collision detection, no trajectory planning and no robot middleware.

## What it does / scope

* **OpenGL robot simulation** (`opengl_robot_sim/`): a Win32/OpenGL program that opens a window (a startup `MessageBox` asks whether to run fullscreen), builds `image.bmp` and `sphere.tga` into two GL textures, lights the scene with `GL_LIGHT0`/`GL_LIGHT1`, and repeatedly draws a robot assembled from textured, scaled cubes. Animation advances by fixed steps per frame, not by the elapsed-time argument the framework supplies.
* **AGV jog demo** (`agv_jog_demo/`): an MFC dialog that drives real motion-control hardware through the Googol `gts.h` API. It reads an axis number plus speed/acceleration/deceleration/smoothing values from edit boxes and issues jog, home, zero-position, servo enable/disable and status-clear commands; jogging runs while a 负向/正向 button is held down.
* **Not present**: inverse kinematics, path planning, sensor or physics models, ROS/ROS 2, unit tests, CI, CMake, or any Linux/ARM target — and no AGV simulator, because `Demo Jog` needs the physical GTS card and its vendor driver.

## Repository layout

```
06-cpp-robot-sim-agv/
├── .gitignore                          Debug/Release, *.obj, *.exe, *.pdb, .vs/, *.user, ipch/
├── agv_jog_demo/                       MFC dialog app that jogs a GTS card (solution "Demo Jog")
│   ├── Demo Jog.sln / .vcxproj / .vcxproj.filters   VS2022 MFC project, v143, UseOfMfc=Dynamic
│   ├── Demo Jog.cpp / Demo Jog.h       CDemoJogApp (CWinApp); InitInstance runs the dialog modally
│   ├── Demo JogDlg.cpp / Demo JogDlg.h CDemoJogDlg (all motion logic) and CAboutDlg
│   ├── DemoJog.rc                      dialog layout, icon, version resource, string table (UTF-16)
│   ├── resource.h                      control IDs: IDC_EDIT_*, IDC_BUTTON_*, ini, tsclr, gohome
│   ├── gts.h                           Googol GTS API: GT_API prototypes, TJogPrm, THomePrm, ...
│   ├── framework.h, pch.h, pch.cpp, targetver.h     MFC and precompiled-header plumbing
│   └── res/Demo Jog.ico                application icon
└── opengl_robot_sim/
    ├── Robot/                          V6 — earliest iteration: immediate-mode robot, no textures
    │   ├── Robot.cpp / Robot.h         Robot : GLApplication; draws and animates the robot
    │   ├── GLFrame.cpp / GLFrame.h     GLApplication + Keys: WinMain, message loop, key state
    │   ├── GLWindow.cpp / GLWindow.h   window, pixel format, GL context, fullscreen switching
    │   ├── CBMPLoader.cpp / .h, TGALoader.cpp / .h    texture loaders (compiled but unused here)
    │   ├── stdafx.cpp / stdafx.h       common includes and OpenGL link pragmas
    │   └── Robot.sln, Robot.vcxproj, Robot.vcxproj.filters, Backup/Robot.sln (VS2008-era, no .vcproj)
    ├── Robot_V61/                      V6.1 — adds textures, hat, Z-key light toggle
    │   ├── Robot.cpp / Robot.h         Robot owns texture1 (BMP) and texture2 (TGA)
    │   ├── CBMPLoader, TGALoader, GLFrame, GLWindow, stdafx    same file set as Robot/
    │   ├── image.bmp, image0.bmp, sphere.tga
    │   └── Robot.sln, Robot.vcxproj, Robot.vcxproj.filters, Backup/Robot.sln
    ├── Robot_V62/                      V6.2 — latest: free camera, font, vector maths, grid, FPS
    │   ├── Robot.cpp / Robot.h         adds m_Camera, m_Font, m_Fps, DrawGrid, DrawHat/Hattop
    │   ├── Camera.cpp / Camera.h       gluLookAt view, mouse look, WASD/arrow movement
    │   ├── Vector.cpp / Vector.h       Vector3: length/normalize/dotProduct/crossProduct, operators
    │   ├── Font.cpp / Font.h           GLFont: GDI font rendered to a bitmap, blitted with glBitmap
    │   ├── CBMPLoader, TGALoader, GLFrame, GLWindow, stdafx    as in V6.1
    │   ├── image.bmp, image0.bmp, sphere.tga
    │   └── Robot.sln, Robot.vcxproj, Robot.vcxproj.filters, Backup/Robot.sln
    └── RobotV60(2)/
        ├── RobotV6/                    copy of the V6 sources (GLFrame, GLWindow, Robot) — no project file
        └── OpenGL开发库/INCLUDE/        GL.H, GLU.H, GLAUX.H, GLEXT.H, WGLEXT.H, GLUT.H, dinput.h
                                        (no matching LIB folder is shipped)
```

## How it works

### A. OpenGL robot simulation (flow of `Robot_V62`, the latest iteration)

1. `WinMain` (`GLFrame.cpp`) calls `GLApplication::Create("OpenGL")`, implemented in `Robot.cpp`, which allocates the derived `Robot`.
2. `GLApplication::Main` registers a `WNDCLASSEX` whose `lpfnWndProc` is `WindowProc`, asks via `MessageBox` whether to run fullscreen, then loops while `m_IsProgramLooping`.
3. `GLWindow::Create` creates the window, chooses and sets a pixel format, calls `wglCreateContext` and `wglMakeCurrent`, then `ReshapeGL` (`glViewport` + `gluPerspective(45, w/h, 1.0, 100.0)`).
4. `Robot::Init` clears state, enables depth test and smooth shading, calls `ResizeDraw(true)`, initialises the font, configures `GL_LIGHT0` from the file-scope `diffuseLight`/`specularLight`/`lightPosition` arrays, loads the textures (`LoadTexture()`), positions the camera with `m_Camera.setCamera(0,1.5,6, 0,1.5,0, 0,1,0)` and configures `GL_LIGHT1` in `SetLight()`.
5. The pump (`PeekMessage`/`DispatchMessage`) delivers input: `WM_KEYDOWN`/`WM_KEYUP` update the `Keys::m_KeyDown[256]` table; `WM_SIZING`/`WM_SIZE` update the window size and call `ReshapeGL`.
6. With no pending message and a visible window the pump calls `Update(dt)`, `Draw()`, `SwapBuffers()`. `Update` maps ESC → `TerminateApplication`, F1 → `ToggleFullscreen`, `Z` → `glDisable(GL_LIGHT1)`/`GL_LIGHTING`, and — unless `X` is held — adds 0.10° to the spin `angle`, wrapping at 360. It ends with `UpdateCamera()`.
7. `UpdateCamera` calls `m_Camera.setViewByMouse()`, then reads arrow keys / `W`,`A`,`S`,`D` for `Camera::moveCamera` (along the view vector) and `Camera::yawCamera` (strafe along view × up). `SHIFT` selects camera speed 0.6 instead of 0.2.
8. `Draw` clears the buffers, calls `m_Camera.setLook()` (`gluLookAt`), draws the grid, then translates to `(0, 5, −30)`, rotates by `angle` about Y, calls `DrawRobot`, and finally `PrintText()` and `glFlush()`.
9. `DrawRobot` places head/torso/legs/arms as scaled cubes and animates them: each limb angle moves by 0.1 per frame and a `static bool` direction flag flips at ±15°, applied as `glRotatef` about X.
10. `PrintText` prints `当前位置:X=.. Y=.. Speed=..` from `m_Camera.getView()` and the camera speed, then `CaculateFrameRate` (frame counting via `GetTickCount`) contributes the `FPS:..` line.
11. `Uninit` frees the texture images and GL texture objects; `GLWindow::Destroy` releases the context, destroys the window and restores the display mode if fullscreen was used.

### B. AGV jog demo (flow of `CDemoJogDlg`)

1. `CDemoJogApp::InitInstance` initialises common controls and the visual manager, then runs `CDemoJogDlg` with `DoModal()`.
2. **初始化** (`init`) brings the card up: `GT_Open()` → `GT_Reset()` → `GT_LoadConfig("gts800.cfg")` → `GT_ClrSts(1, 4)`. The configuration file is opened by relative name, so the working directory is part of the contract.
3. The user enters axis, speed, acceleration, deceleration and smoothing. `getAxis()` reads `IDC_EDIT_axis` with `GetDlgItemText` + `_ttoi`; the other fields use the same pattern with `_ttof`.
4. **伺服使能** (`servoenble`) → `GT_AxisOn(axis)`; **状态清零** (`staticclr`) → `GT_ClrSts(1, 4)`; **位置清零** (`zeropos`) → `GT_ZeroPos(axis)`; **伺服关闭** (`illeg`) → `GT_Stop(1 << (axis-1), 1 << (axis-1))` followed by `GT_AxisOff(axis)`.
5. Holding **负向**/**正向** is intercepted in `PreTranslateMessage`, which watches `WM_LBUTTONDOWN`/`WM_LBUTTONUP` and compares `pMsg->hwnd` with `GetDlgItem(...)->m_hWnd`: it calls `JogMotion(∓1)` on press and `GT_Stop` on release.
6. `JogMotion(direction)` zeroes the axis (`GT_ZeroPos`), selects jog profiling (`GT_PrfJog`), reads the current `TJogPrm` (`GT_GetJogPrm`), overwrites `acc`/`dec`/`smooth` from the edit boxes, writes them back (`GT_SetJogPrm`), sets the signed velocity `GT_SetVel(axis, speed * direction)` and starts motion with `GT_Update(1 << (axis - 1))`.
7. **回零** (`gohome`) calls `axisHomeMotion(axis)`: servo on, zero position, `GT_GetHomePrm`, then a mode-10 (limit) profile is filled in — axes 1–3 with `velHigh=30`, `velLow=20`, `acc=dec=0.25`, `smoothTime=25`, `homeOffset=-10000*dir`, `escapeStep=2000`, and axis 4 with `velHigh=2`, `velLow=1`, `acc=dec=0.1`, `smoothTime=10`, `homeOffset=-2000`, `escapeStep=500`; axis 3 inverts the search direction (`dir = -1`). `GT_GoHome` starts it, and a `do/while` loop polls `GT_GetHomeStatus` until `tHomests.run` is false; the position is then zeroed and `GT_ClrSts(1,4)` is called.

## Key functions and modules

| File | Function / Class | Purpose |
| --- | --- | --- |
| `Robot_V62/GLFrame.cpp` *(same role in Robot, Robot_V61, RobotV6)* | `WinMain`, `WindowProc`, `GLApplication::Main` | Entry point, window-class registration, and the outer/inner message loop that calls `Update`/`Draw`/`SwapBuffers` |
| `Robot_V62/GLFrame.cpp` | `GLApplication::Message` | Handles `WM_CLOSE`, `WM_SIZE`, `WM_SIZING`, `WM_KEYDOWN`/`UP`, `WM_TOGGLEFULLSCREEN`; blocks screensaver and monitor-power commands |
| `Robot_V62/GLFrame.h` | `Keys` (`IsPressed`, `SetPressed`, `SetReleased`, `Clear`) | 256-entry key-state table queried by `Update` and `UpdateCamera` |
| `Robot_V62/GLWindow.cpp` | `GLWindow::Create`, `Destroy`, `ChangeScreenSetting` | Window creation, `ChoosePixelFormat`/`SetPixelFormat`, `wglCreateContext`/`wglMakeCurrent`, fullscreen switching, cleanup |
| `Robot_V62/GLWindow.cpp` | `GLWindow::ReshapeGL` | `glViewport` plus `gluPerspective(45°, width/height, 1.0, 100.0)` on resize |
| `Robot_V62/Robot.cpp` | `GLApplication::Create`, `Robot::Robot` | Instantiates the concrete application; initialises `angle`, `legAngle[]`, `armAngle[]` |
| `Robot_V62/Robot.cpp` | `Robot::Init`, `Robot::Uninit` | GL state, font, lights, textures and camera setup; texture/context cleanup |
| `Robot_V62/Robot.cpp` | `Robot::LoadTexture`, `Robot::SetLight` | Loads `image.bmp` + `sphere.tga` into `texture1`/`texture2`; sets `GL_LIGHT1` ambient/diffuse/specular/position |
| `Robot_V62/Robot.cpp` | `Robot::Update` | ESC/F1 handling, `Z` light toggle, per-frame spin of `angle`, `UpdateCamera()` |
| `Robot_V62/Robot.cpp` | `Robot::Draw` | Clears buffers, places the camera, draws grid and robot, prints the text overlay |
| `Robot_V62/Robot.cpp` | `Robot::DrawRobot` + `DrawHead/Torso/Leg/Arm/Mouth/Nose/Hat/Hattop` | Assembles the robot from scaled cubes and animates the four limb angles |
| `Robot_V62/Robot.cpp` | `Robot::DrawCube` | Unit cube with per-face normals and texture coordinates; shared by every body part |
| `Robot_V62/Robot.cpp` | `Robot::DrawGrid` | Saves lighting/texture state, draws ±50 lines on the XZ plane at y = −6, restores state |
| `Robot_V62/Robot.cpp` | `Robot::UpdateCamera`, `PrintText`, `CaculateFrameRate` | Keyboard camera control; text overlay; per-second FPS counter into `m_Fps` |
| `Robot_V62/Camera.cpp` | `Camera::setCamera`, `setLook`, `rotateView` | Stores position/view/up vectors, applies `gluLookAt`, rotates the view vector with an axis-angle matrix |
| `Robot_V62/Camera.cpp` | `Camera::setViewByMouse`, `moveCamera`, `yawCamera` | Mouse look (cursor recentred, pitch clamped to ±1.0), forward/back movement, strafing |
| `Robot_V62/Vector.cpp` | `Vector3::normalize`, `crossProduct`, `dotProduct`, operators | Vector maths used by the camera for axes and movement directions |
| `Robot_V62/Font.cpp` | `GLFont::InitFont`, `GLFont::PrintText` | Creates a 16-pixel bold 宋体 `HFONT`, renders text into a monochrome bitmap, blits it with `glBitmap` |
| `Robot_V62/CBMPLoader.cpp` | `CBMPLoader::LoadBitmap`, `Load`, `FreeImage` | Reads a 24-bit BMP, swaps BGR→RGB, creates a mipmapped GL texture with linear filtering and repeat wrap |
| `Robot_V62/TGALoader.cpp` | `CTGALoader::LoadTGA`, `Load`, `FreeImage` | Reads an uncompressed TGA (header compared against `{0,0,2,0,...}`), swaps BGR(A)→RGB(A), creates the texture |
| `Robot_V61/Robot.cpp` | `Robot::LoadTexture`, `DrawHead`/`DrawTorso`/`DrawLeg` | First iteration binding `texture1`/`texture2` to body parts; no camera, no text overlay |
| `Robot/Robot.cpp` | `Robot::DrawCube` | Earliest cube: six quads in one `GL_POLYGON`, without normals or texture coordinates |
| `agv_jog_demo/Demo JogDlg.cpp` | `CDemoJogDlg::init`, `getAxis` | `GT_Open`/`GT_Reset`/`GT_LoadConfig("gts800.cfg")`/`GT_ClrSts`; edit-box reading and `short` conversion |
| `agv_jog_demo/Demo JogDlg.cpp` | `CDemoJogDlg::JogMotion` | Builds a `TJogPrm` from the edit boxes and starts a signed-velocity jog (`GT_PrfJog`, `GT_SetJogPrm`, `GT_SetVel`, `GT_Update`) |
| `agv_jog_demo/Demo JogDlg.cpp` | `CDemoJogDlg::axisHomeMotion` | Hard-coded mode-10 `THomePrm` profiles for axes 1–3 and axis 4; `GT_GoHome` plus a polling loop on `GT_GetHomeStatus` |
| `agv_jog_demo/Demo JogDlg.cpp` | `CDemoJogDlg::PreTranslateMessage` | Press-and-hold jogging: mouse down/up on the two buttons → `JogMotion(±1)` / `GT_Stop` |
| `agv_jog_demo/Demo JogDlg.cpp` | `servoenble`, `illeg`, `zeropos`, `staticclr`, `gohome` | Button handlers for `GT_AxisOn`, `GT_Stop` + `GT_AxisOff`, `GT_ZeroPos`, `GT_ClrSts`, homing |
| `agv_jog_demo/Demo Jog.cpp` | `CDemoJogApp::InitInstance` | MFC initialisation; creates `CDemoJogDlg` and runs it modally, returning `FALSE` to exit |
| `agv_jog_demo/gts.h` | `GT_*` prototypes, `TJogPrm`, `THomePrm`, `THomeStatus` | Vendor API declaration (`GT_API` = `extern "C" short __stdcall`) plus status/mode constants |

## Dependencies

* **Toolset**: Visual Studio 2022 projects (`PlatformToolset v143`, `VCProjectVersion 17.0`, `WindowsTargetPlatformVersion 10.0`). The OpenGL projects use `CharacterSet = MultiByte`; `Demo Jog` uses `Unicode` with `UseOfMfc = Dynamic` (shared MFC DLL).
* **OpenGL**: `opengl32.lib` (GL 1.1 fixed-function), `glu32.lib` and the discontinued `glaux.lib`, all requested with `#pragma comment(lib, ...)` in `stdafx.h` and `Robot.cpp`. Headers are included **unprefixed** (`<gl.h>`, `<glu.h>`, `<glaux.h>`, `<glext.h>` in `stdafx.h`; `<gl/gl.h>`, `<gl/glu.h>` in `GLFrame.cpp`/`GLWindow.cpp`), so a legacy OpenGL include directory is required.
* **Bundled headers** in `opengl_robot_sim/RobotV60(2)/OpenGL开发库/INCLUDE/`: `GL.H`, `GLU.H`, `GLAUX.H`, `GLEXT.H`, `WGLEXT.H`, `GLUT.H`, `dinput.h`. Only the first four are included by any source file here; `GLUT.H` and `dinput.h` are shipped but unreferenced.
* **Win32 API**: `windows.h`, `WinMain`, `WNDCLASSEX`, `CreateWindowEx`, `ChoosePixelFormat`, `SetPixelFormat`, `wglCreateContext`, `wglMakeCurrent`, `SwapBuffers`, `GetTickCount`, `GetSystemMetrics`, `GetCursorPos`/`SetCursorPos`, `CreateFont`, `glBitmap`.
* **MFC** (`agv_jog_demo/framework.h`): `<afxwin.h>`, `<afxext.h>`, `<afxdisp.h>`, `<afxdtctl.h>`, `<afxcmn.h>`, `<afxcontrolbars.h>`; classes `CWinApp`, `CDialogEx`, `CString`, `CMenu`, `CShellManager`, `CMFCVisualManagerWindows`.
* **C/C++ runtime**: `<stdio.h>`, `<math.h>`, `<time.h>`, `sprintf`, `fopen`/`fread`, `new`/`delete`.
* **Googol GTS motion-control SDK**: `gts.h` (shipped, 1421 lines) defines the API as `extern "C" short __stdcall` and the version macros `DLL_VERSION_0..DLL_VERSION_8` = `2,1,0,1,5,0,6,0,7`; the import library is requested via `#pragma comment(lib,"gts.lib")` in `Demo JogDlg.cpp`. Neither `gts.lib`, `gts.dll` nor `gts800.cfg` is present in this repository.

## How to run / build

Both projects are Windows-only and build with MSBuild/Visual Studio; there is no CMake or Makefile and nothing here builds on Linux.

**1. AGV jog demo — will not link as shipped.** From a Developer Command Prompt inside `agv_jog_demo`:

```
msbuild "Demo Jog.sln" /p:Configuration=Debug /p:Platform=x64     :: or /p:Platform=Win32
```

`Demo JogDlg.cpp` contains `#pragma comment(lib,"gts.lib")`. Linking and running therefore require, from the vendor's GTS-800 SDK: `gts.lib`, the matching `gts.dll` and driver, and `gts800.cfg`, which `CDemoJogDlg::init` loads by relative filename from the process working directory. Only `gts.h` is shipped here. The card must be installed, powered and recognised by the vendor driver, or `GT_Open()` fails — and because every return code is discarded, the failure is silent. **The application cannot be run without that hardware.**

**2. OpenGL robot simulation — cannot be built standalone as shipped.** From a Developer Command Prompt inside `opengl_robot_sim/Robot_V62` (or `Robot_V61`, or `Robot`):

```
msbuild Robot.sln /p:Configuration=Debug /p:Platform=Win32
```

What is missing or wrong in the shipped tree:

* The Debug|Win32 include/library paths in the three `.vcxproj` files point at a relative `..\..\OpenGL开发库\INCLUDE` and `..\..\OpenGL开发库\LIB` that resolve **outside** this repository; the only copy of those headers here is `opengl_robot_sim/RobotV60(2)/OpenGL开发库/INCLUDE`. The same files also record an absolute, machine-specific OpenGL directory that is not in the repository.
* No `LIB` folder (and no `glaux.lib`) is shipped, and `glaux.lib` is no longer distributed with the Windows SDK, so the glaux-dependent build cannot link without obtaining it separately.
* Only `Debug|Win32` defines those include/library directories; `Release|Win32` does not and relies on inherited `IncludePath`/`LibraryPath`.
* `RobotV60(2)/RobotV6` has sources only — no `.sln` or `.vcxproj` — and the `Backup/Robot.sln` files reference VS2008 `Robot.vcproj` files that are absent.

**Running it once built**: start the executable with `image.bmp` and `sphere.tga` in the working directory (both are committed next to the V6.1/V6.2 sources). A dialog first asks whether to run fullscreen. Controls: `ESC` quits; `F1` switches fullscreen/window; `X` freezes the body spin; `Z` toggles the second light; arrows or `W`/`A`/`S`/`D` move the camera; holding `SHIFT` raises camera speed from 0.2 to 0.6. In the V6 iteration only `ESC`/`F1` apply and the robot rotates always.

## Provenance and attribution

* This is the author's own coursework and practice work from 2023 to 2025. Build artefacts (`.vs` caches, object files, executables) were deliberately excluded from the repository.
* Large parts of the OpenGL skeleton are **not original**: the window framework (`GLFrame`, `GLWindow`, the `WinMain`/message-pump pattern, `Keys`), the texture loaders (`CBMPLoader`, `TGALoader`), `Camera`, `Vector` and `Font` all carry upstream author headers dated 2006–2007 and follow standard textbook/teaching samples. The simulation and robot-motion logic builds on that same standard MFC/Win32/OpenGL scaffolding rather than being written from nothing.
* The `.vcxproj`/`.sln` files were later converted to VS2022 format (`PlatformToolset v143`), and the `Backup/Robot.sln` files still reference VS2008-era `Robot.vcproj` projects not present here.
* No claim is made that any particular file or function was written from scratch by the author. Read the file headers first: they mark which code originated upstream.
* `agv_jog_demo` wraps the vendor's Googol `gts.h` API; the header is redistributed unmodified and remains the property of its vendor.

## Limitations and known issues

* **Return values are ignored.** In `Demo JogDlg.cpp` every `GT_*` call assigns to `sRtn`/`sRun`/`sRTn` and the value is never checked, so motion faults, an unopened card and a missing configuration file are all invisible to the user.
* **Unbounded polling loop.** `axisHomeMotion` blocks the UI thread in `do { GT_GetHomeStatus(...) } while (tHomests.run);` — no timeout, no message pumping, no abort.
* **Axis range is never validated.** `getAxis()` returns whatever `_ttoi` produces, and `JogMotion` and `illeg` build masks as `1 << (axis - 1)`, so axis 0 or a negative value is undefined behaviour; `axisHomeMotion` silently does nothing for axes outside 1–4.
* **Hard-coded homing values** for exactly four axes (branches for 1–3 and 4) with magic numbers.
* **Jog resets the position on every press**: `JogMotion` calls `GT_ZeroPos(axis)` first, so the absolute coordinate is lost each time.
* **Naming defects**: `servoenble`, `illeg`, `servo_disenlble`; `resource.h` defines both `servo_disenlble2` and `gohome` as `1012`, so the two symbols are indistinguishable. `CAboutDlg` lives inside `Demo JogDlg.cpp`.
* **`res/DemoJog.rc2` is missing** although `DemoJog.rc` includes `"res\DemoJog.rc2"`, so resource compilation fails on the shipped tree.
* **`Vector3::length()` returns x²+y²+z², not the Euclidean length**, and `normalize()` divides by that, so results are not unit vectors unless the input already had length 1; `yawCamera`/`moveCamera` therefore scale movement by the squared magnitude. `operator*` and `operator/` mutate the object in place and return `*this`, unlike the usual value semantics.
* **`Robot::m_Fps` is never initialised** in the constructor, so the FPS line can print garbage before the first one-second sample.
* **Animation ignores elapsed time**: `Update` receives `milliseconds`, but the spin and the limb swings advance by fixed 0.10 steps per frame, so speed is frame-rate dependent and the parameter is unused.
* **`Camera::setViewByMouse` calls `ShowCursor(TRUE)` every frame** and assumes the screen centre, which does not fit a windowed application. The `Camera.h` header comment claims terrain-boundary limiting ("对地形的边界进行了限制和修正"), but `Camera.cpp` contains no boundary clamping.
* **Textures are loaded by relative filename** (`"image.bmp"`, `"sphere.tga"`) and `CBMPLoader::Load`/`CTGALoader::Load` call `exit(0)` on failure, so a wrong working directory kills the process instead of reporting an error.
* **`CBMPLoader` ignores BMP row padding.** `image.bmp` is 255×252 at 24 bpp, so every row is padded from 765 to 768 bytes (252 × 768 = 193 536 bytes of pixel data). Because the header's `biSizeImage` is 0, the loader uses `biWidth * biHeight * 3` = 192 780 and reads the rows as if they were contiguous, so the resulting texture is sheared. `image0.bmp` declares `biSizeImage = 12098`, which is not a multiple of 3; the BGR→RGB loop (`for index < biSizeImage; index += 3`) therefore reads `image[12098]` on its last pass, one byte past the end of the 12098-byte buffer. (`sphere.tga` does match the loader: 512×512, 24 bpp, type 2, header `{0,0,2,0,...}`.)
* **Fixed-function OpenGL only**: immediate mode (`glBegin`/`glEnd`), `gluLookAt`, `gluBuild2DMipmaps`; no shaders, no VBOs, no requested GL version. `GLWindow::Create` also asks for `PFD_STEREO`, which this application does not use and many drivers do not offer.
* **`F1` restarts the whole window loop**: `WM_TOGGLEFULLSCREEN` flips `m_CreateFullScreen` and posts `WM_QUIT`, so textures and GL are re-initialised instead of the mode changing in place.
* **Duplicated sources**: the framework, camera, vector and loader files are copied across `Robot`, `Robot_V61`, `Robot_V62` and `RobotV60(2)/RobotV6`, so fixes do not propagate. The copies are not all identical — e.g. `Robot`/`Robot_V61` declare `bool m_KeyDown[MAX_KEYS]` with a `static const UINT WM_TOGGLEFULLSCREEN = (WM_USER + 1)` in `GLFrame.h`, whereas `Robot_V62`/`RobotV6` use a literal `256` and a `#define WM_TOGGLEFULLSCREEN WM_USER + 1` in `GLFrame.cpp`; `Robot/Robot.cpp` and `RobotV6/Robot.cpp` differ only in whether the `<gl.h>`/`<gl\gl.h>` includes are commented out.
* **No tests, no CI, no formatting configuration**, and no `CMakeLists.txt`: the only build definitions are the Visual Studio project files.

> **Note on the OpenGL development library.** The Visual Studio project files originally pointed at an absolute path on the author's machine for the discontinued OpenGL/GLUT/GLAUX development library. That path has been replaced with `$(SolutionDir)third_party\OpenGL\{INCLUDE,LIB}`; the library itself is third-party and is not distributed with this repository, so place it there (or edit the include/library paths) before building.
