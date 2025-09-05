import torch
import torchaudio

model = torch.jit.load("models/altoSaxV1_e02d7ebfe3_streaming_norm.ts", map_location="cpu")
model.eval()

waveform, sr = torchaudio.load("ConfirmingModelInformation/sine.wav")
if sr != 48000:
    waveform = torchaudio.functional.resample(waveform, sr, 48000)

#Processing to the shape expected by the model [batch, channels, samples]）
print("Waveform shape:", waveform.shape)
x = waveform.unsqueeze(0)
print("Input shape:", x.shape)  # Should be [1, 1, num_samples]

with torch.no_grad():
    z = model.encode(x)  
    

print("Latent min:", z.min().item())
print("Latent max:", z.max().item())
