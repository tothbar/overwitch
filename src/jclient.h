/*
 *   jclient.h
 *   Copyright (C) 2021 David García Goñi <dagargo@gmail.com>
 *
 *   This file is part of Overwitch.
 *
 *   Overwitch is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Overwitch is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Overwitch. If not, see <http://www.gnu.org/licenses/>.
 */

#include <jack/ringbuffer.h>
#include <jack/midiport.h>
#include "overwitch.h"
#include "dll.h"

typedef enum
{
  JC_STATUS_ERROR = -1,
  JC_STATUS_STOP,
  JC_STATUS_READY,
  JC_STATUS_BOOT,
  JC_STATUS_TUNE,
  JC_STATUS_RUN
} jclient_status_t;

struct jclient
{
  jclient_status_t status;
  struct overwitch ow;
  struct dll o2j_dll;
  jack_ringbuffer_t *o2j_audio_rb;
  jack_ringbuffer_t *j2o_audio_rb;
  jack_ringbuffer_t *o2j_midi_rb;
  jack_ringbuffer_t *j2o_midi_rb;
  size_t o2j_buf_size;
  size_t j2o_buf_size;
  double o2j_ratio;
  double j2o_ratio;
  jack_client_t *client;
  jack_port_t **output_ports;
  jack_port_t **input_ports;
  jack_port_t *midi_output_port;
  jack_port_t *midi_input_port;
  jack_nframes_t bufsize;
  double samplerate;
  jack_default_audio_sample_t *j2o_buf_in;
  jack_default_audio_sample_t *j2o_buf_out;
  jack_default_audio_sample_t *j2o_aux;
  jack_default_audio_sample_t *j2o_queue;
  size_t j2o_queue_len;
  jack_default_audio_sample_t *o2j_buf_in;
  jack_default_audio_sample_t *o2j_buf_out;
  SRC_STATE *j2o_state;
  SRC_DATA j2o_data;
  SRC_STATE *o2j_state;
  SRC_DATA o2j_data;
  size_t j2o_latency;
  size_t o2j_latency;
  size_t j2o_max_latency;
  size_t o2j_max_latency;
  int cycles_to_skip;
  double jsr;
  double obsr;
  int log_control_cycles;
  int log_cycles;
  int xruns;
  pthread_spinlock_t lock;	//Used to synchronize access to xruns.
  int reading_at_o2j_end;
  //Parameters
  uint8_t bus;
  uint8_t address;
  int blocks_per_transfer;
  int quality;
  int priority;
};

int jclient_run (struct jclient *);

void *jclient_run_thread (void *);

void jclient_exit (struct jclient *);

void jclient_print_latencies (struct jclient *, const char *);
