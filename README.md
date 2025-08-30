# RAVE_for_MIDISynthesizer
Real-time MIDI-Synthesizer powered by the [RAVE](https://github.com/acids-ircam/RAVE) model and the [anira](https://github.com/anira-project/anira/tree/main) library. 

This project functions as a MIDI synthesizer, available both as a plugin and a standalone application. Specifically, a sine wave is used as the input to the RAVE model, and its frequency is controlled by MIDI notes.

The source code was written with reference to the [juce-audio-plugin](https://github.com/anira-project/anira/tree/main/examples/juce-audio-plugin) included in the anira library.

## Demonstration
[Youtube<img width="1017" height="653" alt="Screenshot 2025-08-30 at 1 12 39" src="https://github.com/user-attachments/assets/e77993cc-bd84-4883-be89-4f1152519369" />](https://youtu.be/SheBqi23JsI?si=bAWdzOa2-LXQnk1T)


## About Pre-trained model
In creating this project, I trained three types of RAVE models (water sounds, violin, and alto saxophone).   
You can download the models [here](https://www.dropbox.com/scl/fo/454ud8t5eulpj42d0ztmg/AB4ozeBlznhIaZ2UwxocFqg?rlkey=15en2vvl7gwtlahsg7lgmdnq5&st=350s3y2t&dl=0).  

For each of these, two architectures (v1 and v2) were prepared. As expected, v2 has a larger size, which increases the likelihood of dropout occurring in the audio signal.  
In Anira, the [InferenceConfig class](https://anira-project.github.io/anira/usage.html#inferenceconfig) allows you to specify the maximum inference time during initialization, making this adjustment particularly important.

I compared the v1 and v2 models on my DAW (you can see this in the [Demonstration](https://github.com/yuki-sato-0402/RAVE_for_MIDISynthesiser#demonstration)), and the v2 model was quite unstable on my machine (MacBook Pro 2020 intel CPU).  
I don't recommend using v2 at this time.  
The v1 works without any issues.

## About Training Data
For training the above models, the following datasets were used:
- [bpiyush/sound-of-water](https://huggingface.co/datasets/bpiyush/sound-of-water)

- [String Instruments Dataset](https://www.kaggle.com/datasets/aashnaashahh1504/string-instruments-dataset)

- [Real Saxophone Recordings for Audio-to-Score Music Transcription](https://grfia.dlsi.ua.es/audio-to-score/)

<!--## Demonstration-->


## 🛠️ Build Instructions
```
cd RAVE_for_MIDISynthesiser
git submodule update --init --recursive
cd build
cmake ..
cmake --build .
```

## Other References
- [nn-inference-template](https://github.com/Torsion-Audio/nn-inference-template)

- [RAVEv2_Training.ipynb](https://colab.research.google.com/drive/1ih-gv1iHEZNuGhHPvCHrleLNXvooQMvI?usp=sharing)

- [Tutorial: Build a MIDI synthesiser](https://juce.com/tutorials/tutorial_synth_using_midi_input/)

- [Let's build a synthesizer plug-in with C++ and the JUCE Framework!](https://youtube.com/playlist?list=PLLgJJsrdwhPwJimt5vtHtNmu63OucmPck&si=vfKCEvMZtt56co4B)

## Planned Features
- Allows you to select oscillators other than sine waves as input signals.
  
- Instead of using an oscillator input style, it would be great to control the latent space directly via MIDI input.

- Allows you to select RAVE models in a specific directory using a select box.
