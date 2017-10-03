# CGRA OpenGL Base Project


# FAQ

#### What math library are you using (and why)?
This base project uses `cgra_math.hpp` from the [single_header_math](https://github.com/JJscott/single_header_math) library I wrote. Its design is influeneced by GLSL syntax is designed to be concise, easy to understand and to meet most requirements for basic computer graphics applications.

#### What is ImGui?
[ImGui (dear imgui)](https://github.com/ocornut/imgui) is a lightweight immediate-mode GUI library. Once set up it is easy to design and use simple gui components for the project. The GUI is rebuilt every frame and the code both sets up the GUI and reacts to inputs; For example the following simple code brings up a window with some text, a reactive button, and interactive input field:
```c++
static int i = 0;
if (ImGui::Begin("Window Title")) {
    ImGui::Text("This is some text");
    if (ImGui::Button("This is a button")) {
        // button has been pushed, do something ...
    }
    ImGui::InputInt("This is an Input", &i);
}
ImGui::End();
```

#### What are CGRA Meta-Tags?

The CGRA meta-tags are text file tags that you can insert into any file. The tags let you specify sections of code to manipulate, allowing you to, for instance, write a completed version of an project/assignment and run a parser (such as `cgra_parse.py`) on the project to get the "base code". Unless otherwise specified, the lines that the tags are on are also removed. The following are the currently implemented tags and are typically inserted as comments in the source files :

 - `CGRA_DELETE_FILE` Flags the entire file for deletion.
 - `CGRA_REMOVE` Removes only this line.
 - `CGRA_BEGIN_REMOVE` Removes all lines (including this line) until `CGRA_END_REMOVE` is reached.
 - `CGRA_END_REMOVE` Specifies the end of removal for `CGRA_BEGIN_REMOVE`.
 - `CGRA_BEGIN_COMMENT` Uncomments all lines until `CGRA_END_COMMENT` is reached. Only supports single line comments in C++.
 - `CGRA_END_COMMENT` Specifies the end of uncommenting for `CGRA_BEGIN_COMMENT`.
 - `CGRA_BEGIN_UNCOMMENT` Uncomments all lines until `CGRA_END_UNCOMMENT` is reached. Only supports single line comments in C++.
 - `CGRA_END_UNCOMMENT` Specifies the end of uncommenting for `CGRA_BEGIN_UNCOMMENT`.



# Requirements

The project requires [CMake](https://cmake.org/) to build before compiling. The recommended way to build to project is to create a build folder then using CMake to create to project inside it. The following sections are a guides to building common project types.
```sh
$ mkdir build
```

This project also requires OpenGL v3.3 and a suitable C++17 compiler.



## Linux

#### Command Line

The simpliest way to set up a basic project is to run the shell script `f5.sh` which runs the `cmake`, `make` and run commands for you.
```sh
$ ./f5.sh
```

Alternativiely you can run the commands manually.
```sh
$ cd build
$ cmake ../work
$ make
$ cd ..
```

If the project builds without errors the executable should be located in the `build/bin/` directory and can be run with:
```sh
$ ./build/bin/base [args...]
```

#### Eclipse
Setting up for [Eclipse](https://eclipse.org/) is a little more complicated. Navigate to the build folder and run `cmake` for Eclipse.
```sh
$ cd build
$ cmake  -G "Eclipse  CDT4 - Unix  Makefiles" ../work
```
Start Eclipse and go to `File > Import > Existing Projects into Workspace`, browse to and select the `build/` directory as the project. Make sure  the  box `Copy  Projects into Workspace` is unchecked. Once you've imported the project, and are unable run it, do the following:
 - Go to `Run > Run  Configurations`.  On the left side, select C/C++  Application, then in the `Main` tab, make sure your C/C++ Application field contains `./bin/base` and that `Enable auto build` is checked.
 - On your project, `[right click] > Run As > C/C++  Application`.  This should setup the default way to start the program, so you can simply run the project anytime after that.

If  you  need  to  run  with  arguments  (and  you  will  with  some  projects)  go  to `Run > Run Configurations > Arguments` and enter your arguments there.  For example: `./work/res/assets/teapot.obj `



## Windows

#### Visual Studio

This project requires at least Visual Studio 2015. You can get the latest, [Visual Studio Community 2017](https://www.visualstudio.com/downloads/), for free from Microsoft.

| Product |  XX  |
|:-------:|:----:|
| Visual Studio 2015 | 14 |
| Visual Studio 2017 | 15 |

Run the `cmake` command for Visual Studio with the appropriate version number (XX).
```sh
> cmake -G "Visual Studio XX" ..\work
```

Or if you are building for 64-bit systems.
```sh
> cmake -G "Visual Studio XX Win64" ..\work
```

After opening the solution (`.sln`) you will need to set some additional variables before running.
 - `Solution Explorer > base > [right click] > Set as StartUp Project`
 - `Solution Explorer > base > [right click] > Properties > Configuration Properties > Debugging`
 -- Select `All Configurations` from the configuration drop-down
 -- Set `Working Directory` to `(SolutionDir)../work`
 -- Set `Command Arguments` to whatever is required by your program



## OSX

#### XCode

[Xcode](https://developer.apple.com/xcode/) is an IDE that offers a little more than simple text editing. The setup again is very similar to Eclipse.
```sh
$ cd build
$ cmake -G "Xcode" ../work
$ cd ..
```

Once you're setup, you can build your project with Xcode, but have to execute your program with the terminal (making sure you are in the root directory).
```sh
$ ./build/bin/base [args..]
```



# CGRA Library

In addition to the math library and other external libraries, this project provides some simple classes and functions (in the `cgra` namespace) to get started with a graphics application. Further description and documentation can be found in the respective headers.

| File | Description |
|:----:|:------------|
| `cgra_gui.hpp` | Provides methods for setting up and rendering ImGui  |
| `cgra_image.hpp` | An image class that can loaded from and saved to a file |
| `cgra_math.hpp` | Linear algebra math library which closely resembles GLSL |
| `cgra_mesh.hpp` | Mesh builder class for simple position/normal/uvs meshes |
| `cgra_shader.hpp` | Shader builder class for compiling shaders from files or strings |
| `cgra_util.hpp` | Utility functions, such as one-line string building |
| `cgra_wavefront.hpp` | Minimum viable wavefront asset loader function that returns a `mesh_builder` |

In particular, the `image<T, N>`, `shader_builder`, and `mesh_builder` classes are designed to hold data on the CPU and provide a way to upload this data to OpenGL. They are not responsible for deallocating these objects, although they can re-use them (when you provide them as arguments while generating).

