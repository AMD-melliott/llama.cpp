# ROCm Docker Usage Guide for llama.cpp

This guide explains how to build and use llama.cpp with AMD ROCm GPU support using Docker.

## Overview

The `Dockerfile.rocm` builds llama.cpp with ROCm HIP support for AMD GPUs. This setup includes:

- **Base Image**: `rocm/dev-ubuntu-22.04:6.4.3-complete` - Official AMD ROCm development environment
- **GPU Support**: ROCm HIP backend with ROCm 6.4.3
- **Target Architecture**: gfx942 (optimized for MI300 series, can be modified for other GPUs)
- **Features**: ROCm WMMA flash attention support for improved performance
- **Build Configuration**: Release build with optimized compiler flags

## What the Dockerfile Does

1. **Environment Setup**: Installs ROCm development tools, CMake, and build dependencies
2. **Source Code**: Clones the latest llama.cpp repository from GitHub
3. **Build Configuration**: Configures CMake with ROCm HIP backend enabled
4. **Compilation**: Builds llama.cpp with AMD GPU acceleration
5. **Binary Access**: Creates symlinks for easy access to compiled binaries

## Prerequisites

- Docker installed with AMD GPU runtime support
- AMD GPU with ROCm support (MI200/MI300 series recommended)
- ROCm drivers installed on the host system

## Building the Docker Image

```bash
# Build the ROCm-enabled llama.cpp image
docker build -f Dockerfile.rocm -t llama-cpp-rocm .
```

## Docker Compose Files

We provide three separate Docker Compose files for different use cases:

### 1. Server Mode (`docker-compose.rocm-server.yml`)
**Purpose**: Run llama-server with OpenAI-compatible API
**Use when**: You want to provide an API endpoint for applications

### 2. Interactive Shell (`docker-compose.rocm-shell.yml`)  
**Purpose**: Interactive development and testing
**Use when**: You want to experiment, debug, or run multiple commands

### 3. CLI Mode (`docker-compose.rocm-cli.yml`)
**Purpose**: Run one-off commands and utilities
**Use when**: You want to run specific llama.cpp tools like quantization or benchmarks

## Quick Start

### Running the Server

The easiest way to get started is with the server mode:

```bash
# Start the server (will be available at http://localhost:8000)
docker-compose -f docker-compose.rocm-server.yml up

# Start server in background
docker-compose -f docker-compose.rocm-server.yml up -d

# View server logs
docker-compose -f docker-compose.rocm-server.yml logs -f

# Stop the server
docker-compose -f docker-compose.rocm-server.yml down
```

The server provides these endpoints:
- **Chat Completions**: `POST /v1/chat/completions`
- **Completions**: `POST /v1/completions`
- **Models**: `GET /v1/models`
- **Health**: `GET /health`

### Interactive Development

For development and testing:

```bash
# Start interactive shell
docker-compose -f docker-compose.rocm-shell.yml run --rm llama-shell

# Inside the container, you can:
# - List available binaries: ls /usr/local/bin/llama-cpp/
# - Run inference: /usr/local/bin/llama-cpp/llama-cli -m /models/model.gguf -p "Hello" -n 50
# - Check GPU: /usr/local/bin/llama-cpp/llama-cli --help | grep -i gpu
```

### Running CLI Commands

For specific tasks:

```bash
# Run model quantization
docker-compose -f docker-compose.rocm-cli.yml run --rm llama-cli \
  /usr/local/bin/llama-cpp/llama-quantize /models/input.gguf /models/output.q4_0.gguf q4_0

# Run inference
docker-compose -f docker-compose.rocm-cli.yml run --rm llama-cli \
  /usr/local/bin/llama-cpp/llama-cli -m /models/model.gguf -p "Explain quantum computing" -n 100

# Run benchmarks
docker-compose -f docker-compose.rocm-cli.yml run --rm llama-cli \
  /usr/local/bin/llama-cpp/llama-bench -m /models/model.gguf
```

## Configuration

### Server Configuration

Customize the server with environment variables:

```bash
# Custom model and settings
MODEL_PATH=/models/my-model.gguf \
CONTEXT_SIZE=4096 \
GPU_LAYERS=33 \
SERVER_PORT=8080 \
docker-compose -f docker-compose.rocm-server.yml up
```

Available environment variables:
- `MODEL_PATH`: Path to your GGUF model file (default: `/models/model.gguf`)
- `CONTEXT_SIZE`: Context window size (default: `2048`)
- `GPU_LAYERS`: Number of layers to offload to GPU (default: `999` for all)
- `BATCH_SIZE`: Batch size for processing (default: `512`)
- `SERVER_PORT`: External port for the server (default: `8000`)
- `HIP_VISIBLE_DEVICES`: Which GPU to use (default: `0`)
- `GPU_MEMORY`: GPU memory allocation (default: `auto`)
- `LLAMA_DEBUG`: Enable debug logging (default: `0`)

### GPU Configuration

For different GPU architectures, rebuild the image:

```bash
# For different GPU targets
docker build -f Dockerfile.rocm -t llama-cpp-rocm \
  --build-arg GPU_TARGETS=gfx906,gfx908,gfx90a .
```

## Model Management

### Setting Up Models

1. Create a models directory and place your GGUF files:

```bash
# Create models directory
mkdir -p ./models

# Download a model (example)
wget -O ./models/llama-2-7b-chat.q4_0.gguf https://huggingface.co/example/model.gguf
```

2. The Docker Compose files automatically mount `./models` to `/models` in the container.

### Supported Model Formats

- **GGUF**: Native llama.cpp format (recommended)
- **GGML**: Legacy format (can be converted to GGUF using the CLI tools)

## API Usage Examples

Once the server is running with `docker-compose.rocm-server.yml`, you can use it like any OpenAI-compatible API:

```bash
# Chat completion
curl -X POST http://localhost:8000/v1/chat/completions \
  -H "Content-Type: application/json" \
  -d '{
    "model": "llama-2-7b-chat",
    "messages": [
      {"role": "user", "content": "Hello, how are you?"}
    ],
    "max_tokens": 100
  }'

# Text completion
curl -X POST http://localhost:8000/v1/completions \
  -H "Content-Type: application/json" \
  -d '{
    "model": "llama-2-7b-chat", 
    "prompt": "The capital of France is",
    "max_tokens": 50
  }'

# Check server health
curl http://localhost:8000/health
```

## Common Use Cases

### Development Workflow

```bash
# 1. Start with interactive shell for testing
docker-compose -f docker-compose.rocm-shell.yml run --rm llama-shell

# 2. Run one-off tasks like quantization
docker-compose -f docker-compose.rocm-cli.yml run --rm llama-cli \
  /usr/local/bin/llama-cpp/llama-quantize /models/large-model.gguf /models/small-model.q4_0.gguf q4_0

# 3. Deploy server for applications
docker-compose -f docker-compose.rocm-server.yml up -d
```

### Production Deployment

```bash
# Production server with custom configuration
MODEL_PATH=/models/production-model.gguf \
CONTEXT_SIZE=4096 \
GPU_LAYERS=40 \
docker-compose -f docker-compose.rocm-server.yml up -d
```

## Troubleshooting

### Common Issues

1. **GPU Not Detected**:
   ```bash
   # Check ROCm installation
   docker-compose -f docker-compose.rocm-shell.yml run --rm llama-shell rocm-smi
   ```

2. **Model Not Found**:
   ```bash
   # List available models
   docker-compose -f docker-compose.rocm-shell.yml run --rm llama-shell ls -la /models/
   ```

3. **Out of Memory Errors**:
   ```bash
   # Reduce context size or GPU layers
   CONTEXT_SIZE=1024 GPU_LAYERS=20 docker-compose -f docker-compose.rocm-server.yml up
   ```

4. **Performance Issues**:
   ```bash
   # Check if GPU acceleration is working
   docker-compose -f docker-compose.rocm-cli.yml run --rm llama-cli \
     /usr/local/bin/llama-cpp/llama-cli -m /models/model.gguf -p "test" -n 1 -v
   ```

### Debug Mode

Enable verbose logging:

```bash
# Server with debug output
LLAMA_DEBUG=1 docker-compose -f docker-compose.rocm-server.yml up

# CLI with verbose output  
docker-compose -f docker-compose.rocm-cli.yml run --rm llama-cli \
  /usr/local/bin/llama-cpp/llama-cli -m /models/model.gguf -p "test" -n 1 --verbose
```

## Memory and Performance Tuning

### GPU Memory Settings

```bash
# For 32GB GPU
GPU_MEMORY=32G docker-compose -f docker-compose.rocm-server.yml up

# For multi-GPU setups
HIP_VISIBLE_DEVICES=0,1 docker-compose -f docker-compose.rocm-server.yml up
```

### Context and Batch Size Tuning

```bash
# Large context for long conversations
CONTEXT_SIZE=8192 BATCH_SIZE=1024 docker-compose -f docker-compose.rocm-server.yml up

# Fast inference for short responses  
CONTEXT_SIZE=1024 BATCH_SIZE=256 docker-compose -f docker-compose.rocm-server.yml up
```

## Migration from Old docker-compose.rocm.yml

If you were using the old complex `docker-compose.rocm.yml` file:

- **For server usage**: Use `docker-compose.rocm-server.yml`
- **For shell access**: Use `docker-compose.rocm-shell.yml` 
- **For CLI commands**: Use `docker-compose.rocm-cli.yml`

The commands are now simpler and more focused on specific use cases.

## Support

For issues specific to:
- **llama.cpp**: Check the [main repository](https://github.com/ggml-org/llama.cpp)
- **ROCm**: Consult [AMD ROCm documentation](https://rocm.docs.amd.com/)
- **Docker**: Refer to [Docker documentation](https://docs.docker.com/)