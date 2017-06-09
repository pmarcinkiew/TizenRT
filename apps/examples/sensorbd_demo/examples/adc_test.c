/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 * examples/examples/sensorbd/examples/adc_test.c
 *
 *   Copyright (C) 2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <tinyara/analog/adc.h>
#include <tinyara/analog/ioctl.h>

#define S5J_ADC_MAX_CHANNELS	6

#define AVERAGE_SAMPLE 10000

void adctest_main(int argc, char *argv[])
{
	int fd, ret;
	struct adc_msg_s sample[S5J_ADC_MAX_CHANNELS];
	size_t readsize;
	ssize_t nbytes;

    int averages[S5J_ADC_MAX_CHANNELS];
    int counts[S5J_ADC_MAX_CHANNELS];
    int i,j;
    for (i = 0; i < S5J_ADC_MAX_CHANNELS; ++i) {
        averages[i] = counts[i] = 0;
    }

	fd = open("/dev/adc0", O_RDONLY);
	if (fd < 0) {
		printf("%s: open failed: %d\n", __func__, errno);
		return;
	}

	for (;;) {
		ret = ioctl(fd, ANIOC_TRIGGER, 0);
		if (ret < 0) {
			printf("%s: ioctl failed: %d\n", __func__, errno);
			close(fd);
			return;
		}


		readsize = S5J_ADC_MAX_CHANNELS * sizeof(struct adc_msg_s);
		nbytes = read(fd, sample, readsize);
		if (nbytes < 0) {
			if (errno != EINTR) {
				printf("%s: read failed: %d\n", __func__, errno);
				close(fd);
				return;
			}
		} else if (nbytes == 0) {
			printf("%s: No data read, Ignoring\n", __func__);
		} else {
			int nsamples = nbytes / sizeof(struct adc_msg_s);
			if (nsamples * sizeof(struct adc_msg_s) != nbytes) {
				printf("%s: read size=%ld is not a multiple of sample size=%d, Ignoring\n", __func__, (long)nbytes, sizeof(struct adc_msg_s));
			} else {
//				printf("Sample:\n");
//				int i;
				for (i = 0; i < nsamples; i++) {


                    averages[sample[i].am_channel] += sample[i].am_data;
                    counts[sample[i].am_channel]++;
                    for (j = 0; j < S5J_ADC_MAX_CHANNELS; ++j) {
                        if (counts[j] > AVERAGE_SAMPLE) {
                            int average = averages[j] / counts[j];
                            printf("Channel %d Measurement %d\n", j, average);
                            averages[j] = 0;
                            counts[j] = 0;
                        }
                    }
//					printf("%d: channel: %d, value: %d\n", i + 1, sample[i].am_channel, sample[i].am_data);
				}
			}
		}
	}

	close(fd);
}
