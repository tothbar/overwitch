/*
 * overwitch.h
 * Copyright (C) 2019 Stefan Rehm <droelfdroelf@gmail.com>
 * Copyright (C) 2021 David García Goñi <dagargo@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 Protocol details known so far
 -----------------------------

 For USB configuration and transfer setup, please read the code :)

 All values are big-endian (MSB first)!
 Sample rate is 48kHz

 Data format TO Digitakt (USB interrupt transfer, EP 0x03)
 ----------------------------------------------------------------

 Raw data of the transfer consists of 24 blocks, each block holding
 - a fixed header
 - a sample counter (uint16 BE), increased by 7 per block (because
 each block contains 7 samples)
 - 7 samples (int32 BE) * 2 channels (interleaved)

 (total 168 samples, 2112 Bytes). Structure of a single block:

 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | fixed header: 0x07FF          | sample counter (uint16_t)     |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                                                               |
 +                            unknown                            +
 ...
 +                           (28 Bytes)                          +
 |                                                               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, Master Out 1, int32_t                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, Master Out 2, int32_t                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, Master Out 1, int32_t                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, Master Out 2, int32_t                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                                                               |
 +                        samples 3 .. 7                         +
 ...
 +                     (5*8 Bytes = 40 Bytes)                    +
 |                                                               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



 Data format FROM Digitakt (USB interrupt transfer, EP 0x83)
 ----------------------------------------------------------------

 Raw data of 24 blocks (total 8832 Bytes). Structure of a
 single block:

 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | fixed header: 0x0700          | sample counter (uint16_t)     |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                                                               |
 +                            unknown                            +
 ...
 +                           (28 Bytes)                          +
 |                                                               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, Master 1/FX1, int32_t                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, Master 2/FX2, int32_t                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, CH1, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, CH2, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, CH3, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, CH4, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, CH5, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, CH6, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, CH7, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, CH8, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, EXT IN 1, int32_t                                  |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  1, EXT IN 2, int32_t                                  |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, Master 1/FX1, int32_t                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, Master 2/FX2, int32_t                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, CH1, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, CH2, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, CH3, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, CH4, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, CH5, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, CH6, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, CH7, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, CH8, int32_t                                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, EXT IN 1, int32_t                                  |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | sample  2, EXT IN 2, int32_t                                  |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                                                               |
 +                        samples 3 .. 7                         +
 ...
 +                     (5*48 Bytes = 240 Bytes)                  +
 |                                                               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

 */

#include <libusb.h>
#include <samplerate.h>
#include <pthread.h>
#include "utils.h"

#define OB_SAMPLE_RATE 48000.0
#define OB_FRAMES_PER_BLOCK 7
#define OB_BYTES_PER_FRAME sizeof(float)
#define OB_PADDING_SIZE 28
#define OB_MAX_TRACKS 12

#define OB_MIDI_EVENT_SIZE 4

struct overwitch_usb_blk
{
  uint16_t header;
  uint16_t s_counter;
  uint8_t padding[OB_PADDING_SIZE];
  int32_t data[];
};

typedef enum
{
  OW_OK = 0,
  OW_LIBUSB_INIT_FAILED,
  OW_CANT_OPEN_DEV,
  OW_CANT_SET_USB_CONFIG,
  OW_CANT_CLAIM_IF,
  OW_CANT_SET_ALT_SETTING,
  OW_CANT_CLEAR_EP,
  OW_CANT_PREPARE_TRANSFER,
  OW_CANT_FIND_DEV
} overwitch_err_t;

typedef enum
{
  OW_STATUS_ERROR = -1,
  OW_STATUS_STOP,
  OW_STATUS_READY,
  OW_STATUS_BOOT,
  OW_STATUS_WAIT,
  OW_STATUS_RUN
} overwitch_status_t;

struct overwitch_device_desc
{
  uint16_t pid;
  char *name;
  int inputs;
  int outputs;
  char *input_track_names[OB_MAX_TRACKS];
  char *output_track_names[OB_MAX_TRACKS];
};

typedef void (*overwitch_sample_counter_init_t) (void *, double, int);
typedef void (*overwitch_inc_sample_counter_t) (void *, int);

typedef size_t (*overwitch_buffer_rw_space_t) (void *);
typedef size_t (*overwitch_buffer_rw_t) (void *, char *, size_t);

typedef uint64_t (*overwitch_get_time) ();

struct overwitch
{
  overwitch_status_t status;
  int blocks_per_transfer;
  int frames_per_transfer;
  int j2o_audio_enabled;
  pthread_spinlock_t lock;
  size_t j2o_latency;
  size_t j2o_max_latency;
  pthread_t audio_o2j_midi_thread;
  pthread_t j2o_midi_thread;
  uint16_t s_counter;
  libusb_context *context;
  libusb_device_handle *device_handle;
  const struct overwitch_device_desc *device_desc;
  struct libusb_transfer *xfr_in;
  struct libusb_transfer *xfr_out;
  char *usb_data_in;
  char *usb_data_out;
  void *j2o_audio_buf;
  void *o2j_audio_buf;
  size_t usb_data_in_blk_len;
  size_t usb_data_out_blk_len;
  size_t j2o_transfer_size;
  size_t o2j_transfer_size;
  int usb_data_in_len;
  int usb_data_out_len;
  float *j2o_transfer_buf;
  float *o2j_transfer_buf;
  size_t o2j_frame_size;
  size_t j2o_frame_size;
  //j2o resampler
  float *j2o_resampler_buf;
  SRC_DATA j2o_data;
  //MIDI
  unsigned char *j2o_midi_data;
  unsigned char *o2j_midi_data;
  struct libusb_transfer *xfr_out_midi;
  struct libusb_transfer *xfr_in_midi;
  void *j2o_midi_buf;
  void *o2j_midi_buf;
  int reading_at_j2o_end;
  pthread_spinlock_t j2o_midi_lock;
  int j2o_midi_ready;
  //sample counter
  void *sample_counter_data;
  overwitch_sample_counter_init_t sample_counter_init;
  overwitch_inc_sample_counter_t sample_counter_inc;
  //buffer operations
  overwitch_buffer_rw_space_t buffer_write_space;
  overwitch_buffer_rw_t buffer_write;
  overwitch_buffer_rw_space_t buffer_read_space;
  overwitch_buffer_rw_t buffer_read;
  //time operations
  overwitch_get_time get_time;
};

struct overwitch_midi_event
{
  uint64_t time;
  uint8_t bytes[OB_MIDI_EVENT_SIZE];
};

const char *overbrigde_get_err_str (overwitch_err_t);

void set_self_max_priority ();

overwitch_err_t overwitch_init (struct overwitch *, uint8_t, uint8_t, int);

int overwitch_activate (struct overwitch *);

void overwitch_destroy (struct overwitch *);

void overwitch_wait (struct overwitch *);

overwitch_status_t overwitch_get_status (struct overwitch *);

void overwitch_set_status (struct overwitch *, overwitch_status_t);

int overwitch_list_devices ();

void overwitch_set_j2o_audio_enable (struct overwitch *, int);

int overwitch_is_j2o_audio_enable (struct overwitch *);

int overwitch_get_bus_address (int, char *, uint8_t *, uint8_t *);

int overwitch_is_valid_device (uint16_t, uint16_t, char **);

int overwitch_bytes_to_frame_bytes (int, int);
