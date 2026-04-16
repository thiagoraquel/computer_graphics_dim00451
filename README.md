# Project 1 - Basic Infrastructure

This is the basic architecture for the Ray Tracing Teaching Tool (RT3) Project.

Most of the class are incomplete. For instance, there is no `Vector3f` class.
Replace that with your own math library or expand the class already there.

# Processing flow

The `main.cpp` calls the `api.cpp`, which, in turn, calls various API functions that create the objects, such as camera, integrator, scene, film.
The information to create these classes are stored in the `RenderOptions` struct, while the scene is being parsed.

When the parser finds the tag `world_end` it creates all the objects (film, camera, scene, integrator) and calls the `render()` method.
This method corresponds to the "main loop" of the rendering process.

<!-- <img src="./docs/ray_tracer_pipeline.png" width="650"> -->

![Processing flow diagram](./docs/ray_tracer_pipeline.png)

# To compile

```
cmake -S . -B build
cmake --build build -j
./build/rt3 scenes/scene01.xml (for linux)
.\build\rt3 scenes\scene01.xml
```

After compiling, run the testing pipeline:
```
ctest --test-dir build --output-on-failure
```

# TODO

- [ ] Cameras
- [ ] Integrators
- [ ] Math class (vector and Ray)

---

&copy; DIMAp/UFRN 2024-2026.


# Caution: This deletes the build folder!
rm -rf build (for linux) or rd /s /q build (for windows)
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build

.\build\rt3.exe scenes\proj3_ortho.xml