# SonicWiz — Real-Time Audio Visualizer

SonicWiz is a real-time audio visualizer built in modern C++, designed for smooth, customizable visual effects while capturing and analyzing system playback audio.


## ✨ Features

- 🔊 **System audio capture** via PortAudio
- 📈 **Real-time FFT** using [FFTW3](http://www.fftw.org/) for accurate frequency analysis
- 🎛️ **Frequency band filtering** — toggle bass, mid, treble individually
- 🖥️ **Modern UI overlay** using [Dear ImGui](https://github.com/ocornut/imgui)
- ⚡ **Hardware-accelerated rendering** via [SDL2](https://github.com/libsdl-org/SDL)
- 🧩 Cross-platform (Windows, Linux, macOS) design


## 🛠️ Build Instructions

### Prerequisites
- C++17 (or later) compiler
- [CMake](https://cmake.org/)
- [SDL2](https://github.com/libsdl-org/SDL)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [PortAudio](http://www.portaudio.com/)
- [FFTW3](http://www.fftw.org/)

### Steps
```bash
# Clone the repository
git clone https://github.com/yourusername/SonicWiz.git
cd SonicWiz

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .
````

> ⚠️ **Audio capture notes**:
>
> * **Windows**: Enable WASAPI loopback for system playback capture
> * **Linux**: Use a PulseAudio “monitor” device
> * **macOS**: Select the appropriate playback capture device


## 🧪 Usage

1. Start playback of any audio (music, video, etc.)
2. Launch **SonicWiz**
3. Visualizer bars react in real time to audio input
4. Use the ImGui toolbar to toggle frequency bands, adjust gain, or change colors


## 📦 Project Structure

```
SonicWiz/
├── src/
│   ├── audio/          # AudioEngine (PortAudio)
│   ├── dsp/            # FFTProcessor (FFTW3)
│   ├── ui/             # Toolbar (Dear ImGui)
│   ├── visualizer/     # SDL2 rendering
│   └── main.cpp        # Application entry point
├── third_party/        # External libraries
└── CMakeLists.txt
```


## 🚀 Roadmap

* [ ] Multiple visualization modes (bars, waves, radial)
* [ ] Beat detection and peak highlighting
* [ ] Configurable color themes and layouts
* [ ] Plugin-style effect modules


## 📄 License

This project is released under the MIT License.
See [LICENSE](LICENSE) for details.


## 🙌 Acknowledgements

* [Dear ImGui](https://github.com/ocornut/imgui)
* [SDL2](https://github.com/libsdl-org/SDL)
* [PortAudio](http://www.portaudio.com/)
* [FFTW3](http://www.fftw.org/)


> 💡 Contributions, feedback, and pull requests are welcome!
