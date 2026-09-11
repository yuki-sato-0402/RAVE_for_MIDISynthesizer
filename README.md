# RAVE_for_MIDISynthesizer

**A real-time MIDI Synthesizer plugin (VST3 / AU / Standalone) powered by the [RAVE](https://github.com/acids-ircam/RAVE) model and the [anira](https://github.com/anira-project/anira/tree/main) library.**

This project implements a neural-based MIDI synthesizer using the JUCE framework. Unlike standard RAVE implementations that require audio input, this application generates internal oscillator signals (Sine and Square waves) driven by polyphonic MIDI input as the excitation signal for the RAVE model.

## Key Features
- **Multi-Format Support**: Build as a **VST3 or AU plugin** for use in DAWs, or as a **Standalone** application.
- **Neural Synthesis**: Real-time audio generation using pre-trained RAVE models (v1 architecture).
- **Polyphonic Playback**: Up to 4-voice polyphony for rich harmonic playing.
- **Oscillator Waveform Mixer (Sine + Square)**: Dynamically blend Sine and Square wave oscillators to control the harmonic content of the excitation signal. By sculpting the input overtones, you can unlock and explore a wider variety of rich acoustic features contained within the RAVE model.
- **UI Latent Variable Control**: Independently control the Scale Factor and Bias of 8 latent dimensions via intuitive UI sliders.

## Demonstration
[Youtube<img width="968" height="594" alt="Screenshot 2026-01-19 at 0 26 53" src="https://github.com/user-attachments/assets/970bc822-9339-4915-863a-82c8aad51d12" />](https://youtu.be/ORYQNKNKJjU?si=6QDs7ne-mZo-zgeL) 

## 🛠️ Build Instructions
```
cd RAVE_for_MIDISynthesizer
git submodule update --init --recursive
cd build
cmake ..
cmake --build .
```

## About Pre-trained Model
In creating this project, I trained three types of RAVE models (water sounds, violin, and alto saxophone).   
You can download the models [here](https://www.dropbox.com/scl/fo/454ud8t5eulpj42d0ztmg/AB4ozeBlznhIaZ2UwxocFqg?rlkey=15en2vvl7gwtlahsg7lgmdnq5&st=g60m3rzv&dl=0).  

All three models were trained using the v1 architecture. While it may also depend on the machine's performance, the v2 architecture model frequently experienced audio dropouts and was quite unstable.

In Anira, the [InferenceConfig class](https://anira-project.github.io/anira/usage.html#inferenceconfig) allows you to specify the maximum inference time during initialization, making this adjustment particularly important.

## About the Number of Dimensions of Latent Variables
When training with the v1 architecture, the default number of dimensions for latent variables is 128. Since it would be difficult to control these individually, this application compresses the model to 8 dimensions before use. The number 8 is based on the original [RAVE VST](https://forum.ircam.fr/projects/detail/rave-vst/).  

The dimension of latent variables can be specified when exporting the model's checkpoint file as a torchscript (.ts). Please refer to [this](https://github.com/victor-shepardson/RAVE/blob/vs-ups/scripts/export.py) program.  

To check the number of dimensions in the model, please refer to [this](https://github.com/yuki-sato-0402/RAVE_for_MIDISynthesizer/blob/main/ConfirmingModelInformation/confirmLatentSpace.py) program. To check the minimum and maximum values of the latent variables, please refer to [this](https://github.com/yuki-sato-0402/RAVE_for_MIDISynthesizer/blob/main/ConfirmingModelInformation/confirmRangeRangeOfLatentVariables.py) program.

## About Training Data
For training the above models, the following datasets were used:
- [bpiyush/sound-of-water](https://huggingface.co/datasets/bpiyush/sound-of-water)

- [String Instruments Dataset](https://www.kaggle.com/datasets/aashnaashahh1504/string-instruments-dataset)

- [Real Saxophone Recordings for Audio-to-Score Music Transcription](https://grfia.dlsi.ua.es/audio-to-score/)


## Other References
- [nn-inference-template](https://github.com/Torsion-Audio/nn-inference-template)

- [RAVEv2_Training.ipynb](https://colab.research.google.com/drive/1ih-gv1iHEZNuGhHPvCHrleLNXvooQMvI?usp=sharing)

- [Tutorial: Build a MIDI synthesiser](https://juce.com/tutorials/tutorial_synth_using_midi_input/)

- [Let's build a synthesizer plug-in with C++ and the JUCE Framework!](https://youtube.com/playlist?list=PLLgJJsrdwhPwJimt5vtHtNmu63OucmPck&si=vfKCEvMZtt56co4B)
