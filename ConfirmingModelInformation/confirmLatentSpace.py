import torch

model = torch.jit.load("models/altoSaxV1_e02d7ebfe3_streaming_norm.ts", map_location="cpu")
model.eval()

# RAVE's standard input format (batch, channels, length)
dummy_input = torch.randn(1, 1, 2048)  

with torch.no_grad():
    try:
        # RAVE typically has encode and decode methods.
        if hasattr(model, 'encode'):
            latent = model.encode(dummy_input)
            print(f"Latent shape: {latent.shape}")
            print(f"Latent dimensions: {latent.shape[-2]}")  
        else:
            # Directly infer and deduce from the output
            output = model(dummy_input)
            print(f"Model output shape: {output.shape}")
    except Exception as e:
        print(f"Error: {e}")
        
        