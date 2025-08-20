#ifndef FORMAT_WAV_H
#define FORMAT_WAV_H

#include <stdint.h>

// WAV Header Structure
typedef struct {
    char riff_header[4]; // "RIFF"
    uint32_t wav_size;   // File size - 8
    char wave_header[4]; // "WAVE"
    char fmt_header[4];  // "fmt "
    uint32_t fmt_chunk_size; // Size of the fmt chunk
    uint16_t audio_format;   // Audio format (1 for PCM)
    uint16_t num_channels;   // Number of channels (1 = mono, 2 = stereo)
    uint32_t sample_rate;    // Sampling rate (e.g., 16000 Hz)
    uint32_t byte_rate;      // (SampleRate * NumChannels * BitsPerSample) / 8
    uint16_t block_align;    // (NumChannels * BitsPerSample) / 8
    uint16_t bits_per_sample;// Bits per sample (e.g., 16-bit)
    char data_header[4];     // "data"
    uint32_t data_size;      // Number of bytes in data section
} wav_header_t;

// Macro to create a default WAV header
#define WAV_HEADER_PCM_DEFAULT(data_size, bits, sample_rate, channels)  \
    {                                                                   \
        {'R', 'I', 'F', 'F'},                                           \
        (data_size + 36),                                               \
        {'W', 'A', 'V', 'E'},                                           \
        {'f', 'm', 't', ' '},                                           \
        16,                                                             \
        1,                                                              \
        (channels),                                                     \
        (sample_rate),                                                  \
        ((sample_rate) * (channels) * ((bits) / 8)),                    \
        ((channels) * ((bits) / 8)),                                    \
        (bits),                                                         \
        {'d', 'a', 't', 'a'},                                           \
        (data_size)                                                     \
    }

#endif // FORMAT_WAV_H
