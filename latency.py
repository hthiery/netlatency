#!/usr/bin/env python

# Copyright (c) 2018, Kontron Europe GmbH
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from __future__ import print_function

import argparse
import dateutil.parser
import json
import numpy
import sys


def calc_latency(rx_packet):
    result = {}
    tx_user = numpy.datetime64(rx_packet['tx-user-timestamp'])
    tx_hw = None # not available yet
    rx_hw = numpy.datetime64(rx_packet['rx-hw-timestamp'])
    rx_user = numpy.datetime64(rx_packet['rx-user-timestamp'])
    diff_user_hw = rx_hw - tx_user
    diff_user_user = rx_user - tx_user
    result['type'] = 'latency'
    result['object'] = {
        'latency-user-hw': int(diff_user_hw),
        'latency-user-user': int(diff_user_user),
        'tx-user-timestamp': rx_packet['tx-user-timestamp'],
    }
    return result


def main(args=None):
    parser = argparse.ArgumentParser(
        description='latency')
    parser.add_argument('infile', nargs='?', type=argparse.FileType('r'),
                       default=sys.stdin)
    args = parser.parse_args(args)

    try:
        for line in args.infile:
            try:
                j = json.loads(line)

                if j['type'] == 'rx-error':
                    print(line, file=sys.stdout)
                    sys.stdout.flush()
                elif j['type'] == 'rx-packet':
                    result = calc_latency(j['object'])
                    print(json.dumps(result), file=sys.stdout)
                    sys.stdout.flush()
            except ValueError as e:
                print(e, file=sys.stderr)
                pass
    except KeyboardInterrupt as e:
        pass


if __name__ == '__main__':
    main()
